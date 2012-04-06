/*
  chromejsinterface.cpp - Network access manager implementation file of WildFox-Mimic.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - 
        
  2012/03/20, Maya Posch
  (c) Nyanko.ws
*/


#include "chromejsinterface.h"


// --- CONSTRUCTOR ---
ChromeJSInterface::ChromeJSInterface(MainWindow *window, Bookmarks *bookmarks) {
    mw = window;
    bm = bookmarks;
    
    this->bookmarks = new ChromeBookmarks(bm);
}


// --- DECONSTRUCTOR ---
ChromeJSInterface::~ChromeJSInterface() {
    //
}
