//                                               -*- C++ -*-
/**
 *  @brief Factory for Beta distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/BetaFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/BetaMuSigma.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BetaFactory)

static const Factory<BetaFactory> Factory_BetaFactory;

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

Distribution BetaFactory::build(const Sample & sample) const
{
  return buildAsBeta(sample).clone();
}

Distribution BetaFactory::build(const Point & parameters) const
{
  return buildAsBeta(parameters).clone();
}

Distribution BetaFactory::build() const
{
  return buildAsBeta().clone();
}

Beta BetaFactory::buildAsBeta(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Beta distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Beta distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  Scalar delta = xMax - xMin;
  const Scalar a = xMin - delta / (size + 2);
  const Scalar b = xMax + delta / (size + 2);
  if (!SpecFunc::IsNormal(a) || !SpecFunc::IsNormal(b)) throw InvalidArgumentException(HERE) << "Error: cannot build a Beta distribution if data contains NaN or Inf";
  if (xMin == xMax) throw InvalidArgumentException(HERE) << "Error: cannot estimate a Beta distribution from a constant sample.";

  const Scalar mu = sample.computeMean()[0];
  const Scalar sigma = sample.computeStandardDeviation()[0];
  return buildAsBeta(BetaMuSigma(mu, sigma, a, b).evaluate());
}

Beta BetaFactory::buildAsBeta(const Point & parameters) const
{
  try
  {
    Beta distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Beta distribution from the given parameters";
  }
}

Beta BetaFactory::buildAsBeta() const
{
  return Beta();
}


END_NAMESPACE_OPENTURNS
