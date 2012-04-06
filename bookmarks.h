/*
  bookmarks.h - bookmarks definition file of WildFox-Mimic.
  
  Revision 0
  
  Notes:
        - Definition of the bookmarks functions class.
        
  2012/03/20, Maya Posch
  (c) Nyanko.ws
*/

#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <QSqlDatabase>
#include <QVariantMap>
#include <QDir>
#include <QDebug>
#include <QSqlError>
#include <QMenu>


struct BookmarkData {
    int id;
    int type;
    int fk;
    int parent;
    int position;
    QString title;
    int keyword_id;
    QString folder_type;
    int dateAdded;
    int lastModified;
    QString guid;
};


struct MenuItem {
    int id;
    QMenu* item;
};


struct ActionItem {
    int id;
    QAction* action;
};


enum {
    TYPE_BOOKMARK = 0,
    TYPE_FOLDER = 1
};

class MainWindow;
#include "mainwindow.h"

class Bookmarks : public QObject {
    Q_OBJECT
    
public:
    Bookmarks(MainWindow* window);
    ~Bookmarks();
    
    void createBookmarksMenu();
    QVariantMap create(QVariantMap bookmark);
    
public slots:
    void deleteBookmark();
    
signals:
    void bookmarkCreated();
    
private:
    void recursiveTreeDescent(QMenu* menu, QTreeWidgetItem* parentfolder, int parentID);
    
    QSqlDatabase db;
    QMultiMap<int, MenuItem> menus;
    QMultiMap<int, ActionItem> actions;
    MainWindow* mw;
    QString storagePath;    
    QMenu* contextmenu;
};

#endif // BOOKMARKS_H
