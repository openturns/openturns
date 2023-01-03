//                                               -*- C++ -*-
/**
 *  @brief Factory for Gumbel distribution
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
#include "openturns/GumbelFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/GumbelMuSigma.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GumbelFactory)

static const Factory<GumbelFactory> Factory_GumbelFactory;

/* Default constructor */
GumbelFactory::GumbelFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
GumbelFactory * GumbelFactory::clone() const
{
  return new GumbelFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution GumbelFactory::build(const Sample & sample) const
{
  return buildAsGumbel(sample).clone();
}

Distribution GumbelFactory::build(const Point & parameters) const
{
  return buildAsGumbel(parameters).clone();
}

Distribution GumbelFactory::build() const
{
  return buildAsGumbel().clone();
}

DistributionFactoryResult GumbelFactory::buildEstimator(const Sample & sample) const
{
  return buildBootStrapEstimator(sample, true);
}

Gumbel GumbelFactory::buildAsGumbel(const Sample & sample) const
{
  if (sample.getSize() < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Gumbel distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Gumbel distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar mu = sample.computeMean()[0];
  const Scalar sigma = sample.computeStandardDeviation()[0];
  const Point parameters = {mu, sigma};
  Gumbel result(buildAsGumbel(GumbelMuSigma()(parameters)));
  result.setDescription(sample.getDescription());
  return result;
}

Gumbel GumbelFactory::buildAsGumbel(const Point & parameters) const
{
  try
  {
    Gumbel distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Gumbel distribution from the given parameters";
  }
}

Gumbel GumbelFactory::buildAsGumbel() const
{
  return Gumbel();
}

END_NAMESPACE_OPENTURNS
