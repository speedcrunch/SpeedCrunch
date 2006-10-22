/* This file is part of the SpeedCrunch project
   Copyright (C) 2004,2005 Ariya Hidayat <ariya@kde.org>

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


#ifndef EDITOR
#define EDITOR

class QEvent;
class QKeyEvent;
class QWidget;
#include <qobject.h>
#include <qstringlist.h>
#include <q3textedit.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QKeyEvent>
#include <QEvent>

class Evaluator;
#include "hmath.h"

class Editor : public Q3TextEdit
{
  Q_OBJECT

  public:

    typedef enum
    {
      Number, FunctionName, Variable, MatchedPar
    } ColorType;

    Editor( Evaluator* eval, QWidget* parent = 0, const char* name = 0 );
    ~Editor();

    QSize sizeHint() const;
    QSize xminimumSizeHint() const;

    QStringList history() const;
    void setHistory( const QStringList& history );

    bool autoCompleteEnabled() const;
    void setAutoCompleteEnabled( bool enable );

    bool autoCalcEnabled() const;
    void setAutoCalcEnabled( bool enable );
    void setFormat( char format );
    void setDecimalDigits( int digits );

    void setSyntaxHighlight( bool enable );
    bool isSyntaxHighlightEnabled() const;
    void setHighlightColor( ColorType type, QColor color );
    QColor highlightColor( ColorType type );
    
    void setAnsAvailable( bool avail );
    void stopAutoCalc();

  public slots:
    void appendHistory( const QString& text );
    void clearHistory();

  protected slots:
    void checkAutoComplete();
    void triggerAutoComplete();
    void autoComplete( const QString& item );
    void checkAutoCalc();
    void autoCalc();
    void checkMatching();
    void doMatchingLeft();
    void doMatchingRight();
    void historyBack();
    void historyForward();

  protected:
    void keyPressEvent( QKeyEvent* );
    void wheelEvent( QWheelEvent* );
    QString formatNumber( const HNumber& value ) const;

  private:
    class Private;
    Private* d;
    Editor( const Editor& );
    Editor& operator=( const Editor& );
};


class EditorCompletion : public QObject
{
  Q_OBJECT

  public:
    EditorCompletion( Editor* editor );
    ~EditorCompletion();

    bool eventFilter( QObject *o, QEvent *e );
    void doneCompletion();
    void showCompletion( const QStringList &choices );

  signals:
    void selectedCompletion( const QString& item );

  private:
    class Private;
    Private* d;
    EditorCompletion( const EditorCompletion& );
    EditorCompletion& operator=( const EditorCompletion& );
};

#endif // EDITOR
