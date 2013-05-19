/*
  historydialog.h - Declaration of HistoryDialog class.
  
  Revision 0
  
  Notes:
        - 
        
  2013/05/16, Maya Posch
  (c) Nyanko.ws
*/


#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>

#include "historydatabase.h"

namespace Ui {
    class HistoryDialog;
}

class HistoryDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit HistoryDialog(QString* url, QWidget *parent = 0);
    ~HistoryDialog();
    
private slots:
    void editingTriggered(QString text);
    void accepted();
    void itemActivated(QListWidgetItem* item);
    
private:
    Ui::HistoryDialog* ui;
    QString* url;
    HistoryDatabase* historydb; // the history database
    QList<HistoryObject> objects;   // Objects being shown in address bar popup.
};

#endif // HISTORYDIALOG_H
