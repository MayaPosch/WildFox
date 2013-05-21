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
#include <QWebHistory>

#include "iofile.h"
#include "optionsdialog.h"
#include "json.h"
#include "wfwebhistoryinterface.h"
#include "addressbardelegate.h"
#include "addressbareventfilter.h"
#include "historydialog.h"


// --- CONSTRUCTOR ---
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    
    //qDebug() << "Mew?";
    
    // menu connections
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(ui->actionAbout_WildFox, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionNewTab, SIGNAL(triggered()), this, SLOT(newTab()));
    connect(ui->actionClose_Tab, SIGNAL(triggered()), this, SLOT(closeTab()));
    connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(options()));
    connect(ui->actionBookmarksSidebar, SIGNAL(triggered()), this, SLOT(sidebarBookmarks()));
    
    // other interface connections
    connect(ui->backButton, SIGNAL(pressed()), this, SLOT(back()));
    connect(ui->forwardButton, SIGNAL(pressed()), this, SLOT(forward()));
    connect(ui->reloadButton, SIGNAL(pressed()), this, SLOT(loadInteraction()));
    //ui->tabWidget->addAction(ui->actionReload);
    connect(ui->actionReload, SIGNAL(triggered()), this, SLOT(reload()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(stop()));
    connect(ui->addressBar->lineEdit(), SIGNAL(returnPressed()), this, SLOT(gotoURL()));
    //connect(addressBarGo, SIGNAL(pressed()), this, SLOT(gotoURL()));
    connect(ui->historyButton, SIGNAL(pressed()), 
            this, SLOT(openHistoryDialog()));
    connect(ui->searchBar, SIGNAL(returnPressed()), 
            this, SLOT(startSearch()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, 
            SLOT(changeTab(int)));
    ui->tabWidget->addAction(ui->actionClose_Tab);
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(ui->bookmarksTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), 
            this, SLOT(loadBookmark(QTreeWidgetItem*,int)));
    
    connect(ui->actionBookmark_this_page, SIGNAL(triggered()), this, SLOT(bookmarkAdd()));
    
    addAction(ui->actionOpen_History_Tab);
    connect(ui->actionOpen_History_Tab, SIGNAL(triggered()), 
            this, SLOT(newHistoryTab()));
    addAction(ui->actionHistory_dialog);
    connect(ui->actionHistory_dialog, SIGNAL(triggered()), 
            this, SLOT(openHistoryDialog()));
            
    addAction(ui->actionGo_to_address_bar);
    connect(ui->actionGo_to_address_bar, SIGNAL(triggered()), this, SLOT(gotoAddressBar()));
    //connect(ui->addressBar->lineEdit(), SIGNAL(textEdited(QString)),
            //this, SLOT(addressBarTextChanged(QString)));
    //connect(ui->addressBar, SIGNAL(activated(int)), this, SLOT(historyItemTriggered(int)));
    
    // defaults
    QCoreApplication::setOrganizationName("Nyanko");
    QCoreApplication::setOrganizationDomain("nyanko.ws");
    QCoreApplication::setApplicationName("WildFox");
    QCoreApplication::setApplicationVersion("0.1 Alpha");
    QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery();
    QNetworkProxy::setApplicationProxy(proxies[0]);
    wv = 0;
    //icondb = "./";
    historydb = HistoryDatabase::getInstance();
    ui->addressBar->setMaxVisibleItems(10);
    completer = new QCompleter(this);
    historyModel = new QSqlQueryModel;
    historyModel->setQuery("SELECT url FROM history", historydb->db);
    completer->setModel(historyModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    //completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    ui->addressBar->lineEdit()->setCompleter(completer);
    cookiejar = new CookieJar();
    bookmarks = new Bookmarks(this);
    nam = new QNetworkAccessManager(this);
    nam->setCookieJar(cookiejar);
    WFWebHistoryInterface::setDefaultInterface(new WFWebHistoryInterface);
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
    ui->bookmarksTree->setVisible(settings->value("bookmarkSidebar", false).toBool());
    
    // pass extension filter data to the network access manager
    //nam->setFilters(extFilters);
    
    // change the address bar QComboBox to use a QTableView instead of its default
    // QListView.
    /*historyTable = new QTableWidget();
    ui->addressBar->setModel(historyTable->model());
    ui->addressBar->setView(historyTable);
    historyTable->setRowCount(10);
    historyTable->setColumnCount(2);
    connect(historyTable, SIGNAL(itemActivated(QTableWidgetItem*)),
            this, SLOT(historyItemTriggered(QTableWidgetItem*)));*/
    
    // Add the custom delegate to the address bar.
    AddressBarDelegate* delegate = new AddressBarDelegate;
    ui->addressBar->setItemDelegate(delegate);
    
    // open empty tab
    newTab();
}


// --- DECONSTRUCTOR ---
MainWindow::~MainWindow() {
    //delete nam;
    delete settings;
    //delete historydb;
    delete ui;
}


// --- SET STOP BUTTON ---
// Changes the UI elements to show a stop loading button with associated shortcut.
void MainWindow::setStopButton() {
    //reloadButton->setText(tr("Stop"));
    ui->reloadButton->setIcon(QIcon(":/wildfox/img/process-stop.png"));
    stopbutton = true;
}


// --- SET RELOAD BUTTON ---
// Changes the UI elements to show a reload page button with associated shorcut.
void MainWindow::setReloadButton() {
    //reloadButton->setText(tr("Reload"));
    ui->reloadButton->setIcon(QIcon(":/wildfox/img/view-refresh.png"));
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
    ui->bookmarksTree->setVisible(!ui->bookmarksTree->isVisible());
    settings->setValue("bookmarkSidebar", !ui->bookmarksTree->isVisible());
    /*if (ui->bookmarksTree->isVisible()) { 
        ui->bookmarksTree->hide(); 
        settings->setValue("bookmarkSidebar", false);
    }
    else { ui->bookmarksTree->setVisible(true); }*/
}


// --- GO TO URL ---
// Go to the URL in the addressbar in the currently open tab. If no tab is open,
// open a new tab.
void MainWindow::gotoURL() {
    QString url = ui->addressBar->lineEdit()->text();
    gotoURL(url);
}


// --- GO TO URL ---
// Go to the url provided in the string.
void MainWindow::gotoURL(QString url) {
    if (ui->tabWidget->count() < 1) { 
        newTab(); // open new tab before proceding
    }
    
    /*QWidget* tab = tabWidget->currentWidget();
    if (!tab) { qDebug("tab is null"); return; }
    
    wv = tab->findChild<WFWebView*>("webView");*/
    wv = (WFWebView*) ui->tabWidget->currentWidget();
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
        ui->addressBar->lineEdit()->setText(url);
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
        // obtain the sender which should be a QWebFrame pointer
        QWebFrame* sender = (QWebFrame*) QObject::sender();
        qDebug() << "Sender: " << sender;
        
        //QWidget* tab = tabWidget->currentWidget
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), wv->title());
        ui->tabWidget->setTabIcon(ui->tabWidget->currentIndex(), wv->icon());
        ui->addressBar->lineEdit()->setText(wv->url().toString());
        QString title = wv->title();
        if (title.size() > 200) {
            title.resize(200);
        }
        
        setWindowTitle(title + " - WildFox");
        
        if (wv != 0) {
            ui->backButton->setEnabled(wv->history()->canGoBack());
            ui->forwardButton->setEnabled(wv->history()->canGoForward());
        }
        
        // save page to history database        
        if (sender != 0) {         
            qDebug() << "Adding URL, meow~";
            historydb->addHistoryEntry(sender->url().toString(), sender->title());
        }
        
        // check the page URL against the filters
        if (extFilters.size() < 1) { return; }
        
        QWebPage* page = (QWebPage*) QObject::sender();
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


// --- CHANGE TAB ---
// Perform necessary changes here when the active tab has been changed.
void MainWindow::changeTab(int index) {
    if (index == -1) {  // no tabs in tab widget
        setWindowTitle("WildFox");
        wv = 0;
        return; 
    }
    
    wv = (WFWebView*) ui->tabWidget->currentWidget();
    if (!wv) { return; }
    ui->addressBar->lineEdit()->setText(wv->url().toString());
    setWindowTitle(wv->title());
    ui->backButton->setEnabled(wv->history()->canGoBack());
    ui->forwardButton->setEnabled(wv->history()->canGoForward());
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
    connect(wv, SIGNAL(iconChanged()), this, SLOT(tabIconChanged()));
    connect(wv, SIGNAL(titleChanged(QString)), this, SLOT(tabTitleChanged(QString)));
    
    //wv->addAction(ui->actionReload);
    qDebug("Settings changed.");
    int index = ui->tabWidget->count();
    ++index;
    index = ui->tabWidget->insertTab(index, wv, tr("New Tab"));
    qDebug("Inserted tab.");
    ui->addressBar->lineEdit()->setText("");
    //addressBar->setFocus(Qt::OtherFocusReason);
    gotoAddressBar();
    ui->tabWidget->setCurrentIndex(index);
    setWindowTitle("WildFox");
    
    ui->backButton->setEnabled(false);
    ui->forwardButton->setEnabled(false);
    
    bookmarks->createBookmarksMenu();
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
    if (ui->tabWidget->count() < 1) { return; }
    closeTab(ui->tabWidget->currentIndex());
}


// --- CLOSE TAB ---
// Close the indicated tab.
// TODO: store the history of the tab so that it can be restored.
void MainWindow::closeTab(int index) {
    qDebug("Close tab called");
    if (wv == 0) { return; }
    qDebug("Closing tab...");
    ui->tabWidget->removeTab(index);
    wv = (WFWebView*) ui->tabWidget->currentWidget();
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
    ui->addressBar->lineEdit()->setText(url);
    gotoURL();
    //gotoURL(url);
}


// --- LOAD BOOKMARK ---
// Loads a bookmark selected in the sidebar in the currently active window.
void MainWindow::loadBookmark(QTreeWidgetItem *item, int column) {
    QList<QVariant> data = item->data(0, Qt::UserRole).toList();
    QString url = data[0].toString();
    ui->addressBar->lineEdit()->setText(url);
    gotoURL();
    //gotoURL(url);
}


// --- DOWNLOAD CONTENT ---
// Download the content provided in a QNetworkReply in a separate thread.
void MainWindow::downloadContent(QNetworkReply* reply) {
    // Ask for path to save to.
    QString url = reply->url().toString();
    int pos = url.lastIndexOf("/");
    QString filename = url.mid(pos);
    if (filename.size() < 1) { filename = ""; }
    QString path = QFileDialog::getSaveFileName(this, tr("Save to..."), savedir.path() + filename);
    if (path.isEmpty()) { reply->deleteLater(); return; }
    savedir.setPath(path);
    
    // Launch download dialogue
    QProgressDialog* progress = new QProgressDialog(tr("Downloading %1").arg(reply->url().path()), tr("Cancel"), 0, reply->size());
    IOFile* file = new IOFile;
    file->setFileName(path);
    file->setNetworkReply(reply);
    file->open(QIODevice::WriteOnly);
    connect(reply, SIGNAL(finished()), progress, SLOT(hide()));
    connect(reply, SIGNAL(finished()), progress, SLOT(deleteLater()));
    connect(reply, SIGNAL(readyRead()), file, SLOT(readFromReply()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), file, SLOT(readProgress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), file, SLOT(terminate()));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
    connect(file, SIGNAL(progress(int)), progress, SLOT(setValue(int)));
    connect(progress, SIGNAL(canceled()), file, SLOT(cancel()));
    progress->show();
    //progress->exec();
}


// --- DOWNLOAD CONTENT ---
// Download the content requested by the provided QNetworkRequest in a separate
// thread.
// FIXME: delete NAM and such after use here or make it class members.
void MainWindow::downloadContent(QNetworkRequest request) {
    QNetworkAccessManager* nam = new QNetworkAccessManager(this);
    QNetworkReply* reply = nam->get(request);
    downloadContent(reply);
}


// --- LINK HOVERED ---
// Called when the mouse cursor hovers over a link in a webpage.
void MainWindow::linkHovered(const QString &link, const QString &title, 
                             const QString &textContent) {
    ui->Ui_MainWindow::statusBar->showMessage(link);
}


// --- GO TO ADDRESS BAR ---
// Upon triggering sets the focus on the address bar and selects any text in it.
void MainWindow::gotoAddressBar() {
    ui->addressBar->lineEdit()->setFocus(Qt::ShortcutFocusReason);
    ui->addressBar->lineEdit()->selectAll();
}


// --- START SEARCH ---
// Send a search query to the selected web search engine.
// TODO: Current default is Google, make this configurable.
// Query is HTML-encoded to make it safe for transmission.
void MainWindow::startSearch() {
    QString query = ui->searchBar->text();
    if (query.isEmpty()) { return; }
    query.replace(" ", "+", Qt::CaseInsensitive);
    gotoURL("http://google.com/search?q=" + query);
}


// --- ADDRESS BAR TEXT CHANGED ---
// Called when the text in the address bar's line edit changes. Show the combo
// box's popup and fill it with 10 history items matching the text entered
// so far, sorted by popularity.
void MainWindow::addressBarTextChanged(const QString &text) {
    if (text.size() < 3) { return; }
    QString like = text;
    objects.clear();
    historydb->historyObjectsMatching(like, 12, objects);
    //historyTable->clear();
    ui->addressBar->clear();
    
    int objects_size = objects.size();
    qDebug() << "objects: " << objects_size;
    /*QTableWidgetItem* item = 0;*/
    for (int i = 0; i < objects_size; ++i) {
        //qDebug() << "i: " << i;
        //qDebug() << "qcombobox count: " << ui->addressBar->count();
        //qDebug() << "url: " << objects[i].url;
        //qDebug() << "title: " << objects[i].title;
        ui->addressBar->insertItem(i + 1, 
                                   QIcon(QWebSettings::iconForUrl(QUrl(objects[i].url))), 
                                   objects[i].title,
                                   objects[i].url
                                   );
        /*item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled);
        item->setIcon(QIcon(QWebSettings::iconForUrl(QUrl(objects[i].url))));
        item->setText(objects[i].url);
        item->setData(Qt::UserRole, objects[i].url);
        historyTable->setItem(i, 0, item);
        
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled);
        item->setText(objects[i].title);
        item->setData(Qt::UserRole, objects[i].url);
        historyTable->setItem(i, 1, item);*/
    }
    
    ui->addressBar->showPopup();
}


// --- HISTORY ITEM TRIGGERED ---
// Called when a history item has been triggered in the history popup of the 
// address bar.
// Loads the URL attached to the item.
void MainWindow::historyItemTriggered(int index) {
    ui->addressBar->hidePopup();
    objects.clear();
    qDebug() << "History item triggered.";
    QString url = ui->addressBar->itemData(index).toString();
    ui->addressBar->setEditText(url);
    qDebug() << "Data: " << url;
    gotoURL(url);
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
    
    int index = ui->tabWidget->indexOf(webview);
    if (index == -1) { qDebug("Tab index was -1"); return; }
    ui->tabWidget->setTabText(index, webview->title());
    if (index == ui->tabWidget->currentIndex()) {
        setWindowTitle(webview->title() + " - WildFox");
    }
}


// --- TAB ICON CHANGED ---
// Called by the WebView when the page's icon changes.
void MainWindow::tabIconChanged() {
    // First obtain the sender and cast to webview.
    WFWebView* webview = (WFWebView*) sender();
    if (webview == 0) { qDebug() << "Unknown sender."; return; }
    
    int index = ui->tabWidget->indexOf(webview);
    if (index == -1) { qDebug() << "Tab index was -1"; return; }
    ui->tabWidget->setTabIcon(index, webview->icon());
    
    bookmarks->createBookmarksMenu();
}


// --- NEW HISTORY TAB ---
// Open a new tab and the history dialog.
void MainWindow::newHistoryTab() {
    newTab();
    openHistoryDialog();
}


// --- OPEN HISTORY DIALOG ---
// Open the history dialogue.
void MainWindow::openHistoryDialog() {
    QString url;
    HistoryDialog dialog(&url);
    int r = dialog.exec();
    if (r == QDialog::Accepted) {
        qDebug() << "Accepted.";
        // open URL in the current tab, if URL string is non-empty
        if (url.isEmpty()) { return; }
        qDebug() << "Load URL...";
        gotoURL(url);
    }
}


// --- CLOSE EVENT ---
// Reeimplements the close event function of QWidget to save state.
void MainWindow::closeEvent(QCloseEvent* event)  {
     settings->setValue("geometry", saveGeometry());
     settings->setValue("windowState", saveState());
     settings->setValue("bookmarkSidebar", ui->bookmarksTree->isVisible());
     QMainWindow::closeEvent(event);
 }
