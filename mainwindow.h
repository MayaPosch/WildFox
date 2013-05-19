/*
  mainwindow.h - Main definition file of WildFox-Mimic.
  
  Revision 0
  
  Notes:
        - 
        
  2011/11/22, Maya Posch
  (c) Nyanko.ws
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QDir>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkCookieJar>
#include <QWebFrame>
#include <QWebPage>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QtWidgets>
#include <QCompleter>
#include <QSqlQueryModel>

#include "wfwebview.h"
//#include "networkaccessmanager.h"
#include "cookiejar.h"
#include "types.h"
#include "bookmarks.h"
#include "historydatabase.h"

namespace Ui {
    class MainWindow;
}


class WFWebView;
class Bookmarks;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    friend class Bookmarks;
    
public slots:
    void quit();
    void options();
    void about();
    
    void sidebarBookmarks();
    
    void gotoURL();
    void gotoURL(QString url);
    void diagnoseLoad(bool ok);
    void changeTab(int index);
    void newTab();
    void newTab(WFWebView* &view);
    void closeTab();
    void closeTab(int index);
    void newPage(QWebFrame* frame);
    void back();
    void forward();
    void reload();
    void stop();
    void loadInteraction();
    void bookmarkAdd();
    void loadBookmark();
    void loadBookmark(QTreeWidgetItem* item, int column);
    void downloadContent(QNetworkReply* reply);
    void downloadContent(QNetworkRequest request);
    void linkHovered(const QString &link, const QString &title, 
                     const QString &textContent);
    
    void gotoAddressBar();
    void startSearch();
    
private slots:
    void addressBarTextChanged(const QString &text);
    void historyItemTriggered(int index);
    void tabTitleChanged(QString title);
    void tabIconChanged();
    void newHistoryTab();
    void openHistoryDialog();
    
protected:
    void closeEvent(QCloseEvent* event);

private:
    void setStopButton();
    void setReloadButton();
    
    Ui::MainWindow* ui;
    QSettings* settings;
    QString storagePath;        // storage location for application data.
    WFWebView* wv;              // current webview instance pointer
    QString icondb;             // path to storage location for site icons.
    QDir savedir;               // directory to save to.
    QNetworkAccessManager* nam; // the central network access manager
    CookieJar* cookiejar;       // reference to the cookiejar in use
    Bookmarks* bookmarks;       // the bookmarks object.
    HistoryDatabase* historydb; // the history database
    QFile manifest;
    QList<QWebPage*> extPages;      // Extension pages
    QList<Filter> extFilters;       // Extension content script filters
    bool stopbutton;                // is the stop page load button visible?
    //QTableWidget* historyTable;   // Address bar pop-up showing the history items.
    QList<HistoryObject> objects;   // Objects being shown in address bar popup.
    QCompleter* completer;          // the address bar completer object.
    QSqlQueryModel* historyModel;   // history db query model.
};

#endif // MAINWINDOW_H
