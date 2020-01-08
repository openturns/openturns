//                                               -*- C++ -*-
/**
 *  @brief Factory for ChiSquare distribution
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
#include "openturns/ChiSquareFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ChiSquareFactory)

static const Factory<ChiSquareFactory> Factory_ChiSquareFactory;

/* Default constructor */
ChiSquareFactory::ChiSquareFactory():
  DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
ChiSquareFactory * ChiSquareFactory::clone() const
{
  return new ChiSquareFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution ChiSquareFactory::build(const Sample & sample) const
{
  return buildAsChiSquare(sample).clone();
}

Distribution ChiSquareFactory::build(const Point & parameters) const
{
  return buildAsChiSquare(parameters).clone();
}

Distribution ChiSquareFactory::build() const
{
  return buildAsChiSquare().clone();
}

DistributionFactoryResult ChiSquareFactory::buildEstimator(const Sample & sample) const
{
  return buildBootStrapEstimator(sample, true);
}

ChiSquare ChiSquareFactory::buildAsChiSquare(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a ChiSquare distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a ChiSquare distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar mean = sample.computeMean()[0];
  ChiSquare result(mean);
  result.setDescription(sample.getDescription());
  return result;
}

ChiSquare ChiSquareFactory::buildAsChiSquare(const Point & parameters) const
{
  try
  {
    ChiSquare distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a ChiSquare distribution from the given parameters";
  }
}

ChiSquare ChiSquareFactory::buildAsChiSquare() const
{
  return ChiSquare();
}


END_NAMESPACE_OPENTURNS
