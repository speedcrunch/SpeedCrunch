// This file is part of the SpeedCrunch project
// Copyright (C) 2009 Andreas Scherer <andreas_coder@freenet.de>
// Copyright (C) 2009, 2011, 2013 Helder Correia <helder.pereira.correia@gmail.com>
// Copyright (C) 2012 Roger Lamb <rlamb1id@gmail.com>
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

#include "gui/userfunctionlistwidget.h"

#include "core/evaluator.h"
#include "core/settings.h"

#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>


UserFunctionListWidget::UserFunctionListWidget(QWidget* parent)
    : QWidget(parent)
    , m_filterTimer(new QTimer(this))
    , m_userFunctions(new QTreeWidget(this))
    , m_noMatchLabel(new QLabel(m_userFunctions))
    , m_searchFilter(new QLineEdit(this))
    , m_searchLabel(new QLabel(this))
{
    m_filterTimer->setInterval(500);
    m_filterTimer->setSingleShot(true);

    m_userFunctions->setAlternatingRowColors(true);
    m_userFunctions->setAutoScroll(true);
    m_userFunctions->setColumnCount(2);
    m_userFunctions->setEditTriggers(QTreeWidget::NoEditTriggers);
    m_userFunctions->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_userFunctions->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_userFunctions->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_userFunctions->setRootIsDecorated(false);
    m_userFunctions->setSelectionBehavior(QTreeWidget::SelectRows);
    m_userFunctions->setCursor(QCursor(Qt::PointingHandCursor));

    m_noMatchLabel->setAlignment(Qt::AlignCenter);
    m_noMatchLabel->adjustSize();
    m_noMatchLabel->hide();

    QWidget* searchBox = new QWidget(this);
    QHBoxLayout* searchLayout = new QHBoxLayout;
    searchLayout->addWidget(m_searchLabel);
    searchLayout->addWidget(m_searchFilter);
    searchLayout->setMargin(0);
    searchBox->setLayout(searchLayout);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(3);
    layout->addWidget(searchBox);
    layout->addWidget(m_userFunctions);
    setLayout(layout);

    QMenu* contextMenu = new QMenu(m_userFunctions);
    m_insertAction = new QAction("", contextMenu);
    m_deleteAction = new QAction("", contextMenu);
    m_deleteAllAction = new QAction("", contextMenu);
    m_userFunctions->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_userFunctions->addAction(m_insertAction);
    m_userFunctions->addAction(m_deleteAction);
    m_userFunctions->addAction(m_deleteAllAction);

    QWidget::setTabOrder(m_searchFilter, m_userFunctions);

    retranslateText();

    connect(m_filterTimer, SIGNAL(timeout()), SLOT(fillTable()));
    connect(m_searchFilter, SIGNAL(textChanged(const QString&)), SLOT(triggerFilter()));
    connect(m_userFunctions, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(activateItem()));
    connect(m_insertAction, SIGNAL(triggered()), SLOT(activateItem()));
    connect(m_deleteAction, SIGNAL(triggered()), SLOT(deleteItem()));
    connect(m_deleteAllAction, SIGNAL(triggered()), SLOT(deleteAllItems()));
}

UserFunctionListWidget::~UserFunctionListWidget()
{
    m_filterTimer->stop();
}

void UserFunctionListWidget::fillTable()
{
    setUpdatesEnabled(false);

    m_filterTimer->stop();
    m_userFunctions->clear();
    QString term = m_searchFilter->text();
    QList<Evaluator::UserFunctionDescr> userFunctions = Evaluator::instance()->getUserFunctions();

    for (int i = 0; i < userFunctions.count(); ++i) {
        QString fname = userFunctions.at(i).name + "(" + userFunctions.at(i).arguments.join(";")  + ")";

        QStringList namesAndValues;
        namesAndValues << fname << userFunctions.at(i).expression;

        if (term.isEmpty()
            || namesAndValues.at(0).contains(term, Qt::CaseInsensitive)
            || namesAndValues.at(1).contains(term, Qt::CaseInsensitive))
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(m_userFunctions, namesAndValues);
            item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            item->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
        }
    }

    m_userFunctions->resizeColumnToContents(0);
    m_userFunctions->resizeColumnToContents(1);

    if (m_userFunctions->topLevelItemCount() > 0) {
        m_noMatchLabel->hide();
        m_userFunctions->sortItems(0, Qt::AscendingOrder);
    } else {
        m_noMatchLabel->setGeometry(m_userFunctions->geometry());
        m_noMatchLabel->show();
        m_noMatchLabel->raise();
    }

    m_searchFilter->setFocus();
    setUpdatesEnabled(true);
}

void UserFunctionListWidget::retranslateText()
{
    QStringList titles;
    titles << tr("Name") << tr("Value");
    m_userFunctions->setHeaderLabels(titles);

    m_searchLabel->setText(tr("Search"));
    m_noMatchLabel->setText(tr("No match found"));

    m_insertAction->setText(tr("Insert"));
    m_deleteAction->setText(tr("Delete"));
    m_deleteAllAction->setText(tr("Delete All"));

    QTimer::singleShot(0, this, SLOT(fillTable()));
}

QTreeWidgetItem* UserFunctionListWidget::currentItem() const
{
    return m_userFunctions->currentItem();
}

QString UserFunctionListWidget::getUserFunctionName(const QTreeWidgetItem *item)
{
    return item->text(0).section("(", 0, 0);
}

void UserFunctionListWidget::activateItem()
{
    if (!currentItem() || m_userFunctions->selectedItems().isEmpty())
        return;
    emit itemActivated(currentItem()->text(0));
}

void UserFunctionListWidget::deleteItem()
{
    if (!currentItem() || m_userFunctions->selectedItems().isEmpty())
        return;
    Evaluator::instance()->unsetUserFunction(getUserFunctionName(currentItem()));
    fillTable();
}

void UserFunctionListWidget::deleteAllItems()
{
    Evaluator::instance()->unsetAllUserFunctions();
    fillTable();
}

void UserFunctionListWidget::triggerFilter()
{
    m_filterTimer->stop();
    m_filterTimer->start();
}

void UserFunctionListWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        setLayoutDirection(Qt::LeftToRight);
        retranslateText();
        return;
    }
    QWidget::changeEvent(event);
}

void UserFunctionListWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        Evaluator::instance()->unsetUserFunction(getUserFunctionName(currentItem()));
        fillTable();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}
