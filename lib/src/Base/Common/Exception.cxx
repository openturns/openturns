//                                               -*- C++ -*-
/**
 *  @brief Exception defines top-most exception strategies
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

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

END_NAMESPACE_OPENTURNS
