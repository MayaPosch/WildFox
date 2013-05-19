/*
  addressbareventfilter.cpp - addressBarEventFilter class implementation file of WildFox.

  Revision 0

  Features:
        - Implements an event filter for the QComboBox class.
        - Emits a signal when the combobox's popup is activated.

  Notes:
        -

  2013/04/25, Maya Posch
  (c) Nyanko.ws
*/


#include "addressbareventfilter.h"


// --- CONSTRUCTOR ---
addressBarEventFilter::addressBarEventFilter(QObject *parent) :
    QObject(parent) {
    //
}


// --- EVENT FILTER ---
// Reimplements virtual function to emit a signal when the popup is opened.
bool addressBarEventFilter::eventFilter(QObject *, QEvent *) {
    //
    
    return true;
}
