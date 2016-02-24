//                                               -*- C++ -*-
/**
 *  @brief Factory for Gumbel distribution
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
#include "openturns/GumbelFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GumbelFactory);

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

GumbelFactory::Implementation GumbelFactory::build(const NumericalSample & sample) const
{
  return buildAsGumbel(sample).clone();
}

GumbelFactory::Implementation GumbelFactory::build(const NumericalPoint & parameters) const
{
  return buildAsGumbel(parameters).clone();
}

GumbelFactory::Implementation GumbelFactory::build() const
{
  return buildAsGumbel().clone();
}

DistributionFactoryResult GumbelFactory::buildEstimator(const NumericalSample & sample) const
{
  return buildBootStrapEstimator(sample, true);
}

Gumbel GumbelFactory::buildAsGumbel(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Gumbel distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Gumbel distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  NumericalScalar mu(sample.computeMean()[0]);
  NumericalScalar sigma(sample.computeStandardDeviationPerComponent()[0]);
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "Error: can build a Gumbel distribution only if sigma > 0, here sigma=" << sigma;
  Gumbel result(mu, sigma, Gumbel::MUSIGMA);
  result.setDescription(sample.getDescription());
  return result;
}

Gumbel GumbelFactory::buildAsGumbel(const NumericalPoint & parameters) const
{
  try
  {
    Gumbel distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Gumbel distribution from the given parameters";
  }
}

Gumbel GumbelFactory::buildAsGumbel() const
{
  return Gumbel();
}

END_NAMESPACE_OPENTURNS
