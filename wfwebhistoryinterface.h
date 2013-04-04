/*
  wfwebhistoryinterface.h - declaration of WildFox's WFWebHistoryInterface class.
  
  Revision 0
        
  Notes:
        - 
        
  2013/04/04, Maya Posch
  (c) Nyanko.ws
*/


#ifndef WFWEBHISTORYINTERFACE_H
#define WFWEBHISTORYINTERFACE_H

#include <QWebHistoryInterface>

class WFWebHistoryInterface : public QWebHistoryInterface
{
    Q_OBJECT
public:
    explicit WFWebHistoryInterface(QObject *parent = 0);
    
    void addHistoryEntry(const QString &url);
    bool historyContains(const QString &url) const;
    
signals:
    
public slots:
    
};

#endif // WFWEBHISTORYINTERFACE_H
