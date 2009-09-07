// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
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
//

#include "gui/syntaxhighlighter.hxx"

#include "core/evaluator.hxx"
#include "core/functions.hxx"
#include "core/settings.hxx"
#include "gui/textedit.hxx"

#include <QtCore/QLatin1String>
#include <QtGui/QApplication>
#include <QtGui/QPalette>

struct SyntaxHighlighter::Private
{
    QMap<SyntaxHighlighter::Role, QColor> colors;
    SyntaxHighlighter::Scheme scheme;

    QList<QColor> generateColors( const QColor & bg, const QColor & fg, int noColors )
    {
        QList<QColor> cols;
        const int HUE_BASE = (bg.hue() == -1) ? 90 : bg.hue();
        int h, s, v;

        for ( int i = 0; i < noColors; ++i ) {
            // generate "normal" colors
            h = int( HUE_BASE + (360.0 / noColors * i) ) % 360;
            s = 240;
            v = int( qMax(bg.value(), fg.value()) * 0.85 );

            // adjust particular cases
            const int M = 35;
            if ( (h < bg.hue() + M && h > bg.hue() - M )
                 || (h < fg.hue() + M && h > fg.hue() - M ) )
            {
                h = ((bg.hue() + fg.hue()) / (i+1)) % 360;
                s = ((bg.saturation() + fg.saturation() + 2*i) / 2) % 256;
                v = ((bg.value() + fg.value() + 2*i) / 2) % 256;
            }

            // insert into result list
            cols.append( QColor::fromHsv(h,s,v) );
        }

        return cols;
    }
};

SyntaxHighlighter::SyntaxHighlighter( TextEdit * e )
    : QSyntaxHighlighter( e ), d( new SyntaxHighlighter::Private )
{
    setDocument( e->document() );
    setScheme();
}

void SyntaxHighlighter::highlightBlock( const QString & text )
{
    if ( ! Settings::instance()->syntaxHighlighting ) {
        QWidget * widget = static_cast<QWidget *>( parent() );
        setFormat( 0, text.length(), widget->palette().text().color() );
        return;
    }

    if ( text.startsWith(QLatin1String("=")) ) {
        QFont font;
        font.setPointSize( font.pointSize() + 2 );
        setFormat( 0, text.length(), font );
        return;
    }

    Tokens tokens = Evaluator::instance()->scan( text );
    for ( int i = 0; i < tokens.count(); ++i )
    {
        const Token & token = tokens.at( i );
        const QString text = token.text().toLower();
        QStringList fnames = Functions::instance()->names();
        QColor col;

        switch ( token.type() )
        {
            case Token::stxOperator:
            case Token::stxUnknown:
            case Token::stxOpenPar:
            case Token::stxClosePar:
                col = QApplication::palette().windowText().color();
                break;

            case Token::stxNumber:
            case Token::stxSep:
                col = color( SyntaxHighlighter::Number );
                break;

            case Token::stxIdentifier:
                col = color( SyntaxHighlighter::Variable );
                for ( int i = 0; i < fnames.count(); ++i )
                    if ( fnames.at(i).toLower() == text )
                        col = color( SyntaxHighlighter::Function );
                break;

            default:
                break;
        };

        if ( token.pos() >= 0 )
            setFormat( token.pos(), token.text().length(), col );
    }
}

void SyntaxHighlighter::setScheme( SyntaxHighlighter::Scheme hs )
{
    d->scheme = hs;

    if ( hs == AutoScheme ) {
        const int NO_COLORS = 3;
        QWidget * widget = static_cast<QWidget *>( parent() );
        const QColor bg = widget->palette().color( QPalette::Base );
        const QColor fg = widget->palette().color( QPalette::Text );
        const QList<QColor> list = d->generateColors( bg, fg, NO_COLORS );
        for ( int i = 0; i < NO_COLORS; ++i )
            d->colors[ static_cast<SyntaxHighlighter::Role>(i) ] = list.at( i );

        // generate special case matched parenthesis
        const int h = ((bg.hue() + fg.hue()) / 2) % 359;
        const int s = ((bg.saturation() + fg.saturation()) / 2) % 255;
        const int v = ((bg.value() + fg.value()) / 2) % 255;
        d->colors[SyntaxHighlighter::MatchedPar] = QColor::fromHsv( h, s, v );
    }
}

QColor SyntaxHighlighter::color( Role r )
{
    return d->colors.value( r );
}

void SyntaxHighlighter::setColor( Role r, QColor c )
{
    d->colors[r] = c;
    rehighlight();
}

