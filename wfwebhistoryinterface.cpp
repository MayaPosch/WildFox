/*
  wfwebhistoryinterface.h - declaration of WildFox's WFWebHistoryInterface class.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - 
        
  2013/04/04, Maya Posch
  (c) Nyanko.ws
*/


#include "wfwebhistoryinterface.h"


// --- CONSTRUCTOR ---
WFWebHistoryInterface::WFWebHistoryInterface(QObject *parent) :
    QWebHistoryInterface(parent) {
    // 
}


// --- ADD HISTORY ENTRY ---
// Called by WebKit to add another url to the list of visited pages.
void WFWebHistoryInterface::addHistoryEntry(const QString &url) {
    //
}


// --- HISTORY CONTAINS ---
// Called by the WebKit engine to query whether a certain url has been visited 
// by the user already. Returns true if the url is part of the history of 
// visited links; otherwise returns false.
bool WFWebHistoryInterface::historyContains(const QString &url) const {
    //
    
    return false;
}
