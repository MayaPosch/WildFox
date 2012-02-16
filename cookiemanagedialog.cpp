/*
  cookiemanagedialog.cpp - Cookie Manage dialog implementation file of WildFox-Mimic.
  
  Revision 0
  
  Features:
        - Allows checking of cookie details and deleting cookies.
        
  Notes:
        - 
        
  2012/02/09, Maya Posch
  (c) Nyanko.ws
*/


#include "cookiemanagedialog.h"
#include "ui_cookiemanagedialog.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>


#define FOLDER 1000
#define COOKIE 2000


// --- CONSTRUCTOR ---
CookieManageDialog::CookieManageDialog(QSqlDatabase* db, QMultiMap<QString, QNetworkCookie> *buffer, QWidget *parent) : QDialog(parent),
    ui(new Ui::CookieManageDialog) {
    ui->setupUi(this);
    
    // defaults
    ui->cmLabelName->clear();
    ui->cmLabelValue->clear();
    ui->cmLabelDomain->clear();
    ui->cmLabelSendFor->clear();
    ui->cmLabelPath->clear();
    ui->cmLabelExpires->clear();
    
    // read info from database
    this->db = db;
    this->cookiebuffer = buffer;
    
    QSqlQuery query("SELECT baseDomain, name, value, host, path, expiry, isSecure, isHttpOnly FROM moz_cookies ORDER BY baseDomain");
    query.exec();
    if (!query.isActive()) {
        qDebug() << "Failed to fetch cookies from DB: " << query.lastError().text();
        return;
    }
    else if (query.next()){
        // extract data and assemble into a Tree
        CookieData cd;
        baseDomain = "";
        //QList<QTreeWidgetItem*> items;
        QTreeWidgetItem* parent = 0;
        QTreeWidgetItem* item = 0;
        int cIndex = 0; // current index into cookiedata list
        do {
            // Check whether this top domain folder exists, create if not.
            if (query.value(0) != baseDomain) {
                baseDomain = query.value(0).toString();
                parent = new QTreeWidgetItem((QTreeWidget*) 0, QStringList(baseDomain), FOLDER);
                //items.append(parent);
                ui->cmTree->addTopLevelItem(parent);
            }
            
            QStringList list;
            list << query.value(3).toString() << query.value(1).toString();
            item = new QTreeWidgetItem(parent, list, COOKIE);
            item->setData(0, Qt::UserRole, QVariant(cIndex));
            parent->addChild(item);
            cd.expiry.setTime_t((uint) query.value(5).toULongLong());
            cd.name = query.value(1).toString();
            cd.value = query.value(2).toString();
            cd.host = query.value(3).toString();
            cd.path = query.value(4).toString();
            //cd.expiry = query.value(5);
            cd.isSecure = query.value(6).toBool();
            cd.isHttpOnly = query.value(7).toBool();
            
            // insert into list and increase index count
            cookiedata.append(cd);
            ++cIndex;
        } while (query.next());
    }
    
    // set up connections
    connect(ui->cmTree, SIGNAL(itemSelectionChanged()), this, SLOT(showCookieDetails()));
    connect(ui->cmButtonDelete, SIGNAL(pressed()), this, SLOT(deleteCookie()));
    connect(ui->cmButtonDeleteAll, SIGNAL(pressed()), this, SLOT(deleteAllCookies()));
    connect(ui->cmButtonClose, SIGNAL(pressed()), this, SLOT(close()));
}

// --- DECONSTRUCTOR ---
CookieManageDialog::~CookieManageDialog() {
    delete ui;
}


// --- SHOW COOKIE DETAILS ---
// Show the details of the currently selected cookie. We pick the first item
// in the list of selected items.
void CookieManageDialog::showCookieDetails() {
    QList<QTreeWidgetItem*> items = ui->cmTree->selectedItems();
    CookieData cd = cookiedata[items[0]->data(0, Qt::UserRole).toInt()];
    ui->cmLabelName->setText(cd.name);
    ui->cmLabelValue->setText(cd.value);
    ui->cmLabelDomain->setText(cd.host);
    ui->cmLabelPath->setText(cd.path);
    ui->cmLabelSendFor->setText(QString(cd.isHttpOnly?"HTTP":"HTTP/HTTPS"));
    ui->cmLabelExpires->setText(cd.expiry.toString(Qt::ISODate));
}


// --- DELETE COOKIE ---
// Delete the currently selected cookie(s). Just remove the tree children.
// FIXME: remove the parent if all children are deleted?
void CookieManageDialog::deleteCookie() {
    QList<QTreeWidgetItem*> items = ui->cmTree->selectedItems();
    for (int i = 0; i < items.size(); ++i) {
        if (items[i]->type() == COOKIE) {
            int cookieID = items[i]->data(0, Qt::UserRole).toInt();
            QTreeWidgetItem* parent = items[i]->parent();
            //int index = parent->indexOfChild(items[i]);
            parent->removeChild(items[i]);
            
            // remove the items from the database and cookiebuffer
            CookieData cookie = cookiedata[cookieID];
            QMap<QString, QNetworkCookie>::iterator cit;
            cit = cookiebuffer->find(baseDomain);
            if (cit != cookiebuffer->end()) {
                // delete in the buffer
                while (cit != cookiebuffer->end() && cit.key() == baseDomain) {
                    QNetworkCookie c = cit.value();
                    if (c.name() == cookie.name &&
                            c.domain() == cookie.host &&
                            c.path() == cookie.path) {
                        // same cookie, delete it
                        cookiebuffer->remove(cit.key(), cit.value());
                    }
                }
                
                cit++;
            }
            
            // delete in the database
            QSqlQuery query;
            query.exec("DELETE FROM moz_cookies WHERE baseDomain='" + baseDomain 
                       + "' AND path='" + cookie.path + "' AND name='" 
                       + cookie.name + "'");
            if (!query.isActive()) {
                qDebug() << "Failed to update database with query: " << query.lastQuery();
                continue;
            }
        }
    }
}


// --- DELETE ALL COOKIES ---
// Delete all cookies. Ask for confirmation.
void CookieManageDialog::deleteAllCookies() {
    int response = QMessageBox::question(this, tr("Delete Cookies"), tr("Do you want to delete all stored cookies?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (response == QMessageBox::Yes) {
        cookiebuffer->clear();
    }
    
    // delete in the database
    QSqlQuery query;
    query.exec("DELETE FROM moz_cookies");
    if (!query.isActive()) {
        qDebug() << "Failed to update database with query: " << query.lastQuery();
    }
}


// --- CLOSE ---
// Closes the dialogue.
void CookieManageDialog::close() {
    done(1);
}
