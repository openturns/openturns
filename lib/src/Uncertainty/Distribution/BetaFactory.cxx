//                                               -*- C++ -*-
/**
 *  @brief Factory for Beta distribution
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
#include "openturns/BetaFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(BetaFactory);

/* Default constructor */
BetaFactory::BetaFactory():
  DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
BetaFactory * BetaFactory::clone() const
{
  return new BetaFactory(*this);
}


/* Here is the interface that all derived class must implement */

BetaFactory::Implementation BetaFactory::build(const NumericalSample & sample) const
{
  return buildAsBeta(sample).clone();
}

BetaFactory::Implementation BetaFactory::build(const NumericalPoint & parameters) const
{
  return buildAsBeta(parameters).clone();
}

BetaFactory::Implementation BetaFactory::build() const
{
  return buildAsBeta().clone();
}

Beta BetaFactory::buildAsBeta(const NumericalSample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Beta distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Beta distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const NumericalScalar xMin(sample.getMin()[0]);
  const NumericalScalar a(xMin - std::abs(xMin) / (2.0 + size));
  const NumericalScalar xMax(sample.getMax()[0]);
  const NumericalScalar b(xMax + std::abs(xMax) / (2.0 + size));
  if (!SpecFunc::IsNormal(a) || !SpecFunc::IsNormal(b)) throw InvalidArgumentException(HERE) << "Error: cannot build a Beta distribution if data contains NaN or Inf";
  if (xMin == xMax)
    {
      const NumericalScalar delta(std::max(std::abs(xMin), 100.0) * SpecFunc::NumericalScalarEpsilon);
      Beta result(1.0, 2.0, xMin - delta, xMax + delta);
      result.setDescription(sample.getDescription());
      return result;
    }
  const NumericalScalar mean(sample.computeMean()[0]);
  const NumericalScalar sigma(sample.computeStandardDeviationPerComponent()[0]);
  const NumericalScalar t((b - mean) * (mean - a) / (sigma * sigma) - 1.0);
  const NumericalScalar r(t * (mean - a) / (b - a));
  Beta result(r, t, a, b);
  result.setDescription(sample.getDescription());
  return result;
}

Beta BetaFactory::buildAsBeta(const NumericalPoint & parameters) const
{
  try
  {
    Beta distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Beta distribution from the given parameters";
  }
}

Beta BetaFactory::buildAsBeta() const
{
  return Beta();
}


END_NAMESPACE_OPENTURNS
