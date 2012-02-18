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
#include <QSettings>

#include "wfwebview.h"
#include "cookiejar.h"

namespace Ui {
    class MainWindow;
}

class WFWebView;

class MainWindow : public QMainWindow, public Ui::MainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
    void quit();
    void options();
    void about();
    
    void gotoURL();
    void diagnoseLoad(bool ok);
    void tabTitleChanged(QString title);
    void changeTab(int index);
    void newTab();
    void newTab(WFWebView* &view);
    void closeTab();
    void closeTab(int index);
    void back();
    void forward();
    void reload();
    void stop();
    void loadInteraction();
    void downloadContent(QNetworkReply* reply);
    void downloadContent(QNetworkRequest request);
    void linkHovered(const QString &link, const QString &title, 
                     const QString &textContent);
    
    void gotoAddressBar();

private:
    void setStopButton();
    void setReloadButton();
    
    QSettings* settings;
    QString storagePath;    // storage location for application data.
    WFWebView* wv;          // current webview instance pointer
    QString icondb; // path to storage location for site icons.
    QDir savedir;   // directory to save to.
    QNetworkAccessManager* nam; // the central network access manager
    CookieJar* cookiejar;    // reference to the cookiejar in use
    //QNetworkCookieJar* cookies; // the central cookie jar.
    bool stopbutton;            // is the stop page load button visible?
};

#endif // MAINWINDOW_H
