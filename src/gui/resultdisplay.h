// This file is part of the SpeedCrunch project
// Copyright (C) 2009, 2011, 2013 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef GUI_RESULTDISPLAY_H
#define GUI_RESULTDISPLAY_H

#include <QtCore/QBasicTimer>
#include <QtGui/QPlainTextEdit>

class HNumber;
class FadeOverlay;
class SyntaxHighlighter;

class ResultDisplay : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit ResultDisplay(QWidget* parent = 0);

    void append(const QString& expr, const HNumber& value);
    void appendHistory(const QStringList& expressions, const QStringList& results);
    int count() const;
    bool isEmpty() const { return m_count == 0; }

signals:
    void shiftControlWheelDown();
    void shiftControlWheelUp();
    void textCopied(const QString&);
    void textSelected(const QString&);

public slots:
    void clear();
    void rehighlight();
    void refresh();
    void scrollLines(int);
    void scrollLineUp();
    void scrollLineDown();
    void scrollPageUp();
    void scrollPageDown();
    void scrollToBottom();
    void scrollToTop();
    void zoomIn();
    void zoomOut();

protected:
    virtual void wheelEvent(QWheelEvent*);
    virtual void resizeEvent(QResizeEvent*);
    virtual void timerEvent(QTimerEvent*);
    void fullContentScrollEvent();
    float linesPerPage() { return static_cast<float>(viewport()->height()) / fontMetrics().height(); }
    void pageScrollEvent();
    void scrollToDirection(int);
    void stopActiveScrollingAnimation();
    void updateScrollBarStyleSheet();

private:
    Q_DISABLE_COPY(ResultDisplay)

    friend class FadeOverlay;

    int m_count;
    QStringList m_expressions;
    SyntaxHighlighter* m_highlighter;
    QStringList m_results;
    FadeOverlay* m_fadeOverlay;
    QBasicTimer m_scrollTimer;
    int m_scrolledLines;
    int m_scrollDirection;
    bool m_isScrollingPageOnly;
};

#endif