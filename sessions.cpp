/*
  sessions.cpp - implementation of WildFox's Sessions class.
  
  Revision 0
  
  Features:
        - Allows for the selection of a session file.
        - Saving to a session file.
        - Restoring from a session file.
        
  Notes:
        - 
        
  2013/04/21, Maya Posch
  (c) Nyanko.ws
*/


#include "sessions.h"

#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


// --- CONSTRUCTOR ---
Sessions::Sessions(QObject *parent) : QObject(parent) {
    // defaults;
    active_db = false;
}


// --- LIST SESSIONS ---
// Returns a list of available sessions found in the sessions folder.
QList<SessionInfo> Sessions::listSessions() {
    QList<SessionInfo> list;
    
    return list;
}


// --- OPEN SESSION ---
// Opens the session file indicated by parameter 'name', and closes any existing
// session.
int Sessions::openSession(QString path) {
    QFileInfo info(path);
    if (!info.exists()) { return 1; }
    if (active_db) {
        db.close();
        QSqlDatabase::removeDatabase("sessiondb");
    }
    
    db = QSqlDatabase::addDatabase("QSQLITE", "sessiondb");
    db.setDatabaseName(path);
    if (!db.open()) {
        qDebug() << "Failed to open the session database " << path;
        qDebug() << "Cause: " << db.lastError().text();
        return 1;
    }
    
    active_db = true;
    QSqlQuery query(db);
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='tabs'");
    if (!query.next()) {
        // table doesn't exist, create it.
        bool err = query.exec("CREATE TABLE tabs (id INTEGER, PRIMARY KEY, position INTEGER, lastModified INTEGER)");
        if (!err) {
            qDebug() << "Creating tabs table failed.";
            qDebug() << "Cause: " << db.lastError().text();
            return 1;
        }
        
        err = query.exec("CREATE INDEX tabs_position ON tabs (position)");
        if (!err) {
            qDebug() << "Creating tabs_position index on tabs table failed.";
            qDebug() << "Cause: " << db.lastError().text();
            return 1;
        }
        
        err = query.exec("CREATE INDEX tabs_lastmodified ON tabs (lastModified)");
        if (!err) {
            qDebug() << "Creating tabs_lastmodified index on tabs table failed.";
            qDebug() << "Cause: " << db.lastError().text();
            return 1;
        }
    }
    
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='items'");
    if (!query.next()) {
        // table doesn't exist, create it.
        bool err = query.exec(QString("CREATE TABLE items (id INTEGER PRIMARY KEY, tabs_id INTEGER, ") +
                              QString("url TEXT,title TEXT, lastModified INTEGER)"));
        if (!err) {
            qDebug() << "Creating items table failed.";
            qDebug() << "Cause: " << db.lastError().text();
            return 1;
        }
        
        err = query.exec("CREATE INDEX items_tabs_id ON items (tabs_id)");
        if (!err) {
            qDebug() << "Creating items_tabs_id index on items table failed.";
            qDebug() << "Cause: " << db.lastError().text();
            return 1;
        }
    }
    
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='visits'");
    if (!query.next()) {
        // table doesn't exist, create it.
        bool err = query.exec(QString("CREATE TABLE visits (id INTEGER PRIMARY KEY, tabs_id INTEGER,") +
                              QString("items_id INTEGER, timestamp INTEGER)"));
        if (!err) {
            qDebug() << "Creating visits table failed.";
            qDebug() << "Cause: " << db.lastError().text();
            return 1;
        }
        
        err = query.exec("CREATE INDEX visits_tabs_id ON visits (tabs_id)");
        if (!err) {
            qDebug() << "Creating visits_tabs_id index on visits table failed.";
            qDebug() << "Cause: " << db.lastError().text();
            return 1;
        }
        
        err = query.exec("CREATE INDEX visits_timestamp ON visits (timestamp)");
        if (!err) {
            qDebug() << "Creating visits_timestamp index on visits table failed.";
            qDebug() << "Cause: " << db.lastError().text();
            return 1;
        }
    }
    
    return 0;
}


// --- SAVE SESSION ---
// Save the current session to the provided path and switch to using that session
// file.
int Sessions::saveSession(QString path) {
    //
    
    return 0;
}


// --- NEW SESSION ---
// Create a new, empty session file at the provided path and switch to using that
// file.
int Sessions::newSession(QString path) {
    //
    
    return 0;
}


// --- NEW TAB ---
// Creates a new tab entry in the session file.
int Sessions::newTab() {
    //
    
    return 0;
}


// -- ADD TAB ITEM ---
// Inserts a new history item for the specified tab.
int Sessions::addTabItem(int position) {
    //
    
    return 0;
}


// --- CHANGE TAB POSITION ---
// Changes the tab indicated by oldPos parameter to position newPos. Tabs with a position
// equal to or higher than newPos have their position increased by one, up to the gap
// left by oldPos, if oldPos is higher than newPos.
int Sessions::changeTabPosition(int oldPos, int newPos) {
    //
    
    return 0;
}
