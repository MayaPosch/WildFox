/*
  cookiemanagedialog.h - Cookie Manage dialog definition file of WildFox-Mimic.
  
  Revision 0
  
  Notes:
        - 
        
  2012/02/09, Maya Posch
  (c) Nyanko.ws
*/


#ifndef COOKIEMANAGEDIALOG_H
#define COOKIEMANAGEDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QMultiMap>
#include <QNetworkCookie>
#include <QDateTime>


struct CookieData {
    QString name;
    QString value;
    QString host; 
    QString path;
    QDateTime expiry;
    bool isSecure;
    bool isHttpOnly;
};

namespace Ui {
class CookieManageDialog;
}

class CookieManageDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit CookieManageDialog(QSqlDatabase* db, QMultiMap<QString, QNetworkCookie>* buffer, QWidget *parent = 0);
    ~CookieManageDialog();
    
public slots:
    void showCookieDetails();
    void deleteCookie();
    void deleteAllCookies();
    void close();
    
private:
    Ui::CookieManageDialog* ui;
    
    QSqlDatabase* db;
    QMultiMap<QString, QNetworkCookie>* cookiebuffer;
    QString baseDomain;
    QList<CookieData> cookiedata;
};

#endif // COOKIEMANAGEDIALOG_H
