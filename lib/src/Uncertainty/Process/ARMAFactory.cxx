//                                               -*- C++ -*-
/**
 *  @brief The class that implements all random vectors
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
#include "openturns/ARMAFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ARMAFactory)

/* Default constructor */
ARMAFactory::ARMAFactory()
  : TypedInterfaceObject<ARMAFactoryImplementation>(new ARMAFactoryImplementation())
{
  // Nothing to do
}

/* Parameters constructor */
ARMAFactory::ARMAFactory(const ARMAFactoryImplementation & implementation)
  : TypedInterfaceObject<ARMAFactoryImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
ARMAFactory::ARMAFactory(const Implementation & p_implementation)
  : TypedInterfaceObject<ARMAFactoryImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
ARMAFactory::ARMAFactory(ARMAFactoryImplementation * p_implementation)
  : TypedInterfaceObject<ARMAFactoryImplementation>(p_implementation)
{
  // Nothing to do
}

/* Virtual constructor */
ARMAFactory * ARMAFactory::clone() const
{
  return new ARMAFactory(*this);
}

/* String converter */
String ARMAFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String ARMAFactory::__str__(const String & ) const
{
  return __repr__();
}

/* Build method */
ARMA ARMAFactory::build(const TimeSeries & timeSeries) const
{
  return getImplementation()->build(timeSeries);
}

/* Build method */
ARMA ARMAFactory::build(const ProcessSample & sample) const
{
  return getImplementation()->build(sample);
}

END_NAMESPACE_OPENTURNS
