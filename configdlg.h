/* This file is part of the SpeedCrunch project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef CONFIGDLG
#define CONFIGDLG

#include <qcolor.h>
#include <qdialog.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QShowEvent>
#include <QPaintEvent>

class Evaluator;
class QPaintEvent;
class ShowEvent;

class ColorButtonPrivate;


class ColorButton : public QPushButton
{
  Q_OBJECT
  
  public:
    ColorButton( QWidget* parent = 0, const char* name = 0 );
    ~ColorButton();
    void setColor( QColor c );
    QColor color() const;
    
  protected slots:
    void showColorPicker();  
    
  protected: 
    void paintEvent( QPaintEvent* );
    
  private:
    ColorButtonPrivate* d;
    ColorButton( const ColorButton& );
    ColorButton& operator=( const ColorButton& );  
};

class ConfigDlgPrivate;

class ConfigDlg : public QDialog
{
  Q_OBJECT
  
  public:
    ConfigDlg( QWidget* parent = 0, const char* name = 0 );
    ~ConfigDlg();
    
  signals:
    void settingsChanged();
    
  protected slots:
    virtual void showEvent( QShowEvent* );
    void initUI();
    void chooseFont();
    void customAppearance();
    virtual void accept();    
  
  private:
    ConfigDlgPrivate* d;
    ConfigDlg( const ConfigDlg& );
    ConfigDlg& operator=( const ConfigDlg& );
};


#endif // CONFIGDLG

