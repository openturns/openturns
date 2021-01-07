//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for adaptive strategies
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
#include "openturns/AdaptiveStrategy.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FixedStrategy.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(AdaptiveStrategy)

/* Constructor from an orthogonal basis */
AdaptiveStrategy::AdaptiveStrategy()
  : TypedInterfaceObject<AdaptiveStrategyImplementation>(new AdaptiveStrategyImplementation)
{
  // Nothing to do
}


/* Constructor from an orthogonal basis */
AdaptiveStrategy::AdaptiveStrategy(const OrthogonalBasis & basis,
                                   const UnsignedInteger maximumDimension)
  : TypedInterfaceObject<AdaptiveStrategyImplementation>(new FixedStrategy(basis, maximumDimension))
{
  // Nothing to do
}


/* Constructor from implementation */
AdaptiveStrategy::AdaptiveStrategy(const AdaptiveStrategyImplementation & implementation)
  : TypedInterfaceObject<AdaptiveStrategyImplementation>(implementation.clone())
{
  // Nothing to do
}


/* Basis accessor */
OrthogonalBasis AdaptiveStrategy::getBasis() const
{
  return getImplementation()->getBasis();
}


/* Maximum dimension accessor */
void AdaptiveStrategy::setMaximumDimension(const UnsignedInteger maximumDimension)
{
  copyOnWrite();
  getImplementation()->setMaximumDimension(maximumDimension);
}

UnsignedInteger AdaptiveStrategy::getMaximumDimension() const
{
  return getImplementation()->getMaximumDimension();
}


/* Compute initial basis for the approximation */
void AdaptiveStrategy::computeInitialBasis()
{
  getImplementation()->computeInitialBasis();
}


/* Update the basis for the next iteration of approximation */
void AdaptiveStrategy::updateBasis(const Point & alpha_k,
                                   const Scalar residual,
                                   const Scalar relativeError)

{
  getImplementation()->updateBasis(alpha_k, residual, relativeError);
}


/* Psi accessor */
AdaptiveStrategy::FunctionCollection AdaptiveStrategy::getPsi() const
{
  return getImplementation()->getPsi();
}


/* String converter */
String AdaptiveStrategy::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}


/* String converter */
String AdaptiveStrategy::__str__(const String & ) const
{
  return __repr__();
}




END_NAMESPACE_OPENTURNS
