/*
  addressbareventfilter.h - addressBarEventFilter class header file of WildFox.

  Revision 0

  Notes:
        -

  2013/04/25, Maya Posch
  (c) Nyanko.ws
*/


#ifndef ADDRESSBAREVENTFILTER_H
#define ADDRESSBAREVENTFILTER_H

#include <QObject>

class addressBarEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit addressBarEventFilter(QObject *parent = 0);
    
    bool eventFilter(QObject *, QEvent *);
    
signals:
    void addressBarPopup();
    
public slots:
    
};

#endif // ADDRESSBAREVENTFILTER_H
