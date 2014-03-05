// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008, 2010, 2014 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "gui/autohidelabel.h"

#include <QtCore/QTimer>
#include <QtGui/QToolTip>

#define TIMER_INTERVAL 25
#define FADE_STEP 16

AutoHideLabel::AutoHideLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
    m_fadeTimer = new QTimer(this);
    m_fadeTimer->setInterval(TIMER_INTERVAL);
    connect(m_fadeTimer, SIGNAL(timeout()), SLOT(fade()));

    m_alpha = 255;

    setPalette(QToolTip::palette());
    setAutoFillBackground(true);
    setFrameShape(QFrame::Box);
}

void AutoHideLabel::showText(const QString& text)
{
    setText(text);
    adjustSize();
    show();
    raise();
    m_alpha = 5 * (1000 / TIMER_INTERVAL * FADE_STEP) + 300;
    m_fadeTimer->start();
    fade();
}

void AutoHideLabel::hideText()
{
    if (m_alpha > 255)
        m_alpha = 255;
}

void AutoHideLabel::fade()
{
    if (m_alpha <= 0) {
        m_fadeTimer->stop();
        hide();
    } else {
        m_alpha = qMax(0, m_alpha - FADE_STEP);
        int alpha = qMin(255, m_alpha);

        QPalette palette = QToolTip::palette();
        QColor color1 = palette.window().color();
        QColor color2 = palette.windowText().color();

        color1.setAlpha(alpha);
        color2.setAlpha(alpha);

        palette.setBrush(QPalette::Window, color1);
        palette.setBrush(QPalette::WindowText, color2);
        setPalette(palette);
    }
}