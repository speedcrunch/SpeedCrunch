// This file is part of the SpeedCrunch project
// Copyright (C) 2004-2005 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
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


#include "math/hmath.hxx"

#include <QTextEdit>


class Constants;
class Evaluator;
class Functions;

class QEvent;
class QKeyEvent;
class QWheelEvent;
class QWidget;


class Editor : public QTextEdit
{
  Q_OBJECT

  public:
    enum HighlightScheme { AutoScheme };
    //enum ColorType { Number = 0, Function, Variable, MatchedPar };
    enum ColorType { Variable = 0, Number, Function, MatchedPar };

    explicit Editor( Evaluator *, Functions *, Constants *, char format = 'g',
                     int precision = -1, char radixChar = 'C',
                     QWidget * parent = 0 );
    ~Editor();

    bool        autoCalcEnabled() const;
    bool        autoCompleteEnabled() const;
    void        clearHistory();
    int         cursorPosition() const;
    void        doBackspace();
    Evaluator * evaluator() const;
    Functions * functions() const;
    Constants * constants() const;
    QStringList history() const;
    QStringList historyResults() const;
    QColor      highlightColor( ColorType type );
    bool        syntaxHighlight() const;
    char        radixChar() const;
    void        setAnsAvailable( bool avail );
    void        setAutoCalcEnabled( bool enable );
    void        setAutoCompleteEnabled( bool enable );
    void        setCursorPosition( int pos );
    void        setHighlightColor( ColorType type, QColor color );
    void        setSyntaxHighlight( bool enable );
    void        setHighlightScheme( Editor::HighlightScheme );
    void        setText( const QString & );
    void        setHistory( const QStringList & history );
    void        setHistoryResults( const QStringList & results );
    QSize       sizeHint() const;
    void        stopAutoCalc();
    void        stopAutoComplete();
    QString     text() const;

  signals:
    void autoCalcEnabled( const QString & );
    void autoCalcDisabled();
    void returnPressed();

  public slots:
    void appendHistory( const QString & result, const QString & expression );
    void appendHistory( const QStringList & h, const QStringList & r );
    void evaluate();
    void insertConstant( const QString & );
    void insert( const QString & );
    void setPrecision( int );
    void setFormat( char );
    void setRadixChar( char );

  protected slots:
    void autoCalc();
    void autoCalcSelection();
    void autoComplete( const QString & item );
    void checkAutoCalc();
    void checkAutoComplete();
    void startSelAutoCalcTimer();
    void checkMatching();
    void doMatchingLeft();
    void doMatchingPar();
    void doMatchingRight();
    void historyBack();
    void historyForward();
    void triggerAutoComplete();
    void triggerEnter();

  protected:
    QString formatNumber( const HNumber & value ) const;
    void    keyPressEvent( QKeyEvent * );
    void    wheelEvent( QWheelEvent * );

  private:
    struct Private;
    Private * const d;
    Editor();
    Editor( const Editor & );
    Editor & operator=( const Editor & );
};


class EditorCompletion : public QObject
{
  Q_OBJECT

  public:
    EditorCompletion( Editor * );
    ~EditorCompletion();
    bool eventFilter( QObject *, QEvent * );
    void showCompletion( const QStringList & choices );

  signals:
    void selectedCompletion( const QString & item );

  public slots:
    void doneCompletion();
    void selectItem( const QString & item ); // WORKAROUND 76

  private slots:
    void fade( int );

  private:
    struct Private;
    Private * d;
    EditorCompletion( const EditorCompletion & );
    EditorCompletion & operator=( const EditorCompletion & );
};


class ConstantCompletion : public QObject
{
  Q_OBJECT

  public:
    ConstantCompletion( Editor * );
    ~ConstantCompletion();
    bool eventFilter( QObject *, QEvent * );
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
    struct Private;
    Private * const d;
    ConstantCompletion( const ConstantCompletion & );
    ConstantCompletion & operator=( const ConstantCompletion & );
};


#endif
