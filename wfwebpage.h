/*
  wfwebpage.h - declaration of WildFox's WFWebPage class.
  
  Revision 0
        
  Notes:
        - 
        
  2013/03/26, Maya Posch
  (c) Nyanko.ws
*/


#ifndef WFWEBPAGE_H
#define WFWEBPAGE_H

#include <QWebPage>

class WFWebPage : public QWebPage {
    Q_OBJECT
public:
    explicit WFWebPage(QWidget *parent = 0);
    bool supportsExtension(Extension extension) const;
    bool extension(Extension extension, const ExtensionOption *option, ExtensionReturn *output);
    
signals:
    
public slots:
    
};

#endif // WFWEBPAGE_H
