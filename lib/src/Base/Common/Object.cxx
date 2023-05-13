//                                               -*- C++ -*-
/**
 *  @brief The class Object is the top-most class of OpenTURNS
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Object.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Object)

/* Destructor */
Object::~Object()
{
  // Nothing to do
}

/* String converter */
String Object::__repr__() const
{
  return OSS(true) << "class=" << getClassName();
}

String Object::__repr_html__() const
{
  return __repr__();
}

/* Pretty string converter */
String Object::__str__(const String & ) const
{
  return __repr__();
}

/* Markdown string converter */
String Object::__repr_markdown__() const
{
  return __repr__();
}

/* Output stream converter */
std::ostream & operator <<(std::ostream & os, const Object & obj)
{
  return os << obj.__str__();
}

END_NAMESPACE_OPENTURNS
