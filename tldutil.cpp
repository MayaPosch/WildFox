/*
  tldutil.cpp - URL Top Level Domain utility implementation file of WildFox-Mimic.
  
  Revision 0
  
  Features:
        - Allows one to check whether a provided domain is a public suffix,
            i.e. not a .com, .co.uk or similar non-public domain.
        
  Notes:
        - Modified from the original Qt implementation.
        
  2012/02/07, Maya Posch
  (c) Nyanko.ws
*/


#include "tldutil.h"
#include "qurltlds_p.h"

#include <QtCore>


// --- CONTAINS TLD ENTRY ---
// Returns true if the entry has been found in the TLD list.
static bool containsTLDEntry(const QString &entry) {
    int index = qHash(entry) % tldCount;
    int currentDomainIndex = tldIndices[index];
    while (currentDomainIndex < tldIndices[index+1]) {
        QString currentEntry = QString::fromUtf8(tldData + currentDomainIndex);
        if (currentEntry == entry)
            return true;
        currentDomainIndex += qstrlen(tldData + currentDomainIndex) + 1; // +1 for the ending \0
    }
    return false;
}


// --- IS EFFECTIVE TLD ---
// Return true if provided domain is a top-level-domain per the Mozilla 
// public suffix list.
bool isEffectiveTLD(const QString &domain) {
    // for domain 'foo.bar.com':
    // 1. return if TLD table contains 'foo.bar.com'
    if (containsTLDEntry(domain))
        return true;

    if (domain.contains(QLatin1Char('.'))) {
        int count = domain.size() - domain.indexOf(QLatin1Char('.'));
        QString wildCardDomain;
        wildCardDomain.reserve(count + 1);
        wildCardDomain.append(QLatin1Char('*'));
        wildCardDomain.append(domain.right(count));
        // 2. if table contains '*.bar.com',
        // test if table contains '!foo.bar.com'
        if (containsTLDEntry(wildCardDomain)) {
            QString exceptionDomain;
            exceptionDomain.reserve(domain.size() + 1);
            exceptionDomain.append(QLatin1Char('!'));
            exceptionDomain.append(domain);
            return (! containsTLDEntry(exceptionDomain));
        }
    }
    return false;
}


// --- GET PUBLIC DOMAIN ---
// Reduce a URL until only only the non-public part remains (e.g. .com and .co.uk)
// Return it and the first public section preceding it.
QString getPublicDomain(const QString &domain) {
    QStringList sections = domain.toLower().split(QLatin1Char('.'), QString::SkipEmptyParts);
    if (sections.isEmpty())
        return QString();
    
    QString tld = "";
    for (int i = sections.count() - 1; i >= 0; --i) {
        tld.prepend(QLatin1Char('.') + sections.at(i));
        if (!isEffectiveTLD(tld.right(tld.size() - 1))) {
            tld = '.' + sections.at(i - 1) + tld;
            return tld;
        }
    }
    
    return tld;
}


// --- TOP LEVEL DOMAIN ---
QString topLevelDomain(const QString &domain) {
    QStringList sections = domain.toLower().split(QLatin1Char('.'), QString::SkipEmptyParts);
    if (sections.isEmpty())
        return QString();

    QString level, tld;
    for (int j = sections.count() - 1; j >= 0; --j) {
        level.prepend(QLatin1Char('.') + sections.at(j));
        if (isEffectiveTLD(level.right(level.size() - 1)))
            tld = level;
    }
    return tld;
}
