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

#ifndef BITFIELDWIDGET_H
#define BITFIELDWIDGET_H

#include <QLabel>
#include <QWidget>

class HNumber;

class BitWidget : public QLabel {
    Q_OBJECT

public:
    explicit BitWidget(int apos, QWidget* parent = 0);

    bool state() const { return m_state; }
    void setState(bool state) { m_state = state; update(); }

signals:
    void stateChanged(bool);

protected:
    void mouseReleaseEvent(QMouseEvent*);
    void paintEvent(QPaintEvent*);

private:
    enum {
        SizePixels = 20,
    };

    Q_DISABLE_COPY(BitWidget)

    bool m_state;
};

class BitFieldWidget : public QWidget {
    Q_OBJECT

public:
    explicit BitFieldWidget(QWidget* parent = 0);

signals:
    void bitsChanged(const QString&);

protected:
    virtual void wheelEvent(QWheelEvent *we);

public slots:
    void updateBits(const HNumber&);

private slots:
    void onBitChanged();
    void invertBits();
    void shiftBitsLeft();
    void shiftBitsRight();
    void resetBits();

private:
    enum {
        NumberOfBits = 64
    };

    Q_DISABLE_COPY(BitFieldWidget)

    QList<BitWidget*> m_bitWidgets;
};

#endif // BITFIELDWIDGET_H
