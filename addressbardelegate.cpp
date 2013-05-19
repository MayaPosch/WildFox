/*
  addressbardelegate.cpp - Address Bar Delegate implementation file of WildFox.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - 
        
  2013/04/28, Maya Posch
  (c) Nyanko.ws
*/


#include "addressbardelegate.h"

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QDebug>


// --- CONSTRUCTOR ---
AddressBarDelegate::AddressBarDelegate(QObject *parent) :
    QStyledItemDelegate(parent) {
    //
}


// --- PAINT ---
// Paints the view.
// Takes a rich text format string and inserts it into a QTextDocument. Then uses the
// drawContents function of the QTextDocument instance to paint on the provided rectangle
// in option using the provided painter.
void AddressBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, 
                               const QModelIndex &index) const {
    qDebug() << "Custom delegate called...";
    qDebug() << "Index text: " << index.data().toString();
    
    painter->save();
    QTextDocument doc;
    doc.setHtml(index.data().toString());
    QAbstractTextDocumentLayout::PaintContext context;
    doc.setPageSize(option.rect.size());
    painter->translate(option.rect.x(), option.rect.y());
    doc.documentLayout()->draw(painter, context);
    painter->restore();
}
