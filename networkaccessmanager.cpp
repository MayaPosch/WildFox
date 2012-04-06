/*
  networkaccessmanager.cpp - Network access manager implementation file of WildFox-Mimic.
  
  Revision 0
  
  Features:
        - Implements URL filter to add content scripts to a page.
        - TODO: add adblocker support?
        
  Notes:
        - 
        
  2012/02/21, Maya Posch
  (c) Nyanko.ws
*/

#include "networkaccessmanager.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringList>



// --- SET FILTERS ---
// Sets the filters which activate a particular extension's content scripts
void NetworkAccessManager::setFilters(QList<Filter> &filters) {
    this->filters = filters;
}


// --- GET FILTERS ---
// Return the currently set filters.
QList<Filter>* NetworkAccessManager::getFilters() {
    return &filters;
}


// --- CREATE REQUEST ---
// Extends the default function. Does comparison on the current GET URL.
// Injects content script into page if GET request & match is found.
QNetworkReply* NetworkAccessManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice* outgoingData) {
    if (op == QNetworkAccessManager::GetOperation) {
        if (filters.size() < 1) {
            return QNetworkAccessManager::createRequest(op, request);
        }
        
        QNetworkReply* reply = QNetworkAccessManager::createRequest(op, request);
        if (reply->error() == QNetworkReply::NoError) {
            return reply;
        }
        
        QString url = request.url().toString();
        QStringList urlbit;
        urlbit = url.split("://");
        if (urlbit.size() > 1) {
            QString scheme = urlbit[0];
            QStringList bits = urlbit[1].split(".");
            for (int i = 0; i < filters.size(); ++i) {
                if (filters[i].scheme != scheme) { continue; }
                for (int j = 0; filters[i].segments.size(); ++j) {
                    if (filters[i].segments[j] != bits[j] && filters[i].segments[j] != "*") {
                        continue;
                    }
                }
                
                // matched filter, inject associated scripts into the content.
            }
        }
    }
}
