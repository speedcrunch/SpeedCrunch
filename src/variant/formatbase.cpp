/* formatbase.cpp: interface of formats */
/*
    Copyright (C) 2008 Wolf Lammen.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.


    You may contact the author by:
       e-mail:  ookami1 <at> gmx <dot> de
       mail:  Wolf Lammen
              Oertzweg 45
              22307 Hamburg
              Germany

*************************************************************************/

#include "variant/formatbase.hxx"
#include <QMultiMap>
#include <QStringList>

/*------------------------   FormatList   -------------------------*/

class FormatList: public QMultiMap<QString, Format>
{
  typedef QMultiMap<QString, Format>Base;
  public:
    static FormatList& inst() { return *list; };
    Format findBase(const QString& key, const Format& derived);
    Format find(const QString& key, VariantType);
    void remove(const QString& key, FmtType ft);
    void remove(const Format&);
    void add(const QString& key, const Format&);
  private:
    static FormatList* list;
    void deleteFromList(Iterator);
};

FormatList* FormatList::list = 0;

void FormatList::deleteFromList(Iterator i)
{
  ((FormatIntf*)i.value())->accessible = false;
  erase(i);
}

void FormatList::add(const QString& key, const Format& fmt)
{
  remove(key, fmt.type());
  ((FormatIntf*)(fmt))->accessible = true;
  insert(key, fmt);
}

Format FormatList::findBase(const QString& key, const Format& derived)
{
  Iterator i = Base::find(key);
  while (i != end() && i.key() == key)
  {
    if (derived.isCompatible(i.value()))
      return i.value();
    ++i;
  }
  return Format();
}

Format FormatList::find(const QString& key, VariantType vt)
{
  Iterator i = Base::find(key);
  while (i != end() && i.key() == key)
  {
    if (i.value().canHandle(vt))
      return i.value();
    ++i;
  }
  return Format();
}

void FormatList::remove(const QString& key, FmtType ft)
{
  Iterator i = Base::find(key);
  while (i != end() && i.key() == key)
  {
    if (i.value().type() == ft)
    {
      deleteFromList(i);
      return;
    }
    ++i;
  }
}

void FormatList::remove(const Format& fmt)
{
  Iterator i = begin();
  while (i != end() && i.value() != fmt)
    ++i;
  if (i != end())
    deleteFromList(i);
}

/*----------------------------   FormatIntf   --------------------------*/

FormatIntf::FormatIntf()
  : m_base(0)
{
  accessible = false;
}

FormatIntf* FormatIntf::findBase()
{
  if (m_baseName.isEmpty())
    return m_base;
  return FormatList::inst().find(m_baseName, this);
}

void FormatIntf::releaseBase()
{
  if (m_base)
  {
    ((FormatIntf*)m_base)->release();
    m_base = 0;
  }
}

void FormatIntf::release()
{
  if (--refcount <= 0)
  {
    releaseBase();
    delete this;
  }
}

bool FormatIntf::setBase(const QString& aBase, quint64 imask, FmtLinkage lk)
{
  m_importmask = imask;
  m_baseName = aBase;
  if (lk == fmtRelaxed)
  {
    releaseBase();
    return true;
  }
  FormatIntf* newBase = findBase();
  m_baseName.clear();
  if (newBase != m_base)
    releaseBase();
  m_base = newBase;
  if (lk == fmtInclude)
  {
    import();
    m_base = 0;
  }
  else if (newBase) // fmtFixed
    ((FormatIntf*)m_base)->lock();
  return newBase != 0 || imask == 0;
}

void FormatIntf::import()
{
  if (m_importmask != 0)
  {
    FormatIntf* base = findBase();
    if (!base)
      return;
    Variant value;
    QString propName;
    const QStringList& props = getProps();
    for (int i = -1; ++i < props.size();)
      if ((m_importmask & (1ll << i)) != 0)
      {
        propName = props.at(i);
        value = base->getProp(propName);
        if (!value.is(VariantIntf::nError))
          setProp(propName, value);
      }
  }
}

/*---------------------------   Format   --------------------------*/

void Format::operator = (FormatIntf* fmt)
{
  if (fmt)
    fmt->lock();
  if (isValid())
    p->release();
  p = fmt;
}

Format& Format::operator = (const Format& other)
{
  *this = (FormatIntf*)other;
  return *this;
}

Format::Format(FormatIntf* fmt)
  : p(0)
{
  *this = fmt;
}

Format::Format(const Format& other)
  : p(0)
{
  *this = other;
}

Format::~Format()
{
  *this = 0;
}

FmtType Format::type() const
{
  if (isValid())
    return p->type();
  return 0;
}

bool Format::canHandle(VariantType vt) const
{
  if (isValid())
    return p->canHandle(vt);
  return false;
}

bool Format::isCompatible(const Format& other) const
{
  if (isValid())
    return p->isCompatible(other.type());
  return false;
}

QString Format::format(const Variant& value) const
{
  if (canHandle(value.type()))
    return p->format(value);
  return QString();
}

const QStringList* Format::getProps() const
{
  if (isValid())
    return &p->getProps();
  return 0;
}

bool Format::setProp(const QString& prop, const Variant& value)
{
  if (isValid())
    return p->setProp(prop, value);
  return false;
}

Variant Format::getProp(const QString& prop) const
{
  if (isValid())
    return p->getProp(prop);
  return InvalidParam;
}

Format Format::find(const QString& key, VariantType vt)
{
  return FormatList::inst().find(key, vt);
}

void Format::add(const QString& key, FormatIntf* fmt)
{
  if (!fmt)
    return;
  FormatList::inst().add(key, fmt);
}

/*----------------------   FmtSettings   -----------------------*/

const char* fmtSettings = "settings";

class FmtSettings: public FormatIntf
{
  public:
    FmtType type() const { return fmtSettings; };
    virtual bool setProp(const QString& prop, const Variant& val) = 0;
    virtual Variant getProp(const QString& prop) const = 0;
    virtual const QStringList& getProps() const = 0;
};
