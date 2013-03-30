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

BookDock::BookDock(const QString& path, const QString& indexPage, QWidget* parent)
    : QDockWidget(parent)
    , m_currentPage(indexPage)
    , m_path(path)
{
    QWidget* widget = new QWidget(this);
    QVBoxLayout* bookLayout = new QVBoxLayout;

    m_browser = new QTextBrowser(this);
    m_browser->setLineWrapMode(QTextEdit::NoWrap);
    m_browser->setSearchPaths(QStringList() << m_path);

    connect(m_browser, SIGNAL(anchorClicked(const QUrl&)), SLOT(handleAnchorClick(const QUrl&)));

    bookLayout->addWidget(m_browser);
    widget->setLayout(bookLayout);
    setWidget(widget);

    retranslateText();
}

void BookDock::handleAnchorClick(const QUrl& url)
{
    if (url.toString().startsWith("file:#")) {
        // Avoid appended history garbage after clicking on a formula (unknown).
        m_browser->backward();
        m_browser->forward();
        QString expression = url.toString().mid(6);
        emit expressionSelected(expression);
    } else
        openPage(url);
}

void BookDock::openPage(const QUrl& url)
{
    m_browser->setSource(url);
    m_currentPage = QFileInfo(url.path()).fileName();
}

void BookDock::retranslateText()
{
    setWindowTitle(tr("Formula Book"));

    QString m_language = Settings::instance()->language;
    QString locale = (m_language == "C") ? QLocale().name() : m_language;

    if (locale == "C")
        locale = "en";

    m_language = locale;
    QString fullPath = m_path + m_language + "/";
    QString src = fullPath + m_currentPage;

    if (!QDir(fullPath).isReadable()) {
        QString localeShort = locale.left(2).toLower();
        src = m_path + localeShort + "/" + m_currentPage;
        if (!QDir(m_path + localeShort).isReadable())
            src = m_path + "en" + "/" + m_currentPage;
    }

    m_browser->setSource(QUrl::fromLocalFile(src));
}

void BookDock::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateText();
    else
        QDockWidget::changeEvent(event);
}