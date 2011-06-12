// This file is part of the SpeedCrunch project
// Copyright (C) 2009 Andreas Scherer <andreas_coder@freenet.de>
// Copyright (C) 2009, 2011 Helder Correia <helder.pereira.correia@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "gui/functionswidget.h"

#include "core/functions.h"
#include "core/settings.h"

#include <QtCore/QEvent>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

FunctionsWidget::FunctionsWidget(QWidget *parent)
    : QWidget(parent)
    , m_filterTimer(new QTimer(this))
    , m_functions(new QTreeWidget(this))
    , m_noMatchLabel(new QLabel(m_functions))
    , m_searchFilter(new QLineEdit(this))
    , m_searchLabel(new QLabel(this))
{

    m_filterTimer->setInterval(500);
    m_filterTimer->setSingleShot(true);

    m_functions->setAutoScroll(true);
    m_functions->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded);
    m_functions->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_functions->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_functions->setColumnCount(2);
    m_functions->setAlternatingRowColors(true);
    m_functions->setRootIsDecorated(false);
    m_functions->setEditTriggers(QTreeWidget::NoEditTriggers);
    m_functions->setSelectionBehavior(QTreeWidget::SelectRows);
    m_functions->setCursor(QCursor(Qt::PointingHandCursor));

    m_noMatchLabel->setAlignment(Qt::AlignCenter);
    m_noMatchLabel->adjustSize();
    m_noMatchLabel->hide();

    QWidget *searchBox = new QWidget(this);
    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchLayout->addWidget(m_searchLabel);
    searchLayout->addWidget(m_searchFilter);
    searchLayout->setMargin(0);
    searchBox->setLayout(searchLayout);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(3);
    layout->addWidget(searchBox);
    layout->addWidget(m_functions);
    setLayout(layout);

    retranslateText();

    connect(m_filterTimer, SIGNAL(timeout()), SLOT(fillTable()));
    connect(m_functions, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
            SLOT(handleItemActivated(QTreeWidgetItem *, int)));
    connect(m_searchFilter, SIGNAL(textChanged(const QString &)), SLOT(triggerFilter()));
}

FunctionsWidget::~FunctionsWidget()
{
    m_filterTimer->stop();
}

void FunctionsWidget::fillTable()
{
    setUpdatesEnabled(false);

    m_filterTimer->stop();
    m_functions->clear();
    QString term = m_searchFilter->text();
    QStringList functionNames = Functions::instance()->names();

    for (int k = 0; k < functionNames.count(); ++k)
    {
        Function *f = Functions::instance()->function(functionNames.at(k));
        if (!f)
            continue;

        QStringList str;
        str << f->identifier() << f->name();

        if (term.isEmpty()
            || str.at(0).contains(term, Qt::CaseInsensitive)
            || str.at(1).contains(term, Qt::CaseInsensitive))
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_functions, str);
            if (layoutDirection() == Qt::LeftToRight) {
                item->setTextAlignment(0, Qt::AlignLeft);
                item->setTextAlignment(1, Qt::AlignLeft);
            } else {
                item->setTextAlignment(0, Qt::AlignRight);
                item->setTextAlignment(1, Qt::AlignLeft);
            }
        }
    }

    m_functions->resizeColumnToContents(0);
    m_functions->resizeColumnToContents(1);

    if (m_functions->topLevelItemCount() > 0) {
        m_noMatchLabel->hide();
        m_functions->sortItems(0, Qt::AscendingOrder);
    } else {
        m_noMatchLabel->setGeometry(m_functions->geometry());
        m_noMatchLabel->show();
        m_noMatchLabel->raise();
    }

  m_searchFilter->setFocus();
  setUpdatesEnabled(true);
}

void FunctionsWidget::retranslateText()
{
    QStringList titles;
    const QString identifier = tr("Identifier");
    const QString name = tr("Name");
    titles << identifier << name;
    m_functions->setHeaderLabels(titles);

    m_searchLabel->setText(tr("Search"));
    m_noMatchLabel->setText(tr("No match found"));

    fillTable();
}

QList<QTreeWidgetItem *> FunctionsWidget::selectedItems() const
{
    return m_functions->selectedItems();
}

const QTreeWidgetItem *FunctionsWidget::currentItem() const
{
    return m_functions->currentItem();
}

void FunctionsWidget::handleItemActivated(QTreeWidgetItem *item, int /*column*/)
{
    emit functionSelected(item->text(0));
}

void FunctionsWidget::clearSelection(QTreeWidgetItem * /*item*/)
{
    m_functions->clearSelection();
}

void FunctionsWidget::triggerFilter()
{
    m_filterTimer->stop();
    m_filterTimer->start();
}

void FunctionsWidget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        Functions::instance()->retranslateText();
        retranslateText();
    }
    else
        QWidget::changeEvent(e);
}

