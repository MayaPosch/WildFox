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
#include <QWebFrame>
#include <QWebElement>
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
    connect(actionBookmarksSidebar, SIGNAL(triggered()), this, SLOT(sidebarBookmarks()));
    
    // other interface connections
    connect(backButton, SIGNAL(pressed()), this, SLOT(back()));
    connect(forwardButton, SIGNAL(pressed()), this, SLOT(forward()));
    connect(reloadButton, SIGNAL(pressed()), this, SLOT(loadInteraction()));
    tabWidget->addAction(actionReload);
    connect(actionReload, SIGNAL(triggered()), this, SLOT(reload()));
    connect(actionStop, SIGNAL(triggered()), this, SLOT(stop()));
    connect(addressBar, SIGNAL(returnPressed()), this, SLOT(gotoURL()));
    connect(addressBarGo, SIGNAL(pressed()), this, SLOT(gotoURL()));
    connect(searchBar, SIGNAL(returnPressed()), this, SLOT(startSearch()));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
    tabWidget->addAction(actionClose_Tab);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(bookmarksTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(loadBookmark(QTreeWidgetItem*,int)));
    
    connect(actionBookmark_this_page, SIGNAL(triggered()), this, SLOT(bookmarkAdd()));
    
    addAction(actionGo_to_address_bar);
    connect(actionGo_to_address_bar, SIGNAL(triggered()), this, SLOT(gotoAddressBar()));
    
    // defaults
    QCoreApplication::setOrganizationName("Nyanko");
    QCoreApplication::setOrganizationDomain("nyanko.ws");
    QCoreApplication::setApplicationName("WildFox");
    QCoreApplication::setApplicationVersion("0.1 Alpha");
    QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery();
    QNetworkProxy::setApplicationProxy(proxies[0]);
    wv = 0;
    //icondb = "./";
    cookiejar = new CookieJar();
    bookmarks = new Bookmarks(this);
    nam = new QNetworkAccessManager(this);
    nam->setCookieJar(cookiejar);
    stopbutton = false;
    QApplication::setActiveWindow(this);
    //settings = new QSettings("Nyanko", "WildFox");
    settings = new QSettings();
    storagePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
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
    manifest.setFileName(QString("extensions/xmarks/manifest.json"));
    if (!manifest.exists() || !manifest.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open the manifest file.";
    }
    else {
        QString manifestString = manifest.readAll();
        bool ok;
        QMap<QString, QVariant> result;
        result = QtJson::Json::parse(manifestString, ok).toMap();
        if (!ok) {
            qDebug() << "Parsing of manifest failed.";
        }
        
        if (result.contains("background_page")) {
            // load this extension into a webpage object.
            QWebPage* page = new QWebPage;
            page->mainFrame()->load(manifest.fileName());
            extPages.append(page);
        }
        
        if (result.contains("content_scripts")) {
            // add the content scripts in this list to the specified filters.
            QMap<QString, QVariant> scripts = result["content_scripts"].toMap();
            if (scripts.contains("matches")) {
                QList<QVariant> matches = scripts["matches"].toList();
                QList<QVariant> js = scripts["js"].toList();
                Filter filter;
                filter.extId = extPages.size() - 1;
                for (int i = 0; i < js.size(); ++i) {
                    filter.scripts.append(QString("extensions/xmarks/") + js[i].toString());
                }
                
                for (int i = 0; i < matches.size(); ++i) {
                    QStringList urlbit = matches[i].toString().split("://");
                    if (urlbit.size() < 2) { continue; }
                    filter.scheme = urlbit[0];
                    filter.segments = urlbit[1].split(".");
                    if (filter.scripts.size() < 1 || filter.segments.size() < 1) {
                        break; // empty/useless filter, skip
                    }
                    
                    extFilters.append(filter);
                }
            }
        }
    }
    
    // Restore application state
    restoreGeometry(settings->value("geometry").toByteArray());
    restoreState(settings->value("windowState").toByteArray());
    bookmarksTree->setVisible(settings->value("bookmarkSidebar", false).toBool());
    
    // pass extension filter data to the network access manager
    //nam->setFilters(extFilters);
    
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


// --- SIDE BAR BOOKMARKS ---
// Hides/shows the bookmarks sidebar.
void MainWindow::sidebarBookmarks() {
    //qDebug() << "Setting sidebar visibility...";
    if (bookmarksTree->isVisible()) { bookmarksTree->hide(); }
    else { bookmarksTree->setVisible(true); }
}


// --- GO TO URL ---
// Go to the URL in the addressbar in the currently open tab. If no tab is open,
// open a new tab.
void MainWindow::gotoURL() {
    QString url = addressBar->text();
    gotoURL(url);
}


// --- GO TO URL ---
// Go to the url provided in the string.
void MainWindow::gotoURL(QString url) {
    if (tabWidget->count() < 1) { 
        newTab(); // open new tab before proceding
    }
    
    /*QWidget* tab = tabWidget->currentWidget();
    if (!tab) { qDebug("tab is null"); return; }
    
    wv = tab->findChild<WFWebView*>("webView");*/
    wv = (WFWebView*) tabWidget->currentWidget();
    if (!wv) { qDebug("wv is null"); return; }
    
    url.toLower();
    qDebug() << "Lower-case URL: " << url;
    
    // TODO: perform more advanced URL validation.
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
    wv->load(QUrl(url));
}


// --- DIAGNOSE LOAD ---
// Verify that the page loaded successfully, else present error message.
// If successful, set other data parts and handle filters.
void MainWindow::diagnoseLoad(bool ok) {
    setReloadButton();
    if (!ok) {
        //QMessageBox::critical(this, tr("Error"), tr("Failed to load the URL")); // FIXME: triggers crash with favicon path customized due to QtWebkit bug! 
																					// Don't return to the event loop.
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
        
        // check the page URL against the filters
        if (extFilters.size() < 1) { return; }
        
        QWebPage* page = (QWebPage*) sender();
        if (page == 0) { return; }        
        QWebFrame* frame = page->mainFrame();
        if (frame == 0) { return; }
        QString url = frame->url().path();
        QStringList urlbit = url.split("://");
        QDir extension;
        extension.setPath(manifest.fileName());
        if (urlbit.size() > 1) {
            QString scheme = urlbit[0];
            QStringList bits = urlbit[1].split(".");
            for (int i = 0; i < extFilters.size(); ++i) {
                if (extFilters[i].scheme != scheme) { continue; }
                for (int j = 0; extFilters[i].segments.size(); ++j) {
                    if (extFilters[i].segments[j] != bits[j] && extFilters[i].segments[j] != "*") {
                        continue;
                    }
                }
                
                // matched filter, inject associated scripts into the content.
                QWebElement root = frame->documentElement();
                QWebElement head = root.findFirst("head");
                head.appendOutside("<script type=\"text/javascript\" src=\"" +
                                   extension.absolutePath() + "\" />");
            }
        }
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
    //QMessageBox msgBox;
    wv = new WFWebView();
    //connect(wv, SIGNAL(loadFinished(bool)), this, SLOT(diagnoseLoad(bool)));
    connect(wv->page()->mainFrame(), SIGNAL(loadFinished(bool)), 
            this, SLOT(diagnoseLoad(bool)));
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


// --- NEW PAGE ---
// Called when a new page is created, inside a frameset or iframe.
void MainWindow::newPage(QWebFrame* frame) {
    connect(frame, SIGNAL(loadFinished(bool)), this, SLOT(diagnoseLoad(bool)));
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
    
    wv->stop();
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


// --- BOOKMARK ADD ---
// Add the currently displayed page to the bookmarks.
// TODO: Show dialogue to select the folder to add the bookmarks to.
void MainWindow::bookmarkAdd() {
    if (wv == 0) { return; }
    
    QVariantMap bookmark;
    bookmark.insert("title", wv->title());
    bookmark.insert("url", wv->url().toString());
    bookmarks->create(bookmark);
}


// --- LOAD BOOKMARK ---
// Loads a selected bookmark into the currently active window.
void MainWindow::loadBookmark() {
    QAction* action = (QAction*) sender();
    QString url = action->data().toString();
    addressBar->setText(url);
    gotoURL();
    //gotoURL(url);
}


// --- LOAD BOOKMARK ---
// Loads a bookmark selected in the sidebar in the currently active window.
void MainWindow::loadBookmark(QTreeWidgetItem *item, int column) {
    QList<QVariant> data = item->data(0, Qt::UserRole).toList();
    QString url = data[0].toString();
    addressBar->setText(url);
    gotoURL();
    //gotoURL(url);
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


// --- START SEARCH ---
// Send a search query to the selected web search engine.
// TODO: Current default is Google, make this configurable.
// Query is HTML-encoded to make it safe for transmission.
void MainWindow::startSearch() {
    QString query = searchBar->text();
    if (query.isEmpty()) { return; }
    query.replace(" ", "+", Qt::CaseInsensitive);
    gotoURL("http://google.com/search?q=" + query);
}


// --- CLOSE EVENT ---
// Reeimplements the close event function of QWidget to save state.
void MainWindow::closeEvent(QCloseEvent* event)  {
     settings->setValue("geometry", saveGeometry());
     settings->setValue("windowState", saveState());
     settings->setValue("bookmarkSidebar", bookmarksTree->isVisible());
     QMainWindow::closeEvent(event);
 }
