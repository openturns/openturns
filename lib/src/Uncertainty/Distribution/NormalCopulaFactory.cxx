//                                               -*- C++ -*-
/**
 *  @brief Factory for NormalCopula distribution
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
#include <cmath>
#include "openturns/NormalCopulaFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NormalCopulaFactory)

static const Factory<NormalCopulaFactory> Factory_NormalCopulaFactory;

/* Default constructor */
NormalCopulaFactory::NormalCopulaFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
NormalCopulaFactory * NormalCopulaFactory::clone() const
{
  return new NormalCopulaFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution NormalCopulaFactory::build(const Sample & sample) const
{
  return buildAsNormalCopula(sample).clone();
}

Distribution NormalCopulaFactory::build(const Point & parameters) const
{
  return buildAsNormalCopula(parameters).clone();
}

Distribution NormalCopulaFactory::build() const
{
  return buildAsNormalCopula().clone();
}

NormalCopula NormalCopulaFactory::buildAsNormalCopula(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a NormalCopula distribution from an empty sample";
  CorrelationMatrix R;
  try
  {
    R = NormalCopula::GetCorrelationFromKendallCorrelation(sample.computeKendallTau());
  }
  catch (const NotSymmetricDefinitePositiveException &)
  {
    LOGWARN(OSS() << "Warning! Unable to build a NormalCopula using Kendall's tau, trying Spearman's rho instead.");
    try
    {
      R = NormalCopula::GetCorrelationFromSpearmanCorrelation(sample.computeSpearmanCorrelation());
    }
    catch (const NotSymmetricDefinitePositiveException &)
    {
      throw InvalidArgumentException(HERE) << "Unable to build a NormalCopula using Spearman's rho";
    }
  }
  NormalCopula result(R);
  result.setDescription(sample.getDescription());
  return result;
}

NormalCopula NormalCopulaFactory::buildAsNormalCopula(const Point & parameters) const
{
  try
  {
    NormalCopula copula;
    copula.setParameter(parameters);
    return copula;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a NormalCopula from the given parameters";
  }
}

NormalCopula NormalCopulaFactory::buildAsNormalCopula() const
{
  return NormalCopula();
}

END_NAMESPACE_OPENTURNS
