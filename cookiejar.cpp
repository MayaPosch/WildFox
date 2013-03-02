/*
  cookiejar.cpp - Cookie Jar implementation file of WildFox-Mimic.
  
  Revision 0
  
  Features:
        - Implements a network cookie jar with saving/loading feature.
        - Implements third-party cookie blocking.
        
  Notes:
        - Uses passive clearing of expired cookies for now.
        
  2012/02/04, Maya Posch
  (c) Nyanko.ws
*/


#include <QSqlQuery>
#include <QSqlError>
#include <QDesktopServices>
#include <QDateTime>


#include "cookiejar.h"
#include "tldutil.h"


// --- CONSTRUCTOR ---
CookieJar::CookieJar() {
    // defaults
    /*storagePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation)
            + "/Nyanko/WildFox";*/
    
    // Open the SQLite cookie database if present. Check its integrity. Recreate
    // a default instance if there are any issues.
    storagePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    storagePath = QDir::toNativeSeparators(storagePath + "/");
    db = QSqlDatabase::addDatabase("QSQLITE", "cookiesdb");
    db.setDatabaseName(storagePath + "cookies.sqlite");
    if (!db.open()) {
        qDebug("Failed to open the cookies database.");
        qDebug() << "Cause: " << db.lastError().text();
        return;
    }
    
    QSqlQuery query(db);
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='moz_cookies'");
    if (!query.next()) {
        // table doesn't exist, create it.
        query.finish();
        bool err = query.exec(QString("CREATE TABLE moz_cookies (id INTEGER PRIMARY KEY, ") +
                   QString("baseDomain TEXT, name TEXT, value TEXT, host TEXT, ") +
                   QString("path TEXT, expiry INTEGER, lastAccessed INTEGER, ") +
                   QString("creationTime INTEGER, isSecure INTEGER, isHttpOnly INTEGER, ") +
                   QString("CONSTRAINT moz_uniqueid UNIQUE (name, host, path))"));
        if (!err) {
            qDebug("Creating cookies table failed.");
            qDebug() << "Cause: " << db.lastError().text();
            return;
        }
    }
    
    if (!db.isOpen()) {
        qDebug() << "Cookies db is not open!";
    }
}


// --- DECONSTRUCTOR ---
CookieJar::~CookieJar() {
    db.close();
}


// --- COOKIES FOR URL ---
// Returns the list of cookies for a given URL.
QList<QNetworkCookie> CookieJar::cookiesForUrl(const QUrl &url) const {
    //qDebug() << "Sending cookies...";
    // Determine the top domain.    
    QString baseDomain = getPublicDomain(url.host());
    QString pathAndFileName = url.path();
    QString defaultPath = pathAndFileName.left(pathAndFileName.lastIndexOf(QLatin1Char('/')) + 1);
    if (defaultPath.isEmpty())
        defaultPath = QLatin1Char('/');
    
    bool isEncrypted = url.scheme().toLower() == QLatin1String("https");
    
    // Load the cookies from the database if not found in the buffer. Select 
    // everything for this base domain, use the base domain as the key for the 
    // multi map and QNetworkCookies as the values.
    QList<QNetworkCookie> result;
    
    // if cookies are disabled in settings, return empty list.
    if (settings.value("cookiesEnabled", true).toBool() == false) { return result; }
    
    QMap<QString, QNetworkCookie>::iterator cookies;
    cookies = cookiebuffer.find(baseDomain);
    //qDebug() << "Searched for base domain in buffer: " << baseDomain;
    if (cookies == cookiebuffer.end()) {
        // not in the buffer, search the database.
        //qDebug() << "Searching database for cookies...";
        QList<QNetworkCookie> temp;
        if (!loadCookies(temp, baseDomain)) {
            //qDebug() << "Nothing found, return empty.";
            return result; // nothing found, return empty list
        }
        
        /*for (int i = 0; i < temp.size(); ++i) {
            if (temp[i].isSecure() != isEncrypted) {
                // Skip this cookie
                //qDebug() << "Skipping due to security mismatch.";
                continue;
            }
            //else if (temp[i].path() != defaultPath) { continue; }
            else { result.append(temp[i]); }
            //qDebug() << "Added cookie to output.";
        }*/
        
        // TODO: sort these cookies and verify on path
        //return result;
    }
        
    // in the buffer, copy it to output list and return
    QList<QNetworkCookie> temp = cookiebuffer.values(baseDomain);
    //qDebug() << "Cookies found in buffer. Copying " << temp.size() << " cookies";
    //qDebug() << "URL host: " << url.host();
    QString urlHost = url.host(); // FIXME: do proper domain verification
    //if (urlHost.startsWith(".")) { urlHost.remove(0, 1); }
    for (int i = 0; i < temp.size(); ++i) {
        // verify the cookies retrieved
        QString cookieDomain = temp[i].domain();
        if (cookieDomain.startsWith(".")) { cookieDomain.remove(0, 1); }
        //qDebug() << cookieDomain << ", " << urlHost << ", secure: " << temp[i].isSecure();
        if (urlHost.endsWith(cookieDomain, Qt::CaseInsensitive) 
                && temp[i].isSecure() <= isEncrypted) { 
                //&& temp[i].path() == defaultPath) {
            // add this cookie
            result.append(temp[i]);
            //qDebug() << "Sending cookie for: " << temp[i].domain(); // << ", host: " << url.host();
            //qDebug() << "Cookie name: " << temp[i].name();
            //qDebug() << "Cookie domain: " << cookieDomain;
            // TODO: verification & sorting
        }
    }
    
    qDebug() << "Printing cookie list.";
    for (int i = 0; i < result.size(); ++i) {
        qDebug() << "Domain: " + result[i].domain() << "name: " + result[i].name() << "value: " + result[i].value();
    }
    
    return result;
}


// --- SET COOKIES FROM URL ---
// Adds the cookies in the provided list for the provided URL.
bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url) {
    // if cookies are disabled in settings, return false.
    if (settings.value("cookiesEnabled", true).toBool() == false) { return false; }
        
    QString defaultDomain = url.host();
    QString pathAndFileName = url.path();
    QString defaultPath = pathAndFileName.left(pathAndFileName.lastIndexOf(QLatin1Char('/'))+1);
    if (defaultPath.isEmpty())
        defaultPath = QLatin1Char('/');
    
    int added = 0;
    bool updated = false;
    QDateTime now = QDateTime::currentDateTime();
    QString baseDomain = getPublicDomain(url.host());
    //qDebug() << "Cookie List size: " << cookieList.size();
    //bool skip3rd = settings.value("cookies3rdDisabled", true).toBool();
    QSqlQuery query(db);
    for (int i = 0; i < cookieList.size(); ++i) {
        updated = false;
        QNetworkCookie cookie = cookieList[i];
        //baseDomain = getPublicDomain(cookie.domain());
        // if 3rd-party cookies are disabled in settings, skip this cookie
        // if its domain doesn't match up with that of the current url.
        //qDebug() << "Skip3rd: " << skip3rd << " | " << baseDomain << ", " << getPublicDomain(cookie.domain());
        /*if (skip3rd && (baseDomain.compare(getPublicDomain(cookie.domain()), Qt::CaseInsensitive) != 0)) {
            qDebug() << "Skipping this cookie.";
            continue;
        }*/
        // FIXME: get 3rd-party blocking working...
        
        qDebug() << "Cookie name: " << cookie.name();
        /*bool isDeletion = !cookie.isSessionCookie() &&
                cookie.expirationDate() < now;*/
        if (!cookie.isSessionCookie() && cookie.expirationDate() < now) {
            qDebug() << "Expired: " << cookie.name() << cookie.value();
            continue; // skip this cookie as it's already expired.
        }
        
        // validate the cookie & set the defaults if unset
        if (cookie.path().isEmpty())
            cookie.setPath(defaultPath);
        // don't do path checking. See http://bugreports.qt.nokia.com/browse/QTBUG-5815
        /* else if (!isParentPath(pathAndFileName, cookie.path())) {
                continue;           // not accepted
            }*/
        if (cookie.domain().isEmpty()) {
            cookie.setDomain(defaultDomain);
            if (!cookie.domain().startsWith(QLatin1Char('.')))
                cookie.setDomain(QLatin1Char('.') + cookie.domain());
        } else {
            // Ensure the domain starts with a dot if its field was not empty
            // in the HTTP header. There are some servers that forget the
            // leading dot and this is actually forbidden according to RFC 2109,
            // but all browsers accept it anyway so we do that as well.
            if (!cookie.domain().startsWith(QLatin1Char('.')))
                cookie.setDomain(QLatin1Char('.') + cookie.domain());
            
            QString domain = cookie.domain();
            /*if (!(isParentDomain(domain, defaultDomain)
                  || isParentDomain(defaultDomain, domain)))
                continue; // not accepted*/
            
            // the check for effective TLDs makes the "embedded dot" rule from RFC 2109 section 4.3.2
            // redundant; the "leading dot" rule has been relaxed anyway, see above
            // we remove the leading dot for this check
            if (isEffectiveTLD(domain.remove(0, 1))) {
                qDebug() << "TLD, skipping: " << cookie.name() << cookie.value();
                continue; // not accepted
            }
        }
        
        // Check whether this cookie already exists.
        QList<QNetworkCookie> temp;
        QMultiMap<QString, QNetworkCookie>::iterator cit;
        cit = cookiebuffer.find(baseDomain);
        //qDebug() << "Size of CookieBuffer: " << cookiebuffer.size();
        if (cit == cookiebuffer.end()) {
            if (!loadCookies(temp, baseDomain)) {
                // nothing found, insert all provided cookies
                qDebug() << "No cookies found in database for " << baseDomain;
            }
            else {
                cit = cookiebuffer.find(baseDomain);
                qDebug() << "Found " << temp.size() << " cookies in database for domain " << baseDomain;
            }
        }
        
        if (cit != cookiebuffer.end()) {
            // we found the base domain in question, next search the cookies in it for a match
            qDebug() << "Searching for cookies in cookie buffer matching " << cookie.name() << "...";
            while (cit != cookiebuffer.end() && cit.key() == baseDomain) {
                QNetworkCookie c = cit.value();
                //qDebug() << "Cookie name: " << c.name();
                if (c.name() == cookie.name() &&
                        c.domain() == cookie.domain() &&
                        c.path() == cookie.path()) {
                    // same cookie, overwrite it.
                    qDebug() << "Found cookie match: " << c.name() << " - " << c.value();
                    cit.value() = cookie;
                    updated = true;
                    
                    // update in database too
                    if (cookie.isSessionCookie()) { ++cit; continue; }
                    query.prepare("UPDATE moz_cookies SET value=:value, expiry=" 
                                  + QString::number(cookie.expirationDate().toTime_t())
                                  + ", isSecure=:isSecure, isHttpOnly=:isHttpOnly"
                                  + " WHERE baseDomain=:baseDomain AND host=:host AND path=:path AND name=:name");
                    query.bindValue(":value", cookie.value());
                    query.bindValue(":host", cookie.domain());
                    query.bindValue(":baseDomain", baseDomain);
                    query.bindValue(":path", cookie.path());
                    query.bindValue(":name", cookie.name());
                    query.bindValue(":isSecure", cookie.isSecure());
                    query.bindValue(":isHttpOnly", cookie.isHttpOnly());
                    bool err = query.exec();
                    if (!err) {
                        qDebug() << "Failed to update database with query: " << query.executedQuery();
                        qDebug() << "Cause: " << query.lastError().text();
                        qDebug() << "name: " << cookie.name() << ", host: " << cookie.domain() << ", path: " << cookie.path();
                        ++cit;
                        continue; // continue with next cookie
                    }
                    
                    query.finish();
                    ++added;
                    break;
                }
                
                ++cit;
            }
        }
        
        if (!updated) {
            qDebug() << "No existing cookie, inserting as new. Name: " << cookie.name();
            qDebug() << "Session cookie: " << cookie.isSessionCookie();
            cookiebuffer.insert(baseDomain, cookie);
            if (!cookie.isSessionCookie()) {
                uint tstamp = cookie.expirationDate().toTime_t();
                QSqlQuery query(db);
                query.prepare("INSERT INTO moz_cookies (baseDomain, name, value, host, path, expiry, isSecure, isHttpOnly) VALUES (:baseDomain, :name, :value, :host, :path, "
                              + QString::number(tstamp) + ", "
                              + QString(cookie.isSecure()?"1":"0") + ", "
                              + QString(cookie.isHttpOnly()?"1":"0") + ")");
                query.bindValue(":value", cookie.value());
                query.bindValue(":host", cookie.domain());
                query.bindValue(":baseDomain", baseDomain);
                query.bindValue(":path", cookie.path());
                query.bindValue(":name", cookie.name());
                bool err = query.exec();
                if (!err) {
                    qDebug() << "Failed to insert into database with query: " << query.executedQuery();
                    qDebug() << "Cause: " << query.lastError().text();
                    qDebug() << "name: " << cookie.name() << ", host: " << cookie.domain() << ", path: " << cookie.path();
                    continue;
                }
                
                query.finish();
                ++added;
            }
        }
    }
    
    return (added > 0);
}


// --- ALL COOKIES ---
// Returns a list of all the stored cookies.
QList<QNetworkCookie> CookieJar::allCookies() const {
    QList<QNetworkCookie> allCookies;
    return allCookies;
}


// --- SET ALL COOKIES ---
// Replaces the existing list of cookies with the provided list.
void CookieJar::setAllCookies(const QList<QNetworkCookie> &cookieList) {
    //allCookies = cookieList;
    // TODO: fill in this function.
}


// --- GET BUFFER ---
// Return pointer to the cookie buffer.
QMultiMap<QString, QNetworkCookie> *CookieJar::getBuffer() {
    return &cookiebuffer;
}


// --- GET DATABASE ---
// Return a pointer to the cookie database object.
QSqlDatabase* CookieJar::getDatabase() {
    return &db;
}


// --- IS PARENT DOMAIN ---
inline bool CookieJar::isParentDomain(QString domain, QString reference) const {
    if (!reference.startsWith(QLatin1Char('.')))
        return domain == reference;

    return domain.endsWith(reference) || domain == reference.mid(1);
}


// --- LOAD COOKIES ---
// Load cookies for a top domain into the buffer, if any exist.
bool CookieJar::loadCookies(QList<QNetworkCookie> &cookies, QString baseDomain) const {
    //qDebug() << "Load cookies...";
    QSqlQuery query(db);
    query.prepare("SELECT name, value, host, path, expiry, isSecure, isHttpOnly FROM moz_cookies WHERE baseDomain='" + baseDomain + "'");
    bool err = query.exec();
    if (!err) {
        qDebug() << "Failed to fetch cookies from DB: " << query.lastError().text();
        //qDebug() << "Cause: " << query.lastError().text();
        return false;
    }
    else if (query.next()){
        // extract data and assemble into cookies.
        QDateTime now = QDateTime::currentDateTime();
        QDateTime dt;
        do {
            QNetworkCookie cookie;
            cookie.setDomain(query.value(2).toString());
            cookie.setPath(query.value(3).toString());
            cookie.setName(query.value(0).toByteArray());
            //qDebug() << "Cookie fetched: " << cookie.name();
            dt.setTime_t((uint) query.value(4).toULongLong());
            if (dt < now) { 
                //qDebug() << cookie.name() << ": dt: " << dt.toString() << " | now: " << now.toString();
                //qDebug() << "UNIX: " << query.value(4).toInt() << " | " << now.toTime_t();
                // remove this cookie
                QSqlQuery delquery(db);
                delquery.prepare("DELETE FROM moz_cookies WHERE baseDomain=:baseDomain AND path=:path AND name=:name");
                delquery.bindValue(":baseDomain", baseDomain);
                delquery.bindValue(":path", cookie.path());
                delquery.bindValue(":name", cookie.name());
                err = delquery.exec();
                if (!err) {
                    qDebug() << "Failed to delete from database with query: " << delquery.lastQuery();
                    qDebug() << "Cause: " << delquery.lastError().text();
                    continue;
                }
                
                continue; 
            }
            
            cookie.setExpirationDate(dt);
            cookie.setValue(query.value(1).toByteArray());
            cookie.setSecure(query.value(5).toBool());
            cookie.setHttpOnly(query.value(6).toBool());
            
            // insert into buffer and result list.
            cookiebuffer.insert(baseDomain, cookie);
            cookies.append(cookie);
        } while (query.next());
    }
    else {
        // nothing found, return false;
        //qDebug() << "No cookies found for " << baseDomain << ".";
        return false;
    }
    
    return true;
}
