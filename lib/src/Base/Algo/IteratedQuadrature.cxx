//                                               -*- C++ -*-
/**
 *  @brief Implement an iterated quadrature based on a 1D quadrature
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
#include "openturns/IteratedQuadrature.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DatabaseFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IteratedQuadrature
 */

CLASSNAMEINIT(IteratedQuadrature)

static const Factory<IteratedQuadrature> Factory_IteratedQuadrature;

/* Constructor without parameters */
IteratedQuadrature::IteratedQuadrature()
  : IntegrationAlgorithmImplementation()
  , algorithm_(GaussKronrod(ResourceMap::GetAsUnsignedInteger("IteratedQuadrature-MaximumSubIntervals"), ResourceMap::GetAsScalar("IteratedQuadrature-MaximumError"), GaussKronrodRule(GaussKronrodRule::G3K7)))
{
  // Nothing to do
}

/* Parameters constructor */
IteratedQuadrature::IteratedQuadrature(const IntegrationAlgorithm & algorithm)
  : IntegrationAlgorithmImplementation()
  , algorithm_(algorithm)
{
  // Nothing to do
}

/* Virtual constructor */
IteratedQuadrature * IteratedQuadrature::clone() const
{
  return new IteratedQuadrature(*this);
}

/* Compute an approximation of \int_a^b\int_{L_1(x_1)}^{U_1(x_1)}\int_{L_1(x_1,x_2)}^{U_2(x_1,x_2)}\dots\int_{L_1(x_1,\dots,x_{n-1})}^{U_2(x_1,\dots,x_{n-1})} f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an 1D interval, L_k and U_k are functions from R^k into R.
 */
Point IteratedQuadrature::integrate(const Function & function,
                                    const Scalar a,
                                    const Scalar b,
                                    const FunctionCollection & lowerBounds,
                                    const FunctionCollection & upperBounds,
                                    const Bool check) const
{
  const UnsignedInteger inputDimension = function.getInputDimension();
  if (check)
  {
    // Check the compatibility of the lower bound functions and the upper bound functions
    // First, check the number of such functions
    if (lowerBounds.getSize() != inputDimension - 1) throw InvalidArgumentException(HERE) << "Error: expected " << inputDimension - 1 << " lower bound functions, got " << lowerBounds.getSize();
    if (upperBounds.getSize() != inputDimension - 1) throw InvalidArgumentException(HERE) << "Error: expected " << inputDimension - 1 << " upper bound functions, got " << upperBounds.getSize();
    // Second, check the input and output dimensions of each bound function
    for (UnsignedInteger i = 1; i < inputDimension; ++i)
    {
      const Function lower(lowerBounds[i - 1]);
      if (lower.getInputDimension() != i) throw InvalidArgumentException(HERE) << "Error: expected a lower bound function of rank=" << i - 1 << " to be of input dimension=" << i << ", got " << lower.getInputDimension();
      if (lower.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a lower bound function of rank=" << i - 1 << " to be of output dimension=1, got " << lower.getOutputDimension();
      const Function upper(lowerBounds[i - 1]);
      if (upper.getInputDimension() != i) throw InvalidArgumentException(HERE) << "Error: expected an upper bound function of rank=" << i - 1 << " to be of input dimension=" << i << ", got " << upper.getInputDimension();
      if (upper.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected an upper bound function of rank=" << i - 1 << " to be of output dimension=1, got " << upper.getOutputDimension();
    } // bounds dimensions
  } // check
  if (inputDimension == 1) return algorithm_.integrate(function, Interval(a, b));
  // Prepare the integrand using a PartialFunctionWrapper::evaluate
  const Function partialFunction(PartialFunctionWrapper(*this, function, lowerBounds, upperBounds).clone());
  return algorithm_.integrate(partialFunction, Interval(a, b));
}

Point IteratedQuadrature::integrate(const Function & function,
                                    const Interval & interval) const
{
  const UnsignedInteger inputDimension = function.getInputDimension();
  if (interval.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected an interval of dimension=" << inputDimension << ", got dimension=" << interval.getDimension();
  if (interval.getDimension() == 1) return algorithm_.integrate(function, interval);
  // Build the bound functions associated with the interval
  const Point lower(interval.getLowerBound());
  const Point upper(interval.getUpperBound());
  Scalar a = lower[0];
  Scalar b = upper[0];
  FunctionCollection lowerBounds(inputDimension - 1);
  FunctionCollection upperBounds(inputDimension - 1);
  for (UnsignedInteger i = 1; i < inputDimension; ++i)
  {
    lowerBounds[i - 1] = DatabaseFunction(Sample(1, i), Sample(1, Point(1, lower[i])));
    upperBounds[i - 1] = DatabaseFunction(Sample(1, i), Sample(1, Point(1, upper[i])));
  }
  return integrate(function, a, b, lowerBounds, upperBounds, false);
}

/* String converter */
String IteratedQuadrature::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IteratedQuadrature::GetClassName()
      << ", algorithm=" << algorithm_;
  return oss;
}

/* String converter */
String IteratedQuadrature::__str__(const String & ) const
{
  OSS oss(false);
  oss << IteratedQuadrature::GetClassName()
      << "(algorithm=" << algorithm_
      << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void IteratedQuadrature::save(Advocate & adv) const
{
  IntegrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("algorithm_", algorithm_);
}

/* Method load() reloads the object from the StorageManager */
void IteratedQuadrature::load(Advocate & adv)
{
  IntegrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("algorithm_", algorithm_);
}

END_NAMESPACE_OPENTURNS
