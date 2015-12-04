//                                               -*- C++ -*-
/**
 *  @brief Factory for NormalCopula distribution
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
#include <cmath>
#include "NormalCopulaFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NormalCopulaFactory);

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

NormalCopulaFactory::Implementation NormalCopulaFactory::build(const NumericalSample & sample) const
{
  return buildAsNormalCopula(sample).clone();
}

NormalCopulaFactory::Implementation NormalCopulaFactory::build(const NumericalPoint & parameters) const
{
  return buildAsNormalCopula(parameters).clone();
}

NormalCopulaFactory::Implementation NormalCopulaFactory::build() const
{
  return buildAsNormalCopula().clone();
}

NormalCopula NormalCopulaFactory::buildAsNormalCopula(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a NormalCopula distribution from an empty sample";
  try
  {
    NormalCopula result(NormalCopula::GetCorrelationFromKendallCorrelation(sample.computeKendallTau()));
    result.setDescription(sample.getDescription());
    return result;
  }
  catch (...)
  {
    LOGWARN(OSS() << "Warning! Unable to build a NormalCopula using Kendall's tau, trying Spearman's rho instead.");
    return NormalCopula(NormalCopula::GetCorrelationFromSpearmanCorrelation(sample.computeSpearmanCorrelation()));
  }
}

NormalCopula NormalCopulaFactory::buildAsNormalCopula(const NumericalPoint & parameters) const
{
  try
  {
    NormalCopula copula;
    copula.setParameter(parameters);
    return copula;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a NormalCopula from the given parameters";
  }
}

NormalCopula NormalCopulaFactory::buildAsNormalCopula() const
{
  return NormalCopula();
}

END_NAMESPACE_OPENTURNS
