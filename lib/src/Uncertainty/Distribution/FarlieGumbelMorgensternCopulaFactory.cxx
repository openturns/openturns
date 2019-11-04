//                                               -*- C++ -*-
/**
 *  @brief Factory for FarlieGumbelMorgensternCopula distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdlib>
#include <cmath>

#include "openturns/FarlieGumbelMorgensternCopulaFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FarlieGumbelMorgensternCopulaFactory)

static const Factory<FarlieGumbelMorgensternCopulaFactory> Factory_FarlieGumbelMorgensternCopulaFactory;

/* Default constructor */
FarlieGumbelMorgensternCopulaFactory::FarlieGumbelMorgensternCopulaFactory():
  DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
FarlieGumbelMorgensternCopulaFactory * FarlieGumbelMorgensternCopulaFactory::clone() const
{
  return new FarlieGumbelMorgensternCopulaFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution FarlieGumbelMorgensternCopulaFactory::build(const Sample & sample) const
{
  return buildAsFarlieGumbelMorgensternCopula(sample).clone();
}

Distribution FarlieGumbelMorgensternCopulaFactory::build(const Point & parameters) const
{
  return buildAsFarlieGumbelMorgensternCopula(parameters).clone();
}

Distribution FarlieGumbelMorgensternCopulaFactory::build() const
{
  return buildAsFarlieGumbelMorgensternCopula().clone();
}

FarlieGumbelMorgensternCopula FarlieGumbelMorgensternCopulaFactory::buildAsFarlieGumbelMorgensternCopula(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a FarlieGumbelMorgensternCopula distribution from an empty sample";
  if (sample.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a FarlieGumbelMorgensternCopula distribution from a sample of dimension not equal to 2";
  Scalar theta = 4.5 * sample.computeKendallTau().operator()(0, 1);
  // If it is not possible to use Kendall's tau to build the copula, try Spearman's rho
  if (std::abs(theta) > 1.0)
  {
    LOGWARN(OSS() << "Warning! Unable to build a FarlieGumbelMorgensternCopula based on Kendall's tau: it leads to theta=" << theta << ". Trying to use Spearman's rho instead.");
    theta = 3.0 * sample.computeSpearmanCorrelation().operator()(0, 1);
    if (!(std::abs(theta) <= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot build a FarlieGumbelMorgensternCopula from Spearman's rho either: it leads to theta=" << theta;
  }
  FarlieGumbelMorgensternCopula result(theta);
  result.setDescription(sample.getDescription());
  return result;
}

FarlieGumbelMorgensternCopula FarlieGumbelMorgensternCopulaFactory::buildAsFarlieGumbelMorgensternCopula(const Point & parameters) const
{
  try
  {
    FarlieGumbelMorgensternCopula copula;
    copula.setParameter(parameters);
    return copula;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a FarlieGumbelMorgensternCopula from the given parameters";
  }
}

FarlieGumbelMorgensternCopula FarlieGumbelMorgensternCopulaFactory::buildAsFarlieGumbelMorgensternCopula() const
{
  return FarlieGumbelMorgensternCopula();
}

END_NAMESPACE_OPENTURNS
