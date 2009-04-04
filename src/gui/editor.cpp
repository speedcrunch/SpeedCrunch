// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2004,2005 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2009 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "gui/editor.hxx"

#include "core/constants.hxx"
#include "core/evaluator.hxx"
#include "core/functions.hxx"
#include "core/settings.hxx"
#include "gui/syntaxhighlighter.hxx"

#include <QtCore/QEvent>
#include <QtCore/QTimeLine>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QKeyEvent>
#include <QtGui/QLineEdit>
#include <QtGui/QStyle>
#include <QtGui/QTreeWidget>
#include <QtGui/QWheelEvent>

#include <algorithm>

struct Editor::Private
{
    bool ansAvailable;
    bool autoCalcEnabled;
    QTimer * autoCalcSelTimer;
    QTimer * autoCalcTimer;
    bool autoCompleteEnabled;
    EditorCompletion * completion;
    QTimer * completionTimer;
    ConstantCompletion * constantCompletion;
    Constants * constants;
    Evaluator * eval;
    SyntaxHighlighter * highlighter;
    QStringList history;
    QStringList historyResults;
    int index;
    QTimer * matchingTimer;
    Settings * settings;
    bool syntaxHighlightEnabled;
};

Editor::Editor( QWidget * parent )
    : QTextEdit( parent ), d( new Editor::Private )
{
    d->eval = Evaluator::instance();
    d->constants = Constants::instance();
    d->settings = Settings::instance();
    d->index = 0;
    d->autoCompleteEnabled = true;
    d->completion = new EditorCompletion( this );
    d->constantCompletion = 0;
    d->completionTimer = new QTimer( this );
    d->autoCalcEnabled = true;
    d->syntaxHighlightEnabled = true;
    d->highlighter = new SyntaxHighlighter( this );
    d->autoCalcTimer = new QTimer( this );
    d->autoCalcSelTimer = new QTimer( this );
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

    connect( d->autoCalcTimer, SIGNAL(timeout()), SLOT(autoCalc()) );
    connect( d->autoCalcSelTimer, SIGNAL(timeout()), SLOT(autoCalcSelection()) );
    connect( d->completion, SIGNAL(selectedCompletion(const QString &)),
             SLOT(autoComplete(const QString &)) );
    connect( d->completionTimer, SIGNAL(timeout()), SLOT(triggerAutoComplete()) );
    connect( d->matchingTimer, SIGNAL(timeout()), SLOT( doMatchingPar()) );
    connect( this, SIGNAL(selectionChanged()), SLOT(startSelAutoCalcTimer()) );
    connect( this, SIGNAL(textChanged()), SLOT(checkAutoCalc()) );
    connect( this, SIGNAL(textChanged()), SLOT(checkAutoComplete()) );
    connect( this, SIGNAL(textChanged()), SLOT(checkMatching()) );

    adjustSize();
}

Editor::~Editor()
{
}

QString Editor::text() const
{
    return toPlainText();
}

void Editor::setText( const QString & str )
{
    setPlainText( str );
}

void Editor::insert( const QString & str )
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
    const QFontMetrics fm = fontMetrics();
    const int h = qMax( fm.lineSpacing() + 2, 14 );
    const int w = fm.width( 'x' ) * 20;
    const int m = frameWidth() * 2;

    return style()->sizeFromContents( QStyle::CT_LineEdit, 0, QSize(w+m, h+m).
                                      expandedTo(QApplication::globalStrut()), this );
}

QStringList Editor::history() const
{
    return d->history;
}

QStringList Editor::historyResults() const
{
    return d->historyResults;
}

void Editor::setHistory( const QStringList & h )
{
    d->history = h;
    d->index = d->history.count();
}

void Editor::setHistoryResults( const QStringList & results )
{
    d->historyResults = results;
}

void Editor::appendHistory( const QStringList & h, const QStringList & r )
{
    d->history += h;
    d->historyResults += r;
    d->index = d->history.count();
}

void Editor::clearHistory()
{
    d->history.clear();
    d->historyResults.clear();
}

bool Editor::autoCompleteEnabled() const
{
    return d->autoCompleteEnabled;
}

void Editor::setAutoCompletionEnabled( bool enable )
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

void Editor::appendHistory( const QString & expression, const QString & result )
{
    if ( expression.isEmpty() || result.isEmpty() )
        return;

    d->history.append( expression );
    d->historyResults.append( result );
    d->index = d->history.count() - 1;
}

void Editor::checkAutoComplete()
{
    if ( ! d->autoCompleteEnabled )
        return;

    d->completionTimer->stop();
    d->completionTimer->setSingleShot( true );
    d->completionTimer->start( 500 );
}

void Editor::checkMatching()
{
    if ( ! d->syntaxHighlightEnabled )
        return;

    d->matchingTimer->stop();
    d->matchingTimer->setSingleShot( true );
    d->matchingTimer->start( 200 );
}

void Editor::checkAutoCalc()
{
    if ( ! d->autoCalcEnabled )
        return;

    d->autoCalcTimer->stop();
    d->autoCalcTimer->setSingleShot( true );
    d->autoCalcTimer->start( 1 );

    emit autoCalcDisabled();
}

void Editor::startSelAutoCalcTimer()
{
    if ( ! d->autoCalcEnabled )
        return;

    d->autoCalcSelTimer->stop();
    d->autoCalcSelTimer->setSingleShot( true );
    d->autoCalcSelTimer->start( 1 );
}

void Editor::doMatchingPar()
{
    // clear previous
    setExtraSelections( QList<ExtraSelection>() );

    if ( ! d->syntaxHighlightEnabled )
        return;

    doMatchingLeft();
    doMatchingRight();
}

void Editor::doMatchingLeft()
{
    // tokenize the expression
    const int curPos = textCursor().position();

    // check for right par
    QString subtext = text().left( curPos );
    Tokens tokens = d->eval->scan( subtext );
    if ( ! tokens.valid() || tokens.count() < 1 )
        return;
    Token lastToken = tokens.at( tokens.count() - 1 );

    // right par ?
    if ( lastToken.type() == Token::stxClosePar && lastToken.pos() == curPos - 1 ) {
        // find the matching left par
        unsigned par = 1;
        int k = 0;
        int matchPos = -1;

        for ( k = tokens.count() - 2; k >= 0 && par > 0; --k ) {
            Token matchToken = tokens.at(k);
            switch ( matchToken.type() ) {
                case Token::stxOpenPar : --par; break;
                case Token::stxClosePar: ++par; break;
                default:;
            }
            matchPos = matchToken.pos();
        }

        if ( par == 0 ) {
            ExtraSelection hilite1;
            hilite1.cursor = textCursor();
            hilite1.cursor.setPosition( matchPos );
            hilite1.cursor.setPosition( matchPos + 1, QTextCursor::KeepAnchor );
            hilite1.format.setBackground( d->highlighter->color(SyntaxHighlighter::MatchedPar) );

            ExtraSelection hilite2;
            hilite2.cursor = textCursor();
            hilite2.cursor.setPosition( lastToken.pos() );
            hilite2.cursor.setPosition( lastToken.pos() + 1, QTextCursor::KeepAnchor );
            hilite2.format.setBackground( d->highlighter->color(SyntaxHighlighter::MatchedPar) );

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
    const int curPos = textCursor().position();

    // check for left par
    QString subtext = text().right( text().length() - curPos );
    Tokens tokens = d->eval->scan( subtext );
    if ( ! tokens.valid() || tokens.count() < 1 )
        return;
    Token firstToken = tokens.at( 0 );

    // left par ?
    if ( firstToken.type() == Token::stxOpenPar && firstToken.pos() == 0 ) {
        // find the matching right par
        unsigned par = 1;
        int k = 0;
        Token matchToken;
        int matchPos = -1;

        for ( k = 1; k < tokens.count() && par > 0; ++k ) {
            const Token matchToken = tokens.at( k );
            switch ( matchToken.type() ) {
                case Token::stxOpenPar : ++par; break;
                case Token::stxClosePar: --par; break;
                default:;
            }
            matchPos = matchToken.pos();
        }

        if ( par == 0 ) {
            ExtraSelection hilite1;
            hilite1.cursor = textCursor();
            hilite1.cursor.setPosition( curPos+matchPos );
            hilite1.cursor.setPosition( curPos+matchPos + 1, QTextCursor::KeepAnchor );
            hilite1.format.setBackground( d->highlighter->color(SyntaxHighlighter::MatchedPar) );

            ExtraSelection hilite2;
            hilite2.cursor = textCursor();
            hilite2.cursor.setPosition( curPos+firstToken.pos() );
            hilite2.cursor.setPosition( curPos+firstToken.pos() + 1, QTextCursor::KeepAnchor );
            hilite2.format.setBackground( d->highlighter->color(SyntaxHighlighter::MatchedPar) );

            QList<ExtraSelection> extras;
            extras << hilite1;
            extras << hilite2;
            setExtraSelections( extras );
        }
    }
}

void Editor::triggerAutoComplete()
{
    if ( ! d->autoCompleteEnabled )
        return;

    // tokenize the expression (don't worry, this is very fast)
    const int curPos = textCursor().position();
    QString subtext = text().left( curPos );
    const Tokens tokens = d->eval->scan( subtext );
    if ( ! tokens.valid() || tokens.count() < 1  )
        return;

    Token lastToken = tokens.at( tokens.count()-1 );

    // last token must be an identifier
    if ( ! lastToken.isIdentifier() )
        return;
    const QString id = lastToken.text();
    if ( id.length() < 1 )
        return;

    // no space after identifier
    if ( lastToken.pos() + id.length() < subtext.length() )
        return;

    // find matches in function names
    const QStringList fnames = Functions::instance()->names();
    QStringList choices;
    for ( int i = 0; i < fnames.count(); ++i ) {
        if ( fnames.at(i).startsWith(id, Qt::CaseInsensitive) ) {
            QString str = fnames.at( i );
            ::Function * f = Functions::instance()->function( str );
            if ( f )
                str.append( ':' ).append( f->description() );
            choices.append( str );
        }
    }
    choices.sort();

    // find matches in variables names
    QStringList vchoices;
    for ( int i = 0; i < d->eval->variables().count(); ++i )
        if ( d->eval->variables().at(i).name.startsWith(id, Qt::CaseInsensitive) )
            vchoices.append( QString("%1: %2")
                                 .arg(d->eval->variables().at(i).name)
                                 .arg(formatNumber(d->eval->variables().at(i).value)) );
    vchoices.sort();
    choices += vchoices;

    // no match, don't bother with completion
    if ( ! choices.count() )
        return;

    // single perfect match, no need to give choices
    if ( choices.count() == 1 )
        if ( choices.at(0).toLower() == id.toLower() )
            return;

    // one match, complete it for the user
    if ( choices.count() == 1 ) {
        QString str = choices.at( 0 ).split( ':' ).at( 0 );
        str = str.remove( 0, id.length() );
        const int curPos = textCursor().position();
        if ( textCursor().selectionStart() == textCursor().selectionEnd() ) {
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

void Editor::autoComplete( const QString & item )
{
    if ( ! d->autoCompleteEnabled || item.isEmpty() )
        return;

    const int curPos = textCursor().position();
    const QString subtext = text().left( curPos );
    const Tokens tokens = d->eval->scan( subtext );
    if ( ! tokens.valid() || tokens.count() < 1 )
        return;

    const Token lastToken = tokens.at( tokens.count() - 1 );
    if ( ! lastToken.isIdentifier() )
        return;

    const QStringList str = item.split( ':' );

    blockSignals( true );
    QTextCursor cursor = textCursor();
    cursor.setPosition( lastToken.pos() );
    cursor.setPosition( lastToken.pos() + lastToken.text().length(), QTextCursor::KeepAnchor );
    setTextCursor( cursor );
    insert( str.at(0) );
    blockSignals( false );
}

void Editor::autoCalc()
{
    if ( ! d->autoCalcEnabled )
        return;

    const QString str = d->eval->autoFix( text() );
    if ( str.isEmpty() )
        return;

    // very short (just one token) and still no calculation, then skip
    if ( ! d->ansAvailable ) {
        const Tokens tokens = d->eval->scan( text() );
        if ( tokens.count() < 2 )
            return;
    }

    // too short even after autofix ? do not bother either...
    const Tokens tokens = d->eval->scan( str );
    if ( tokens.count() < 2 )
        return;

    // strip off assignment operator, e.g. "x=1+2" becomes "1+2" only
    // the reason is that we want only to evaluate (on the fly) the expression,
    // not to update (put the result in) the variable
    //if( tokens.count() > 2 ) // reftk
    //if( tokens.at(0).isIdentifier() )
    //if( tokens.at(1).asOperator() == Token::Equal )
    //  str.remove( 0, tokens.at(1).pos()+1 );

    // same reason as above, do not update "ans"
    d->eval->setExpression( str );
    const HNumber num = d->eval->evalNoAssign();

    if ( d->eval->error().isEmpty() ) {
        const QString ss = tr( "Current result: <b>%1</b>" ).arg( formatNumber(num) );
        emit autoCalcEnabled( ss );
    } else
        emit autoCalcEnabled( d->eval->error() );
}

void Editor::autoCalcSelection()
{
    if ( ! d->autoCalcEnabled )
        return;

    const QString str = d->eval->autoFix( textCursor().selectedText() );
    if ( str.isEmpty() )
        return;

    // very short (just one token) and still no calculation, then skip
    if ( ! d->ansAvailable ) {
        const Tokens tokens = d->eval->scan( text() );
        if ( tokens.count() < 2 )
            return;
    }

    // too short even after autofix ? do not bother either...
    const Tokens tokens = d->eval->scan( str );
    if ( tokens.count() < 2 )
        return;

    // strip off assignment operator, e.g. "x=1+2" becomes "1+2" only
    // the reason is that we want only to evaluate (on the fly) the expression,
    // not to update (put the result in) the variable
    //if( tokens.count() > 2 ) // reftk
    //if( tokens.at(0).isIdentifier() )
    //if( tokens.at(1).asOperator() == Token::Equal )
    //  str.remove( 0, tokens.at(1).pos()+1 );

    // same reason as above, do not update "ans"
    d->eval->setExpression( str );
    const HNumber num = d->eval->evalNoAssign();

    if ( d->eval->error().isEmpty() ) {
        const QString ss = tr( "Selection result: <b>%1</b>" ).arg( formatNumber(num) );
        emit autoCalcEnabled( ss );
    } else
        emit autoCalcEnabled( d->eval->error() );
}

void Editor::insertConstant( const QString & c )
{
    QString s = c;
    if ( d->settings->radixCharacter() == ',' )
        s.replace( '.', ',' );
    if ( ! c.isNull() )
        insert( s );
    if ( d->constantCompletion ) {
        disconnect( d->constantCompletion );
        d->constantCompletion->deleteLater();
        d->constantCompletion = 0;
    }
}

void Editor::cancelConstantCompletion()
{
    if ( d->constantCompletion ) {
        disconnect( d->constantCompletion );
        d->constantCompletion->deleteLater();
        d->constantCompletion = 0;
    }
}

void Editor::evaluate()
{
    triggerEnter();
}

QString Editor::formatNumber( const HNumber & value ) const
{
    char * str = HMath::format( value, d->settings->resultFormat, d->settings->resultPrecision );
    QString s = QString::fromLatin1( str );
    if ( d->settings->radixCharacter() != '.' )
        s.replace( '.', d->settings->radixCharacter() );
    free( str );
    return s;
}

void Editor::historyBack()
{
    if ( d->history.count() ) {
        --(d->index);
        if ( d->index < 0 )
            d->index = 0;
        setText( d->history.at(d->index) );
        QTextCursor cursor = textCursor();
        cursor.movePosition( QTextCursor::EndOfBlock );
        setTextCursor( cursor );
        ensureCursorVisible();
        return;
    }
}

void Editor::historyForward()
{
    if ( d->history.count() ) {
        ++(d->index);
        if ( d->index >= (int)d->history.count() )
            d->index = d->history.count() - 1;
        setText( d->history.at(d->index) );
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
    d->autoCalcSelTimer->stop();
    emit returnPressed();
}

void Editor::keyPressEvent( QKeyEvent * e )
{
    if ( e->key() == Qt::Key_Enter ) {
        QTimer::singleShot( 0, this, SLOT(triggerEnter()) );
        e->accept();
        return;
    }

    if ( e->key() == Qt::Key_Return ) {
        QTimer::singleShot( 0, this, SLOT(triggerEnter()) );
        e->accept();
        return;
    }

    if ( e->key() == Qt::Key_Up ) {
        historyBack();
        e->accept();
    }

    if ( e->key() == Qt::Key_Down ) {
        historyForward();
        e->accept();
    }

    if ( e->key() == Qt::Key_Left || e->key() == Qt::Key_Right || e->key() == Qt::Key_Home
         || e->key() == Qt::Key_End )
    {
        checkMatching();
    }

    if ( e->key() == Qt::Key_Space && e->modifiers() == Qt::ControlModifier
         && ! d->constantCompletion )
    {
        d->constantCompletion = new ConstantCompletion( this );
        connect( d->constantCompletion, SIGNAL(selectedCompletion(const QString &)),
                 SLOT(insertConstant(const QString &)) );
        connect( d->constantCompletion, SIGNAL(canceledCompletion()),
                 SLOT(cancelConstantCompletion()) );
        d->constantCompletion->showCompletion();
        e->accept();
        return;
    }

    QTextEdit::keyPressEvent( e );
}

void Editor::wheelEvent( QWheelEvent * e )
{
    if ( e->delta() > 0 )
        historyBack();
    else if ( e->delta() < 0 )
        historyForward();
    e->accept();
}

void Editor::setSyntaxHighlightingEnabled( bool enable )
{
    d->syntaxHighlightEnabled = enable;
    d->highlighter->rehighlight();
}

bool Editor::syntaxHighlight() const
{
    return d->syntaxHighlightEnabled;
}

void Editor::setAnsAvailable( bool avail )
{
    d->ansAvailable = avail;
}

void Editor::stopAutoCalc()
{
    d->autoCalcTimer->stop();
    d->autoCalcSelTimer->stop();
    emit autoCalcDisabled();
}

void Editor::stopAutoComplete()
{
    d->completionTimer->stop();
    d->completion->selectItem( QString() ); // WORKAROUND 76
    d->completion->doneCompletion();
    setFocus();
}

// uncomment to activate fade-away effect when the completion pop-up disappears
//#define COMPLETION_FADE_EFFECT

struct EditorCompletion::Private
{
    Editor * editor;
#ifdef COMPLETION_FADE_EFFECT
    QTimeLine * fader;
#endif
    QTreeWidget * popup;
};

EditorCompletion::EditorCompletion( Editor * editor )
    : QObject( editor ), d( new EditorCompletion::Private )
{
    d->editor = editor;

    d->popup = new QTreeWidget;
    d->popup->setFrameShape( QFrame::NoFrame );
    d->popup->setColumnCount( 2 );
    d->popup->setRootIsDecorated( false );
    d->popup->header()->hide();
    d->popup->setEditTriggers( QTreeWidget::NoEditTriggers );
    d->popup->setSelectionBehavior( QTreeWidget::SelectRows );
    d->popup->setMouseTracking( true );
    d->popup->installEventFilter( this );

    connect( d->popup, SIGNAL(itemClicked(QTreeWidgetItem *, int)), SLOT(doneCompletion()) );

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
}

bool EditorCompletion::eventFilter( QObject * obj, QEvent * ev )
{
    if ( obj == d->popup ) {
        if ( ev->type() == QEvent::KeyPress ) {
            QKeyEvent * ke = (QKeyEvent *) ev;
            if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return
                 || ke->key() == Qt::Key_Tab )
            {
                doneCompletion();
                return true;
            } else if ( ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down
                        || ke->key() == Qt::Key_Home || ke->key() == Qt::Key_End
                        || ke->key() == Qt::Key_PageUp || ke->key() == Qt::Key_PageDown )
            {
                return false;
            }

            d->popup->hide();
            d->editor->setFocus();
            if ( ke->key() != Qt::Key_Escape )
                QApplication::sendEvent( d->editor, ev );
            return true;
        } else if ( ev->type() == QEvent::MouseButtonPress ) {
            d->popup->hide();
            d->editor->setFocus();
            return true;
        }
    }

    return false;
}

void EditorCompletion::doneCompletion()
{
#ifdef COMPLETION_FADE_EFFECT
    d->fader->start();
    QTimer::singleShot( 750, d->popup, SLOT(hide()) ); // sentinel
#else
    d->popup->hide();
#endif
    d->editor->setFocus();
    QTreeWidgetItem * item = d->popup->currentItem();
    emit selectedCompletion( item ? item->text(0) : QString() );
}

void EditorCompletion::showCompletion( const QStringList & choices )
{
    if ( ! choices.count() )
        return;

#ifdef COMPLETION_FADE_EFFECT
    d->fader->stop();
    fade( 0 );
#endif

    d->popup->setUpdatesEnabled( false );
    d->popup->clear();
    for ( int i = 0; i < choices.count(); ++i ) {
        QTreeWidgetItem * item = new QTreeWidgetItem( d->popup, choices.at(i).split(':') );
        if ( item && d->editor->layoutDirection() == Qt::RightToLeft )
            item->setTextAlignment( 0, Qt::AlignRight );
    }
    d->popup->sortItems( 0, Qt::AscendingOrder );
    d->popup->sortItems( 1, Qt::AscendingOrder );
    d->popup->setCurrentItem( d->popup->topLevelItem( 0 ) );
    d->popup->adjustSize();
    d->popup->setUpdatesEnabled( true );

    // size of the pop-up
    d->popup->adjustSize();
    const int h = d->popup->sizeHintForRow( 0 ) * qMin( 7, choices.count() ) + 3;
    const int w = d->popup->width();
    d->popup->resize( w, h );

    // position, reference is editor's cursor position in global coord
    QFontMetrics fm( d->editor->font() );
    const int curPos = d->editor->textCursor().position();
    const int pixelsOffset = fm.width( d->editor->text(), curPos );
    QPoint pos = d->editor->mapToGlobal( QPoint( pixelsOffset, d->editor->height() ) );

    // if popup is partially invisible, move to other position
    const QRect screen = QApplication::desktop()->availableGeometry( d->editor );
    if ( pos.y() + h > screen.y() + screen.height() )
        pos.setY( pos.y() - h - d->editor->height() );
    if ( pos.x() + w > screen.x() + screen.width() )
        pos.setX( screen.x() + screen.width() - w );

    d->popup->move( pos );
    d->popup->setFocus();
    d->popup->show();
}

void EditorCompletion::selectItem( const QString & item ) // WORKAROUND 76
{
    if ( item.isNull() )
        d->popup->setCurrentItem( 0 );
    else {
        QList<QTreeWidgetItem *> targets = d->popup->findItems( item, Qt::MatchExactly );
        if ( targets.count() > 0 )
            d->popup->setCurrentItem( targets.at(0) );
    }
}

void EditorCompletion::fade( int v )
{
    d->popup->setWindowOpacity( (qreal)(100-v) / 100 );
}

struct ConstantCompletion::Private
{
    QTreeWidget * categoryList;
    QList<Constant> constants;
    Editor * editor;
    QString lastCategory;
    QTreeWidget * list;
    QFrame * popup;
    QTimeLine * slider;
};

ConstantCompletion::ConstantCompletion( Editor * editor )
    : QObject( editor ), d( new ConstantCompletion::Private )
{
    d->editor = editor;

    d->popup = new QFrame;
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
    d->categoryList->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

    connect( d->categoryList, SIGNAL(itemClicked(QTreeWidgetItem *, int)), SLOT(showConstants()) );

    d->list = new QTreeWidget( d->popup );
    d->list->setFrameShape( QFrame::NoFrame );
    d->list->setColumnCount( 2 );
    d->list->setColumnHidden( 1, true );
    d->list->setRootIsDecorated( false );
    d->list->header()->hide();
    d->list->setEditTriggers( QTreeWidget::NoEditTriggers );
    d->list->setSelectionBehavior( QTreeWidget::SelectRows );
    d->list->setMouseTracking( true );
    d->list->installEventFilter( this );
    d->list->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

    // FIXME: why does it crash when clicking a constant description?
    connect( d->list, SIGNAL(itemClicked(QTreeWidgetItem *, int)), SLOT(doneCompletion()) );

    d->slider = new QTimeLine( 250, d->popup );
    d->slider->setCurveShape( QTimeLine::EaseInCurve );
    connect( d->slider, SIGNAL(frameChanged(int)), SLOT(slide(int)) );

    const Constants * ct = Constants::instance();
    d->constants = ct->list();

    // populate categories
    QStringList str;
    str << tr( "All" );
    QTreeWidgetItem * all = new QTreeWidgetItem( d->categoryList, str );
    for ( int k = 0; k < ct->categories().count(); ++k ) {
        str.clear();
        str << ct->categories().at( k );
        new QTreeWidgetItem( d->categoryList, str );
    }
    d->categoryList->setCurrentItem( all );

    // populate constants
    d->lastCategory = tr( "All" );
    for ( int k = 0; k < ct->list().count(); ++k ) {
        QStringList str;
        str << ct->list().at( k ).name;
        str << ct->list().at( k ).name.toUpper();
        new QTreeWidgetItem( d->list, str );
    }
    d->list->sortItems( 0, Qt::AscendingOrder );

    // find size, the biggest between both
    d->list->resizeColumnToContents( 0 );
    d->categoryList->resizeColumnToContents( 0 );
    int ww = qMax( d->list->width(), d->categoryList->width() );
    const int h1 = d->list->sizeHintForRow( 0 ) * qMin( 7, d->constants.count() ) + 3;
    const int h2 = d->categoryList->sizeHintForRow( 0 ) * qMin( 7, ct->categories().count() ) + 3;
    const int hh = qMax( h1, h2 );
    ww += 200; // extra space (FIXME scrollbar size?)

    // adjust dimensions
    d->popup->resize( ww, hh );
    d->list->resize( ww, hh );
    d->categoryList->resize( ww, hh );
}

ConstantCompletion::~ConstantCompletion()
{
    delete d->popup;
    d->editor->setFocus();
}

void ConstantCompletion::showCategory()
{
    d->slider->setFrameRange( d->popup->width(), 0 );
    d->slider->stop();
    d->slider->start();
    d->categoryList->setFocus();
}

void ConstantCompletion::showConstants()
{
    d->slider->setFrameRange( 0, d->popup->width() );
    d->slider->stop();
    d->slider->start();
    d->list->setFocus();

    QString chosenCategory;
    if ( d->categoryList->currentItem() )
        chosenCategory = d->categoryList->currentItem()->text( 0 );

    if ( d->lastCategory == chosenCategory )
        return;

    d->list->clear();
    for ( int k = 0; k < d->constants.count(); ++k ) {
        QStringList str;
        str << d->constants.at(k).name;
        str << d->constants.at(k).name.toUpper();

        const bool include = (chosenCategory == tr( "All" )) ?
            true : (d->constants.at(k).category == chosenCategory);

        if ( ! include )
            continue;

        new QTreeWidgetItem( d->list, str );
    }
    d->list->sortItems( 0, Qt::AscendingOrder );
    d->list->setCurrentItem( d->list->itemAt(0, 0) );
    d->lastCategory = chosenCategory;
}

bool ConstantCompletion::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::Hide ) {
        emit canceledCompletion();
        return true;
    }

    if ( obj == d->list ) {
        if ( ev->type() == QEvent::KeyPress ) {
            QKeyEvent * ke = (QKeyEvent *) ev;
            if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return
                 || ke->key() == Qt::Key_Tab )
            {
                doneCompletion();
                return true;
            } else if ( ke->key() == Qt::Key_Left ) {
                showCategory();
                return true;
            } else if ( ke->key() == Qt::Key_Right || ke->key() == Qt::Key_Up
                        || ke->key() == Qt::Key_Down || ke->key() == Qt::Key_Home
                        || ke->key() == Qt::Key_End || ke->key() == Qt::Key_PageUp
                        || ke->key() == Qt::Key_PageDown )
            {
                return false;
            }

            if ( ke->key() != Qt::Key_Escape )
                QApplication::sendEvent( d->editor, ev );
            emit canceledCompletion();
            return true;
        }
    }

    if ( obj == d->categoryList ) {
        if ( ev->type() == QEvent::KeyPress ) {
            QKeyEvent * ke = (QKeyEvent *) ev;
            if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return
                 || ke->key() == Qt::Key_Right )
            {
                showConstants();
                return true;
            } else if ( ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down
                        || ke->key() == Qt::Key_Home || ke->key() == Qt::Key_End
                        || ke->key() == Qt::Key_PageUp || ke->key() == Qt::Key_PageDown )
            {
                return false;
            }

            if ( ke->key() != Qt::Key_Escape )
                QApplication::sendEvent( d->editor, ev );
            emit canceledCompletion();
            return true;
        }
    }

    return false;
}

void ConstantCompletion::doneCompletion()
{
    d->editor->setFocus();
    const QTreeWidgetItem * item = d->list->currentItem();
    emit selectedCompletion( item ?
            std::find_if(d->constants.begin(), d->constants.end(),
                constant_name_is(item->text( 0 )))->value
            : QString() );
}

void ConstantCompletion::showCompletion()
{
    // position, reference is editor's cursor position in global coord
    QFontMetrics fm( d->editor->font() );
    const int curPos = d->editor->textCursor().position();
    const int pixelsOffset = fm.width( d->editor->text(), curPos );
    QPoint pos = d->editor->mapToGlobal( QPoint( pixelsOffset, d->editor->height() ) );

    const int h = d->popup->height();
    const int w = d->popup->width();

    // if popup is partially invisible, move to other position
    const QRect screen = QApplication::desktop()->availableGeometry( d->editor );
    if ( pos.y() + h > screen.y() + screen.height() )
        pos.setY( pos.y() - h - d->editor->height() );
    if ( pos.x() + w > screen.x() + screen.width() )
        pos.setX( screen.x() + screen.width() - w );

    // start with category
    d->categoryList->setFocus();
    slide( 0 );

    d->popup->move( pos );
    d->popup->show();
}

void ConstantCompletion::slide( int v )
{
    d->categoryList->move( -v, 0 );
    d->list->move( d->popup->width() - v, 0 );
}

