/*
  optionsdialog.h - Options Dialog definition file of WildFox-Mimic.
  
  Revision 0
  
  Notes:
        - 
        
  2012/02/02, Maya Posch
  (c) Nyanko.ws
*/


#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include "ui_optionsdialog.h"
#include <QDialog>
#include <QSettings>
#include <QSqlDatabase>

#include "cookiejar.h"

class OptionsDialog : public QDialog {
    Q_OBJECT
    
public:
    OptionsDialog(CookieJar* cookiejar);
    ~OptionsDialog();
    
public slots:
    void manageCookies();
    void saveSettings();
    void cancel();
    
private:
    Ui::OptionsDialog dialog;
    QSettings settings;
    CookieJar* cookies;
};

#endif // OPTIONSDIALOG_H
