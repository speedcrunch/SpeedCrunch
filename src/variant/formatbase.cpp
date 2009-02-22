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
#include "core/settings.hxx"
#include <QMultiMap>

const char* fmtDefault = "default";

/*------------------------   FormatList   -------------------------*/

class FormatList: public QMultiMap<QString, Format>
{
  typedef QMultiMap<QString, Format>Base;
  public:
    static FormatList& inst() { return *list; };
    static void initClass();
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

void FormatList::initClass()
{
  list = new FormatList;
}

void FormatList::deleteFromList(Iterator i)
{
  ((FormatIntf*)i.value())->key.clear();
  erase(i);
}

void FormatList::add(const QString& key, const Format& fmt)
{
  remove(fmt.key(), fmt.type());
  remove(key, fmt.type());
  ((FormatIntf*)(fmt))->key = key;
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
  : m_base(0), m_importmask(0)
{
}

bool FormatIntf::isCompatible(FmtType ft) const
{
  return ft == type()
    || ft == fmtSettings;
};

int FormatIntf::indexOfProp(const QString& prop) const
{
  return getProps().indexOf(prop);
}

bool FormatIntf::setProp(const QString& prop, const Variant& val, bool inherit)
{
  int idx = indexOfProp(prop);
  bool result = idx >= 0;
  if (result)
  {
    quint64 iflag = flag(idx);
    m_importmask |= iflag;
    if (!inherit)
      m_importmask ^= iflag;
    result = setValue(idx, val);
  }
  return result;
}

Variant FormatIntf::getProp(const QString& prop)
{
  int idx = indexOfProp(prop);
  if (idx < 0)
    return NotImplemented;
  if ((m_importmask & flag(idx)) != 0)
  {
    Variant result;
    Format base = findBase();
    if (base.isValid())
      result = base.getProp(prop);
    if (!result.is(VariantIntf::nError))
      return result;
  }
  return getValue(idx);
}

bool FormatIntf::isImporting(const QString& prop) const
{
  int idx = indexOfProp(prop);
  return idx < 0? false : (flag(idx) & m_importmask) != 0;
}

Format FormatIntf::findBase()
{
  if (m_baseName.isEmpty())
  {
    if (m_base.isValid() && m_base.key().isEmpty())
    {
      import();
      m_base = 0;
    }
    return m_base;
  }
  return FormatList::inst().find(m_baseName, this);
}

void FormatIntf::release()
{
  if (--refcount <= 0);
    delete this;
}

bool FormatIntf::usesBase() const
{
  return m_importmask != 0
         && (m_base.isValid() || !m_baseName.isEmpty());
}

bool FormatIntf::setBase(const QString& aBase, quint64 imask, FmtLinkage lk)
{
  m_importmask = imask;
  m_baseName = aBase;
  if (lk == fmtRelaxed)
  {
    m_base = 0;
    return true;
  }
  m_base = findBase();
  m_baseName.clear();
  if (lk == fmtInclude)
  {
    import();
    m_base = 0;
  }
  return aBase.isEmpty() ^ m_base.isValid();
}

void FormatIntf::import()
{
  if (m_importmask != 0)
  {
    Format base = findBase();
    if (base.isValid())
      cloneFrom(base);
  }
}

void FormatIntf::cloneFrom(Format basedOn)
{
  Variant value;
  QString propName;
  const QStringList& props = getProps();
  for (int i = -1; ++i < props.size();)
    if ((m_importmask & flag(i)) != 0)
    {
      propName = props.at(i);
      value = basedOn.getProp(propName);
      if (!value.is(VariantIntf::nError))
        setProp(propName, value, false);
    }
}

bool FormatIntf::setChar(QChar& dest, const Variant& val)
{
  QString s = val;
  bool result = val.is(VariantIntf::nString) && s.size() == 1;
  if (result)
    dest = s.at(0);
  return result;
}

bool FormatIntf::setInt(int& dest, const Variant& val)
{
  bool result = val.is(VariantIntf::nLongReal);
  if (result)
  {
    double v = val;
    dest = (int)v;
    result = dest == v;
  }
  return result;
}

void FormatIntf::initClass(QStringList** propList, const char** props)
{
  *propList = new QStringList;
  while (*props)
    **propList << *(props++);
}

quint64 FormatIntf::flag(char ofs)
{
  return Q_UINT64_C(1) << ofs;
}

/*----------------------   FmtSettings   -----------------------*/

const char* fmtSettings = "settings";

static const char* fmtSettingsProps[] =
{
  "dot",
  0
};

class FmtSettings: public FormatIntf
{
  public:
    FmtType type() const { return fmtSettings; };
    Variant getValue(int idx) const;
    const QStringList& getProps() const { return *propList; };
    static void initClass();
  private:
    static QStringList* propList;
    QChar dot;
};

void FmtSettings::initClass()
{
  FormatIntf::initClass(&propList, &fmtSettingsProps[0]);
  Format::add(fmtSettings, new FmtSettings);
}

Variant FmtSettings::getValue(int idx) const
{
  switch (idx)
  {
    case 0:
      return QString(Settings::instance()->radixCharacter());
    default:
      return NotImplemented;
  }
}

QStringList* FmtSettings::propList = 0;

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

QString Format::format(const Variant& val, const QString& key)
{
  QString aKey = key.isEmpty()? key : fmtDefault;
  return find(aKey, val.type()).format(val);
}

const QStringList* Format::getProps() const
{
  if (isValid())
    return &p->getProps();
  return 0;
}

bool Format::setBase(const QString& aBase, quint64 flags, FmtLinkage lk)
{
  if (isValid())
    return p->setBase(aBase, flags, lk);
  return false;
}

bool Format::setProp(const QString& prop, const Variant& value, bool inherit)
{
  if (isValid())
    return p->setProp(prop, value, inherit);
  return false;
}

Variant Format::getProp(const QString& prop) const
{
  if (isValid())
    return p->getProp(prop);
  return NotImplemented;
}

QString Format::key() const
{
  if (isValid())
    return QString();
  return p->key;
}

Format Format::find(const QString& key, VariantType vt)
{
  return FormatList::inst().find(key, vt);
}

void Format::add(const QString& key, Format fmt)
{
  if (!fmt.isValid())
    return;
  FormatList::inst().add(key, fmt);
}

void Format::initClass()
{
  FormatList::initClass();
  FmtSettings::initClass();
}

