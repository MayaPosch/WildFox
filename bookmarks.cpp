/*
  bookmarks.cpp - Network access manager implementation file of WildFox-Mimic.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - 
        
  2012/02/21, Maya Posch
  (c) Nyanko.ws
*/


#include "bookmarks.h"

#include <QSqlQuery>
#include <QAction>
#include <QTreeWidgetItem>
#include <QDesktopServices>
#include <QDateTime>


// --- CONSTRUCTOR ---
Bookmarks::Bookmarks(MainWindow *window) {
    mw = window;
    
    // open the bookmarks database file if present. Check its integrity. Create
    // a default instance if there are any issues.
    storagePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    storagePath = QDir::toNativeSeparators(storagePath + "/");
    db = QSqlDatabase::addDatabase("QSQLITE", "bookmarksdb");
    db.setDatabaseName(storagePath + "bookmarks.sqlite");
    if (!db.open()) {
        qDebug("Failed to open the bookmarks database.");
        qDebug() << "Cause: " << db.lastError().text();
        return;
    }
    
    QSqlQuery query(db);
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='bookmarks'");
    if (!query.next()) {
        // table doesn't exist, create it.
        query.finish();
        bool err = query.exec(QString("CREATE TABLE bookmarks (id INTEGER PRIMARY KEY, ") +
                   QString("type INTEGER, fk INTEGER DEFAULT NULL, parent INTEGER, ") +
                   QString("position INTEGER, title TEXT, url TEXT, keyword_id INTEGER, ") + 
                   QString("folder_type TEXT, dateAdded INTEGER, lastModified INTEGER, guid TEXT)"));
        if (!err) {
            qDebug("Creating bookmarks table failed.");
            qDebug() << "Cause: " << db.lastError().text();
            return;
        }        
        
        // add default content
        query.finish();
        err = query.exec("INSERT INTO bookmarks (id, type, parent, position, title, url, " +
                QString("keyword_id, folder_type, dateAdded, lastModified, guid)") +
                QString(" VALUES (1, 1, 0, 0, '', '', 0, '', 0, 0, 'dRm4PcMt67gO')"));
        if (!err) {
            qDebug() << "Failed to insert default bookmark data.";
            qDebug() << "Cause: " << query.lastError().text();
            return; 
        }
    }
    
    query.finish();
    contextmenu = new QMenu(mw->ui->bookmarksTree);
    mw->ui->bookmarksTree->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* deleteAction = new QAction(tr("Delete Bookmark"), contextmenu);
    mw->ui->bookmarksTree->addAction(deleteAction);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteBookmark()));
    createBookmarksMenu();
}


// --- DECONSTRUCTOR ---
Bookmarks::~Bookmarks() {
    db.close();
}


// --- CREATE BOOKMARKS MENU ---
// Assemble the bookmarks menu from scratch.
void Bookmarks::createBookmarksMenu() {
    mw->ui->menuBookmarks->clear();
    mw->ui->bookmarksTree->clear();
    menus.clear();
    actions.clear();
    QAction* actionAdd = mw->ui->menuBookmarks->addAction("Bookmark this page");
    actionAdd->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    connect(actionAdd, SIGNAL(triggered()), mw, SLOT(bookmarkAdd()));
    mw->ui->menuBookmarks->addSeparator();
    
    // Assemble the in-memory bookmark data structures
    // FIXME: just retrieving in a 'flat' style now, not dealing with folders.
    //          Fetching only bookmarks and basic info now.
    QSqlQuery query(db);
    bool err = query.exec("SELECT id, type, parent, title, url FROM bookmarks");
    if (!err) {
        qDebug() << "Failed to fetch bookmarks from database.";
        qDebug() << "Cause: " << query.lastError().text();
        return;
    }
    
    while (query.next()) {
        int type = query.value(1).toInt();
        int parent = query.value(2).toInt();
        qDebug() << "Type: " << type << ", parent: " << parent;
        MenuItem item;
        ActionItem action;
        if (type == TYPE_FOLDER) {
            item.id = query.value(0).toInt();
            item.item = new QMenu(query.value(3).toString());
            menus.insert(parent, item);
        }
        else if (type == TYPE_BOOKMARK) {
            action.id = query.value(0).toInt();
            action.action = new QAction(query.value(3).toString(), mw);
            action.action->setData(query.value(4).toString());
            action.action->setIcon(QIcon(QWebSettings::iconForUrl(QUrl(query.value(4).toString()))));
            connect(action.action, SIGNAL(triggered()), mw, SLOT(loadBookmark()));
            actions.insert(parent, action);
        }
    }
    
    // Set the bookmarks actions/menus in the GUI.
    // First fetch the root folder (ID:1), call the recursive function on it.
    if (menus.contains(1) || actions.contains(1)) {
        QTreeWidgetItem* treeitem = new QTreeWidgetItem(mw->ui->bookmarksTree);
        treeitem->setText(0, "Bookmarks");
        //qDebug() << "Adding items to root.";
        recursiveTreeDescent(mw->ui->menuBookmarks, treeitem, 1);
        mw->ui->bookmarksTree->expandAll();
    }
    else {
        qDebug() << "No root folder found for bookmarks.";
    }
}


// --- RECURSIVE TREE DESCENT ---
// Constructs a single menu folder, calls itself recursively to construct sub-folders.
void Bookmarks::recursiveTreeDescent(QMenu* menu, QTreeWidgetItem *parentfolder, int parentID) {
    QList<MenuItem> folders = menus.values(parentID);
    QTreeWidgetItem* item;
    for (int i = 0; i < folders.size(); ++i) {
        //qDebug() << "Found folder, adding...";
        item = new QTreeWidgetItem(parentfolder);
        item->setText(0, folders[i].item->title());
        //qDebug() << "Name: " << item->text(0);
        recursiveTreeDescent(folders[i].item, item, folders[i].id);
        menu->addMenu(folders[i].item);
    }
    
    QList<ActionItem> bookmarks = actions.values(parentID);
    //menu->addActions(bookmarks);
    QList<QVariant> bookmarkdata;
    for (int i = 0; i < bookmarks.size(); ++i) {
        menu->addAction(bookmarks[i].action);
        bookmarkdata.clear();
        //qDebug() << "Found bookmark, adding...";
        item = new QTreeWidgetItem(parentfolder);
        item->setText(0, bookmarks[i].action->text());
        item->setIcon(0, QIcon(QWebSettings::iconForUrl(QUrl(bookmarks[i].action->data().toString()))));
        //qDebug() << "Name: " << item->text(0);
        bookmarkdata.append(bookmarks[i].action->data());   // ID 0
        bookmarkdata.append(bookmarks[i].id);               // ID 1
        item->setData(0, Qt::UserRole, bookmarkdata);
    }
}


// --- CREATE ---
// Create the new bookmark, regenerate the bookmarks menu and return the node map.
// Return immediately if no URL is present in the bookmark data.
QVariantMap Bookmarks::create(QVariantMap bookmark) {
    if (!bookmark.contains("url")) { return bookmark; }
    //QVariantMap node;
    QSqlQuery query(db);
    QDateTime time;
    query.prepare("INSERT INTO bookmarks (type, parent, position, title, url, " +
                  QString("keyword_id, folder_type, dateAdded, lastModified, guid) ") +
                  QString("VALUES (:type, :parent, :position, :title, :url, :kid,") +
                  QString(":ft, :added, :modified, :guid)"));
    query.bindValue(":type", TYPE_BOOKMARK);
    query.bindValue(":parent", 1); // attach to root
    query.bindValue(":position", 0); // unsorted
    query.bindValue(":title", bookmark.value("title", "Untitled").toString());
    query.bindValue(":url", bookmark.value("url", "").toString());
    query.bindValue(":kid", 0);
    query.bindValue(":ft", 0);
    query.bindValue(":added", time.toTime_t());
    query.bindValue(":modified", time.toTime_t());
    query.bindValue(":guid", ""); // TODO: generate GUID
    bool err = query.exec();
    if (!err) {
        qDebug() << "Failed to update database with query: " << query.executedQuery();
        qDebug() << "Cause: " << query.lastError().text();
        return bookmark;
    }
    
    emit bookmarkCreated();
    createBookmarksMenu();
    
    return bookmark;
}


// --- DELETE BOOKMARK ---
// Delete the currently selected bookmark.
void Bookmarks::deleteBookmark() {
    qDebug() << "Delete triggered.";
    QTreeWidgetItem* item = mw->ui->bookmarksTree->currentItem();
    int id = item->data(0, Qt::UserRole).toList()[1].toInt();
    QSqlQuery query(db);
    query.prepare("DELETE FROM bookmarks WHERE id=:id");
    query.bindValue(":id", id);
    bool err = query.exec();
    if (!err) {
        qDebug() << "Failed to delete bookmark from database with query: " << query.executedQuery();
        qDebug() << "Cause: " << query.lastError().text();
    }
    
    createBookmarksMenu(); // re-create the menu
}
