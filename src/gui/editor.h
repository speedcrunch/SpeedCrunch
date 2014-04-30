// This file is part of the SpeedCrunch project
// Copyright (C) 2004, 2005, 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2009, 2013, 2014 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef GUI_EDITOR_H
#define GUI_EDITOR_H

#include <QPlainTextEdit>

struct Constant;
class ConstantCompletion;
class EditorCompletion;
class Evaluator;
class HNumber;
class SyntaxHighlighter;

class QEvent;
class QKeyEvent;
class QMimeData;
class QTimeLine;
class QTreeWidget;
class QWheelEvent;
class QWidget;

class Editor : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit Editor(QWidget* parent = 0);

    bool isAutoCalcEnabled() const;
    bool isAutoCompletionEnabled() const;
    void clearHistory();
    int cursorPosition() const;
    void doBackspace();
    QStringList history() const;
    QStringList historyResults() const;
    char radixChar() const;
    void setAnsAvailable(bool);
    void setAutoCalcEnabled(bool);
    void setAutoCompletionEnabled(bool);
    void setCursorPosition(int pos);
    void setText(const QString&);
    void setHistory(const QStringList&);
    void setHistoryResults(const QStringList&);
    QSize sizeHint() const;
    void stopAutoCalc();
    void stopAutoComplete();
    QString text() const;

signals:
    void autoCalcEnabled(const QString&);
    void autoCalcDisabled();
    void controlPageDownPressed();
    void controlPageUpPressed();
    void copySequencePressed();
    void pageDownPressed();
    void pageUpPressed();
    void returnPressed();
    void shiftDownPressed();
    void shiftUpPressed();
    void shiftPageDownPressed();
    void shiftPageUpPressed();

public slots:
    void appendHistory(const QString& result, const QString& expression);
    void appendHistory(const QStringList& expressions, const QStringList& results);
    void cancelConstantCompletion();
    void evaluate();
    void insert(const QString&);
    void insertConstant(const QString&);
    void rehighlight();

protected slots:
    virtual void insertFromMimeData(const QMimeData*);
    void autoCalc();
    void autoCalcSelection();
    void autoComplete(const QString&);
    void checkAutoCalc();
    void checkAutoComplete();
    void checkMatching();
    void doMatchingLeft();
    void doMatchingPar();
    void doMatchingRight();
    void historyBack();
    void historyForward();
    void startSelAutoCalcTimer();
    void triggerAutoComplete();
    void triggerEnter();

protected:
    virtual void focusOutEvent(QFocusEvent*);
    virtual void keyPressEvent(QKeyEvent*);
    virtual void paintEvent(QPaintEvent*);
    virtual void wheelEvent(QWheelEvent*);

private:
    Q_DISABLE_COPY(Editor)

    bool m_isAnsAvailable;
    bool m_isAutoCalcEnabled;
    QTimer* m_autoCalcSelTimer;
    QTimer* m_autoCalcTimer;
    bool m_isAutoCompletionEnabled;
    EditorCompletion* m_completion;
    QTimer* m_completionTimer;
    ConstantCompletion* m_constantCompletion;
    Evaluator* m_evaluator;
    SyntaxHighlighter* m_highlighter;
    QStringList m_history;
    QStringList m_historyResults;
    QString m_savedCurrentEditor;
    int m_currentHistoryIndex;
    QTimer* m_matchingTimer;
    bool m_shouldPaintCustomCursor;
};

class EditorCompletion : public QObject {
    Q_OBJECT

public:
    EditorCompletion(Editor*);
    ~EditorCompletion();

    bool eventFilter(QObject*, QEvent*);
    void showCompletion(const QStringList&);

signals:
    void selectedCompletion(const QString&);

public slots:
    void doneCompletion();
    void selectItem(const QString&);

private:
    Q_DISABLE_COPY(EditorCompletion)

    Editor* m_editor;
    QTreeWidget* m_popup;
};

class ConstantCompletion : public QObject {
    Q_OBJECT

public:
    ConstantCompletion(Editor*);
    ~ConstantCompletion();

    bool eventFilter(QObject*, QEvent*);
    void showCompletion();

signals:
    void canceledCompletion();
    void selectedCompletion(const QString&);

public slots:
    void doneCompletion();

protected slots:
    void setHorizontalPosition(int);
    void showCategory();
    void showConstants();

private:
    Q_DISABLE_COPY(ConstantCompletion)

    QTreeWidget* m_categoryWidget;
    QList<Constant> m_constantList;
    Editor* m_editor;
    QString m_lastCategory;
    QTreeWidget* m_constantWidget;
    QFrame* m_popup;
    QTimeLine* m_slider;
};

#endif
