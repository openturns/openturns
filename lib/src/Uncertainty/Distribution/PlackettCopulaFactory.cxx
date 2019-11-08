//                                               -*- C++ -*-
/**
 *  @brief Factory for PlackettCopula distribution
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

#include "openturns/PlackettCopulaFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Brent.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PlackettCopulaFactory)

static const Factory<PlackettCopulaFactory> Factory_PlackettCopulaFactory;

/* Default constructor */
PlackettCopulaFactory::PlackettCopulaFactory():
  DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
PlackettCopulaFactory * PlackettCopulaFactory::clone() const
{
  return new PlackettCopulaFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution PlackettCopulaFactory::build(const Sample & sample) const
{
  return buildAsPlackettCopula(sample).clone();
}

Distribution PlackettCopulaFactory::build(const Point & parameters) const
{
  return buildAsPlackettCopula(parameters).clone();
}

Distribution PlackettCopulaFactory::build() const
{
  return buildAsPlackettCopula().clone();
}

PlackettCopula PlackettCopulaFactory::buildAsPlackettCopula(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a PlackettCopula distribution from an empty sample";
  if (sample.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a PlackettCopula distribution from a sample of dimension not equal to 2";
  const Scalar m = sample.computeEmpiricalCDF(sample.computeMedian());
  const Scalar ratio = 1.0 / (0.5 / m - 1.0);
  return PlackettCopula(ratio * ratio);
}

PlackettCopula PlackettCopulaFactory::buildAsPlackettCopula(const Point & parameters) const
{
  try
  {
    PlackettCopula copula;
    copula.setParameter(parameters);
    return copula;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a PlackettCopula from the given parameters";
  }
}

PlackettCopula PlackettCopulaFactory::buildAsPlackettCopula() const
{
  return PlackettCopula();
}

END_NAMESPACE_OPENTURNS
