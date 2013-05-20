// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Petri Damstén <damu@iki.fi>
// Copyright (C) 2008-2013 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "gui/bookdock.h"

#include "core/book.h"
#include "core/settings.h"

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtCore/QFile>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>

BookDock::BookDock(QWidget* parent)
    : QDockWidget(parent)
    , m_book(new Book(this))
{
    QWidget* widget = new QWidget(this);
    QVBoxLayout* bookLayout = new QVBoxLayout;

    m_browser = new TextBrowser(this);
    m_browser->setLineWrapMode(QTextEdit::NoWrap);

    connect(m_browser, SIGNAL(anchorClicked(const QUrl&)), SLOT(handleAnchorClick(const QUrl&)));

    bookLayout->addWidget(m_browser);
    widget->setLayout(bookLayout);
    setWidget(widget);

    retranslateText();
    openPage(QUrl("index"));
}

void BookDock::handleAnchorClick(const QUrl& url)
{
    if (url.toString().startsWith("formula:")) {
        QString expression = url.toString().mid(8);
        emit expressionSelected(expression);
    } else
        openPage(url);
}

void BookDock::openPage(const QUrl& url)
{
    QString content = m_book->getPageContent(url.toString());
    if (!content.isNull())
        m_browser->setHtml(content);
}

void BookDock::retranslateText()
{
    setWindowTitle(tr("Formula Book"));
    QString content = m_book->getCurrentPageContent();
    if (!content.isNull())
        m_browser->setHtml(content);
}

void BookDock::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateText();
    else
        QDockWidget::changeEvent(event);
}
