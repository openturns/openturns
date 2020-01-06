//                                               -*- C++ -*-
/**
 *  @brief Implementation of a generic grid iterator.
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

#include "openturns/SphereUniformNorm.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SphereUniformNorm)

/** Default constructor */
SphereUniformNorm::SphereUniformNorm()
  : TypedInterfaceObject<SphereUniformNormImplementation>(new SphereUniformNormImplementation(Point(0), true))
{
}

/** Default constructor */
SphereUniformNorm::SphereUniformNorm(const Point step, const Bool symmetric)
  : TypedInterfaceObject<SphereUniformNormImplementation>(new SphereUniformNormImplementation(step, symmetric))
{
  // Nothing to do
}

/** Copy constructor */
SphereUniformNorm::SphereUniformNorm(const SphereUniformNormImplementation & implementation)
  : TypedInterfaceObject<SphereUniformNormImplementation>(implementation.clone())
{
}

/** Constructor from implementation */
SphereUniformNorm::SphereUniformNorm(const Implementation & p_implementation)
  : TypedInterfaceObject<SphereUniformNormImplementation>(p_implementation)
{
}

/** Constructor from implementation pointer */
SphereUniformNorm::SphereUniformNorm(SphereUniformNormImplementation * p_implementation)
  : TypedInterfaceObject<SphereUniformNormImplementation>(p_implementation)
{
}

/** Constructor from grid steps */
SphereUniformNorm SphereUniformNorm::GetFromGridSteps(const Point & step, const Bool symmetric)
{
  SphereUniformNorm result(SphereUniformNormImplementation::GetFromGridSteps(step, symmetric));
  return result;
}

/** Symmettric accessor */
Bool SphereUniformNorm::isSymmetric() const
{
  return getImplementation()->isSymmetric();
}

Sample SphereUniformNorm::getPoints(const UnsignedInteger distance) const
{
  return getImplementation()->getPoints(distance);
}

/** Get index offset, needed by RandomMixture cache */
UnsignedInteger SphereUniformNorm::getOffsetLevel(const UnsignedInteger distance) const
{
  return getImplementation()->getOffsetLevel(distance);
}

/* String converter */
String SphereUniformNorm::__repr__() const
{
  return getImplementation()->__repr__();
}

String SphereUniformNorm::__str__(const String & offset) const
{
  return getImplementation()->__str__( offset );
}

END_NAMESPACE_OPENTURNS

