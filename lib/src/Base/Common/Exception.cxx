//                                               -*- C++ -*-
/**
 *  @brief Exception defines top-most exception strategies
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdlib>
#include <assert.h>
//#include <typeinfo>
#include "openturns/OSS.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS


/* Default constructor */
Exception::Exception(const PointInSourceFile & point)
  : std::exception()
  , point_(point)
  , reason_()
  , className_("Exception")
{
  // Nothing to do
  //LOGDEBUG(OSS() << className_  << " typeid = " << typeid(*this).name() );
}

/* Copy constructor */
Exception::Exception(const Exception & other)
  : std::exception(other)
  , point_(other.point_)
  , reason_(other.reason_)
  , className_(other.className_)
{
  //LOGDEBUG(OSS() << className_  << " emited at " << point_.str() << ": " << String(reason_) );
}

/* Inheritance constructor */
Exception::Exception(const PointInSourceFile & point,
                     const char * className)
  : std::exception()
  , point_(point)
  , reason_()
  , className_(className)
{
  // Nothing to do
}


/* Destructor */
Exception::~Exception() throw()
{
  // Nothing to do
}


/* String converter */
String Exception::__repr__() const throw()
{
  return OSS() << className_ << " : " << reason_;
}


/* Point accessor */
const char * Exception::where() const throw()
{
  return point_.str().c_str();
}


/* Reason accessor */
const char * Exception::what() const throw()
{
  return reason_.c_str();
}


/* Class name accessor */
const char * Exception::type() const throw()
{
  return className_;
}


/*
 * @fn std::ostream & operator <<(std::ostream & os, const Exception & obj)
 * @brief Output stream converter
 * @param os A STL output stream exception
 * @param obj The exception read by \em os
 * @return A reference to \em os
 *
 * Operator << converts the Exception object to an output stream
 * so it is easy to show the reason of the exception.
 */
std::ostream & operator <<(std::ostream & os, const Exception & obj)
{
  return os << obj.__repr__();
}

OStream & operator <<(OStream & OS, const Exception & obj)
{
  return OS << obj.__repr__();
}

#define DEFINE_EXCEPTION( CName )               \
  static const char * CName ## Name = #CName;   \
  static const CName CName ## Obj ( HERE );     \
  CName::CName(const PointInSourceFile & point) \
    : Exception(point, CName ## Name) {}        \
  CName::~CName () throw() {}

DEFINE_EXCEPTION( FileNotFoundException )
DEFINE_EXCEPTION( InternalException )
DEFINE_EXCEPTION( InvalidArgumentException )
DEFINE_EXCEPTION( InvalidDimensionException )
DEFINE_EXCEPTION( NotYetImplementedException )
DEFINE_EXCEPTION( OutOfBoundException )
DEFINE_EXCEPTION( XMLException )
DEFINE_EXCEPTION( XMLParserException )
DEFINE_EXCEPTION( DynamicLibraryException )
DEFINE_EXCEPTION( NotSymmetricDefinitePositiveException )
DEFINE_EXCEPTION( InvalidRangeException )
DEFINE_EXCEPTION( NotDefinedException )
DEFINE_EXCEPTION( FileOpenException )
DEFINE_EXCEPTION( StudyFileParsingException )
DEFINE_EXCEPTION( ObjectNotInStudyException )
DEFINE_EXCEPTION( ConfigurationFileParsingException )

#undef DEFINE_EXCEPTION

END_NAMESPACE_OPENTURNS
