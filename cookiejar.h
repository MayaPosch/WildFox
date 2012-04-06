/*
  cookiejar.h - Cookie Jar definition file of WildFox-Mimic.
  
  Revision 0
  
  Notes:
        - 
        
  2012/02/04, Maya Posch
  (c) Nyanko.ws
*/


#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <QNetworkCookieJar>
#include <QSqlDatabase>
#include <QMultiMap>
#include <QSettings>
//#include <QDateTime>

/*struct Cookie {
    QString domain;
    QDateTime expiry;
    bool isHttpOnly;
    bool isSecure();
    bool isSessionCookie();
    QByteArray name;
    QString path;
    QByteArray value;
};*/

// --- COOKIE JAR ---
// Reimplementation of the QNetworkCookieJar class.
class CookieJar : public QNetworkCookieJar {
    Q_OBJECT
    
public:
    CookieJar();
    ~CookieJar();
    QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);
    QMultiMap<QString, QNetworkCookie>* getBuffer();
    QSqlDatabase* getDatabase();
    
protected:
    QList<QNetworkCookie> allCookies() const;
    void setAllCookies(const QList<QNetworkCookie> &cookieList);
    
private:
    inline bool isParentDomain(QString domain, QString reference) const;
    bool loadCookies(QList<QNetworkCookie> &cookies, QString baseDomain) const;
    
    QSqlDatabase db;
    QString storagePath;
    mutable QMultiMap<QString, QNetworkCookie> cookiebuffer;
    QSettings settings;
};

#endif // COOKIEJAR_H
