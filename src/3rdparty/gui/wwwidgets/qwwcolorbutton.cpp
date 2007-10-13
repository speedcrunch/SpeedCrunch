//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef WW_NO_COLORBUTTON
#include "qwwcolorbutton.h"
#include <QStandardItemModel>
#include <QAbstractItemDelegate>
#include <QPainter>
#include <QListView>
#include <QLayout>
#include <cmath>
#include <QScrollBar>
#include <QColorDialog>
#include "wwglobal_p.h"
#include <QtDebug>
#include <QEvent>
#include <QApplication>
#include <QDrag>
#include <QMouseEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDesktopWidget>
#include "colormodel.h"

/**
 *  @todo Change ToolTipRole to DisplayRole
 *  @todo Change setFixedSize of the popup to setGeometry
 */


/**
 *  \internal
 */
class ColorDelegate : public QAbstractItemDelegate {
public:
    ColorDelegate(QObject *parent = 0) : QAbstractItemDelegate(parent) {}
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const {
        if (!index.isValid()) return;
        QColor c = qvariant_cast<QColor>(index.data(Qt::DecorationRole));
        painter->save();
        painter->setBrush(c);
        painter->drawRect(option.rect);
        if (option.state & QStyle::State_Selected) {
            QPen p = painter->pen();
            p.setWidth(2);
            painter->setPen(p);
            painter->drawRect(option.rect.adjusted(1,1,0,0));
        }
        painter->restore();

    }
    QSize sizeHint ( const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/ ) const {
        return QSize(16,16);
    }
};

/**
 *  \internal
 */
class QwwColorPopup : public QWidget {
public:
    QwwColorPopup(ColorModel *model, QWidget *parent = 0) : QWidget(parent, Qt::Popup) {
        QVBoxLayout *l = new QVBoxLayout(this);
        l->setSpacing(0);
        l->setMargin(0);
        view = new QListView;
        view->setFlow(QListView::LeftToRight);
        view->setResizeMode(QListView::Adjust);
        view->setWrapping(true);
        view->setUniformItemSizes(true);
        view->setMouseTracking(true);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        l->addWidget(view);
        button = new QPushButton;
        button->setText(tr("Other"));
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        l->addWidget(button);
        view->setItemDelegate(new ColorDelegate(this));
        view->setModel(model);
        if (parent)
            connect(button, SIGNAL(clicked()), parent, SLOT(_q_colorDialogRequested()));
        connect(view, SIGNAL(entered(const QModelIndex &)), parent, SLOT(_q_setCurrentIndex(const QModelIndex &)));
        setMaximumHeight(250);
    }
    QListView *colorView() const {
        return view;
    }

private:
    QListView *view;
    QPushButton *button;
};

/**
 *  \internal
 */
class QwwColorButtonPrivate : public QwwPrivate {
public:
    QwwColorButtonPrivate(QwwColorButton *q) : QwwPrivate(q) {
        popup = 0;
        model = 0;
        showName = true;
        dragEnabled = false;
    }
    QSize sizeHint() const {
        Q_Q(const QwwColorButton);
        if (!model)
            return QSize(80,100);

        int margin = 2*q->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, q)+1;
        int cols = model->rowCount();
        int fit = qMax(3, (q->width()-margin)/16) ; // this no fits in the button width
        int fitRows = cols % fit == 0 ? cols/fit : (1+cols/fit); // this no of rows is needed to show all colors
        int scExt = q->style()->pixelMetric(QStyle::PM_ScrollBarExtent, 0, q);
        if(fitRows<=12)
            return QSize(fit*16+margin, (fitRows)*16+margin+27);
        // let's try with a scroll bar now
        fit = qMax(3, (q->width()-margin-scExt)/16);
        fitRows = cols/fit;
        return QSize(fit*16+margin+scExt+1, 16*12+margin+30);

    }
    QwwColorPopup *popup;
    ColorModel *model;
    bool showName;
    QColor curCol;
    QString curNam;
    bool dragEnabled;
    QPoint pressPos;
    WW_DECLARE_PUBLIC(QwwColorButton);
};

/**
 *  @class  QwwColorButton
 *  @brief  Button acting as a color chooser
 *
 *          This widget provides a push button that can be used
 *          to choose a color from either a predefined set
 *          or a custom one using a QColorDialog.
 *
 *          Active color can be changed either by clicking the button
 *          and choosing a different entry or by dropping a color from
 *          another widget or another application.
 *
 *  @image html images/qwwcolorbutton.png "QwwColorButton"
 *
 *          Colors can be added to the button with addColor(), setColors()
 *          or setStandardColors().
 *
 *
 *
 * @fn      QwwColorButton::colorPicked(QColor)
 * @brief   a color has been chosen
 *
 *          This signal is emitted when a color is picked and displayed
 *          on the button
 *
 */

QwwColorButton::QwwColorButton(QWidget *parent)
        : QPushButton(parent), QwwPrivatable(new QwwColorButtonPrivate(this)) {
    Q_D(QwwColorButton);
    setAcceptDrops(true);
    d->model = new ColorModel(this);
    connect(this, SIGNAL(clicked()), SLOT(_q_clicked()));
    setStandardColors();
    _q_activated(d->model->index(0,0));
}


struct QEffects {
    enum Direction {
        LeftScroll  = 0x0001,
        RightScroll = 0x0002,
        UpScroll    = 0x0004,
        DownScroll  = 0x0008
    };

    typedef uint DirFlags;
};

// void qFadeEffect(QWidget*, int time = -1);
void qScrollEffect(QWidget*, QEffects::DirFlags dir = QEffects::DownScroll, int time = -1);

/**
 * \internal
 */
void QwwColorButton::_q_clicked() {
    Q_D(QwwColorButton);
    if (!d->popup) {
        d->popup = new QwwColorPopup(d->model, this);
        connect(d->popup->colorView(), SIGNAL(clicked(const QModelIndex&)), SLOT(_q_activated(const QModelIndex&)));
    }
    QPoint p = rect().bottomLeft();
    p = mapToGlobal(p);
    QRect avail = QDesktopWidget().availableGeometry();
    int hei = avail.height()-p.y();
    d->popup->move(p);
    QSize hint = d->sizeHint();
    if(hint.height()>hei)
        hint.setHeight(hei);
    d->popup->setFixedSize(hint);
    if(qApp->isEffectEnabled(Qt::UI_AnimateCombo))
        qScrollEffect(d->popup, QEffects::DownScroll);
    else
        d->popup->show();

}

/**
 * @internal
 */
void QwwColorButton::_q_activated(const QModelIndex &ind) {
    Q_D(QwwColorButton);
    if (d->popup)
        d->popup->hide();
    QColor c = qvariant_cast<QColor>(ind.data(Qt::DecorationRole));
    d->curCol = c;
    d->curNam = ind.data(Qt::ToolTipRole).toString();
    QPixmap px(64, 64);
    QPainter pt(&px);
    pt.setBrush(c);
    pt.drawRect(0, 0, px.width()-1, px.height()-1);
    setIcon(c.isValid() ? px : QIcon());
    if (showName())
        setText(ind.data(Qt::ToolTipRole).toString());
    if(c.isValid())
        emit colorPicked(c);
    if(d->popup)
        d->popup->colorView()->setCurrentIndex(ind);
}

/**
 * @brief Returns the currently chosen color
 */
QColor QwwColorButton::currentColor() const {
    Q_D(const QwwColorButton);
    return d->curCol;
}

/**
 * @brief Adds a \a color with a \a name to the list of colors to choose from
 * @param color color to add
 * @param name name to associate with the color
 */
void QwwColorButton::addColor(const QColor & color, const QString & name) {
    Q_D(QwwColorButton);
    d->model->addColor(color, name);
}

/**
 * @brief Clears the color list
 */
void QwwColorButton::clear() {
    Q_D(QwwColorButton);
    d->model->clear();
}

/**
 * @brief Sets standard Qt colors as the list of colors to choose from
 */
void QwwColorButton::setStandardColors() {
    QStringList clist = QColor::colorNames();
    clear();
    foreach(QString col, clist) {
        QColor c;
        c.setNamedColor(col);
        addColor(c, col);
    }
}

/**
 * @brief list of colors known to the button.
 *
 */
QStringList QwwColorButton::colors() const {
    Q_D(const QwwColorButton);
    QStringList slist;
    for (int i=0;i<d->model->rowCount();i++) {
        QModelIndex ind = d->model->index(i,0);
        slist << QString("%1,%2").arg(qvariant_cast<QColor>(ind.data(Qt::DecorationRole)).name()).arg(ind.data(Qt::ToolTipRole).toString());
    }
    return slist;
}

/**
 *  @brief Sets a list of colors encoded into strings as available colors
 *
 */
void QwwColorButton::setColors(const QStringList &map) {
    clear();
    Q_D(QwwColorButton);
    foreach(QString nam, map) {
        QStringList slist = nam.split(",");
        QColor c;
        c.setNamedColor(slist[0]);
        addColor(c, slist[1]);
    }
    _q_activated(d->model->index(0,0));
    update();
}

/**
 * @internal
 */
void QwwColorButton::_q_colorDialogRequested() {
    QColor c = QColorDialog::getColor(currentColor(), this);
    if (c.isValid()) {
        setCurrentColor(c);
    }
}

/**
 * Sets the current color
 * @param c
 */
void QwwColorButton::setCurrentColor(const QColor & c) {
    Q_D(QwwColorButton);
    if (c==d->curCol) return;
    QModelIndex ind = d->model->contains(c);
    if (!ind.isValid()) {
        QString stdName = wwFindStandardColorName(c);
        addColor(c, stdName.isEmpty() ? tr("Custom color") : stdName);
        ind = d->model->index(d->model->rowCount()-1, 0);
    }
    _q_activated(ind);

}

/**
 *
 * @internal
 */
void QwwColorButton::_q_setCurrentIndex(const QModelIndex &index) {
    Q_D(QwwColorButton);
    d->popup->colorView()->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}

/**
 * @brief   This property holds whether the color name should be displayed on the button.
 *
 */
bool QwwColorButton::showName() const {
    Q_D(const QwwColorButton);
    return d->showName;
}

void QwwColorButton::setShowName(bool v) {
    Q_D(QwwColorButton);
    if (v==d->showName)
        return;
    setText( v ? d->curNam : "");
    d->showName = v;
}

/**
 *
 * @internal
 */
void QwwColorButton::mousePressEvent(QMouseEvent *ev) {
    Q_D(QwwColorButton);
    QPushButton::mousePressEvent(ev);
    if (dragEnabled() && ev->button() & Qt::LeftButton) {
        d->pressPos = ev->pos();
    }
}

/**
 *
 * @internal
 */
void QwwColorButton::mouseMoveEvent(QMouseEvent *ev) {
    Q_D(QwwColorButton);
    QPushButton::mouseMoveEvent(ev);
    if ((ev->buttons() & Qt::LeftButton) && dragEnabled() && (ev->pos() - d->pressPos).manhattanLength() >= QApplication::startDragDistance()) {
        ColorDrag *drag = new ColorDrag(this, currentColor(), currentColor().name());
        drag->exec();
        setDown(false); // Qt bug workaround ?
    }


}


/**
 * @brief   This property holds whether the button starts a drag if the user
 *          presses and moves the mouse away from the button.
 *
 *          Dragging is disabled by default.
 *
 */
bool QwwColorButton::dragEnabled() const {
    Q_D(const QwwColorButton);
    return d->dragEnabled;
}

void QwwColorButton::setDragEnabled(bool v) {
    Q_D(QwwColorButton);
    d->dragEnabled = v;
}

/**
 *
 * @internal
 */
void QwwColorButton::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasColor()) {
        QColor col(qvariant_cast<QColor>(event->mimeData()->colorData()));
        if (col.isValid())
            event->acceptProposedAction();
    } else if (event->mimeData()->hasFormat("text/plain")) {
        QColor col;
        col.setNamedColor(event->mimeData()->text());
        if (col.isValid())
            event->acceptProposedAction();
    } else event->ignore();

}

/**
 *
 * @internal
 */
void QwwColorButton::dropEvent(QDropEvent *event) {
    if (event->source()==this)
        return;
    QColor col;
    if (event->mimeData()->hasColor())
        col = qvariant_cast<QColor>(event->mimeData()->colorData());
    else
        col.setNamedColor(event->mimeData()->text());
    setCurrentColor(col);
}

#endif
