//                                               -*- C++ -*-
/**
 *  @brief Factory for GeneralizedExtremeValue distribution
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
#include "openturns/GeneralizedExtremeValueFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FrechetFactory.hxx"
#include "openturns/WeibullMaxFactory.hxx"
#include "openturns/GumbelFactory.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/FittingTest.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedExtremeValueFactory)

static const Factory<GeneralizedExtremeValueFactory> Factory_GeneralizedExtremeValueFactory;

/* Default constructor */
GeneralizedExtremeValueFactory::GeneralizedExtremeValueFactory()
  : DistributionFactoryImplementation()
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}

/* Virtual constructor */
GeneralizedExtremeValueFactory * GeneralizedExtremeValueFactory::clone() const
{
  return new GeneralizedExtremeValueFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution GeneralizedExtremeValueFactory::build(const Sample & sample) const
{
  return buildAsGeneralizedExtremeValue(sample).clone();
}

Distribution GeneralizedExtremeValueFactory::build(const Point & parameters) const
{
  return buildAsGeneralizedExtremeValue(parameters).clone();
}

Distribution GeneralizedExtremeValueFactory::build() const
{
  return buildAsGeneralizedExtremeValue().clone();
}

DistributionFactoryResult GeneralizedExtremeValueFactory::buildEstimator(const Sample & sample) const
{
  return buildBootStrapEstimator(sample, true);
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildAsGeneralizedExtremeValue(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedExtremeValue distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  Collection<DistributionFactory> factoryCollection;
  factoryCollection.add(FrechetFactory());
  factoryCollection.add(GumbelFactory());
  factoryCollection.add(WeibullMaxFactory());
  Scalar bic = -1.0;
  return FittingTest::BestModelBIC(sample, factoryCollection, bic);
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildAsGeneralizedExtremeValue(const Point & parameters) const
{
  try
  {
    GeneralizedExtremeValue distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedExtremeValue distribution from the given parameters";
  }
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildAsGeneralizedExtremeValue() const
{
  return GeneralizedExtremeValue();
}


END_NAMESPACE_OPENTURNS
