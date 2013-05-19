/*
  addressbarcompleter.cpp - implementation of the address bar completer class.
  
  Revision 0
  
  Features:
        - Reimplements the QCompleter class to allow more useful filtering.
        
  Notes:
        - 
        
  2013/05/18, Maya Posch
  (c) Nyanko.ws
*/


#include "addressbarcompleter.h"


// --- CONSTRUCTOR ---
AddressBarCompleter::AddressBarCompleter(QObject *parent) : QCompleter(parent) {
    //
}


// --- SET COMPLETION PREFIX ---
// Overrides the function in QCompleter. This reimplementation doesn't set the text
// from the line edit (address bar) as the prefix to search for, but 
