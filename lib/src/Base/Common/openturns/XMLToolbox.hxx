//                                               -*- C++ -*-
/**
 *  @brief This file provides basic XML functionalities
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_XMLTOOLBOX_HXX
#define OPENTURNS_XMLTOOLBOX_HXX

#include <cstdarg>
#include "openturns/OTprivate.hxx"
#include "openturns/OTconfig.hxx"

#if defined OPENTURNS_HAVE_LIBXML2
#include <libxml/parser.h>
#define XML_SUPPORTED
#endif

BEGIN_NAMESPACE_OPENTURNS

#if defined OPENTURNS_HAVE_LIBXML2

/**
 * @class XMLDoc
 *
 * This class stores a pointer to an XML document and manages its life cycle
 */

class OT_API XMLDoc
{
  xmlDocPtr doc_;

public:

  XMLDoc();
  XMLDoc( const XMLDoc & other );
  XMLDoc( const FileName & pathToFile );
  XMLDoc(const char * buffer, int size);
  ~XMLDoc() throw();

  XMLDoc & operator =( const XMLDoc & other );

  operator xmlDocPtr() const;

  void save( const FileName & fileName ) const;
  void setCompressionLevel(const UnsignedInteger compressionLevel);
  String __repr__() const;

  Bool hasDTD() const;
  Bool validate() const;
  Bool validate( const String & name, const FileName & dtd ) const;
};




/**
 * @class XML
 *
 * This is a compound class to gather general methods
 */

class OT_API XML
{
public:

  typedef xmlNodePtr Node;
  typedef std::basic_string<xmlChar> xmlString;

  /** Convert XML string to basic string */
  static String ToString( const xmlString & st );

  /** Returns true if node 'elt' is an XML text node */
  static Bool IsText( const Node & elt );

  /** Returns true if node 'elt' is an XML Element node */
  static Bool IsElement( const Node & elt );

  /** Returns true if node 'elt' is an XML element node named 'name' */
  static Bool IsElement( const Node & elt, const String & name );

  /** Returns true if node 'elt' has a attribute named 'name' */
  static Bool ElementHasAttribute( const Node & elt, const String & name );

  /** Returns the value of the attribute named 'name' hold by 'node' */
  static String GetAttributeByName( const Node & node, const String & name );

  /** Set the attribute of a node */
  static void SetAttribute( const Node & node, const String & attribute, const String & value );

  /** Returns the child node named 'name' of node 'parent' */
  static Node FindElementByName( const Node & parent, const String & name );

  /** Returns the brother node named 'name' of node 'node' */
  static Node FindNextElementByName( const Node & node, const String & name );

  /** Returns the content of 'node' */
  static String GetNodeValue( const Node & node );

  /** Returns the name of 'node' */
  static String GetNodeName( const Node & node );

  /** Returns the line number where 'node' appears */
  static UnsignedInteger GetNodeLineNumber( const Node & node );

  /** Create a new node */
  static Node NewNode( const String & name );
  static Node NewNode( const String & name, const String & value );
  static Node NewTextNode( const String & value );

  /** Link a child node to its parent */
  static void AddChild( const Node & parent, const Node & child );

  /** Get/Set the root node of an XML file */
  static Node GetRootNode( const XMLDoc & doc );
  static void SetRootNode( const XMLDoc & doc, const Node & root );

  /** Node tree traversal */
  static Node GetFirstChild( const Node & node );
  static Node GetNextNode( const Node & node );

  /** Set the DTD of the document */
  static void SetDTD( const XMLDoc & doc, const String & name, const String & path );

  /** An error handler to write out messages to Log */
  static void WarningHandler( void * ptr, const char * format, ...);
  static void ErrorHandler( void * ptr, const char * format, ...);

protected:
  static char * MakeMessage(const char *fmt, va_list args);


}; /* end class XML */

OT_API std::ostream & operator <<(std::ostream & os, const xmlNodePtr & node);

#endif /* OPENTURNS_HAVE_LIBXML2 */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_XMLTOOLBOX_HXX */
