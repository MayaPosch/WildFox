/*
  chromebookmarks.cpp - Chrome JS ext API implementation file of WildFox-Mimic.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - 
        
  2012/02/21, Maya Posch
  (c) Nyanko.ws
*/


#include "chromebookmarks.h"


// --- CONSTRUCTOR ---
ChromeBookmarks::ChromeBookmarks(Bookmarks *bookmarks) {
    bm = bookmarks;
}


// --- DECONSTRUCTOR ---
ChromeBookmarks::~ChromeBookmarks() {
    //
}


// --- CREATE ---
QVariantMap ChromeBookmarks::create(QVariantMap bookmark, QString callback) {
    QVariantMap map = bm->create(bookmark);
    return map;
}
