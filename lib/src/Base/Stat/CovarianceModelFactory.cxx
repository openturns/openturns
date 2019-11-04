//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
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

#include "openturns/CovarianceModelFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CovarianceModelFactory)

/* Default constructor */
CovarianceModelFactory::CovarianceModelFactory()
  : TypedInterfaceObject<CovarianceModelFactoryImplementation>(new CovarianceModelFactoryImplementation())
{
  // Nothing to do
}

/* Parameters constructor */
CovarianceModelFactory::CovarianceModelFactory(const CovarianceModelFactoryImplementation & implementation)
  : TypedInterfaceObject<CovarianceModelFactoryImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
CovarianceModelFactory::CovarianceModelFactory(const Implementation & p_implementation)
  : TypedInterfaceObject<CovarianceModelFactoryImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
CovarianceModelFactory::CovarianceModelFactory(CovarianceModelFactoryImplementation * p_implementation)
  : TypedInterfaceObject<CovarianceModelFactoryImplementation>(p_implementation)
{
  // Nothing to do
}


/* Virtual constructor */
CovarianceModelFactory * CovarianceModelFactory::clone() const
{
  return new CovarianceModelFactory(*this);
}

/* String converter */
String CovarianceModelFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CovarianceModelFactory::GetClassName()
      << " implementation = " << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String CovarianceModelFactory::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

CovarianceModel CovarianceModelFactory::build(const ProcessSample & sample) const
{
  return getImplementation()->build(sample);
}

CovarianceModel CovarianceModelFactory::build(const Field & timeSeries) const
{
  return getImplementation()->build(timeSeries);
}

END_NAMESPACE_OPENTURNS
