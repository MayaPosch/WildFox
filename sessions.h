/*
  sessions.h - declaration of WildFox's Sessions class.
  
  Revision 0
        
  Notes:
        - 
        
  2013/04/21, Maya Posch
  (c) Nyanko.ws
*/

#ifndef SESSIONS_H
#define SESSIONS_H

#include <QObject>
#include <QSqlDatabase>


struct SessionInfo {
    QString name;
    QString filename;
};


class Sessions : public QObject {
    Q_OBJECT
public:
    explicit Sessions(QObject *parent = 0);
    
    QList<SessionInfo> listSessions();
    int openSession(QString path);
    int saveSession(QString path);
    int newSession(QString path);
    
    int newTab();
    int addTabItem(int position);
    int changeTabPosition(int oldPos, int newPos);
    
signals:
    
public slots:
    
private:
    QSqlDatabase db;
    bool active_db;
};

#endif // SESSIONS_H
