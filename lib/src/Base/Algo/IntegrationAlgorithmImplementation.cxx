//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/IntegrationAlgorithmImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IntegrationAlgorithmImplementation
 */

CLASSNAMEINIT(IntegrationAlgorithmImplementation)

static const Factory<IntegrationAlgorithmImplementation> Factory_IntegrationAlgorithmImplementation;

/* Constructor without parameters */
IntegrationAlgorithmImplementation::IntegrationAlgorithmImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
IntegrationAlgorithmImplementation * IntegrationAlgorithmImplementation::clone() const
{
  return new IntegrationAlgorithmImplementation(*this);
}

/* Compute an approximation of \int_{[a,b]}f(x)dx, where [a,b]
 * is an n-D interval
 */
Point IntegrationAlgorithmImplementation::integrate(const Function & function,
    const Interval & interval) const
{
  Scalar error = 0.0;
  return integrate(function, interval, error);
}

Point IntegrationAlgorithmImplementation::integrate(const Function & function,
    const Interval & interval,
    Point & error) const
{
  error = Point(1);
  return integrate(function, interval, error[0]);
}

Point IntegrationAlgorithmImplementation::integrate(const Function &,
    const Interval &,
    Scalar & ) const
{
  throw NotYetImplementedException(HERE) << "In IntegrationAlgorithmImplementation::integrate(const Function & function, const Interval & interval, Scalar & error) const";
}

/* String converter */
String IntegrationAlgorithmImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IntegrationAlgorithmImplementation::GetClassName();
  return oss;
}

/* String converter */
String IntegrationAlgorithmImplementation::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << IntegrationAlgorithmImplementation::GetClassName();
  return oss;
}

END_NAMESPACE_OPENTURNS
