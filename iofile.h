/*
  iofile.h - IO File definition file of WildFox-Mimic.
  
  Revision 0
  
  Notes:
        - This class allows one to directly couple a network download to a
            QFile.
        
  2011/11/22, Maya Posch
  (c) Nyanko.ws
*/

#ifndef IOFILE_H
#define IOFILE_H

#include <QFile>
#include <QtNetwork/QNetworkReply>

class IOFile : public QFile {
    Q_OBJECT
    
    // we add a new function to assign a QNetworkReply instance and a new slot
    // to respond to newly available data.
public:
    void setNetworkReply(QNetworkReply* reply);
    
public slots:
    void readProgress(qint64 current, qint64 total);
    void readFromReply();
    void cancel();
    void terminate();
    
signals:
    void progress(int percentage);
    
private:
    QNetworkReply* reply;
};

#endif // IOFILE_H
