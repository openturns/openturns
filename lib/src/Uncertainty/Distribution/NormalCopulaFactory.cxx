//                                               -*- C++ -*-
/**
 *  @brief Factory for NormalCopula distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ResourceMap.hxx"
#include "openturns/SymmetricMatrix.hxx"

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
  return buildAsNormalCopula(sample);
}

Distribution NormalCopulaFactory::build(const Point & parameters) const
{
  return buildAsNormalCopula(parameters);
}

Distribution NormalCopulaFactory::build() const
{
  return buildAsNormalCopula();
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
    LOGWARN(OSS() << "Unable to build a NormalCopula using Kendall's tau, trying Spearman's rho instead.");
    try
    {
      R = NormalCopula::GetCorrelationFromSpearmanCorrelation(sample.computeSpearmanCorrelation());
    }
    catch (const NotSymmetricDefinitePositiveException &)
    {
      LOGWARN(OSS() << "Unable to build a NormalCopula using Spearman's rho, regularizing the sample linear correlation instead, see #3321.");
      R = sample.computeLinearCorrelation();
    }
  }
  try
  {
    NormalCopula result(R);
    result.setDescription(sample.getDescription());
    adaptToKnownParameter(sample, &result);
    return result;
  }
  catch (const NotSymmetricDefinitePositiveException &)
  {
    LOGWARN(OSS() << "The sample correlation matrix is not positive definite, regularizing it, see #3321.");
  }
  // Last resort: (R + eps I) / (1 + eps) with doubling eps. The unit
  // diagonal is preserved, so the result stays a correlation matrix, and
  // the loop exits at eps = 1 at the latest; beyond that point only
  // non-finite entries can still fail, and they throw below.
  Scalar regularization = ResourceMap::GetAsScalar("NormalCopulaFactory-RegularizationEpsilon");
  if (!(regularization > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected a positive regularization epsilon, here epsilon=" << regularization;
  const UnsignedInteger dimension = R.getNbRows();
  for (; regularization <= 1.0; regularization *= 2.0)
  {
    SymmetricMatrix regularizedR(R);
    const Scalar factor = 1.0 / (1.0 + regularization);
    for (UnsignedInteger i = 0; i < dimension; ++i)
      for (UnsignedInteger j = 0; j < i; ++j)
        regularizedR(i, j) *= factor;
    try
    {
      const CorrelationMatrix regularizedCorrelation(regularizedR);
      NormalCopula result(regularizedCorrelation);
      result.setDescription(sample.getDescription());
      adaptToKnownParameter(sample, &result);
      return result;
    }
    catch (const NotSymmetricDefinitePositiveException &)
    {
      // Retry with a larger epsilon
    }
  }
  throw InvalidArgumentException(HERE) << "Unable to build a NormalCopula, even regularized.";
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
