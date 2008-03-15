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

typedef VariantData::Constructor Constructor;
typedef VariantIntf::VariantType VariantType;

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
    VariantData* construct() const;
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

VariantData* TypeNode::construct() const
{
  if (!m_constructor)
    return 0;
  return m_constructor();
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

VariantData* VariantData::clone() const
{
  VariantData* newval = construct(type());
  if (newval)
    *newval = *this;
  return newval;
}
