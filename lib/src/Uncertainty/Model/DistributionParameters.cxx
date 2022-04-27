//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distribution parameters
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DistributionParameters.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(DistributionParameters)

/* Default constructor */
DistributionParameters::DistributionParameters()
  : TypedInterfaceObject<DistributionParametersImplementation>(new DistributionParametersImplementation)
{
  // Nothing to do
}

/* Default constructor */
DistributionParameters::DistributionParameters(const DistributionParametersImplementation & implementation)
  : TypedInterfaceObject<DistributionParametersImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
DistributionParameters::DistributionParameters(const Implementation & p_implementation)
  : TypedInterfaceObject<DistributionParametersImplementation>(p_implementation)
{
  // Nothing to do
}


/* Constructor from implementation pointer */
DistributionParameters::DistributionParameters(DistributionParametersImplementation * p_implementation)
  : TypedInterfaceObject<DistributionParametersImplementation>(p_implementation)
{
  // Nothing to do
}


/* Build a distribution based on a set of native parameters */
Distribution DistributionParameters::getDistribution() const
{
  return getImplementation()->getDistribution();
}


/* Compute jacobian / native parameters */
Point DistributionParameters::evaluate() const
{
  return getImplementation()->evaluate();
}


/* Compute jacobian / native parameters */
Matrix DistributionParameters::gradient() const
{
  return getImplementation()->gradient();
}


/* Conversion operator */
Point DistributionParameters::operator () (const Point & inP) const
{
  return getImplementation()->operator ()(inP);
}


/* Conversion from native parameters */
Point DistributionParameters::inverse(const Point & inP) const
{
  return getImplementation()->inverse(inP);
}


/* Parameters value and description accessor */
void DistributionParameters::setValues(const Point & values)
{
  copyOnWrite();
  getImplementation()->setValues(values);
}

Point DistributionParameters::getValues() const
{
  return getImplementation()->getValues();
}

Description DistributionParameters::getDescription() const
{
  return getImplementation()->getDescription();
}

/* String converter */
String DistributionParameters::__repr__() const
{
  return getImplementation()->__repr__();
}

/* String converter */
String DistributionParameters::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}


END_NAMESPACE_OPENTURNS
