// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008 Helder Correia <helder.pereira.correia@gmail.com>
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

#include <gui/tipwidget.h>

#include <QtCore/QTimer>
#include <QtCore/QTimeLine>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QToolTip>

struct TipWidget::Private
{
    QLabel *tipLabel;
    QTimer *hideTimer;
    QTimeLine *fadeTimeLine;
};

TipWidget::TipWidget(QWidget *parent) :
    QFrame(parent), d(new TipWidget::Private)
{
    setObjectName("TipWidget");

    d->tipLabel = new QLabel(this);
    d->tipLabel->setWordWrap(true);

    d->hideTimer = new QTimer(this);
    d->hideTimer->setInterval(4000);
    d->hideTimer->setSingleShot( true );
    connect(d->hideTimer, SIGNAL(timeout()), SLOT(disappear()));

    d->fadeTimeLine = new QTimeLine(750, this);
    d->fadeTimeLine->setFrameRange(0, 100);
    d->fadeTimeLine->setCurveShape(QTimeLine::EaseInCurve);
    connect(d->fadeTimeLine, SIGNAL(frameChanged(int)), this, SLOT(animateFade(int)));

    setPalette(QToolTip::palette());

    setFrameShape(QFrame::Box);
    setAutoFillBackground(true);

    setFixedWidth(fontMetrics().width('X') * 50);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void TipWidget::showText(const QString &text, const QString &title)
{
    setPalette(QToolTip::palette());

    QString tip = QString("<b>%1</b><br>%2").arg(title).arg(text);
    d->tipLabel->setText(tip);

    const int spacing = 4;
    const int margin = 10;

    d->tipLabel->adjustSize();
    d->tipLabel->move(margin, 0);
    d->tipLabel->resize((width() - spacing - margin), d->tipLabel->sizeHint().height());

    QTimer::singleShot(0, this, SLOT(appear()));
}

void TipWidget::hideText()
{
    if (d->fadeTimeLine->state() == QTimeLine::NotRunning) {
        d->hideTimer->stop();
        QTimer::singleShot(0, this, SLOT(disappear()));
    }
}

void TipWidget::appear()
{
    resize(width(), sizeHint().height());

    show();
    raise();

    d->fadeTimeLine->stop();
    d->fadeTimeLine->setFrameRange(0, height());
    animateFade(0);

    d->hideTimer->stop();
    d->hideTimer->start();
}

void TipWidget::disappear()
{
    d->fadeTimeLine->setFrameRange(0, height());
    d->fadeTimeLine->start();
}

void TipWidget::animateFade(int v)
{
    resize(width(), (d->fadeTimeLine->endFrame() - v));
    d->tipLabel->move(10, -v);

    QPalette palette = QToolTip::palette();
    QColor color1 = palette.window().color();
    QColor color2 = palette.windowText().color();
    int a = qMax(0, (240 - (v << 2)));
    color1.setAlpha(a);
    color2.setAlpha(a);
    palette.setBrush(QPalette::Window, color1);
    palette.setBrush(QPalette::WindowText, color2);
    setPalette(palette);
}

QSize TipWidget::sizeHint() const
{
    return d->tipLabel->sizeHint() + QSize(100, 0);
}
