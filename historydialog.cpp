/*
  historydialog.cpp - Definition of HistoryDialog class.
  
  Revision 0
  
  Features:
        - 
  
  Notes:
        - 
        
  2013/05/16, Maya Posch
  (c) Nyanko.ws
*/


#include <QDebug>

#include "historydialog.h"
#include "ui_historydialog.h"


// --- CONSTRUCTOR ---
HistoryDialog::HistoryDialog(QString *url, QWidget *parent) : QDialog(parent),
    ui(new Ui::HistoryDialog) {
    ui->setupUi(this);
    
    historydb = HistoryDatabase::getInstance();
    this->url = url;
    
    // connections
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->addressBar, SIGNAL(textEdited(QString)), 
            this, SLOT(editingTriggered(QString)));
    connect(ui->historyList, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(itemActivated(QListWidgetItem*)));
}


// --- DECONSTRUCTOR ---
HistoryDialog::~HistoryDialog() {
    delete ui;
}


// --- EDITING TRIGGERED ---
// When the contents of the address bar are being changed by the user this slot
// gets called. Use the address bar contents to search for matches in the history
// database and present them in the history list (QLineWidget).
void HistoryDialog::editingTriggered(QString text) {
    if (text.size() < 3) { return; }
    QString like = text;
    objects.clear();
    historydb->historyObjectsMatching(like, 25, objects);
    ui->historyList->clear();
    
    int objects_size = objects.size();
    qDebug() << "objects: " << objects_size;
    QListWidgetItem* item = 0;
    for (int i = 0; i < objects_size; ++i) {
        //qDebug() << "i: " << i;
        //qDebug() << "qcombobox count: " << ui->addressBar->count();
        //qDebug() << "url: " << objects[i].url;
        //qDebug() << "title: " << objects[i].title;
        item = new QListWidgetItem();
        item->setText(objects[i].title + " - " + objects[i].url);
        item->setData(Qt::UserRole, objects[i].url);
        ui->historyList->addItem(item);
    }
}


// --- ACCEPTED ---
// Ensures the current URL is saved, if any, before calling accept().
void HistoryDialog::accepted() {
    *url = ui->historyList->selectedItems()[0]->data(Qt::UserRole).toString();
    accept();
}


// --- ITEM ACTIVATED ---
// Called when an item in the history list gets activated.
void HistoryDialog::itemActivated(QListWidgetItem* item) {
    *url = item->data(Qt::UserRole).toString();
    accept();
}
