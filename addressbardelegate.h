/*
  addressbardelegate.h - Address Bar Delegate declaration file of WildFox.
  
  Revision 0
        
  Notes:
        - 
        
  2013/04/28, Maya Posch
  (c) Nyanko.ws
*/


#ifndef ADDRESSBARDELEGATE_H
#define ADDRESSBARDELEGATE_H

#include <QStyledItemDelegate>

class AddressBarDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit AddressBarDelegate(QObject *parent = 0);
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
signals:
    
public slots:
    
};

#endif // ADDRESSBARDELEGATE_H
