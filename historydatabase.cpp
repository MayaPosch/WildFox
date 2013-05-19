/*
  historydatabase.cpp - implementation of WildFox's HistoryDatabase class.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - 
        
  2013/04/05, Maya Posch
  (c) Nyanko.ws
*/



#include <QSqlQuery>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QSqlError>

#include "historydatabase.h"


HistoryDatabase* HistoryDatabase::instance = 0;
QString HistoryDatabase::storagePath;
QSqlDatabase HistoryDatabase::db;
QSqlQueryModel* HistoryDatabase::queryModel;


// --- CONSTRUCTOR ---
HistoryDatabase::HistoryDatabase() {
    // Open the SQLite history database if present. Check its integrity. Recreate
    // a default instance if there are any issues.
    storagePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    storagePath = QDir::toNativeSeparators(storagePath + "/");
    db = QSqlDatabase::addDatabase("QSQLITE", "historydb");
    db.setDatabaseName(storagePath + "history.sqlite");
    if (!db.open()) {
        qDebug("Failed to open the history database.");
        qDebug() << "Cause: " << db.lastError().text();
        return;
    }
    
    QSqlQuery query(db);
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='history'");
    if (!query.next()) {
        // table doesn't exist, create it.
        query.finish();
        bool err = query.exec(QString("CREATE TABLE history (id INTEGER PRIMARY KEY, ") +
                   QString("url TEXT, title TEXT, visit_count INTEGER, ") +
                   QString("lastModified INTEGER)"));
        if (!err) {
            qDebug("Creating history table failed.");
            qDebug() << "Cause: " << db.lastError().text();
            return;
        }
        
        query.finish();
        err = query.exec(QString("CREATE INDEX history_url ON history (url)"));
        if (!err) {
            qDebug("Creating url index on history table failed.");
            qDebug() << "Cause: " << db.lastError().text();
            return;
        }
        
        query.finish();
        err = query.exec(QString("CREATE INDEX history_visitcount ON history (visit_count)"));
        if (!err) {
            qDebug("Creating visitcount index on history table failed.");
            qDebug() << "Cause: " << db.lastError().text();
            return;
        }
        
        query.finish();
        err = query.exec(QString("CREATE INDEX history_lastmodified ON history (lastModified)"));
        if (!err) {
            qDebug("Creating lastModified index on history table failed.");
            qDebug() << "Cause: " << db.lastError().text();
            return;
        }
    }
}


// --- DECONSTRUCTOR ---
HistoryDatabase::~HistoryDatabase() {
    db.close();
}


// --- ADD HISTORY ENTRY ---
// Used by Webkit. Adds a new history entry if it doesn't exist yet in the database.
// NOTE: currently not called as this doesn't add a title. New overloaded function
// now being called from main window after load.
void HistoryDatabase::addHistoryEntry(const QString &url) {
    if (historyContains(url)) {
        reportVisit(url);
        return;
    }
    
    qDebug() << "Adding history entry:" << url;
    QSqlQuery query(db);
    query.prepare("INSERT INTO history (url, title, visit_count, lastModified) VALUES (:url, :title, :visit_count, :lastModified)");
    query.bindValue(":url", url);
    query.bindValue(":title", "Unknown");
    query.bindValue(":visit_count", 0);
    query.bindValue(":lastModified", QDateTime::currentDateTime().toTime_t());
    int err = query.exec();
    if (!err) {
        qDebug("Inserting new entry into history table failed.");
        qDebug() << "Cause: " << db.lastError().text();
        return;
    }
}


// --- ADD HISTORY ENTRY ---
// Overloaded function. Adds a new history entry with its page title if it 
// doesn't exist yet in the database.
void HistoryDatabase::addHistoryEntry(const QString &url, const QString &title) {
    if (historyContains(url)) {
        reportVisit(url);
        return;
    }
    
    qDebug() << "Adding history entry:" << url;
    QSqlQuery query(db);
    query.prepare("INSERT INTO history (url, title, visit_count, lastModified) VALUES (:url, :title, :visit_count, :lastModified)");
    query.bindValue(":url", url);
    query.bindValue(":title", title);
    query.bindValue(":visit_count", 0);
    query.bindValue(":lastModified", QDateTime::currentDateTime().toTime_t());
    int err = query.exec();
    if (!err) {
        qDebug("Inserting new entry into history table failed.");
        qDebug() << "Cause: " << db.lastError().text();
        return;
    }
}


// --- HISTORY CONTAINS ---
// Used by Webkit. Returns true if the provided URL exists in the database, 
// otherwise returns false.
bool HistoryDatabase::historyContains(const QString &url) const {
    //qDebug() << "History contains:" << url;
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM history WHERE url=:url");
    query.bindValue(":url", url);
    bool err = query.exec();
    if (!err) {
        qDebug("Searching for URL in history table failed.");
        qDebug() << "Cause: " << db.lastError().text();
        return false;
    }
    
    if (query.next()) {
        //qDebug() << "Checking value...";
        if (query.value(0).toInt() > 0) {
            //qDebug() << "True." << query.value(0).toInt();
            return true;
        }
    }
    
    //qDebug() << "False." << query.value(0).toInt();
    return false;
}


// --- REPORT VISIT ---
// Called when a URL is visited. Increases visit count if the item already exists.
void HistoryDatabase::reportVisit(const QString &url) {
    qDebug() << "Reporting visit:" << url;
    QSqlQuery query(db);
    query.prepare("UPDATE history SET visit_count=visit_count + 1 WHERE url=:url AND lastModified=:lastModified");
    query.bindValue(":url", url);
    query.bindValue(":lastModified", QDateTime::currentDateTime().toTime_t());
    bool err = query.exec();
    if (!err) {
        qDebug("Updating visit count for URL in history table failed.");
        qDebug() << "Cause: " << db.lastError().text();
        return;
    }
}


// --- HISTORY OBJECTS MATCHING ---
// Return up to limit objects matching the provided string, sorted by visit count.
void HistoryDatabase::historyObjectsMatching(QString &like, int limit, QList<HistoryObject> &objects) {
    QSqlQuery query(db);
    query.prepare("SELECT url, title FROM history WHERE url LIKE :like ORDER BY visit_count ASC LIMIT :limit");
    query.bindValue(":like", QString("%%1%").arg(like));
    query.bindValue(":limit", limit);
    bool err = query.exec();
    if (!err) {
        qDebug("Finding URL matches in history table failed.");
        qDebug() << "Cause: " << db.lastError().text();
        return;
    }
    
    //QList<HistoryObject> objects;
    HistoryObject object;
    while (query.next()) {
        object.url = query.value(0).toString();
        object.title = query.value(1).toString();
        //qDebug() << "URL: " << object.url;
        objects.append(object);
    }
}
