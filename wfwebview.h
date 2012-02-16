/*
  wfwebview.h - Definition file of WildFox-Mimic WFWebView class.
  
  Revision 0
  
  Notes:
        - 
        
  2011/11/22, Maya Posch
  (c) Nyanko.ws
*/


#ifndef WFWEBVIEW_H
#define WFWEBVIEW_H

#include "mainwindow.h"
#include <QWebView>

class MainWindow;

class WFWebView : public QWebView {
    Q_OBJECT
    
public:
    WFWebView();
    
    //void setTabManager(TabManager* tabman);
    //void setUIParent(MainWindow *parent);
    QWebView* createWindow(QWebPage::WebWindowType type);
    
protected:
    //void contextMenuEvent(QContextMenuEvent*);
    
private:
    //TabManager* tabmanager;
    MainWindow* uip;
};

#endif // WFWEBVIEW_H
