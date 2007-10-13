//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "colormodel.h"



ColorModel::ColorModel(QObject *parent) : QStandardItemModel(parent) {
    setColumnCount(1);
}

QModelIndex ColorModel::contains(const QColor &c) {
    QVariant tst(c);
    QModelIndexList lst = match(index(0,0), Qt::DecorationRole, tst, 1, Qt::MatchExactly);
    return lst.isEmpty() ? QModelIndex() : lst.first();
}
ColorModel* ColorModel::clone(QObject *par) const {
    ColorModel *model = new ColorModel(par);
    int cnt = rowCount();
    for (int i=0;i<cnt;i++) {
        model->insertRow(i);
        QMap<int, QVariant> oldData = itemData(index(i, 0));
        model->setItemData(model->index(i, 0), oldData);
    }
    return model;
}
QModelIndex ColorModel::addColor(const QColor &c, const QString &name) {
    QStandardItem *item = new QStandardItem;
    item->setText(name);
    item->setData(c, Qt::DecorationRole);
    item->setData(name, Qt::ToolTipRole);
    appendRow(item);
    return item->index();
}

QModelIndex ColorModel::insertColor(int index, const QColor & c, const QString & name)
{
    QStandardItem *item = new QStandardItem;
    item->setText(name);
    item->setData(c, Qt::DecorationRole);
    insertRow(index, item);
    return item->index();
}



