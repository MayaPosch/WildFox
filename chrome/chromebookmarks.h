/*
  chromebookmarks.h - bookmarks definition file of WildFox-Mimic.
  
  Revision 0
  
  Notes:
        - Definition of the bookmarks functions class.
        
  2012/03/20, Maya Posch
  (c) Nyanko.ws
*/


#ifndef CHROMEBOOKMARKS_H
#define CHROMEBOOKMARKS_H


#include <QtWebKit>

//class Bookmarks;
#include "bookmarks.h"


class ChromeBookmarks : public QObject {
    Q_OBJECT
    
public:    
    ChromeBookmarks(Bookmarks* bookmarks);
    ~ChromeBookmarks();
    
    Q_INVOKABLE QVariantMap create(QVariantMap bookmark, QString callback);
    
private:
    Bookmarks* bm;
};

#endif // CHROMEBOOKMARKS_H
