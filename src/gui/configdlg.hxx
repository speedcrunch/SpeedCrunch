// This file is part of the SpeedCrunch project
// Copyright (C) 2004-2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>
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


#ifndef CONFIGDLG_HXX
#define CONFIGDLG_HXX


#include <QDialog>


struct ConfigDlgPrivate;

class Evaluator;

class QPaintEvent;
class QShowEvent;


class ConfigDlg : public QDialog
{
  Q_OBJECT

  public:
    ConfigDlg( QWidget * parent = 0 );
    ~ConfigDlg();

  signals:
    void settingsChanged();

  public slots:
    void adaptToLanguageChange();

  protected slots:
    void chooseFont();
    void customAppearance();
    void initUI();
    virtual void accept();
    virtual void showEvent( QShowEvent * );

  private:
    ConfigDlgPrivate * d;
    ConfigDlg( const ConfigDlg & );
    ConfigDlg & operator=( const ConfigDlg & );
};


#endif
