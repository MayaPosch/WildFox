/*
  networkaccessmanager.h - network access manager declaration file of WildFox-Mimic.
  
  Revision 0
  
  Notes:
        - 
        
  2012/02/21, Maya Posch
  (c) Nyanko.ws
*/

#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include "types.h"


class NetworkAccessManager : public QNetworkAccessManager {
    Q_OBJECT
    
public:
    void setFilters(QList<Filter> &filters);
    QList<Filter>* getFilters();
    
protected:
    QNetworkReply* createRequest(Operation op, const QNetworkRequest &request, QIODevice* outgoingData);
    
private:
    QList<Filter> filters;
};
        

#endif // NETWORKACCESSMANAGER_H
