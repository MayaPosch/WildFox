#include "historydialog.h"
#include "ui_historydialog.h"

HistoryDialog::HistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistoryDialog)
{
    ui->setupUi(this);
}

HistoryDialog::~HistoryDialog()
{
    delete ui;
}
