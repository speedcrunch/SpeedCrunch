// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008, 2009, 2010, 2011 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "gui/constantswidget.h"

#include "core/constants.h"
#include "core/settings.h"

#include <QEvent>
#include <QTimer>
#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <algorithm>

ConstantsWidget::ConstantsWidget(QWidget *parent)
  : QWidget(parent)
{
    m_categoryLabel = new QLabel(this);
    m_category = new QComboBox(this);
    m_category->setEditable(false);
    m_category->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    connect(m_category, SIGNAL(activated(int)), SLOT(filter()));

    QWidget *categoryBox = new QWidget(this);
    QHBoxLayout *categoryLayout = new QHBoxLayout;
    categoryBox->setLayout(categoryLayout);
    categoryLayout->addWidget(m_categoryLabel);
    categoryLayout->addWidget(m_category);
    categoryLayout->setMargin(0);

    m_label = new QLabel(this);

    m_filter = new QLineEdit(this);
    m_filter->setMinimumWidth(fontMetrics().width('X') * 10);

    connect(m_filter, SIGNAL(textChanged(const QString &)), SLOT(triggerFilter()));

    QWidget *searchBox = new QWidget(this);
    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchBox->setLayout(searchLayout);
    searchLayout->addWidget(m_label);
    searchLayout->addWidget(m_filter);
    searchLayout->setMargin(0);

    m_list = new QTreeWidget(this);
    m_list->setAutoScroll(true);
    m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_list->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_list->setColumnCount(3);
    m_list->setRootIsDecorated(false);
    m_list->setMouseTracking(true);
    m_list->setEditTriggers(QTreeWidget::NoEditTriggers);
    m_list->setSelectionBehavior(QTreeWidget::SelectRows);
    m_list->setAlternatingRowColors(true);
    m_list->setCursor(QCursor(Qt::PointingHandCursor));

    connect(m_list, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
            SLOT(handleItem(QTreeWidgetItem *)));

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    layout->setMargin(3);
    layout->addWidget(categoryBox);
    layout->addWidget(searchBox);
    layout->addWidget(m_list);

    m_filterTimer = new QTimer(this);
    m_filterTimer->setInterval(500);
    m_filterTimer->setSingleShot(true);
    connect(m_filterTimer, SIGNAL(timeout()), SLOT(filter()));

    m_noMatchLabel = new QLabel(this);
    m_noMatchLabel->setAlignment(Qt::AlignCenter);
    m_noMatchLabel->adjustSize();
    m_noMatchLabel->hide();

    retranslateText();
}

ConstantsWidget::~ConstantsWidget()
{
    m_filterTimer->stop();
}

void ConstantsWidget::handleRadixCharacterChange()
{
    updateList();
}

void ConstantsWidget::retranslateText()
{
    m_categoryLabel->setText(tr("Category"));
    m_label->setText(tr("Search"));
    m_noMatchLabel->setText(tr("No match found"));

    QStringList titles;
    const QString name = tr("Name");
    const QString value = tr("Value");
    const QString unit = tr("Unit");
    if (layoutDirection() == Qt::LeftToRight)
        titles << name << value << unit;
    else
        titles << name << unit << value;
    m_list->setHeaderLabels(titles);

    updateList();
}

void ConstantsWidget::filter()
{
    const QList<Constant> &clist = Constants::instance()->list();
    const char radixChar = Settings::instance()->radixCharacter();
    QString term = m_filter->text();

    m_filterTimer->stop();
    setUpdatesEnabled(false);

    QString chosenCategory = m_category->currentText();

    m_list->clear();
    for (int k = 0; k < clist.count(); ++k) {
        QStringList str;
        str << clist.at(k).name;

        QString radCh = (radixChar != '.') ?
            QString(clist.at(k).value).replace('.', radixChar)
            : clist.at(k).value;

        if (layoutDirection() == Qt::RightToLeft) {
            str << clist.at(k).unit + QChar(0x200e); // Unicode LRM
            str << radCh;
        } else {
            str << radCh;
            str << clist.at(k).unit;
        }

        str << clist.at(k).name.toUpper();
        str << QString("");

        bool include = (chosenCategory == tr("All")) ?
            true : (clist.at(k).category == chosenCategory);

        if (!include)
            continue;

        QTreeWidgetItem *item = 0;
        if (term.isEmpty())
            item = new QTreeWidgetItem(m_list, str);
        else
            if (str.at(0).contains(term, Qt::CaseInsensitive))
                item = new QTreeWidgetItem(m_list, str);
        if (item) {
            QString tip;
            tip += QString(QChar(0x200E));
            tip += QString("<b>%1</b><br>%2").arg( clist.at(k).name, clist.at(k).value);
            tip += QString(QChar(0x200E));
            if (!clist.at(k).unit.isEmpty())
                tip.append(" ").append(clist.at(k).unit);
            if (radixChar != '.')
                tip.replace('.', radixChar);
            tip += QString(QChar(0x200E));
            item->setToolTip(0, tip);
            item->setToolTip(1, tip);
            item->setToolTip(2, tip);

            if (layoutDirection() == Qt::RightToLeft) {
                item->setTextAlignment(1, Qt::AlignRight);
                item->setTextAlignment(2, Qt::AlignLeft);
            } else {
                item->setTextAlignment(1, Qt::AlignLeft);
                item->setTextAlignment(2, Qt::AlignLeft);
            }
        }
    }

    m_list->resizeColumnToContents(0);
    m_list->resizeColumnToContents(1);
    m_list->resizeColumnToContents(2);

    if (m_list->topLevelItemCount() > 0) {
        m_noMatchLabel->hide();
        m_list->sortItems(0, Qt::AscendingOrder);
    } else {
        m_noMatchLabel->setGeometry(m_list->geometry());
        m_noMatchLabel->show();
        m_noMatchLabel->raise();
    }

    setUpdatesEnabled(true);
}

void ConstantsWidget::handleItem(QTreeWidgetItem *item)
{
    const QList<Constant> & c = Constants::instance()->list();
    emit constantSelected(std::find_if(c.begin(), c.end(), constant_name_is(item->text(0)))->value);
}

void ConstantsWidget::triggerFilter()
{
    m_filterTimer->stop();
    m_filterTimer->start();
}

void ConstantsWidget::updateList()
{
    m_category->clear();
    Constants::instance()->retranslateText();
    m_category->addItems(Constants::instance()->categories());
    m_category->insertItem(0, tr("All"));
    m_category->setCurrentIndex(0);
    filter();
}

void ConstantsWidget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        Constants::instance()->retranslateText();
        retranslateText();
    }
    else
        QWidget::changeEvent(e);
}

