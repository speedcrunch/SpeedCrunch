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
#ifndef BITFIELDWIDGET_H
#define BITFIELDWIDGET_H

#include <QtGui/QLabel>
#include <QtGui/QWidget>

class HNumber;

class BitWidget : public QLabel {
    Q_OBJECT

    public:
        explicit BitWidget(int apos, QWidget *parent = 0);
        ~BitWidget();

        bool state() const;
        void setState(bool state);

    signals:
        void stateChanged(bool);

    protected:
        void mouseReleaseEvent(QMouseEvent *ev);
        void paintEvent(QPaintEvent *e);

    private:
        Q_DISABLE_COPY(BitWidget)

        bool m_state;
};

class BitfieldWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BitfieldWidget(QWidget *parent = 0);
    ~BitfieldWidget();

signals:
    void bitsChanged(const QString&);

public slots:
    void updateBits(const HNumber& number);

private slots:
    void onBitChanged();

private:
    Q_DISABLE_COPY(BitfieldWidget)

    QList<BitWidget*> m_bitWidgets;
    static int BitCount;
};

#endif // BITFIELDWIDGET_H
