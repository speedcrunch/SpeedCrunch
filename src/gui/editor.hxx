// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2004,2005 Ariya Hidayat <ariya@kde.org>
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


#ifndef EDITOR_HXX
#define EDITOR_HXX


#include <math/hmath.hxx>

#include <QTextEdit>


class Evaluator;
class EditorPrivate;

class QEvent;
class QKeyEvent;
class QWheelEvent;
class QWidget;


class Editor : public QTextEdit
{
  Q_OBJECT

  public:
    typedef enum { Number, FunctionName, Variable, MatchedPar } ColorType;

    explicit Editor( Evaluator * eval, QWidget * parent = 0 );
    ~Editor();
    bool        autoCalcEnabled() const;
    bool        autoCompleteEnabled() const;
    void        clearHistory();
    int         cursorPosition() const;
    void        doBackspace();
    QStringList history() const;
    QStringList historyResults() const;
    QColor      highlightColor( ColorType type );
    bool        isSyntaxHighlightEnabled() const;
    void        setAnsAvailable( bool avail );
    void        setAutoCalcEnabled( bool enable );
    void        setAutoCompleteEnabled( bool enable );
    void        setCursorPosition( int pos );
    void        setDecimalDigits( int digits );
    void        setFormat( char format );
    void        setHighlightColor( ColorType type, QColor color );
    void        setSyntaxHighlight( bool enable );
    void        setText( const QString & );
    void        setHistory( const QStringList & history );
    void        setHistoryResults( const QStringList & results );
    QSize       sizeHint() const;
    void        stopAutoCalc();
    void        stopAutoComplete();
    QString     text() const;

  public slots:
    void appendHistory( const QString & result, const QString & expression );
    void evaluate();
    void insertConstant( const QString & );
    void insert( const QString & );

  protected slots:
    void autoCalc();
    void autoComplete( const QString & item );
    void checkAutoCalc();
    void checkAutoComplete();
    void checkMatching();
    void doMatchingLeft();
    void doMatchingPar();
    void doMatchingRight();
    void historyBack();
    void historyForward();
    void triggerAutoComplete();
    void triggerEnter();

  signals:
    void autoCalcActivated( const QString & );
    void autoCalcDeactivated();
    void returnPressed();

  protected:
    QString formatNumber( const HNumber & value ) const;
    void    keyPressEvent( QKeyEvent * );
    void    wheelEvent( QWheelEvent * );

  private:
    EditorPrivate * d;
    Editor( const Editor & );
    Editor & operator=( const Editor & );
};


class EditorCompletionPrivate;


class EditorCompletion : public QObject
{
  Q_OBJECT

  public:
    EditorCompletion( Editor * editor );
    ~EditorCompletion();
    bool eventFilter( QObject * o, QEvent * e );
    void showCompletion( const QStringList & choices );

  signals:
    void selectedCompletion( const QString & item );

  public slots:
    void doneCompletion();
    void selectItem( const QString & item ); // WORKAROUND 76

  private slots:
    void fade( int );

  private:
    EditorCompletionPrivate * d;
    EditorCompletion( const EditorCompletion & );
    EditorCompletion & operator=( const EditorCompletion & );
};


class ConstantCompletionPrivate;


class ConstantCompletion : public QObject
{
  Q_OBJECT

  public:
    ConstantCompletion( Editor * editor );
    ~ConstantCompletion();
    bool eventFilter( QObject * o, QEvent * e );
    void showCompletion();

  signals:
    void selectedCompletion( const QString & item );

  public slots:
    void doneCompletion();

  private slots:
    void showCategory();
    void showConstants();
    void slide( int );

  private:
    ConstantCompletionPrivate * d;
    ConstantCompletion( const ConstantCompletion & );
    ConstantCompletion & operator=( const ConstantCompletion & );
};


#endif
