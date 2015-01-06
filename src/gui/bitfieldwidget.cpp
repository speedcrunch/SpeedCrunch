// This file is part of the SpeedCrunch project
// Copyright (C) 2014 Sébastien Szymanski <seb.szymanski@gmail.com>
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

#include "bitfieldwidget.h"

#include "math/hmath.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListIterator>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>

BitWidget::BitWidget(int bitPosition, QWidget* parent)
    : QLabel(parent),
    m_state(false)
{
    setFixedSize(SizePixels, SizePixels);

    HNumber number(HMath::raise(HNumber(2), bitPosition));
    setToolTip(QString("2<sup>%1</sup> = %2")
        .arg(bitPosition)
        .arg(HMath::format(number, 'd')));
}

void BitWidget::mouseReleaseEvent(QMouseEvent*)
{
    setState(!m_state);
    emit stateChanged(m_state);
}

void BitWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_state)
        painter.fillRect(event->rect(), Qt::SolidPattern);
    else
        painter.drawRect(event->rect());
}

BitFieldWidget::BitFieldWidget(QWidget* parent) :
    QWidget(parent)
{
    m_bitWidgets.reserve(NumberOfBits);
    for (int i = 0; i < NumberOfBits; ++i) {
        BitWidget* bitWidget = new BitWidget(i);
        connect(bitWidget, SIGNAL(stateChanged(bool)), this, SLOT(onBitChanged()));
        m_bitWidgets.append(bitWidget);
    }

    QGridLayout* fieldLayout = new QGridLayout;
    int bitOffset = 0;

    for (int column = 0; column < 17; ++column) {
        if ((column % 2) == 0) {
            if ((column % 4) != 0)
                continue;

            QLabel* topNumberLabel = new QLabel;
            QLabel* bottomNumberLabel = new QLabel;

            int topNumber = NumberOfBits - column * 2;
            int bottomNumber = topNumber - NumberOfBits / 2;

            if (column == 0) {
                --topNumber;
                --bottomNumber;
            }

            topNumberLabel->setText(QString("%1").arg(topNumber));
            bottomNumberLabel->setText(QString("%1").arg(bottomNumber));

            fieldLayout->addWidget(topNumberLabel, 0, column);
            fieldLayout->addWidget(bottomNumberLabel, 1, column);

        } else {
            QHBoxLayout* bottomLayout(new QHBoxLayout);
            QHBoxLayout* topLayout(new QHBoxLayout);

            for (int j = 0; j < 4; ++j) {
                const int topIndex = NumberOfBits - 1 - bitOffset * 4 - j;
                topLayout->addWidget(m_bitWidgets.at(topIndex));
                bottomLayout->addWidget(m_bitWidgets.at(topIndex - NumberOfBits / 2));
            }

            ++bitOffset;

            fieldLayout->addLayout(bottomLayout, 1, column, Qt::AlignCenter);
            fieldLayout->addLayout(topLayout, 0, column, Qt::AlignCenter);
        }
    }

    QPushButton* resetButton = new QPushButton("0");
    resetButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(resetButton, SIGNAL(clicked()), this, SLOT(resetBits()));

    QPushButton* invertButton = new QPushButton("~");
    invertButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(invertButton, SIGNAL(clicked()), this, SLOT(invertBits()));

    QPushButton* shiftLeftButton = new QPushButton("<<");
    shiftLeftButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(shiftLeftButton, SIGNAL(clicked()), this, SLOT(shiftBitsLeft()));

    QPushButton* shiftRightButton = new QPushButton(">>");
    shiftRightButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(shiftRightButton, SIGNAL(clicked()), this, SLOT(shiftBitsRight()));

    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(resetButton);
    buttonsLayout->addWidget(shiftLeftButton);

    QVBoxLayout* buttonsLayout2 = new QVBoxLayout;
    buttonsLayout2->addWidget(invertButton);
    buttonsLayout2->addWidget(shiftRightButton);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addLayout(fieldLayout);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addLayout(buttonsLayout2);
    mainLayout->addStretch();
}

void BitFieldWidget::wheelEvent(QWheelEvent* we)
{
    if (we->delta() > 0)
        shiftBitsLeft();
    else
        shiftBitsRight();
}

void BitFieldWidget::updateBits(const HNumber& number)
{
    QString binaryNumberString = HMath::format(number, 'b');
    QListIterator<BitWidget*> bitsIterator(m_bitWidgets);

    if (number.isZero() || !number.isInteger())
        binaryNumberString.clear();
    else if (number.isNegative())
        binaryNumberString.remove(0, 3); // Remove '-0b'.
    else
        binaryNumberString.remove(0, 2); // Remove '0b'.

    auto iterator = binaryNumberString.end();
    while (bitsIterator.hasNext()) {
        if (iterator != binaryNumberString.begin()) {
            --iterator;
            bitsIterator.next()->setState(*iterator == '1');
        } else
            bitsIterator.next()->setState(false);
    }
}

void BitFieldWidget::onBitChanged()
{
    QListIterator<BitWidget*> bitsIterator(m_bitWidgets);
    QString expression;

    while (bitsIterator.hasNext())
        expression.prepend(bitsIterator.next()->state() ? "1" : "0");

    expression.remove(QRegExp(QString("^0{,%1}").arg(NumberOfBits - 1)));
    expression.prepend("0b");

    emit bitsChanged(expression);
}

void BitFieldWidget::invertBits()
{
    foreach (BitWidget* w, m_bitWidgets)
        w->setState(!w->state());

    onBitChanged();
}

void BitFieldWidget::resetBits()
{
    foreach (BitWidget* w, m_bitWidgets)
        w->setState(false);

    onBitChanged();
}

void BitFieldWidget::shiftBitsLeft()
{
    auto it = m_bitWidgets.constEnd();
    auto itBegin = m_bitWidgets.constBegin();

    --it;
    while (it != itBegin) {
        (*it)->setState((*(it-1))->state());
        --it;
    }

    (*itBegin)->setState(false);
    onBitChanged();
}

void BitFieldWidget::shiftBitsRight()
{
    auto it = m_bitWidgets.constBegin();
    auto itEnd = m_bitWidgets.constEnd();

    --itEnd;
    while (it != itEnd) {
        (*it)->setState((*(it+1))->state());
        it++;
    }

    (*itEnd)->setState(false);
    onBitChanged();
}
