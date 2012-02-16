/*
  mainwindow.cpp - Main implementation file of WildFox-Mimic.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - Uses the JSON parser from http://ereilin.tumblr.com/post/6857765046/lightweight-json-parser-serializer-class-for-qt
        
  2011/11/22, Maya Posch
  (c) Nyanko.ws
*/

#include "mainwindow.h"

#include <QtGui>
#include <QWebView>
#include <QtUiTools/QUiLoader>
#include <QDesktopServices>
#include <QNetworkProxyFactory>
#include <QList>

#include "iofile.h"
#include "optionsdialog.h"
#include "json.h"


// --- CONSTRUCTOR ---
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
    setupUi(this);
    
    // menu connections
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(actionAbout_WildFox, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionNewTab, SIGNAL(triggered()), this, SLOT(newTab()));
    connect(actionClose_Tab, SIGNAL(triggered()), this, SLOT(closeTab()));
    connect(actionOptions, SIGNAL(triggered()), this, SLOT(options()));
    
    // other interface connections
    connect(backButton, SIGNAL(pressed()), this, SLOT(back()));
    connect(forwardButton, SIGNAL(pressed()), this, SLOT(forward()));
    connect(reloadButton, SIGNAL(pressed()), this, SLOT(loadInteraction()));
    connect(actionReload, SIGNAL(triggered()), this, SLOT(reload()));
    connect(actionStop, SIGNAL(triggered()), this, SLOT(stop()));
    connect(addressBar, SIGNAL(returnPressed()), this, SLOT(gotoURL()));
    connect(addressBarGo, SIGNAL(pressed()), this, SLOT(gotoURL()));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
    tabWidget->addAction(actionClose_Tab);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    
    addAction(actionGo_to_address_bar);
    connect(actionGo_to_address_bar, SIGNAL(triggered()), this, SLOT(gotoAddressBar()));
    
    // defaults
    QCoreApplication::setOrganizationName("Nyanko");
    QCoreApplication::setOrganizationDomain("nyanko.ws");
    QCoreApplication::setApplicationName("WildFox");
    QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery();
    QNetworkProxy::setApplicationProxy(proxies[0]);
    wv = 0;
    icondb = "./";
    //cookies = new QNetworkCookieJar();
    cookiejar = new CookieJar();
    nam = new QNetworkAccessManager(this);
    nam->setCookieJar(cookiejar);
    stopbutton = false;
    QApplication::setActiveWindow(this);
    //settings = new QSettings("Nyanko", "WildFox");
    settings = new QSettings();
    storagePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QDir dir(storagePath);
    if (!dir.exists()) {
        dir.mkpath(storagePath); // create path if it doesn't exist yet
    }
    
    qDebug() << "Storage path: " << storagePath;
    QWebSettings* ws = QWebSettings::globalSettings();
    ws->setAttribute(QWebSettings::PluginsEnabled, true);
    ws->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    ws->setIconDatabasePath(storagePath);
    
    // load any extensions. For each found extension, read in the JSON manifest
    // and create a WebPage object for background scripts and load the extension
    // into it. Register URL filters.
    // TESTING: XMarks extension is at ./extensions/xmarks/manifest.json
    QFile manifest("extensions/xmarks/manifest.json");
    if (!manifest.exists() || !manifest.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open the manifest file.";
    }
    else {
        QString manifestString = manifest.readAll();
        bool ok;
        QList<QString, QVariant> result;
        result = Json::parse(manifestString, ok).toMap();
        if (!ok) {
            qDebug() << "Parsing of manifest failed.";
        }
        
        if (result.contains("background_page")) {
            // load this extension into a webpage object.
        }
        
        if (result.contains("content_scripts")) {
            // add the content scripts in this list to the specified filters.
        }
    }
    
    // open empty tab
    newTab();
}


// --- DECONSTRUCTOR ---
MainWindow::~MainWindow() {
    //delete nam;
    delete settings;
}


// --- SET STOP BUTTON ---
// Changes the UI elements to show a stop loading button with associated shortcut.
void MainWindow::setStopButton() {
    reloadButton->setText(tr("Stop"));
    stopbutton = true;
}


// --- SET RELOAD BUTTON ---
// Changes the UI elements to show a reload page button with associated shorcut.
void MainWindow::setReloadButton() {
    reloadButton->setText(tr("Reload"));
    stopbutton = false;
}


// --- QUIT ---
void MainWindow::quit() {
    exit(0);
}


// --- OPTIONS ---
// Start the options dialogue.
void MainWindow::options() {
    OptionsDialog dialog(cookiejar);
    dialog.exec();
}


// --- ABOUT ---
void MainWindow::about() {
    QMessageBox::about(this, tr("About WildFox"), tr("WildFox 0.1 by Maya Posch\n\nwww.mayaposch.com"));
}


// --- GO TO URL ---
// Go to the URL in the addressbar in the currently open tab. If no tab is open,
// open a new tab.
void MainWindow::gotoURL() {
    if (tabWidget->count() < 1) { 
        newTab(); // open new tab before proceding
    }
    
    /*QWidget* tab = tabWidget->currentWidget();
    if (!tab) { qDebug("tab is null"); return; }
    
    wv = tab->findChild<WFWebView*>("webView");*/
    wv = (WFWebView*) tabWidget->currentWidget();
    if (!wv) { qDebug("wv is null"); return; }
    
    QString url = addressBar->text();
    url.toLower();
    qDebug() << "Lower-case URL: " << url;
    
    if (url.size() < 1) {
        QMessageBox::critical(this, "Error", "No URL provided: " + url);
        return;
    }
    
    if (!url.startsWith("http://") && !url.startsWith("https://")) {
        url.prepend("http://");
        addressBar->setText(url);
    }
    
    // Load the URL, change the UI elements to allow stopping this action.
    setStopButton();
    wv->setUrl(QUrl(url));
}


// --- DIAGNOSE LOAD ---
// Verify that the page loaded successfully, else present error message.
// If successful, set other data parts.
void MainWindow::diagnoseLoad(bool ok) {
    setReloadButton();
    if (!ok) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load the URL"));
    }
    else {
        //QWidget* tab = tabWidget->currentWidget
        tabWidget->setTabText(tabWidget->currentIndex(), wv->title());
        tabWidget->setTabIcon(tabWidget->currentIndex(), wv->icon());
        addressBar->setText(wv->url().toString());
        QString title = wv->title();
        if (title.size() > 200) {
            title.resize(200);
        }
        
        setWindowTitle(title + " - WildFox");
    }
}


// --- TAB TITLE CHANGED ---
// Called by the WebView when the page title changes.
void MainWindow::tabTitleChanged(QString title) {
    // TODO: change main window title if current title as well
    /*QString title = wv->title();
    if (title.size() > 200) {
        title.resize(200);
    }
    
    setWindowTitle(title + " - WildFox");*/
    // obtain the index of this tab via the pointer to the WebView calling.
    QObject* w = sender();
    WFWebView* webview = (WFWebView*) w;
    if (w == 0) { qDebug("Unknown sender"); return; }
    
    int index = tabWidget->indexOf(webview);
    if (index == -1) { qDebug("Tab index was -1"); return; }
    tabWidget->setTabText(index, webview->title());
    if (index == tabWidget->currentIndex()) {
        setWindowTitle(webview->title() + " - WildFox");
    }
}


// --- CHANGE TAB ---
// Perform necessary changes here when the active tab has been changed.
void MainWindow::changeTab(int index) {
    if (index == -1) {  // no tabs in tab widget
        setWindowTitle("WildFox");
        wv = 0;
        return; 
    }
    
    wv = (WFWebView*) tabWidget->currentWidget();
    if (!wv) { return; }
    addressBar->setText(wv->url().toString());
    setWindowTitle(wv->title());
}


// --- NEW TAB ---
// Open a new browser tab at the end of the tab bar.
void MainWindow::newTab() {
    qDebug("Loading new tab...");
    QMessageBox msgBox;
    wv = new WFWebView();
    connect(wv, SIGNAL(loadFinished(bool)), this, SLOT(diagnoseLoad(bool)));
    connect(wv, SIGNAL(titleChanged(QString)), 
            this, SLOT(tabTitleChanged(QString)));
    if (wv == 0) {
        qDebug("WV is null");
        return;
    }
    
    wv->page()->setNetworkAccessManager(nam);
    /*QWebSettings* ws = wv->settings();
    ws->setAttribute(QWebSettings::PluginsEnabled, true);
    ws->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    ws->setIconDatabasePath(icondb);*/
    wv->page()->setForwardUnsupportedContent(true); // enable downloads
    connect(wv->page(), SIGNAL(unsupportedContent(QNetworkReply*)), 
            this, SLOT(downloadContent(QNetworkReply*))); // set up slot
    connect(wv->page(), SIGNAL(downloadRequested(QNetworkRequest)), 
            this, SLOT(downloadContent(QNetworkRequest))); // context menu call
    connect(wv->page(), SIGNAL(linkHovered(QString,QString,QString)), 
            this, SLOT(linkHovered(QString,QString,QString)));
    qDebug("Settings changed.");
    int index = tabWidget->count();
    ++index;
    index = tabWidget->insertTab(index, wv, tr("New Tab"));
    qDebug("Inserted tab.");
    addressBar->setText("");
    //addressBar->setFocus(Qt::OtherFocusReason);
    gotoAddressBar();
    tabWidget->setCurrentIndex(index);
    setWindowTitle("WildFox");
}


// --- NEW TAB ---
// Overloaded function. Open new browser window as in original function, but
// also returns a pointer to the WFWebView object in the new tab.
void MainWindow::newTab(WFWebView* &view) {
    newTab();
    qDebug("Passing new webview instance pointer...");
    //qDebug() << "wv: " << wv;
    view = wv;
    qDebug() << "view: " << view;
}


// --- CLOSE TAB ---
// Close the currently visible tab.
void MainWindow::closeTab() {
    if (tabWidget->count() < 1) { return; }
    closeTab(tabWidget->currentIndex());
}


// --- CLOSE TAB ---
// Close the indicated tab.
// TODO: store the history of the tab so that it can be restored.
void MainWindow::closeTab(int index) {
    qDebug("Close tab called");
    if (wv == 0) { return; }
    qDebug("Closing tab...");
    tabWidget->removeTab(index);
    wv = (WFWebView*) tabWidget->currentWidget();
    if (wv != 0) {
        setWindowTitle(wv->title() + " - WildFox");
    }
    else {
        setWindowTitle("WildFox");
    }
}


// --- BACK ---
// Go back one page in the current tab.
void MainWindow::back() {
    if (wv == 0) { return; }
    
    wv->back();
}


// --- FORWARD ---
// Go forward one page in the current tab.
void MainWindow::forward() {
    if (wv == 0) { return; }
    
    wv->forward();
}


// --- RELOAD ---
// Reload the contents of the current tab.
void MainWindow::reload() {
    if (wv == 0) { return; }
    
    wv->reload();
}


// --- STOP ---
// Stop the loading of the currently displayed page.
void MainWindow::stop() {
    if (wv == 0) { return; }
    // TODO: check that this webview is loading something.
    
    wv->stop();
}


// --- LOAD INTERACTION ---
// Responds to the reload/stop button and acts depending on the current state.
void MainWindow::loadInteraction() {
    if (stopbutton) { stop(); }
    else { reload(); }
}


// --- DOWNLOAD CONTENT ---
// Download the content provided in a QNetworkReply in a separate thread.
void MainWindow::downloadContent(QNetworkReply* reply) {
    // Ask for path to save to.
    QString path = QFileDialog::getSaveFileName(this, tr("Save to..."), savedir.path());
    if (path.isEmpty()) { reply->deleteLater(); return; }
    savedir.setPath(path);
    
    // Launch download dialogue
    QProgressDialog* progress = new QProgressDialog(tr("Downloading %1").arg(reply->url().path()), tr("Cancel"), 0, reply->size());
    IOFile* file = new IOFile;
    file->setFileName(path);
    file->open(QIODevice::WriteOnly);
    connect(reply, SIGNAL(finished()), progress, SLOT(hide()));
    connect(reply, SIGNAL(finished()), progress, SLOT(deleteLater()));
    connect(reply, SIGNAL(readyRead()), file, SLOT(readFromReply()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), file, SLOT(readProgress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), file, SLOT(terminate()));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
    connect(file, SIGNAL(progress(int)), progress, SLOT(setValue(int)));
    connect(progress, SIGNAL(canceled()), file, SLOT(cancel()));
}


// --- DOWNLOAD CONTENT ---
// Download the content requested by the provided QNetworkRequest in a separate
// thread.
void MainWindow::downloadContent(QNetworkRequest request) {
    QNetworkAccessManager* nam = new QNetworkAccessManager(this);
    QNetworkReply* reply = nam->get(request);
    downloadContent(reply);
}


// --- LINK HOVERED ---
// Called when the mouse cursor hovers over a link in a webpage.
void MainWindow::linkHovered(const QString &link, const QString &title, 
                             const QString &textContent) {
    Ui_MainWindow::statusBar->showMessage(link);
}


// --- GO TO ADDRESS BAR ---
// Upon triggering sets the focus on the address bar and selects any text in it.
void MainWindow::gotoAddressBar() {
    addressBar->setFocus(Qt::ShortcutFocusReason);
    addressBar->selectAll();
}
