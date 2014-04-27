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

#include "bitfieldwidget.h"

#include "math/hmath.h"

#include <QtCore/QListIterator>

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

BitWidget::BitWidget(int apos, QWidget *parent) :
    QLabel(parent),
    m_state(false)
{
    setMinimumSize(20, 20);

    HNumber num(HMath::raise(HNumber(2), apos));
    setToolTip(QString("Bit %1. Decimal value: %2").arg(apos).arg(HMath::format(num, 'd')));
}

BitWidget::~BitWidget()
{
}

bool BitWidget::state() const
{
    return m_state;
}

void BitWidget::setState(bool state)
{
    m_state = state;
    repaint();
}

void BitWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);

    setState(!m_state);
    emit stateChanged(m_state);
}

void BitWidget::paintEvent(QPaintEvent *e) {
    QPainter p(this);

    p.save();

    p.setRenderHint(QPainter::Antialiasing);
    if (m_state)
        p.fillRect(e->rect(), Qt::SolidPattern);
    else
        p.drawRect(e->rect());

    p.restore();
}


int BitfieldWidget::BitCount(64);

BitfieldWidget::BitfieldWidget(QWidget *parent) :
    QWidget(parent)
{
    m_bitWidgets.reserve(BitCount);
    for (int i=0; i<BitCount; ++i) {
        BitWidget* bitWidget(new BitWidget(i));

        connect(bitWidget, SIGNAL(stateChanged(bool)), this, SLOT(onBitChanged()));
        m_bitWidgets.append(bitWidget);
    }

    QGridLayout* fieldLayout(new QGridLayout);

    int bitoffset(0);
    for (int col=0; col<17; ++col) {
        if ((col%2) == 0) {
            if ((col%4) == 0) {
                QLabel* topNumberLabel(new QLabel);
                QLabel* bottomNumberLabel(new QLabel);
                int topNumber(BitCount-col*2);
                int bottomNumber(topNumber-(BitCount/2));

                if (col == 0) {
                    --topNumber;
                    --bottomNumber;
                }

                topNumberLabel->setText(QString("%1").arg(topNumber));
                bottomNumberLabel->setText(QString("%1").arg(bottomNumber));

                fieldLayout->addWidget(topNumberLabel, 0, col);
                fieldLayout->addWidget(bottomNumberLabel, 1, col);
            }
        } else {
            QHBoxLayout* bottomLayout(new QHBoxLayout);
            QHBoxLayout* topLayout(new QHBoxLayout);

            for (int j=0; j<4; ++j) {
                int topIndex(BitCount-1-(bitoffset*4)-j);

                topLayout->addWidget(m_bitWidgets.at(topIndex));
                bottomLayout->addWidget(m_bitWidgets.at(topIndex-(BitCount/2)));
            }

            ++bitoffset;

            fieldLayout->addLayout(bottomLayout, 1, col, Qt::AlignCenter);
            fieldLayout->addLayout(topLayout, 0, col, Qt::AlignCenter);
        }
    }

    QHBoxLayout* mainLayout(new QHBoxLayout(this));
    mainLayout->addStretch();
    mainLayout->addLayout(fieldLayout);
    mainLayout->addStretch();
}

BitfieldWidget::~BitfieldWidget()
{
}

void BitfieldWidget::updateBits(const HNumber& number) {
    QString str(HMath::format(number, 'b'));
    QListIterator<BitWidget*> bitsIterator(m_bitWidgets);

    if (number.isZero() || !number.isInteger())
        str.clear();
    else if (number.isNegative())
        str.remove(0, 3); // remove '-0b'
    else
        str.remove(0, 2); // remove '0b'

    QString::ConstIterator strIt(str.end());
    while (bitsIterator.hasNext()) {
        if (strIt != str.begin()) {
            --strIt;

            bitsIterator.next()->setState(*strIt == '1' ? true : false);
        } else {
            bitsIterator.next()->setState(false);
        }
    }
}

void BitfieldWidget::onBitChanged()
{
    QListIterator<BitWidget*> bitsIterator(m_bitWidgets);
    QString expr;

    while (bitsIterator.hasNext()) {
        expr.prepend(bitsIterator.next()->state() ? "1" : "0");
    }

    expr.remove(QRegExp(QString("^0{,%1}").arg(BitCount-1)));
    expr.prepend("0b");

    emit bitsChanged(expr);
}
