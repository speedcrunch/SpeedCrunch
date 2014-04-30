// This file is part of the SpeedCrunch project
// Copyright (C) 2014 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "gui/manualwindow.h"

#include "core/manual.h"

#include <QtCore/QEvent>

ManualWindow::ManualWindow(QWidget* parent)
    : QTextBrowser(parent)
    , m_manual(new Manual(this))
{
    setLineWrapMode(QTextEdit::NoWrap);
    retranslateText();
    openPage(QUrl("index"));
}

void ManualWindow::openPage(const QUrl& url)
{
    QString content = m_manual->getPageContent(url.toString());
    if (!content.isNull())
        setHtml(content);
}

void ManualWindow::retranslateText()
{
    setWindowTitle(tr("User Manual"));
    QString content = m_manual->getCurrentPageContent();
    if (!content.isNull())
        setHtml(content);
}
void ManualWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateText();
    else
        QTextBrowser::changeEvent(event);
}

void ManualWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed();
    QTextBrowser::closeEvent(event);
}