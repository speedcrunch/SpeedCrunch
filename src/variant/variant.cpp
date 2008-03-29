/* variant.cpp: variant class */
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

#include "variant/variant.hxx"
#include <QtXml/QDomText>

const char* Variant::xmlTagName = "variant";
const char* xmlTypeAttrName = "type";
const char* VariantIntf::nBool = "Boolean";
const char* VariantIntf::nError = "Error";
VariantType VariantBase::vtBool;
VariantType VariantBase::vtError;

void VariantBase::initClass()
{
  vtBool = registerType(builtInConstructor, nBool);
  vtError = registerType(builtInConstructor, nError);
}

void Variant::initClass()
{
  VariantIntf::initClass();
  VariantBase::initClass();
}

VariantBase::operator Error() const
{
  return isError()? error : Success;
}

VariantBase::operator bool() const
{
  return isBool()? boolval : (Error)(*this) == Success;
}

VariantBase::operator const VariantData*() const
{
  return isBuiltIn()? 0 : val;
}

void VariantBase::setup()
{
  m_type = btError;
  error = NoOperand;
}

VariantType VariantBase::type() const
{
  if (m_type == btBool)
    return vtBool;
  return vtError;
}

bool VariantBase::operator = (bool newval)
{
  teardown();
  m_type = btBool;
  boolval = newval;
  return newval;
}

Error VariantBase::operator = (Error newval)
{
  teardown();
  m_type = btError;
  error = newval;
  return newval;
}

void VariantBase::operator = (VariantData* newval)
{
  m_type = btExtern;
  val = newval;
}

void VariantBase::operator = (const VariantBase& other)
{
  if (other.isBool())
    *this = (bool)other;
  else
    *this = (Error)other;
}

bool VariantBase::toBuiltInType(const char* name)
{
  if (qstrcmp(name, nBool) == 0)
    *this = true;
  else if (qstrcmp(name, nError) == 0)
    *this = NoOperand;
  else
    return false;
  return true;
}

QByteArray VariantBase::xmlTypeAttr(const QDomNode& node)
{
  // pre: node is an element
  return node.toElement().attribute(xmlTypeAttrName, "").toUtf8();
}

void VariantBase::xmlWrite(QDomDocument& doc, QDomNode& parent) const
{
  QString buf;
  if (isBool())
    buf.setNum(int(boolval));
  else
    buf.setNum(int(Error(*this)));
  xmlWriteText(doc, parent, buf);
}

bool VariantBase::xmlRead(QDomNode& node)
{
  // pre: node is an element
  bool ok;
  unsigned newval;
  QByteArray typeName = xmlTypeAttr(node);
  QString txt = xmlReadText(node, &ok);
  if (ok)
    newval = txt.toUInt(&ok);
  ok |= newval <= unsigned(NotAnError);
  if (ok && qstrcmp(typeName, nBool) == 0)
  {
    switch (newval)
    {
      case 0:
      case 1: *this = bool(newval); break;
      default: ok = false;
    }
  }
  else if (ok && qstrcmp(typeName, nError) == 0)
    *this = Error(newval);
  return ok;
}

VariantType Variant::type() const
{
  if (isBuiltIn())
    return VariantBase::type();
  return variantData()->type();
}

void Variant::operator = (VariantData* newval)
{
  if (newval != (VariantData*)this || !isBuiltIn())
  {
    teardown();
    VariantBase::operator = (newval->lock());
  }
}

Variant& Variant::operator = (const Variant& other)
{
  if (&other != this)
  {
    if (!other.isBuiltIn())
      *this = ((const VariantData*)(other))->clone();
    else
      VariantBase::operator = (other);
  }
  return *this;
}

void Variant::teardown()
{
  if (!isBuiltIn())
    variantData()->release();
}

QDomElement Variant::createEmptyElement(QDomDocument& doc, const char* type)
{
  QDomElement result = doc.createElement(xmlTagName);
  doc.appendChild(result);
  result.setAttribute(xmlTypeAttrName, type);
  return result;
}


void Variant::xmlWrite(QDomDocument& doc, QDomNode& node) const
{
  QDomElement elem = createEmptyElement(doc, typeName(type()));
  if (isBuiltIn())
    VariantBase::xmlWrite(doc, elem);
  else
    variantData()->xmlWrite(doc, elem);
}

bool Variant::xmlRead(QDomNode& node)
{
  bool ok = node.isElement();
  if (ok)
  {
    VariantType vt = variantType(xmlTypeAttr(node));
    ok = node.toElement().tagName().toUtf8() == xmlTagName
         && vt != 0;
    if (ok)
    {
      VariantData* vd = construct(vt);
      if (vd)
      {
        ok = vd->xmlRead(node);
        if (ok)
          *this = vd;
        else
          vd->release();
      }
      else
        ok = VariantBase::xmlRead(node);
    }
  }
  return ok;
}

QByteArray Variant::toUtf8() const
{
  QDomDocument doc;
  xmlWrite(doc, doc);
  QByteArray result = doc.toByteArray(0);
  return result.simplified();
}

Variant Variant::fromUtf8(const char* utf8, const char* type)
{
  // pre: type must be an UTF8 encoded, valid XML attribute string value
  // without the enclosing "
  QByteArray data = QByteArray::fromRawData(utf8, qstrlen(utf8));
  QDomDocument doc;
  if (type)
  {
    QDomNode elem = createEmptyElement(doc, type);
    xmlWriteText(doc, elem, data);
  }
  else
    doc.setContent(data);
  Variant result;
  QDomNode variantElem = doc.firstChild();
  if (result.xmlRead(variantElem))
    return result;
  return BadLiteral;
}

Variant::Variant(floatnum f, Error e)
{
  if (e != Success)
    *this = e;
  else
    *this = LongReal::create(f);
}
