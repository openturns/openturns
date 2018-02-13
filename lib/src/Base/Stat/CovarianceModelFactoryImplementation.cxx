//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/CovarianceModelFactoryImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CovarianceModelFactoryImplementation)
static const Factory<CovarianceModelFactoryImplementation> Factory_CovarianceModelFactoryImplementation;

/* Default constructor */
CovarianceModelFactoryImplementation::CovarianceModelFactoryImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
CovarianceModelFactoryImplementation * CovarianceModelFactoryImplementation::clone() const
{
  return new CovarianceModelFactoryImplementation(*this);
}

/* String converter */
String CovarianceModelFactoryImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CovarianceModelFactoryImplementation::GetClassName();
  return oss;
}

/* String converter */
String CovarianceModelFactoryImplementation::__str__(const String & offset) const
{
  return this->getClassName();
}

/* Here is the interface that all derived class must implement */

CovarianceModelImplementation::Implementation CovarianceModelFactoryImplementation::build(const FieldSample & sample) const
{
  throw NotYetImplementedException(HERE) << "In CovarianceModelFactoryImplementation::build(const FieldSample & sample) const";
}

CovarianceModelImplementation::Implementation CovarianceModelFactoryImplementation::build(const Field & timeSeries) const
{
  throw NotYetImplementedException(HERE) << "In CovarianceModelFactoryImplementation::build(const Field & timeSeries) const";
}

/* Method save() stores the object through the StorageManager */
void CovarianceModelFactoryImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void CovarianceModelFactoryImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
}
END_NAMESPACE_OPENTURNS
