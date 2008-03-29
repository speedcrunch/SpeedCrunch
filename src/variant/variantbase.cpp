/* variantbase.cpp: interface of variant type and derivates */
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

#include "variant/variantbase.hxx"
#include "variant/variant.hxx"
#include <QVector>
#include <QByteArray>
#include <QMultiMap>

typedef VariantData::Constructor Constructor;

template<class T>
class PtrList: public QVector<T*>
{
  private:
    typedef QVector<T*> Base;
  public:
    ~PtrList() { qDeleteAll(*this); };
    const T* add(T* newT)
    {
      int i = -1;
      while (++i < Base::size()
             && !(*newT == *Base::at(i)));
      if (i == Base::size())
        append(newT);
      else
        delete newT;
      return Base::at(i);
    };
};

template<class T>
class SinglePtrList: public PtrList<T>
{
  public:
    static void create();
    static void destroy();
    static SinglePtrList* inst() { return singleton; };
  private:
    static SinglePtrList* singleton;
};

template<class T>
void SinglePtrList<T>::create()
{
  if (!singleton)
    singleton = new SinglePtrList<T>;
}

template<class T>
void SinglePtrList<T>::destroy()
{
  if (!singleton)
    delete singleton;
}

template<class T>
SinglePtrList<T>* SinglePtrList<T>::singleton = 0;

/* class holding general global data of a specific variant type */
class TypeNode
{
  public:
    TypeNode(const char* name, Constructor c)
      : m_name(name), m_constructor(c) {};
    const char* name() const { return m_name; };
    VariantData* construct() const { return m_constructor(); };
    bool operator == (const TypeNode& other) const;
  private:
    const char* m_name;
    Constructor m_constructor;
};

bool TypeNode::operator == (const TypeNode& other) const
{
  return qstrcmp(name(), other.name()) == 0;
}

class TypeNodes: protected SinglePtrList<TypeNode>
{
  public:
    static void create();
    static void destroy();
    static VariantType registerType(Constructor, const char* name);
    static VariantType getType(const char* name);
    static const TypeNode* typeNode(VariantType vt)
      { return static_cast<const TypeNode*>(vt); };
};

void TypeNodes::create()
{
  SinglePtrList<TypeNode>::create();
}

void TypeNodes::destroy()
{
  SinglePtrList<TypeNode>::destroy();
}

VariantType TypeNodes::getType(const char* name)
{
  for (int i = inst()->size(); --i >= 0;)
  {
    TypeNode* tn = inst()->at(i);
    if (qstrcmp(tn->name(), name) == 0)
      return (VariantType)tn;
  }
  return 0;
}

VariantType TypeNodes::registerType(Constructor c, const char* name)
{
  return (VariantType) inst()->add(new TypeNode(name, c));
}

void VariantIntf::initClass()
{
  TypeNodes::create();
}

VariantType VariantIntf::registerType(Constructor c, const char* name)
{
  return TypeNodes::registerType(c, name);
}

VariantType VariantIntf::variantType(const char* name)
{
  return TypeNodes::getType(name);
}

const char* VariantIntf::typeName(VariantType vt)
{
  return TypeNodes::typeNode(vt)->name();
}

VariantData* VariantIntf::construct(VariantType vt)
{
  return TypeNodes::typeNode(vt)->construct();
}

void VariantIntf::xmlWriteText(QDomDocument& doc,
                    QDomNode& parent, QString val)
{
  if (!val.isEmpty())
  {
    // these characters are subject to automatic escaping
    const char reject[] = "&><";
    const char* p = reject;
    while(*p && !val.contains(*p))
      ++p;
    if (*p)
    {
      QStringList list = val.split("]]>");
      QString part;
      for (int i = 0; i < list.size();)
      {
        part = list.at(i);
        if (i != 0)
          part = '>' + part;
        if (++i != list.size())
          part += "]]";
        parent.appendChild(doc.createCDATASection(part));
      }
    }
    else
      parent.appendChild(doc.createTextNode(val));
  }
}

QString VariantIntf::xmlReadText(QDomNode& parent, bool* textOnly)
{
  QString result;
  bool ok = true;
  QDomNodeList list = parent.childNodes();
  for (int i = -1; ++i < list.size();)
  {
    QDomNode cur = list.at(i);
    if (cur.isText())
      result += cur.toText().data();
    else
      ok = false;
  }
  if (textOnly != 0)
    *textOnly = ok;
  return result;
}

VariantData* VariantData::clone() const
{
  VariantData* newval = construct(type());
  if (newval)
    *newval = *this;
  return newval;
}

/*-------------------------   Formats   ------------------------------------*/

FormatIntf::FormatIntf(FormatIntf* aBase)
  : m_base(0)
{
  if (isCompatible(aBase))
  {
    m_base = aBase;
    aBase->lock();
  }
}

void FormatIntf::releaseBase()
{
  if (m_base)
  {
    m_base->release();
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

bool FormatIntf::isCompatible(const FormatIntf* other) const
{
  return other
         && (   other->isLocale()
             || qstrcmp(type(), other->type()) == 0);
}

class FormatList: public QMultiMap<QString, Format>
{
  typedef QMultiMap<QString, Format>Base;
  typedef Format::FmtType FmtType;
  public:
    static FormatList& inst() { return *list; };
    Format* find(const QString& key, VariantType vt);
    Format* findByType(const QString& key, FmtType type);
    void remove(const QString& key, FmtType type);
  private:
    static FormatList* list;
};

FormatList* FormatList::list = 0;

Format* FormatList::find(const QString& key, VariantType vt)
{
  Iterator i = Base::find(key);
  while (i != end() && i.key() == key)
  {
    if (i.value().canHandle(vt))
      return &i.value();
    ++i;
  }
  return 0;
}

Format* FormatList::findByType(const QString& key, FmtType fmttype)
{
  Iterator i = Base::find(key);
  while (i != end() && i.key() == key)
  {
    Format& fmt = i.value();
    if (fmt.type() == fmttype)
      return &fmt;
    ++i;
  }
  return 0;
}

void FormatList::remove(const QString& key, FmtType fmttype)
{
  Iterator i = Base::find(key);
  while (i != end() && i.key() == key && i.value().type() != fmttype)
    ++i;
  if (i != end())
    erase(i);
}

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

Format::Format(const QString& key, VariantType vt)
  : p(0)
{
  *this = find(key, vt);
}

Format::~Format()
{
  *this = 0;
}

Format::FmtType Format::type() const
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

QString Format::format(const Variant& value) const
{
  if (canHandle(value.type()))
    return p->format(value);
  return QString();
}

Format Format::clone() const
{
  if (isValid())
    return p->clone();
  return 0;
}

QStringList Format::getProps() const
{
  if (isValid())
    return p->getProps();
  return QStringList();
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
  Format* fmt = FormatList::inst().find(key, vt);
  if (fmt)
    return *fmt;
  return 0;
}

Format Format::findByType(const QString& key, FmtType type)
{
  Format* fmt = FormatList::inst().findByType(key, type);
  if (fmt)
    return *fmt;
  return 0;
}

Format Format::add(const QString& key, FormatIntf* fmt)
{
  if (!fmt)
    return 0;
  Format result = fmt;
  FormatList::inst().remove(key, result.type());
  FormatList::inst().insert(key, result);
  return result;
}
