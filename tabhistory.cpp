/*
  tabhistory.cpp - implementation of WildFox's TabHistory class.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - 
        
  2013/06/01, Maya Posch
  (c) Nyanko.ws
*/


#include "tabhistory.h"


// --- CONSTRUCTOR ---
TabHistory::TabHistory(QObject *parent) : QObject(parent) {
    //
}


// --- CLEAR ---
// Clear the list of history items.
void TabHistory::clear() {
    historyItems.clear();
}


// --- ITEMS ---
// Return the list of history items.
QList<TabHistoryItem> TabHistory::items() const {
    return historyItems;
}


// --- BACK ITEMS ---
// Return the maxItems last history items from current item towards the back of
// the history items list.
QList<TabHistoryItem> TabHistory::backItems(int maxItems) const {
    QList<TabHistoryItem> list;
    
    return list;
}


// --- FORWARD ITEMS ---
// Return the first maxItems history items from current item towards the front of
// the history items list.
QList<TabHistoryItem> TabHistory::forwardItems(int maxItems) const {
    QList<TabHistoryItem> list;
    return list;
}


// --- CAN GO BACK ---
// Whether there's an item behind the current history item.
bool TabHistory::canGoBack() const {
    //
    
    return false;
}


// --- CAN GO FORWARD ---
// Whether there's an item before the current history item.
bool TabHistory::canGoForward() const {
    //
    
    return false;
}


// --- BACK ---
