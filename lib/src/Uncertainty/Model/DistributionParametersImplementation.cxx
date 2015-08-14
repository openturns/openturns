//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions parameters
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#include "PersistentObjectFactory.hxx"
#include "DistributionParametersImplementation.hxx"
#include "DistributionParameters.hxx"
#include "Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DistributionParametersImplementation);

// static Factory<DistributionParametersImplementation> RegisteredFactory("DistributionParametersImplementation");

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
Matrix DistributionParametersImplementation::gradient() const
{
  throw NotYetImplementedException(HERE) << "In DistributionParametersImplementation::gradient() const";
}


/* Conversion operator */
NumericalPoint DistributionParametersImplementation::operator () (const NumericalPoint & inP) const
{
  throw NotYetImplementedException(HERE) << "In DistributionParametersImplementation::operator() const";
}


/* Parameters value and description accessor */
DistributionParametersImplementation::NumericalPointWithDescriptionCollection DistributionParametersImplementation::getParametersCollection() const
{
  throw NotYetImplementedException(HERE) << "In DistributionParametersImplementation::getParametersCollection() const";
}


/* String converter */
String DistributionParametersImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << DistributionParametersImplementation::GetClassName();
  return oss;
}


/* String converter */
String DistributionParametersImplementation::__str__(const String & offset) const
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