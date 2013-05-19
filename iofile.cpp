/*
  iofile.cpp - IO File implementation file of WildFox-Mimic.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - 
        
  2011/11/22, Maya Posch
  (c) Nyanko.ws
*/

#include "iofile.h"

#include <QDebug>


// --- SET NETWORK REPLY ---
// Set the QNetworkReply instance to use.
void IOFile::setNetworkReply(QNetworkReply* reply) {
    this->reply = reply;
}


// --- READ PROGRESS ---
// Connected to IODevice's progress indicator. Returns a percentage of total
// as a signal.
void IOFile::readProgress(qint64 current, qint64 total) {
    int percentage = ((double) current) / ((double) total) * 100;
    emit progress(percentage);
}


// --- READ FROM REPLY ---
// Called when the QNetworkReply has new data available.
// Read new data from the QNetworkReply instance and write it to the open file.
void IOFile::readFromReply() {
    qDebug() << "Reading from reply...";
    if (!isOpen()) { return; }
    
    qDebug() << "Reading...";
    qDebug() << "Bytes:" << reply->bytesAvailable();
    write(reply->readAll());
    
    /*char* bytes = new char[4096];
    qint64 count = reply->read(bytes, 4096);
    if (count == 0) {
        qDebug() << "No more data available.";
        return;
    }
    else if (count < 0) {
        qDebug() << "An error occurred on the Reply socket.";
        return;
    }
    
    qDebug() << "Writing...";
    //while (reply->bytesAvailable() > 0) {
        write(bytes, count);
   // }*/
}


// --- CANCEL ---
// Cancel the current transfer and clean up resources.
void IOFile::cancel() {
    reply->abort();
    reply->close();
    terminate();
}


// --- TERMINATE ---
// Shut down, clean up resources.
void IOFile::terminate() {
    if (isOpen()) {
        close();
    }
    
    deleteLater();
}
