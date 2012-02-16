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
    if (!isOpen()) { return; }
    
    write(reply->readAll());
    
    /*while (reply->bytesAvailable() > 0) {
        write(reply->read(0, 2048), 2048);
    }*/
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
