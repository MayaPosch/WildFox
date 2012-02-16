/*
  mainwindow.cpp - Main implementation file of WildFox-Mimic.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - 
        
  2011/11/22, Maya Posch
  (c) Nyanko.ws
*/

#include "wfwebview.h"
#include <QDebug>

// --- CONSTRUCTOR ---
WFWebView::WFWebView() {
    // set defaults
    uip = 0;
    /*QWidgetList list = QApplication::topLevelWidgets();
    for (int i = 0; i < list.size(); ++i) {
        if (list[i]->isWindow()) {
            uip = qobject_cast<MainWindow*>(list[i]);
            break;
        }
    }*/
    
    uip = (MainWindow*) QApplication::activeWindow();
    if (uip == 0) {
        qDebug() << "Failed to find main window: " << uip;
    }
}


// --- SET TAB MANAGER ---
// Set the tab manager to use with this WFWebView instance.
/*void WFWebView::setTabManager(TabManager* tabman) {
    tabmanager = tabman;
}*/


// --- SET UI PARENT ---
// Sets the object managing the UI objects.
/*void WFWebView::setUIParent(MainWindow* parent) {
    uip = parent;
}*/


// --- CREATE WINDOW ---
QWebView* WFWebView::createWindow(QWebPage::WebWindowType type) {
    if (type == QWebPage::WebBrowserWindow) {
        // ensure we have a pointer to the main window object
        if (uip == 0) {
            // no object set just yet. We return null;
            return 0;
        }
        
        WFWebView* wv = 0;
        uip->newTab(wv);
        qDebug() << "WFWebView wv: " << wv;
        return wv;
    }
    else if (type == QWebPage::WebModalDialog) {
        // TODO: just return a new tab for now.
        WFWebView* wv = 0;
        uip->newTab(wv);
        return wv;
    }
    
    return 0;
}


// --- CONTEXT MENU EVENT ---
// Handle context menu events.
