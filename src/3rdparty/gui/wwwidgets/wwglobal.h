//
// C++ Interface: wwglobal
//
// Description:
//
//
// Author: Witold Wysota <wysota@wysota.eu.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef __WW_GLOBAL_H
#define __WW_GLOBAL_H

#include <QString>
#include <QColor>

#define Q_WW_EXPORT

class QwwPrivate;
/**
 * \internal
 * @class QwwPrivatable
 */
class QwwPrivatable {
protected:
    QwwPrivatable(QwwPrivate *p);
    ~QwwPrivatable();
    QwwPrivate *d_ww_ptr;
};


#define WW_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(d_ww_ptr); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(d_ww_ptr); } \
    friend class Class##Private;


 Q_WW_EXPORT QString wwFindStandardColorName(const QColor &);

#endif
