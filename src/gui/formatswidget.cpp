// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008, 2009, 2010, 2011 Helder Correia <helder.pereira.correia@gmail.com>
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

// TODO:
// - dvojkovy doplnek
// - hex/bin/oct - only for whole part of the number
// - autocalc disabled

#include <QEvent>
#include <QLabel>
#include <QPlainTextEdit>
#include <QGridLayout>
#include <QSize>

#include "gui/formatswidget.h"
#include "core/settings.h"
#include "math/hmath.h"
#include "syntaxhighlighter.h"

//------------------------------------------------------------------------------

class FormatLabel : public QPlainTextEdit
{
public:

    FormatLabel(QWidget *parent, char format)
    : QPlainTextEdit(parent),
      format(format)
    {
        setReadOnly(true);
        setFrameStyle(QFrame::NoFrame);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        setTabChangesFocus(true);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        adjustSize();
        setFixedHeight(sizeHint().height());
        setLineWrapMode(QPlainTextEdit::NoWrap);

        m_highlighter = new SyntaxHighlighter(this);
    }

    virtual QSize sizeHint() const
    {
        ensurePolished();
        const QFontMetrics metrics = fontMetrics();
        const int width = metrics.width('x') * 10;
        const int height = metrics.lineSpacing() + 10;
        return QSize(width, height);
    }

public slots:

    void updateNumber(const HNumber& number)
    {
      QString str = HMath::format(number, format);
      setPlainText("= " + str);

      if (Settings::instance()->radixCharacter() != '.')
        handleRadixCharacterChange();
    }

    void rehighlight()
    {
        m_highlighter->update();
    }

    void handleRadixCharacterChange()
    {
      const char radixChar = Settings::instance()->radixCharacter();
      setPlainText(toPlainText().replace(QRegExp("[,.]"), QString(radixChar)));
    }

protected:

    virtual void changeEvent(QEvent *e)
    {
        if (e->type() == QEvent::FontChange)
            setFixedHeight(sizeHint().height());
        QPlainTextEdit::changeEvent(e);
    }

private:

    SyntaxHighlighter *m_highlighter;
    char format;

}; // FmtText

//------------------------------------------------------------------------------

void FormatsWidget::createLblFmt(QLabel*& lbl, FormatLabel*& fmt, char format)
{
    lbl = new QLabel(this);
    fmt = new FormatLabel(this, format);

    m_fmtLbls.append(fmt);

    int row = layout->rowCount();
    layout->addWidget(lbl, row, 1);
    layout->addWidget(fmt, row, 0);
}

FormatsWidget::FormatsWidget(QWidget *parent)
  : QWidget(parent)
{
    layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->setVerticalSpacing(0);

    createLblFmt(m_lblDec, m_fmtDec, 'f');
    createLblFmt(m_lblHex, m_fmtHex, 'h');
    createLblFmt(m_lblOct, m_fmtOct, 'o');
    createLblFmt(m_lblBin, m_fmtBin, 'b');
    createLblFmt(m_lblEng, m_fmtEng, 'n');
    createLblFmt(m_lblSci, m_fmtSci, 'e');

    // add empty row eating as much as possible (no spacing between format rows)
    int row = layout->rowCount();
    layout->addWidget(new QWidget(), row, 0, -1, -1);
    layout->setRowStretch(row, 1);

    retranslateText();
}

void FormatsWidget::updateNumber(const HNumber& number)
{
    for (int i = 0; i <  m_fmtLbls.size(); i++)
        m_fmtLbls.at(i)->updateNumber(number);
}

void FormatsWidget::retranslateText()
{
    m_lblDec->setText(tr("Dec"));
    m_lblHex->setText(tr("Hex"));
    m_lblOct->setText(tr("Oct"));
    m_lblBin->setText(tr("Bin"));
    m_lblEng->setText(tr("Eng"));
    m_lblSci->setText(tr("Sci"));
}

void FormatsWidget::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        setLayoutDirection(Qt::LeftToRight);
        retranslateText();
        break;

    case QEvent::FontChange:
        for (int i = 0; i <  m_fmtLbls.size(); i++)
            m_fmtLbls.at(i)->setFont(font());
    default:
        ;
    }

    QWidget::changeEvent(e);
}

void FormatsWidget::setNumberFont(const QFont& f)
{
  setFont(f);
}

void FormatsWidget::rehighlight()
{
    for (int i = 0; i <  m_fmtLbls.size(); i++)
        m_fmtLbls.at(i)->rehighlight();

    //~ setStyleSheet(QString(
        //~ "QPlainTextEdit {"
        //~ "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %2, stop: 1 %1);"
        //~ "}"
    //~ ).arg(m_highlighter->colorForRole(SyntaxHighlighter::Background).name())
     //~ .arg(m_highlighter->colorForRole(SyntaxHighlighter::EditorFade).name())
    //~ );
}


void FormatsWidget::handleRadixCharacterChange()
{
    for (int i = 0; i <  m_fmtLbls.size(); i++)
        m_fmtLbls.at(i)->handleRadixCharacterChange();
}
