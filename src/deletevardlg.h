/* This file is part of the SpeedCrunch project
   Copyright (C) 2005 Ariya Hidayat <ariya@kde.org>

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

#ifndef DELETEVARDLG
#define DELETEVARDLG

#include <QDialog>

#include "ui_listdialog.h"

class Evaluator;
class DeleteVariableDlgPrivate;

class DeleteVariableDlg : public QDialog
{
  Q_OBJECT

  public:
    DeleteVariableDlg( Evaluator* eval, QWidget* parent = 0, const char* name = 0 );
    ~DeleteVariableDlg();

    void updateList();

  private slots:
    void initUI();
    void deleteVar();

  private:
    DeleteVariableDlgPrivate* d;
    Ui::ListDialog ui;

    DeleteVariableDlg( const DeleteVariableDlg& );
    DeleteVariableDlg& operator=( const DeleteVariableDlg& );
};


#endif // DELETEVARDLG

