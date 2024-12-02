//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the identity function
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/VertexFunction.hxx"
#include "openturns/VertexEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(VertexFunction)

/* Default constructor */
VertexFunction::VertexFunction()
  : Function()
{
  // Nothing to do
}

/* Parameters constructor */
VertexFunction::VertexFunction(const PointToFieldFunction & function,
                               const Indices & indices)
  : Function(new VertexEvaluation(function, indices))
{
  // Nothing to do
}

/* Comparison operator */
Bool VertexFunction::operator ==(const VertexFunction & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* String converter */
String VertexFunction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << VertexFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

String VertexFunction::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << VertexFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__str__();
  return oss;
}

END_NAMESPACE_OPENTURNS
