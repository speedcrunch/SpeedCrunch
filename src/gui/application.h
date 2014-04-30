/*
  This file is part of the SpeedCrunch project
  Copyright (C) 2009, 2010 Helder Correia <helder.pereira.correia@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef GUI_APPLICATION_H
#define GUI_APPLICATION_H

#include <QScopedPointer>
#include <QApplication>

class ApplicationPrivate;

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char *argv[]);
    ~Application();

    bool isRunning();
    bool sendRaiseRequest();

signals:
    void raiseRequested();

private slots:
    void receiveMessage();

private:
    QScopedPointer<ApplicationPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Application)
    Q_DISABLE_COPY(Application)
};

#endif // GUI_APPLICATION_H
