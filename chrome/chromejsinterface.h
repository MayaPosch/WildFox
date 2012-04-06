/*
  chromejsinterface.h - Chrome JS Interface definition file of WildFox-Mimic.
  
  Revision 0
  
  Notes:
        - Definition of the Chrome extension support implementation.
        
  2012/03/19, Maya Posch
  (c) Nyanko.ws
*/

#ifndef CHROMEJSINTERFACE_H
#define CHROMEJSINTERFACE_H

//#include <QObject>


class MainWindow;
class Bookmarks;
//class ChromeBookmarks;
class ChromeBrowserAction;
class ChromeExtension;
class ChromeI18n;
class ChromeSelf;
class ChromeTabs;
class ChromeWindows;


#include "chromebookmarks.h"


class ChromeJSInterface : public QObject {
    Q_OBJECT
    
public:
    ChromeJSInterface(MainWindow* window, Bookmarks* bookmarks);
    ~ChromeJSInterface();
    
private:
    MainWindow* mw;
    Bookmarks* bm;
    ChromeBookmarks* bookmarks;
    ChromeBrowserAction* browserAction;
    ChromeExtension* extension;
    ChromeI18n* i18n;
    ChromeSelf* self;
    ChromeTabs* tabs;
    ChromeWindows* windows;
};

#endif // CHROMEJSINTERFACE_H
