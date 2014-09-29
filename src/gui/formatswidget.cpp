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

#include <QEvent>
#include <QPushButton>
#include <QToolButton>
#include <QPlainTextEdit>
#include <QGridLayout>
#include <QSize>

#include "gui/formatswidget.h"
#include "core/settings.h"
#include "syntaxhighlighter.h"

//------------------------------------------------------------------------------

FormatLabel::FormatLabel(QWidget *parent, char& format)
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

QSize FormatLabel::sizeHint() const
{
    ensurePolished();
    const QFontMetrics metrics = fontMetrics();
    const int width = metrics.width('x') * 10;
    const int height = metrics.lineSpacing() + 10;
    return QSize(width, height);
}

void FormatLabel::updateNumber(const HNumber& number)
{
  this->number = number;
  QString prefix("= ");

  switch (format) {
  case 'H':
  case 'O':
  case 'B':
      if (!number.isNan() && !number.isInteger()) {
          // show warning - truncated to integer part
          prefix = QString::fromUtf8("≈ ");
      }
      break;
  default:
      break;
  }

  QString str = HMath::format(number, format);
  setPlainText(prefix + str);

  if (Settings::instance()->radixCharacter() != '.')
    handleRadixCharacterChange();
}

void FormatLabel::rehighlight()
{
    m_highlighter->update();
}

void FormatLabel::handleRadixCharacterChange()
{
  const char radixChar = Settings::instance()->radixCharacter();
  setPlainText(toPlainText().replace(QRegExp("[,.]"), QString(radixChar)));
}

// switches between H-h, O-o and B-b formats
void FormatLabel::setAltFormat(bool altFormat)
{
  switch (tolower(format)) {
  case 'h': format = (altFormat ? 'H' : 'h'); updateNumber(number); break;
  case 'o': format = (altFormat ? 'O' : 'o'); updateNumber(number); break;
  case 'b': format = (altFormat ? 'B' : 'b'); updateNumber(number); break;
  case 'e':
  case 'n': format = (altFormat ? 'n' : 'e'); updateNumber(number); break;
  default:
      break;
  }
}

void FormatLabel::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::FontChange)
        setFixedHeight(sizeHint().height());
    QPlainTextEdit::changeEvent(e);
}

void FormatLabel::focusOutEvent(QFocusEvent* e)
{
    if (e->lostFocus())
    {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::End);
        setTextCursor(cursor);
    }
    QPlainTextEdit::focusOutEvent(e);
}


//------------------------------------------------------------------------------

void FormatsWidget::createLblFmt(QAction*& act, FormatLabel*& fmt, char& format)
{
    act = new QAction(this);
    fmt = new FormatLabel(this, format);

    connect(act, SIGNAL(toggled(bool)), fmt, SLOT(setAltFormat(bool)));

    QToolButton *btn = new QToolButton(this);

    btn->setDefaultAction(act);
    btn->setAutoRaise(true);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    switch (tolower(format)) {
    case 'h':
    case 'o':
    case 'b':
        act->setCheckable(true);
        if (tolower(format) != format)
          act->setChecked(true);
        break;
    case 'e':
    case 'n':
        act->setCheckable(true);
        if (format == 'n')
          act->setChecked(true);
        break;
    default:
        break;
    }

    m_fmtLbls.append(fmt);

    int row = layout->rowCount();
    layout->addWidget(btn, row, 1);
    layout->addWidget(fmt, row, 0);
}

FormatsWidget::FormatsWidget(QWidget *parent)
  : QWidget(parent)
{
    layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->setVerticalSpacing(0);
    layout->setHorizontalSpacing(0);
    layout->setColumnStretch(0, 1);

    static char formatsDec = 'f';
    createLblFmt(m_actDec, m_fmtDec, formatsDec);
    createLblFmt(m_actHex, m_fmtHex, Settings::instance()->formatsHex);
    createLblFmt(m_actOct, m_fmtOct, Settings::instance()->formatsOct);
    createLblFmt(m_actBin, m_fmtBin, Settings::instance()->formatsBin);
    createLblFmt(m_actExp, m_fmtExp, Settings::instance()->formatsExp);

    // add empty row eating as much as possible (no spacing between format rows)
    QWidget *spacer = new QWidget();
    int row = layout->rowCount();
    layout->addWidget(spacer, row, 0, -1, -1);
    layout->setRowStretch(row, 1);

    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    retranslateText();
}

void FormatsWidget::updateNumber(const HNumber& number)
{
    for (int i = 0; i <  m_fmtLbls.size(); i++)
        m_fmtLbls.at(i)->updateNumber(number);
}

void FormatsWidget::retranslateText()
{
    QString tooltip = tr("Toggle between default and two's complement format");

    m_actDec->setText(tr("Dec"));
    m_actHex->setText(tr("Hex"));  m_actHex->setToolTip(tooltip);
    m_actOct->setText(tr("Oct"));  m_actOct->setToolTip(tooltip);
    m_actBin->setText(tr("Bin"));  m_actBin->setToolTip(tooltip);
    m_actExp->setText(tr("Exp"));  m_actExp->setToolTip(tr("Toggle between scientific and engineering format"));
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

void FormatsWidget::increaseFontPointSize()
{
    QFont newFont = font();
    const int newSize = newFont.pointSize() + 1;
    if (newSize > 96)
        return;
    newFont.setPointSize(newSize);
    setNumberFont(newFont);
}

void FormatsWidget::decreaseFontPointSize()
{
    QFont newFont = font();
    const int newSize = newFont.pointSize() - 1;
    if (newSize < 8)
        return;
    newFont.setPointSize(newSize);
    setNumberFont(newFont);
}

void FormatsWidget::rehighlight()
{
    for (int i = 0; i <  m_fmtLbls.size(); i++)
        m_fmtLbls.at(i)->rehighlight();
}


void FormatsWidget::handleRadixCharacterChange()
{
    for (int i = 0; i <  m_fmtLbls.size(); i++)
        m_fmtLbls.at(i)->handleRadixCharacterChange();
}
