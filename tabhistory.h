/*
  tabhistory.h - declaration of WildFox's TabHistory class.
  
  Revision 0
  
  Notes:
        - 
        
  2013/06/01, Maya Posch
  (c) Nyanko.ws
*/


#ifndef TABHISTORY_H
#define TABHISTORY_H

#include <QObject>
#include <QIcon>
#include <QUrl>
#include <QDateTime>


struct TabHistoryItem {
    QIcon icon;
    QUrl originalUrl;
    QUrl url;
    QString title;
    QDateTime lastVisited;
};


class TabHistory : public QObject {
    Q_OBJECT
public:
    explicit TabHistory(QObject *parent = 0);
    
    void clear();
    
    QList<TabHistoryItem> items() const;
    QList<TabHistoryItem> backItems(int maxItems) const;
    QList<TabHistoryItem> forwardItems(int maxItems) const;
    
    bool canGoBack() const;
    bool canGoForward() const;
    
    void back();
    void forward();
    void goToItem(const TabHistoryItem &item);
    
    TabHistoryItem backItem() const;
    TabHistoryItem currentItem() const;
    TabHistoryItem forwardItem() const;
    TabHistoryItem itemAt(int i) const;
    
    int currentItemIndex() const;
    
    int count() const;
    
    int maximumItemCount() const;
    void setMaximumItemCount(int count);
    
signals:
    
public slots:
    
private:
    QList<TabHistoryItem> historyItems;    
};

#endif // TABHISTORY_H
