// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Petri Damstén <damu@iki.fi>
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

BookDock::BookDock(const QString &path, const QString &homePage, QWidget *parent)
    : QDockWidget(parent)
    , m_file(homePage)
    , m_homePage(homePage)
    , m_language(Settings::instance()->language)
    , m_path(path)
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *bookLayout = new QVBoxLayout;

    m_browser = new QTextBrowser(this);
    m_browser->setLineWrapMode(QTextEdit::NoWrap);
    m_browser->setSearchPaths(QStringList() << m_path);

    connect(m_browser, SIGNAL(anchorClicked(const QUrl &)),
            SLOT(handleAnchorClick(const QUrl &)));

    m_buttonLayoutWidget = new QWidget;
    m_buttonLayout = new QHBoxLayout(m_buttonLayoutWidget);
    m_buttonLayout->setSpacing(0);
    m_buttonLayout->setMargin(0);

    m_backButton = new QPushButton("", this);
    m_backButton->setIcon(QPixmap(":/book_back.png"));
    m_backButton->setFlat(true);

    connect(m_backButton, SIGNAL(clicked()), m_browser, SLOT(backward()));
    connect(m_browser, SIGNAL(backwardAvailable(bool)), m_backButton, SLOT(setEnabled(bool)));

    m_buttonLayout->addWidget(m_backButton);

    m_forwardButton = new QPushButton("", this);
    m_forwardButton->setIcon(QPixmap(":/book_forward.png"));
    m_forwardButton->setFlat(true);

    connect(m_forwardButton, SIGNAL(clicked()), m_browser, SLOT(forward()));
    connect(m_browser, SIGNAL(forwardAvailable(bool)), m_forwardButton, SLOT(setEnabled(bool)));

    m_buttonLayout->addWidget(m_forwardButton);

    m_indexButton = new QPushButton("", this);
    m_indexButton->setIcon(QPixmap(":/book_home.png"));
    m_indexButton->setFlat(true);

    connect(m_indexButton, SIGNAL(clicked()), SLOT(openHomePage()));

    m_buttonLayout->addWidget(m_indexButton);
    m_buttonLayout->addStretch();

    bookLayout->addWidget(m_buttonLayoutWidget);
    bookLayout->addWidget(m_browser);

    widget->setLayout(bookLayout);
    setWidget(widget);

    retranslateText();
}

void BookDock::handleAnchorClick(const QUrl &link)
{
    if (link.toString().startsWith("file:#")) {
        // Avoid appended history garbage after clicking on a formula (unknown).
        m_browser->backward();
        m_browser->forward();
        QString expression = link.toString().mid(6);
        emit expressionSelected(expression);
    } else {
        m_browser->setSource(link);
        m_file = QFileInfo(link.path()).fileName();
    }
    // Necessary for QTextBrowser to always draw correctly (why?).
    // Causes a bug on Window or maybe just about the version number.
    // m_browser->adjustSize();
}

void BookDock::openHomePage()
{
    m_browser->setSource(m_homePage);
    m_file = m_homePage;
}

void BookDock::retranslateText()
{
    setWindowTitle(tr("Formula Book"));

    // buttons
    m_backButton->setText(tr("Back"));
    m_forwardButton->setText(tr("Forward"));
    m_indexButton->setText(tr("Index"));

    // page
    m_language = Settings::instance()->language;
    QString locale = (m_language == "C") ? QLocale().name() : m_language;

    // fallback to English
    if (locale == "C")
        locale = "en";

    m_language = locale;
    QString fullPath = m_path + m_language + "/";
    QString src = fullPath + m_file;

    if (!QDir(fullPath).isReadable()) {
        QString localeShort = locale.left(2).toLower();
        src = m_path + localeShort + "/" + m_file;
        if (!QDir(m_path + localeShort).isReadable())
            src = m_path + "en" + "/" + m_file;
    }

    m_browser->setSource(QUrl::fromLocalFile(src));
    handleLayoutDirection();
}

void BookDock::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateText();
    else if (event->type() == QEvent::LayoutDirectionChange)
        handleLayoutDirection();
    else
        QDockWidget::changeEvent(event);
}

void BookDock::handleLayoutDirection()
{
    if (layoutDirection() == Qt::RightToLeft) {
        m_backButton->setIcon(QPixmap(":/book_forward.png"));
        m_forwardButton->setIcon(QPixmap(":/book_back.png"));
    } else {
        m_backButton->setIcon(QPixmap(":/book_back.png"));
        m_forwardButton->setIcon(QPixmap(":/book_forward.png"));
    }
}

