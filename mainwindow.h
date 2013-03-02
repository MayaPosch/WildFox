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

#include "wfwebview.h"
//#include "networkaccessmanager.h"
#include "cookiejar.h"
#include "types.h"
#include "bookmarks.h"

namespace Ui {
    class MainWindow;
}


class WFWebView;
class Bookmarks;

class MainWindow : public QMainWindow, public Ui::MainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
    void quit();
    void options();
    void about();
    
    void sidebarBookmarks();
    
    void gotoURL();
    void gotoURL(QString url);
    void diagnoseLoad(bool ok);
    void tabTitleChanged(QString title);
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

private:
    void setStopButton();
    void setReloadButton();
    
    QSettings* settings;
    QString storagePath;        // storage location for application data.
    WFWebView* wv;              // current webview instance pointer
    QString icondb;             // path to storage location for site icons.
    QDir savedir;               // directory to save to.
    QNetworkAccessManager* nam;  // the central network access manager
    CookieJar* cookiejar;       // reference to the cookiejar in use
    Bookmarks* bookmarks;        // the bookmarks object.
    QFile manifest;
    QList<QWebPage*> extPages;  // Extension pages
    QList<Filter> extFilters;   // Extension content script filters
    bool stopbutton;            // is the stop page load button visible?
};

#endif // MAINWINDOW_H
