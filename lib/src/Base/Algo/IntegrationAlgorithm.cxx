//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/IntegrationAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/GaussKronrod.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IntegrationAlgorithm)

/* Constructor with parameters */
IntegrationAlgorithm::IntegrationAlgorithm()
  : TypedInterfaceObject<IntegrationAlgorithmImplementation>(new GaussKronrod())
{
  // Nothing to do
}

/* Parameters constructor */
IntegrationAlgorithm::IntegrationAlgorithm(const IntegrationAlgorithmImplementation & implementation)
  : TypedInterfaceObject<IntegrationAlgorithmImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
IntegrationAlgorithm::IntegrationAlgorithm(const Implementation & p_implementation)
  : TypedInterfaceObject<IntegrationAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
IntegrationAlgorithm::IntegrationAlgorithm(IntegrationAlgorithmImplementation * p_implementation)
  : TypedInterfaceObject<IntegrationAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}


/* Compute an approximation of \int_{[a,b]}f(x)dx, where [a,b]
 * is an n-D interval
 */
Point IntegrationAlgorithm::integrate(const Function & function,
                                      const Interval & interval) const
{
  return getImplementation()->integrate(function, interval);
}

Point IntegrationAlgorithm::integrate(const Function & function,
                                      const Interval & interval,
                                      Scalar & error) const
{
  return getImplementation()->integrate(function, interval, error);
}

Point IntegrationAlgorithm::integrate(const Function & function,
                                      const Interval & interval,
                                      Point & error) const
{
  return getImplementation()->integrate(function, interval, error);
}

/* String converter */
String IntegrationAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IntegrationAlgorithm::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String IntegrationAlgorithm::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << IntegrationAlgorithm::GetClassName()
      << " implementation=" << getImplementation()->__str__(offset);
  return oss;
}

END_NAMESPACE_OPENTURNS
