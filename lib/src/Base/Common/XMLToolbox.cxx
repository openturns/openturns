//                                               -*- C++ -*-
/**
 *  @brief This file provides basic XML functionalities
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OTprivate.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"
#include "openturns/XMLToolbox.hxx"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#if defined OPENTURNS_HAVE_LIBXML2
#include <libxml/tree.h>
#include <libxml/xmlsave.h>
#endif

BEGIN_NAMESPACE_OPENTURNS


#if defined OPENTURNS_HAVE_LIBXML2

static //inline
String xmlStringToString( const XML::xmlString & xst )
{
  return String(reinterpret_cast<const char*>(xst.c_str()), xst.size());
}

static //inline
XML::xmlString StringToXmlString( const String & st )
{
  return XML::xmlString(reinterpret_cast<const unsigned char *>(st.c_str()), st.size());
}


XMLDoc::XMLDoc() : doc_(xmlNewDoc(reinterpret_cast<const xmlChar *> ("1.0")))
{
  // Nothing to do
}

XMLDoc::XMLDoc(const XMLDoc & other) : doc_(xmlCopyDoc( other.doc_, 1 ))
{
  // Nothing to do
}

XMLDoc::XMLDoc(const FileName & fileName) : doc_(0)
{
  if (!std::ifstream(fileName).good())
    throw FileOpenException(HERE) << "Cannot open file " << fileName << " for reading";
  doc_ = xmlReadFile(fileName.c_str(), "UTF-8", 0);
  if (doc_ == NULL) throw XMLParserException(HERE) << "Error in parsing XML file " << fileName;
}

XMLDoc::XMLDoc(const char * buffer, int size) : doc_(0)
{
  doc_ = xmlParseMemory(buffer, size);
  if (doc_ == NULL) throw XMLParserException(HERE) << "Error in parsing XML";
}

XMLDoc::~XMLDoc()
{
  xmlFreeDoc( doc_ );
}

XMLDoc & XMLDoc::operator =(const XMLDoc & other)
{
  if (this != &other)
  {
    xmlFreeDoc( doc_ );
    doc_ = xmlCopyDoc( other.doc_, 1 );
  }

  return *this;
}

XMLDoc::operator xmlDocPtr() const
{
  return doc_;
}

void XMLDoc::save(const FileName & fileName) const
{
  if (!std::ofstream(fileName).good())
    throw FileOpenException(HERE) << "Cannot open file " << fileName << " for writing";
  int rc = xmlSaveFormatFileEnc(fileName.c_str(), doc_, "UTF-8", 1);
  if (rc < 0)
    throw InternalException(HERE) << "XMLDoc: Could not save XML file " << fileName;
}

void XMLDoc::setCompressionLevel( const UnsignedInteger compressionLevel )
{
  xmlSetDocCompressMode(doc_, std::min(compressionLevel, 9UL));
}

String XMLDoc::__repr__() const
{
  xmlSaveCtxt * xctx;
  xmlBuffer * buf;

  // Buffer allocation
  if ( !( buf = xmlBufferCreate() ) )
    return String();

  // Context definition
  if ( !( xctx = xmlSaveToBuffer( buf, NULL, XML_SAVE_NO_DECL ) ) )
  {
    xmlBufferFree( buf );
    return String();
  }

  // Document streaming
  if ( xmlSaveDoc( xctx, doc_ ) < 0 )
  {
    xmlSaveClose( xctx );
    xmlBufferFree( buf );
    return String();
  }

  // Context closing (?)
  if ( xmlSaveClose( xctx ) <= 0)
  {
    xmlBufferFree( buf );
    return String();
  }

  String data = reinterpret_cast<char *>(buf->content);

  xmlBufferFree( buf );

  return data;
}


Bool XMLDoc::hasDTD() const
{
  xmlDtdPtr dtd = xmlGetIntSubset( doc_ );
  return (dtd != NULL);
}

Bool XMLDoc::validate() const
{
  int ok = 0;
  xmlValidCtxtPtr validCtxt = xmlNewValidCtxt();
  if (validCtxt != NULL)
  {
    validCtxt->error    = reinterpret_cast<xmlValidityErrorFunc>(XML::ErrorHandler);
    validCtxt->warning  = reinterpret_cast<xmlValidityWarningFunc>(XML::WarningHandler);
    ok = xmlValidateDocument( validCtxt, doc_ );
    xmlFreeValidCtxt( validCtxt );
  }
  else
    LOGWARN( OSS() << "Internal Error: Can't allocate storage for validation. No validation" );
  return (ok == 1);
}

Bool XMLDoc::validate(const String & name, const FileName & dtd) const
{
  int ok = 0;
  XML::xmlString aName  = StringToXmlString( name );
  XML::xmlString aDtd   = StringToXmlString( dtd );
  xmlDtdPtr theDTD = xmlParseDTD( NULL, aDtd.c_str() );
  xmlValidCtxtPtr validCtxt = xmlNewValidCtxt();
  if (validCtxt != NULL)
  {
    validCtxt->userData = (void *) stderr;
    validCtxt->error    = reinterpret_cast<xmlValidityErrorFunc>(XML::ErrorHandler);
    validCtxt->warning  = reinterpret_cast<xmlValidityWarningFunc>(XML::WarningHandler);
    ok = xmlValidateDtd( validCtxt, doc_, theDTD );
    xmlFreeValidCtxt( validCtxt );
  }
  else
    LOGWARN( OSS() << "Internal Error: Can't allocate storage for validation. No validation" );
  xmlFreeDtd( theDTD );
  return (ok == 1);
}




void XML::WarningHandler( void *, const char * format, ...)
{
  // int ret = 0;
  va_list args;
  char *msg;

  va_start(args, format);
  msg = MakeMessage( format, args );
  va_end(args);

  LOGWARN( OSS() << "XML::WarningHandler: " << msg );
  free(msg);
}


void XML::ErrorHandler( void *, const char * format, ...)
{
  // int ret = 0;
  va_list args;
  char *msg;

  va_start(args, format);
  msg = MakeMessage( format, args );
  va_end(args);

  LOGERROR( OSS() << "XML::ErrorHandler: " << msg );
  free(msg);
}



/* From printf(3) linux manpage */
char * XML::MakeMessage(const char *fmt, va_list args)
{
  /* Guess we need no more than 100 bytes. */
  int n, size = 100;
  char *p, *np;
  va_list args_copy;

  if ((p = (char*) calloc (sizeof(char), size)) == NULL)
    return NULL;

  while (1)
  {
    /* Try to print in the allocated space. */
#ifdef _MSC_VER
    args_copy = args;
#else
    va_copy(args_copy, args);
#endif
    n = vsnprintf (p, size, fmt, args_copy);
    va_end(args_copy);
    /* If that worked, return the string. */
    if (n > -1 && n < size)
      return p;
    /* Else try again with more space. */
    if (n > -1)    /* glibc 2.1 */
      size = n + 1; /* precisely what is needed */
    else           /* glibc 2.0 */
      size *= 2;  /* twice the old size */
    if ((np = (char*) realloc (p, size)) == NULL)
    {
      free(p);
      return NULL;
    }
    else
    {
      p = np;
    }
  }
}


String XML::ToString(const xmlString & st)
{
  return xmlStringToString( st );
}

Bool XML::IsText(const Node & elt)
{
  return elt ? (elt->type == XML_TEXT_NODE) : false;
}



Bool XML::IsElement(const Node & elt)
{
  return elt ? (elt->type == XML_ELEMENT_NODE) : false;
}



Bool XML::IsElement(const Node & elt, const String & name)
{
  xmlString aName = StringToXmlString( name );
  Bool isElt      = IsElement( elt );
  return isElt && ( xmlStrcmp( elt->name, aName.c_str() ) == 0 );
}



Bool XML::ElementHasAttribute(const Node & elt, const String & name)
{
  xmlString aName = StringToXmlString( name );
  assert(elt);
  return xmlHasProp(elt, aName.c_str()) != NULL;
}



String XML::GetAttributeByName(const Node & node, const String & name)
{
  String attrVal;
  if (node)
  {
    xmlString aName = StringToXmlString( name );
    if ( xmlHasProp( node, aName.c_str() ) )
    {
      xmlChar * prop = xmlGetProp(node, aName.c_str() );
      xmlString val = prop;
      attrVal = xmlStringToString( val );
      xmlFree( prop );
    }
  }
  return attrVal;
}


void XML::SetAttribute(const Node & node, const String & attribute, const String & value)
{
  if (node)
  {
    xmlString aAttr  = StringToXmlString( attribute );
    xmlString aValue = StringToXmlString( value );
    xmlNewProp( node, aAttr.c_str(), aValue.c_str() );
  }
}


XML::Node XML::FindElementByName(const Node & node, const String & name)
{
  Node cur = NULL;
  if (node)
  {
    for(cur = node->children; cur; cur = cur->next)
    {
      if (IsElement(cur, name))
      {
        break;
      }
    }
  }
  return cur;
}



XML::Node XML::FindNextElementByName(const Node & node, const String & name)
{
  Node cur = node;
  while (cur)
  {
    cur = cur->next;
    if (IsElement(cur, name)) break;
  }
  return cur;
}



String XML::GetNodeValue(const Node & node)
{
  String value;
  Node cur = NULL;
  if (node)
  {
    for(cur = node->children; cur; cur = cur->next)
    {
      if (IsText(cur))
      {
        xmlString val = cur->content;
        value = xmlStringToString( val );
        break;
      }
    }
  }
  return value;
}


String XML::GetNodeName(const Node & node)
{
  String name;
  if (node)
  {
    xmlString aName = node->name;
    name = xmlStringToString( aName );
  }
  return name;
}



UnsignedInteger XML::GetNodeLineNumber(const Node & node)
{
  UnsignedInteger lineno = 0;
  if (node) lineno = xmlGetLineNo(node);
  return lineno;
}


XML::Node XML::NewNode(const String & name)
{
  xmlString aName = StringToXmlString( name );
  Node node = xmlNewNode( NULL, aName.c_str() );
  return node;
}



XML::Node XML::NewTextNode(const String & value)
{
  xmlString aValue = StringToXmlString( value );
  Node node = xmlNewText( aValue.c_str() );
  return node;
}



XML::Node XML::NewNode(const String & name, const String & value)
{
  Node node = NewNode( name );
  Node child = NewTextNode( value );
  AddChild( node, child );
  return node;
}

void XML::AddChild(const Node & parent, const Node & child)
{
  xmlAddChild( parent, child );
}


XML::Node XML::GetRootNode( const XMLDoc & doc )
{
  return xmlDocGetRootElement( doc );
}


void XML::SetRootNode( const XMLDoc & doc, const Node & root )
{
  assert(root);
  xmlDocSetRootElement( doc, root );
}

XML::Node XML::GetFirstChild( const Node & node )
{
  assert(node);
  return node->children;
}

XML::Node XML::GetNextNode( const Node & node )
{
  assert(node);
  return node->next;
}

void XML::SetDTD( const XMLDoc & doc, const String & name, const String & path )
{
  xmlString aName = StringToXmlString( name );
  xmlString aPath = StringToXmlString( path );
  xmlCreateIntSubset( doc, aName.c_str(), NULL, aPath.c_str() );
}

std::ostream & operator <<(std::ostream & os, const xmlNodePtr & node)
{
  assert(node);
  String name = XML::GetNodeName( node );
  os << "XML node='" << name << "'";

  os << " attributes{";
  xmlAttrPtr attr = node->properties;
  while(attr)
  {
    XML::xmlString aName = attr->name;
    xmlChar * prop = xmlGetProp(node, aName.c_str() );
    XML::xmlString val = prop;
    String attrName = xmlStringToString( aName );
    String attrVal = xmlStringToString( val );
    os << attrName << "=" << attrVal << " ";
    attr = attr->next;
  }
  os << " }";

  os << " children={ ";
  xmlNodePtr child = node->xmlChildrenNode;
  while (child)
  {
    os << child << " ";
    child = child->next;
  }
  os << " }";

  if (XML::IsText(node))
  {
    XML::xmlString val = node->content;
    os << " value='" <<  xmlStringToString( val ) << "'";
  }
  return os;
}

#endif /* OPENTURNS_HAVE_LIBXML2 */

END_NAMESPACE_OPENTURNS
