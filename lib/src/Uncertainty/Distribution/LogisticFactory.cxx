//                                               -*- C++ -*-
/**
 *  @brief Factory for Logistic distribution
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
#include "openturns/LogisticFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LogisticFactory)

static const Factory<LogisticFactory> Factory_LogisticFactory;

/* Default constructor */
LogisticFactory::LogisticFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
LogisticFactory * LogisticFactory::clone() const
{
  return new LogisticFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution LogisticFactory::build(const Sample & sample) const
{
  return buildAsLogistic(sample).clone();
}

Distribution LogisticFactory::build(const Point & parameters) const
{
  return buildAsLogistic(parameters).clone();
}

Distribution LogisticFactory::build() const
{
  return buildAsLogistic().clone();
}

DistributionFactoryResult LogisticFactory::buildEstimator(const Sample & sample) const
{
  return buildBootStrapEstimator(sample, true);
}

Logistic LogisticFactory::buildAsLogistic(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Logistic distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Logistic distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  Scalar mu = sample.computeMean()[0];
  Scalar beta = sample.computeStandardDeviation()[0] * SpecFunc::SQRT3_PI;
  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Error: can build a Logistic distribution only if beta > 0.0, here beta=" << beta;
  Logistic result(mu, beta);
  result.setDescription(sample.getDescription());
  return result;
}

Logistic LogisticFactory::buildAsLogistic(const Point & parameters) const
{
  try
  {
    Logistic distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Logistic distribution from the given parameters";
  }
}

Logistic LogisticFactory::buildAsLogistic() const
{
  return Logistic();
}


END_NAMESPACE_OPENTURNS
