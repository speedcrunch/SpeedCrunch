/* This file is part of the SpeedCrunch project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2004,2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>

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


#ifndef EDITOR
#define EDITOR

#include <math/hmath.hxx>

class QEvent;
class QKeyEvent;
class QWheelEvent;
class QWidget;

#include <QTextEdit>

class Evaluator;

class EditorPrivate;

class Editor : public QTextEdit
{
  Q_OBJECT

  public:

    typedef enum
    {
      Number, FunctionName, Variable, MatchedPar
    } ColorType;

    explicit Editor( Evaluator* eval, QWidget* parent = 0 );
    ~Editor();

    QString text() const;
    void setText( const QString& );

    void doBackspace();

    int cursorPosition() const;
    void setCursorPosition( int pos );

    QSize sizeHint() const;
    QSize xminimumSizeHint() const;

    QStringList history() const;
    QStringList historyResults() const;
    void setHistory( const QStringList& history );
    void setHistoryResults( const QStringList& results );
    void clearHistory();

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
    void stopAutoComplete();

  public slots:
    void insert( const QString & );
    void insertConstant( const QString & );
    void evaluate();
    void appendHistory( const QString & result, const QString & expression );

  protected slots:
    void checkAutoComplete();
    void triggerAutoComplete();
    void autoComplete( const QString& item );
    void checkAutoCalc();
    void autoCalc();
    void checkMatching();
    void doMatchingPar();
    void doMatchingLeft();
    void doMatchingRight();
    void historyBack();
    void historyForward();
    void triggerEnter();

  signals:
    void returnPressed();
    void autoCalcDeactivated();
    void autoCalcActivated( const QString& );

  protected:
    void keyPressEvent( QKeyEvent* );
    void wheelEvent( QWheelEvent* );
    QString formatNumber( const HNumber& value ) const;

  private:
    EditorPrivate* d;
    Editor( const Editor& );
    Editor& operator=( const Editor& );
};

class EditorCompletionPrivate;

class EditorCompletion : public QObject
{
  Q_OBJECT

  public:
    EditorCompletion( Editor* editor );
    ~EditorCompletion();

    bool eventFilter( QObject *o, QEvent *e );
    void showCompletion( const QStringList &choices );

  signals:
    void selectedCompletion( const QString& item );

  public slots:
    void selectItem( const QString& item ); // WORKAROUND 76
    void doneCompletion();

  private slots:
    void fade( int );

  private:
    EditorCompletionPrivate* d;
    EditorCompletion( const EditorCompletion& );
    EditorCompletion& operator=( const EditorCompletion& );
};

class ConstantCompletionPrivate;

class ConstantCompletion : public QObject
{
  Q_OBJECT

  public:
    ConstantCompletion( Editor* editor );
    ~ConstantCompletion();

    bool eventFilter( QObject *o, QEvent *e );
    void showCompletion();

  signals:
    void selectedCompletion( const QString& item );

  public slots:
    void doneCompletion();

  private slots:
    void showCategory();
    void showConstants();
    void slide( int );

  private:
    ConstantCompletionPrivate* d;
    ConstantCompletion( const ConstantCompletion& );
    ConstantCompletion& operator=( const ConstantCompletion& );
};

#endif // EDITOR
