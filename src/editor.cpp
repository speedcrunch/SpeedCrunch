/* This file is part of the SpeedCrunch project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2004,2005 Ariya Hidayat <ariya@kde.org>
                 2005-2006 Johan Thelin <e8johan@gmail.com>

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

#include "functions.h"
#include "editor.h"
#include "evaluator.h"
#include "settings.h"
#include "constants.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QFrame>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLineEdit>
#include <QStyle>
#include <QSyntaxHighlighter>
#include <QTimer>
#include <QTimeLine>
#include <QTreeWidget>
#include <QWheelEvent>


class EditorHighlighter : public QSyntaxHighlighter
{
private:
  Editor* editor;

public:
  EditorHighlighter( Editor* e ): QSyntaxHighlighter( e ), editor( e ) {}

  void highlightBlock( const QString& text )
  {
    if( !editor->isSyntaxHighlightEnabled() )
    {
      setFormat( 0, text.length(), editor->palette().text().color() );
      return;
    }

    Tokens tokens = Evaluator::scan( text, Settings::self()->decimalPoint );
    for( int i = 0; i < tokens.count(); i++ )
    {
      Token& token = tokens[i];
      QString text = token.text().toLower();
      QColor color = Qt::black;
      switch( token.type() )
      {
        case Token::Number:
          color = editor->highlightColor( Editor::Number );
        break;

        case Token::Identifier:
          {
            color = editor->highlightColor( Editor::Variable );
            QStringList fnames = FunctionRepository::self()->functionNames();
            for( int i=0; i<fnames.count(); i++ )
              if( fnames[i].toLower() == text )
                color = editor->highlightColor( Editor::FunctionName );
          }
          break;

        case Token::Operator:
          break;

        default: break;
      };
  	  if( token.pos() >= 0 )
        setFormat( token.pos(), token.text().length(), color );
    }
  }

};

class EditorPrivate
{
public:
  Evaluator* eval;
  QStringList history;
  int index;
  bool autoCompleteEnabled;
  EditorCompletion* completion;
  ConstantCompletion* constantCompletion;
  QTimer* completionTimer;
  bool autoCalcEnabled;
  char format;
  int decimalDigits;
  QTimer* autoCalcTimer;
  bool syntaxHighlightEnabled;
  EditorHighlighter* highlighter;
  QMap<Editor::ColorType,QColor> highlightColors;
  QTimer* matchingTimer;
  bool ansAvailable;
};

Editor::Editor( Evaluator* e, QWidget* parent ):
  QTextEdit( parent )
{
  d = new EditorPrivate;
  d->eval = e;
  d->index = 0;
  d->autoCompleteEnabled = true;
  d->completion = new EditorCompletion( this );
  d->constantCompletion = new ConstantCompletion( this );
  d->completionTimer = new QTimer( this );
  d->autoCalcEnabled = true;
  d->syntaxHighlightEnabled = true;
  d->highlighter = new EditorHighlighter( this );
  d->autoCalcTimer = new QTimer( this );
  d->matchingTimer = new QTimer( this );
  d->ansAvailable = false;

  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  setLineWrapMode( QTextEdit::NoWrap );
  setTabChangesFocus( true );
  setWordWrapMode( QTextOption::NoWrap );
  setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  setAcceptRichText( false );
  setAutoFormatting( QTextEdit::AutoNone );

  connect( d->completion, SIGNAL( selectedCompletion( const QString& ) ),
    SLOT( autoComplete( const QString& ) ) );
  connect( this, SIGNAL( textChanged() ), SLOT( checkAutoComplete() ) );
  connect( d->completionTimer, SIGNAL( timeout() ), SLOT( triggerAutoComplete() ) );

  connect( d->constantCompletion, SIGNAL( selectedCompletion( const QString& ) ), 
    SLOT( insertConstant( const QString& ) ) );

  connect( this, SIGNAL( textChanged() ), SLOT( checkMatching() ) );
  connect( d->matchingTimer, SIGNAL( timeout() ), SLOT( doMatchingPar() ) );
  connect( this, SIGNAL( textChanged() ), SLOT( checkAutoCalc() ) );
  connect( d->autoCalcTimer, SIGNAL( timeout() ), SLOT( autoCalc() ) );

  setHighlightColor( Number, QColor(0,0,127) );
  setHighlightColor( FunctionName, QColor(85,0,0) );
  setHighlightColor( Variable, QColor(0,85,0) );
  setHighlightColor( MatchedPar, QColor(255,255,183) );

  adjustSize();
}

Editor::~Editor()
{
  delete d;
}

QString Editor::text() const
{
  return toPlainText();
}

void Editor::setText( const QString& str )
{
  setPlainText( str );
}

void Editor::insert( const QString& str )
{
  insertPlainText( str );
}

void Editor::doBackspace()
{
  QTextCursor cursor = textCursor();
  cursor.deletePreviousChar();
  setTextCursor( cursor );
}

int Editor::cursorPosition() const
{
  return textCursor().position();
}

void Editor::setCursorPosition( int pos )
{
  QTextCursor cursor = textCursor();
  cursor.setPosition( pos );
  setTextCursor( cursor );
}

QSize Editor::sizeHint() const
{
  ensurePolished();
  QFontMetrics fm = fontMetrics();
  int h = qMax(fm.lineSpacing()+2, 14);
  int w = fm.width( 'x' ) * 20;
  int m = frameWidth() * 2;

  return( style()->sizeFromContents( QStyle::CT_LineEdit,
                                     0,
                                     QSize( w + m, h + m ).expandedTo(QApplication::globalStrut()),
                                     this )
        );
}

QStringList Editor::history() const
{
  return d->history;
}

void Editor::setHistory( const QStringList& h )
{
  d->history = h;
  d->index = d->history.count();
}

bool Editor::autoCompleteEnabled() const
{
  return d->autoCompleteEnabled;
}

void Editor::setAutoCompleteEnabled( bool enable )
{
  d->autoCompleteEnabled = enable;
}

bool Editor::autoCalcEnabled() const
{
  return d->autoCalcEnabled;
}

void Editor::setAutoCalcEnabled( bool enable )
{
  d->autoCalcEnabled = enable;
}

void Editor::setFormat( char format )
{
  d->format = format;
}

void Editor::setDecimalDigits( int digits )
{
  d->decimalDigits = digits;
}

void Editor::appendHistory( const QString& text )
{
  if( text.isEmpty() ) return;

  QString lastText;
  if( d->history.count() )
    lastText = d->history[ d->history.count()-1 ];
  if( text == lastText ) return;

  d->history.append( text );
  d->index = d->history.count()-1;
}

void Editor::clearHistory()
{
  d->history.clear();
  d->index = 0;
}

void Editor::checkAutoComplete()
{
  if( !d->autoCompleteEnabled ) return;

  d->completionTimer->stop();
  d->completionTimer->setSingleShot( true );
  d->completionTimer->start( 500 );
}

void Editor::checkMatching()
{
  if( !d->syntaxHighlightEnabled ) return;

  d->matchingTimer->stop();
  d->matchingTimer->setSingleShot( true );
  d->matchingTimer->start( 200 );
}

void Editor::checkAutoCalc()
{
  // Calc-As-You-Type
  if( !d->autoCalcEnabled ) return;

  d->autoCalcTimer->stop();
  d->autoCalcTimer->setSingleShot( true );
  d->autoCalcTimer->start( 1000 );

  emit autoCalcDeactivated();
}

void Editor::doMatchingPar()
{
  // clear previous
  setExtraSelections( QList<ExtraSelection>() );

  if( !d->syntaxHighlightEnabled ) return;

  doMatchingLeft();
  doMatchingRight();
}

void Editor::doMatchingLeft()
{
  // tokenize the expression
  int curPos = textCursor().position();

  // check for right par
  QString subtext = text().left( curPos );
  Tokens tokens = Evaluator::scan( subtext, Settings::self()->decimalPoint );
  if( !tokens.valid() ) return;
  if( tokens.count()<1 ) return;
  Token lastToken = tokens[ tokens.count()-1 ];

  // right par ?
  if( lastToken.isOperator() )
  if( lastToken.asOperator() == Token::RightPar )
  if( lastToken.pos() == curPos-1 )
  {
    // find the matching left par
    unsigned par = 1;
    int k = 0;
    Token matchToken;
    int matchPos = -1;

    for( k = tokens.count()-2; k >= 0; k-- )
    {
      if( par < 1 ) break;
      Token matchToken = tokens[k];
      if( matchToken.isOperator() )
      {
        if( matchToken.asOperator() == Token::RightPar )
          par++;
        if( matchToken.asOperator() == Token::LeftPar )
          par--;
        if( par == 0 ) matchPos = matchToken.pos();
      }
    }

    if( matchPos >= 0 )
    {
      ExtraSelection hilite1;
      hilite1.cursor = textCursor();
      hilite1.cursor.setPosition( matchPos );
      hilite1.cursor.setPosition( matchPos+1, QTextCursor::KeepAnchor );
      hilite1.format.setBackground( highlightColor( Editor::MatchedPar ) );

      ExtraSelection hilite2;
      hilite2.cursor = textCursor();
      hilite2.cursor.setPosition( lastToken.pos() );
      hilite2.cursor.setPosition( lastToken.pos()+1, QTextCursor::KeepAnchor );
      hilite2.format.setBackground( highlightColor( Editor::MatchedPar ) );

      QList<ExtraSelection> extras;
      extras << hilite1;
      extras << hilite2;
      setExtraSelections( extras );
    }
  }
}

void Editor::doMatchingRight()
{
  // tokenize the expression
  int curPos = textCursor().position();

  // check for left par
  QString subtext = text().right( text().length() - curPos );
  Tokens tokens = Evaluator::scan( subtext, Settings::self()->decimalPoint );
  if( !tokens.valid() ) return;
  if( tokens.count()<1 ) return;
  Token firstToken = tokens[ 0 ];

  // left par ?
  if( firstToken.isOperator() )
  if( firstToken.asOperator() == Token::LeftPar )
  if( firstToken.pos() == 0 )
  {
    // find the matching right par
    unsigned par = 1;
    int k = 0;
    Token matchToken;
    int matchPos = -1;

    for( k = 1; k < tokens.count(); k++ )
    {
      if( par < 1 ) break;
      Token matchToken = tokens[k];
      if( matchToken.isOperator() )
      {
        if( matchToken.asOperator() == Token::LeftPar )
          par++;
        if( matchToken.asOperator() == Token::RightPar )
          par--;
        if( par == 0 ) matchPos = matchToken.pos();
      }
    }

    if( matchPos >= 0 )
    {
      ExtraSelection hilite1;
      hilite1.cursor = textCursor();
      hilite1.cursor.setPosition( curPos+matchPos );
      hilite1.cursor.setPosition( curPos+matchPos+1, QTextCursor::KeepAnchor );
      hilite1.format.setBackground( highlightColor( Editor::MatchedPar ) );

      ExtraSelection hilite2;
      hilite2.cursor = textCursor();
      hilite2.cursor.setPosition( curPos+firstToken.pos() );
      hilite2.cursor.setPosition( curPos+firstToken.pos()+1, QTextCursor::KeepAnchor );
      hilite2.format.setBackground( highlightColor( Editor::MatchedPar ) );

      QList<ExtraSelection> extras;
      extras << hilite1;
      extras << hilite2;
      setExtraSelections( extras );
    }
  }
}

void Editor::triggerAutoComplete()
{
  if( !d->autoCompleteEnabled ) return;

  // tokenize the expression (don't worry, this is very fast)
  int curPos = textCursor().position();
  QString subtext = text().left( curPos );
  Tokens tokens = Evaluator::scan( subtext, Settings::self()->decimalPoint );
  if( !tokens.valid() ) return;
  if( tokens.count()<1 ) return;

  Token lastToken = tokens[ tokens.count()-1 ];

  // last token must be an identifier
  if( !lastToken.isIdentifier() ) return;
  QString id = lastToken.text();
  if( id.length() < 1 ) return;

  // no space after identifier
  if( lastToken.pos() + id.length() < subtext.length() ) return;

  // find matches in function names
  QStringList fnames = FunctionRepository::self()->functionNames();
  QStringList choices;
  for( int i=0; i<fnames.count(); i++ )
    if( fnames[i].startsWith( id, Qt::CaseInsensitive ) )
    {
      QString str = fnames[i];
      ::Function* f = FunctionRepository::self()->function( str );
      if( f )
        str.append( ':' ).append( f->description() );
      choices.append( str );
    }
  choices.sort();

  // find matches in variables names
  QStringList vchoices;
  for( int i=0; i<d->eval->variables().count(); i++ )
    if( d->eval->variables()[i].name.startsWith( id, Qt::CaseInsensitive ) )
      vchoices.append( QString("%1: %2").arg( d->eval->variables()[i].name ).
        arg( formatNumber( d->eval->variables()[i].value ) ) );
  vchoices.sort();
  choices += vchoices;

  // no match, don't bother with completion
  if( !choices.count() ) return;

  // single perfect match, no need to give choices
  if( choices.count()==1 )
    if( choices[0].toLower() == id.toLower() )
      return;

  // one match, complete it for the user
  if( choices.count()==1 )
  {
    QString str = choices[0].split( ':' )[0];
    str = str.remove( 0, id.length() );
    int curPos = textCursor().position();
    if( textCursor().selectionStart() == textCursor().selectionEnd() )
    {
      blockSignals( true );
      QTextCursor cursor = textCursor();
      cursor.clearSelection();
      cursor.insertText( str );
      cursor.setPosition( curPos );
      cursor.setPosition( curPos+str.length(), QTextCursor::KeepAnchor );
      setTextCursor( cursor );
      blockSignals( false );
    }
    return;
  }

  // present the user with completion choices
  d->completion->showCompletion( choices );
}

void Editor::autoComplete( const QString& item )
{
  if( !d->autoCompleteEnabled ) return;
  if( item.isEmpty() ) return;

  int curPos = textCursor().position();
  QString subtext = text().left( curPos );
  Tokens tokens = Evaluator::scan( subtext, Settings::self()->decimalPoint );
  if( !tokens.valid() ) return;
  if( tokens.count()<1 ) return;

  Token lastToken = tokens[ tokens.count()-1 ];
  if( !lastToken.isIdentifier() ) return;

  QStringList str = item.split( ':' );

  blockSignals( true );
  QTextCursor cursor = textCursor();
  cursor.setPosition( lastToken.pos() );
  cursor.setPosition( lastToken.pos()+lastToken.text().length(), QTextCursor::KeepAnchor );
  setTextCursor( cursor );
  insert( str[0] );
  blockSignals( false );
}

void Editor::autoCalc()
{
  if( !d->autoCalcEnabled ) return;

  QString str = Evaluator::autoFix( text(), Settings::self()->decimalPoint );
  if( str.isEmpty() ) return;

  // very short (just one token) and still no calculation, then skip
  if( !d->ansAvailable )
  {
    Tokens tokens = Evaluator::scan( text(), Settings::self()->decimalPoint );
    if( tokens.count() < 2 )
      return;
  }

  // too short even after autofix ? do not bother either...
  Tokens tokens = Evaluator::scan( str, Settings::self()->decimalPoint );
  if( tokens.count() < 2 ) return;

  // strip off assignment operator, e.g. "x=1+2" becomes "1+2" only
  // the reason is that we want only to evaluate (on the fly) the expression,
  // not to update (put the result in) the variable
  if( tokens.count() > 2 )
  if( tokens[0].isIdentifier() )
  if( tokens[1].asOperator() == Token::Equal )
    str.remove( 0, tokens[1].pos()+1 );

  // same reason as above, do not update "ans"
  HNumber oldAns = d->eval->get( QString("ans") );

  d->eval->setExpression( str );
  HNumber num = d->eval->eval();
  d->eval->set( QString("ans"), oldAns );

  if( d->eval->error().isEmpty() )
  {
    QString ss = QString("%1&nbsp;<b>%2</b>").arg(tr("Result:")).
      arg( formatNumber( num ) );
    emit autoCalcActivated( ss );
  }
  else
  {
    // invalid expression
    emit autoCalcDeactivated();
  }
}

void Editor::insertConstant( const QString& c )
{
  insert( c );
}

QString Editor::formatNumber( const HNumber& value ) const
{
  char* str = HMath::format( value, d->format, d->decimalDigits );
  QString s = QString::fromLatin1( str );
  free( str );
  return s;
}

void Editor::historyBack()
{
  if( d->history.count() )
  {
    d->index--;
    if( d->index < 0 ) d->index = 0;
    setText( d->history[ d->index ] );
    QTextCursor cursor = textCursor();
    cursor.movePosition( QTextCursor::EndOfBlock );
    setTextCursor( cursor );
    ensureCursorVisible();
    return;
  }
}

void Editor::historyForward()
{
  if( d->history.count() )
  {
    d->index++;
    if( d->index >= (int)d->history.count() )
      d->index = d->history.count()-1;
    setText( d->history[ d->index ] );
    QTextCursor cursor = textCursor();
    cursor.movePosition( QTextCursor::EndOfBlock );
    setTextCursor( cursor );
    return;
  }
}

void Editor::triggerEnter()
{
  d->completionTimer->stop();
  d->matchingTimer->stop();
  d->autoCalcTimer->stop();
  emit returnPressed();
}

void Editor::keyPressEvent( QKeyEvent* e )
{
  if( e->key() == Qt::Key_Enter )
  {
    QTimer::singleShot( 0, this, SLOT(triggerEnter()));
    e->accept();
    return;
  }

  if( e->key() == Qt::Key_Return )
  {
    QTimer::singleShot( 0, this, SLOT(triggerEnter()));
    e->accept();
    return;
  }

  if( e->key() == Qt::Key_Up )
  {
    historyBack();
    e->accept();
  }

  if( e->key() == Qt::Key_Down )
  {
    historyForward();
    e->accept();
  }

  if( e->key() == Qt::Key_Left ) checkMatching();
  if( e->key() == Qt::Key_Right ) checkMatching();
  if( e->key() == Qt::Key_Home ) checkMatching();
  if( e->key() == Qt::Key_End ) checkMatching();

  if( e->key() == Qt::Key_Space )
  if( e->modifiers() == Qt::ControlModifier )
  {
    d->constantCompletion->showCompletion();
    e->accept();
    return;
  }

  QTextEdit::keyPressEvent( e );
}

void Editor::wheelEvent( QWheelEvent* e )
{
  if( e->delta() > 0 )
    historyBack();
  else if( e->delta() < 0 )
    historyForward();

  e->accept();
}

void Editor::setSyntaxHighlight( bool enable )
{
  d->syntaxHighlightEnabled = enable;
  d->highlighter->rehighlight();
}

bool Editor::isSyntaxHighlightEnabled() const
{
  return d->syntaxHighlightEnabled;
}

void Editor::setHighlightColor( ColorType type, QColor color )
{
  d->highlightColors[ type ] = color;
  d->highlighter->rehighlight();
  doMatchingPar();
}

QColor Editor::highlightColor( ColorType type )
{
  return d->highlightColors[ type ];
}

void Editor::setAnsAvailable(bool avail)
{
  d->ansAvailable = avail;
}

// typically caled by the main window, just after the user evaluates seomthing
// useful so we don't pop up the autocalc tooltip again
void Editor::stopAutoCalc()
{
  d->autoCalcTimer->stop();
  emit autoCalcDeactivated();
}

// uncomment to activate fade-away effect when the completion pop-up disappears
// #define COMPLETION_FADE_EFFECT

class EditorCompletionPrivate
{
public:
  Editor* editor;
  QTreeWidget *popup;
#ifdef COMPLETION_FADE_EFFECT
  QTimeLine* fader;
#endif
};

EditorCompletion::EditorCompletion( Editor* editor ): QObject( editor )
{
  d = new EditorCompletionPrivate;
  d->editor = editor;

  d->popup = new QTreeWidget( 0 );
  d->popup->setFrameShape( QFrame::NoFrame );
  d->popup->setColumnCount( 2 );
  d->popup->setRootIsDecorated( false );
  d->popup->header()->hide();
  d->popup->setEditTriggers( QTreeWidget::NoEditTriggers );
  d->popup->setSelectionBehavior( QTreeWidget::SelectRows );
  d->popup->setMouseTracking( true );
  d->popup->installEventFilter( this );
  connect( d->popup, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(doneCompletion()) );

  d->popup->hide();
  d->popup->setParent( 0, Qt::Popup );
  d->popup->setFocusPolicy( Qt::NoFocus );
  d->popup->setFocusProxy( editor );

  d->popup->setFrameStyle( QFrame::Box | QFrame::Plain );

#ifdef COMPLETION_FADE_EFFECT
  d->fader = new QTimeLine( 500, this );
  d->fader->setFrameRange( 0, 100 );
  d->fader->setCurveShape( QTimeLine::EaseInCurve );
  connect( d->fader, SIGNAL(frameChanged(int)), SLOT(fade(int)) );
#endif
}

EditorCompletion::~EditorCompletion()
{
  delete d->popup;
  delete d;
}

bool EditorCompletion::eventFilter( QObject *obj, QEvent *ev )
{
  if ( obj == d->popup )
  {

    if ( ev->type() == QEvent::KeyPress )
    {
      QKeyEvent *ke = (QKeyEvent*)ev;
      if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Tab )
      {
        doneCompletion();
        return true;
      }
      else if ( ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Right ||
      ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down ||
      ke->key() == Qt::Key_Home || ke->key() == Qt::Key_End ||
      ke->key() == Qt::Key_PageUp || ke->key() == Qt::Key_PageDown )
      {
        return false;
      }

      d->popup->hide();
      d->editor->setFocus();
      QApplication::sendEvent( d->editor, ev );
      return true;
    }
  }

  return false;
}

void EditorCompletion::doneCompletion()
{
#ifdef COMPLETION_FADE_EFFECT
  d->fader->start();
  QTimer::singleShot( 750, d->popup, SLOT( hide() ) ); // sentinel
#else
  d->popup->hide();
#endif
  d->editor->setFocus();
  QTreeWidgetItem* item = d->popup->currentItem();
  emit selectedCompletion( item ? item->text(0) : QString() );
}

void EditorCompletion::showCompletion( const QStringList& choices )
{
  if( !choices.count() ) return;

#ifdef COMPLETION_FADE_EFFECT
  d->fader->stop();
  fade( 0 );
#endif

  d->popup->setUpdatesEnabled( false );
  d->popup->clear();
  for( int i = 0; i < choices.count(); i++ )
    new QTreeWidgetItem( d->popup, choices[i].split(':') );
  d->popup->sortItems( 0, Qt::AscendingOrder );
  d->popup->setCurrentItem( d->popup->topLevelItem(0) );
  d->popup->adjustSize();
  d->popup->setUpdatesEnabled( true );

  // size of the pop-up
  d->popup->adjustSize();
  int h = d->popup->sizeHintForRow(0) * qMin(7, choices.count()) + 3;
  int w = d->popup->width();
  d->popup->resize( w, h );

  // position, reference is editor's cursor position in global coord
  QFontMetrics fm( d->editor->font() );
  int curPos = d->editor->textCursor().position();
  int pixelsOffset = fm.width( d->editor->text(), curPos );
  QPoint pos = d->editor->mapToGlobal( QPoint( pixelsOffset, d->editor->height() ) );


  // if popup is partially invisible, move to other position
  const QRect screen = QApplication::desktop()->availableGeometry( d->editor );
  if( pos.y() + h > screen.y()+screen.height() )
    pos.setY( pos.y() - h - d->editor->height() );
  if( pos.x() + w > screen.x()+screen.width() )
    pos.setX(  screen.x()+screen.width() - w );

  d->popup->move( pos );
  d->popup->setFocus();
  d->popup->show();
}

void EditorCompletion::fade( int v )
{
  d->popup->setWindowOpacity( (qreal)(100-v)/100 );
}

class ConstantCompletionPrivate
{
public:
  Editor* editor;
  QFrame* popup;
  QTreeWidget* categoryList;
  QTreeWidget* constantList;
  QList<Constant> constants;
  QString lastCategory;
  QTimeLine* slider;
};

ConstantCompletion::ConstantCompletion( Editor* editor ): QObject( editor )
{
  d = new ConstantCompletionPrivate;
  d->editor = editor;

  d->popup = new QFrame( 0 );
  d->popup->hide();
  d->popup->setParent( 0, Qt::Popup );
  d->popup->setFocusPolicy( Qt::NoFocus );
  d->popup->setFocusProxy( editor );
  d->popup->setFrameStyle( QFrame::Box | QFrame::Plain );

  d->categoryList = new QTreeWidget( d->popup );
  d->categoryList->setFrameShape( QFrame::NoFrame );
  d->categoryList->setColumnCount( 1 );
  d->categoryList->setRootIsDecorated( false );
  d->categoryList->header()->hide();
  d->categoryList->setEditTriggers( QTreeWidget::NoEditTriggers );
  d->categoryList->setSelectionBehavior( QTreeWidget::SelectRows );
  d->categoryList->setMouseTracking( true );
  d->categoryList->installEventFilter( this );
  connect( d->categoryList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(doneCompletion()) );

  d->constantList = new QTreeWidget( d->popup );
  d->constantList->setFrameShape( QFrame::NoFrame );
  d->constantList->setColumnCount( 2 );
  d->constantList->setColumnHidden( 1, true );
  d->constantList->setRootIsDecorated( false );
  d->constantList->header()->hide();
  d->constantList->setEditTriggers( QTreeWidget::NoEditTriggers );
  d->constantList->setSelectionBehavior( QTreeWidget::SelectRows );
  d->constantList->setMouseTracking( true );
  d->constantList->installEventFilter( this );
  connect( d->constantList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(doneCompletion()) );

  d->slider = new QTimeLine( 250, this );
  d->slider->setCurveShape( QTimeLine::EaseInCurve );
  connect( d->slider, SIGNAL(frameChanged(int)), SLOT(slide(int)) );

  // FIXME share constants with others
  Constants* ct = new Constants( this );
  d->constants = ct->constantList;

  // populate categories
  QStringList str;
  str << tr("All");
  QTreeWidgetItem* all = new QTreeWidgetItem( d->categoryList, str );
  for( int k = 0; k < ct->categoryList.count(); k++ )
  {
    str.clear();	
    str << ct->categoryList[k];
    new QTreeWidgetItem( d->categoryList, str );
  }
  d->categoryList->setCurrentItem( all );

  // populate constants
  d->lastCategory = tr("All");
  for( int k = 0; k < ct->constantList.count(); k++ )
  {
    QStringList str;
    str << ct->constantList[k].name;
    str << ct->constantList[k].name.toUpper();
    new QTreeWidgetItem( d->constantList, str );
  }

  // find size, the biggest between both
  d->constantList->resizeColumnToContents( 0 );
  d->categoryList->resizeColumnToContents( 0 );
  int ww = qMax( d->constantList->width(), d->categoryList->width() );
  int h1 = d->constantList->sizeHintForRow(0) * qMin(7, d->constants.count()) + 3;
  int h2 = d->categoryList->sizeHintForRow(0) * qMin(7, ct->categoryList.count()) + 3;
  int hh = qMax( h1, h2 );
  ww += 64; // extra space (FIXME scrollbar size?)

  // adjust dimensions
  d->popup->resize( ww, hh );
  d->constantList->resize( ww, hh );
  d->categoryList->resize( ww, hh );
}

ConstantCompletion::~ConstantCompletion()
{
  delete d->popup;
  delete d;
}

void ConstantCompletion::showCategory()
{
  d->slider->setFrameRange( d->popup->width(), 0 );
  d->slider->start();
  d->categoryList->setFocus();
}

void ConstantCompletion::showConstants()
{
  d->slider->setFrameRange( 0, d->popup->width() );
  d->slider->start();
  d->constantList->setFocus();

  QString chosenCategory;
  if( d->categoryList->currentItem() )
	  chosenCategory = d->categoryList->currentItem()->text( 0 );

  if( d->lastCategory == chosenCategory )
	  return;
  
  d->constantList->clear();
  for( int k = 0; k < d->constants.count(); k++ )
  {
      QStringList str;
      str << d->constants[k].name;
      str << d->constants[k].name.toUpper();

      bool include = (chosenCategory == tr("All")) ? true:
        d->constants[k].categories.contains( chosenCategory );

      if( !include )
        continue;

      QTreeWidgetItem* item = 0;
      item = new QTreeWidgetItem( d->constantList, str );
  }
  d->constantList->sortItems( 1, Qt::AscendingOrder );

  d->lastCategory = chosenCategory;
}

bool ConstantCompletion::eventFilter( QObject *obj, QEvent *ev )
{
  if ( obj == d->constantList )
  {
    if ( ev->type() == QEvent::KeyPress )
    {
      QKeyEvent *ke = (QKeyEvent*)ev;
      if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Tab )
      {
        doneCompletion();
        return true;
      }
      else if ( ke->key() == Qt::Key_Left )
      {
        showCategory();
        return true;
      }
      else if ( ke->key() == Qt::Key_Right || ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down ||
      ke->key() == Qt::Key_Home || ke->key() == Qt::Key_End ||
      ke->key() == Qt::Key_PageUp || ke->key() == Qt::Key_PageDown )
      {
        return false;
      }

      d->popup->hide();
      d->editor->setFocus();
      QApplication::sendEvent( d->editor, ev );
      return true;
    }
  }

  if ( obj == d->categoryList )
  {
    if ( ev->type() == QEvent::KeyPress )
    {
      QKeyEvent *ke = (QKeyEvent*)ev;
      if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Right )
      {
        showConstants();
        return true;
      }
	  else if ( ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down ||
      ke->key() == Qt::Key_Home || ke->key() == Qt::Key_End ||
      ke->key() == Qt::Key_PageUp || ke->key() == Qt::Key_PageDown )
      {
        return false;
      }

      d->popup->hide();
      d->editor->setFocus();
      QApplication::sendEvent( d->editor, ev );
	  return true;
    }
  }

  return false;
}

void ConstantCompletion::doneCompletion()
{
  d->popup->hide();
  d->editor->setFocus();
  QTreeWidgetItem* item = d->constantList->currentItem();
  for( int k = 0; k < d->constants.count(); k++ )
    if( d->constants[k].name == item->text(0) )
      emit selectedCompletion( d->constants[k].value );
}

void ConstantCompletion::showCompletion()
{
  // position, reference is editor's cursor position in global coord
  QFontMetrics fm( d->editor->font() );
  int curPos = d->editor->textCursor().position();
  int pixelsOffset = fm.width( d->editor->text(), curPos );
  QPoint pos = d->editor->mapToGlobal( QPoint( pixelsOffset, d->editor->height() ) );

  int h = d->popup->height();
  int w = d->popup->width();

  // if popup is partially invisible, move to other position
  const QRect screen = QApplication::desktop()->availableGeometry( d->editor );
  if( pos.y() + h > screen.y()+screen.height() )
    pos.setY( pos.y() - h - d->editor->height() );
  if( pos.x() + w > screen.x()+screen.width() )
    pos.setX(  screen.x()+screen.width() - w );

  // start with category
  d->categoryList->setFocus();
  slide( 0 );

  d->popup->move( pos );
  d->popup->show();
}

void ConstantCompletion::slide( int v )
{
  d->categoryList->move( -v, 0 );
  d->constantList->move( d->popup->width()-v, 0 );
}