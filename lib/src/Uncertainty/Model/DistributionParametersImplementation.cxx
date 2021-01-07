//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions parameters
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistributionParametersImplementation.hxx"
#include "openturns/DistributionParameters.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DistributionParametersImplementation)

static const Factory<DistributionParametersImplementation> Factory_DistributionParametersImplementation;

/* Default constructor */
DistributionParametersImplementation::DistributionParametersImplementation()
  : PersistentObject()
{

}


/* Virtual constructor */
DistributionParametersImplementation * DistributionParametersImplementation::clone() const
{
  return new DistributionParametersImplementation(*this);
}


/* Build a distribution based on a set of native parameters */
Distribution DistributionParametersImplementation::getDistribution() const
{
  throw NotYetImplementedException(HERE) << "In DistributionParametersImplementation::getDistribution() const";
}


/* Compute jacobian / native parameters */
Point DistributionParametersImplementation::evaluate() const
{
  return operator()(getValues());
}


/* Compute jacobian / native parameters */
Matrix DistributionParametersImplementation::gradient() const
{
  throw NotYetImplementedException(HERE) << "In DistributionParametersImplementation::gradient() const";
}


/* Conversion operator */
Point DistributionParametersImplementation::operator () (const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In DistributionParametersImplementation::operator() const";
}


/* Conversion from native parameters */
Point DistributionParametersImplementation::inverse(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In DistributionParametersImplementation::inverse const";
}


/* Parameters value and description accessor */
void DistributionParametersImplementation::setValues(const Point & )
{
  throw NotYetImplementedException(HERE) << "In DistributionParametersImplementation::setValues()";
}


Point DistributionParametersImplementation::getValues() const
{
  throw NotYetImplementedException(HERE) << "In DistributionParametersImplementation::getValues() const";
}

Description DistributionParametersImplementation::getDescription() const
{
  throw NotYetImplementedException(HERE) << "In DistributionParametersImplementation::getDescription() const";
}

/* String converter */
String DistributionParametersImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << DistributionParametersImplementation::GetClassName();
  return oss;
}


/* String converter */
String DistributionParametersImplementation::__str__(const String & ) const
{
  return __repr__();
}


/* Method save() stores the object through the StorageManager */
void DistributionParametersImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void DistributionParametersImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
}

END_NAMESPACE_OPENTURNS

