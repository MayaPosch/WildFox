/*
  historydatabase.h - declaration of WildFox's HistoryDatabase class.
  
  Revision 0
        
  Notes:
        - 
        
  2013/04/05, Maya Posch
  (c) Nyanko.ws
*/


#ifndef HISTORYDATABASE_H
#define HISTORYDATABASE_H

#include <QSqlDatabase>
#include <QSqlQueryModel>


struct HistoryObject {
    QString url;
    QString title;
};


class HistoryDatabase {
public:
    static HistoryDatabase* getInstance() {
        if (instance == 0) {
            instance = new HistoryDatabase();
        }
        
        return instance;
    }
    
    void addHistoryEntry(const QString &url);
    void addHistoryEntry(const QString &url, const QString &title);
    bool historyContains(const QString &url) const;
    void reportVisit(const QString &url);
    void reportVisitAndTitle(const QString &url, const QString title);
    void historyObjectsMatching(QString &like, int limit, QList<HistoryObject> &objects);
    
    static QSqlDatabase db;
    
protected:
    
private:  
    HistoryDatabase();
    ~HistoryDatabase();  
    
    static HistoryDatabase* instance;
    //static HistoryDatabase instance;
    static QString storagePath;
    static QSqlQueryModel* queryModel;
};

#endif // HISTORYDATABASE_H
