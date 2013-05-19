/*
  addressbarcompleter.h - declaration of the address bar completer class.
  
  Revision 0
  
  Notes:
        - 
        
  2013/05/18, Maya Posch
  (c) Nyanko.ws
*/

#ifndef ADDRESSBARCOMPLETER_H
#define ADDRESSBARCOMPLETER_H

#include <QCompleter>

class AddressBarCompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit AddressBarCompleter(QObject *parent = 0);
    
    void setCompletionPrefix(const QString &prefix);
    
signals:
    
public slots:
    
};

#endif // ADDRESSBARCOMPLETER_H
