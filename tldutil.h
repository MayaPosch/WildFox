/*
  tldutil.h - URL Top Level Domain utility definition file of WildFox-Mimic.
  
  Revision 0
  
  Notes:
        - 
        
  2012/02/07, Maya Posch
  (c) Nyanko.ws
*/

#ifndef TLDUTIL_H
#define TLDUTIL_H

#include <QtCore>

bool isEffectiveTLD(const QString &domain);
QString getPublicDomain(const QString &domain);
QString topLevelDomain(const QString &domain);

#endif // TLDUTIL_H
