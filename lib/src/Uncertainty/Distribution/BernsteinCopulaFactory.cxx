//                                               -*- C++ -*-
/**
 *  @brief This class implements a non parametric density estimator for copulas
 *         based on the Bernstein copula approximation of the empirical copula
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/BernsteinCopulaFactory.hxx"
#include "openturns/Beta.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/Mixture.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BernsteinCopulaFactory
 *
 * The class describes the probabilistic concept of BernsteinCopulaFactory.
 */

CLASSNAMEINIT(BernsteinCopulaFactory);

static const Factory<BernsteinCopulaFactory> Factory_BernsteinCopulaFactory;

/* Default constructor */
BernsteinCopulaFactory::BernsteinCopulaFactory()
  : DistributionFactoryImplementation()
  , isParallel_(ResourceMap::GetAsBool("BernsteinCopulaFactory-Parallel"))
{
  setName("BernsteinCopulaFactory");
}

/* Virtual constructor */
BernsteinCopulaFactory * BernsteinCopulaFactory::clone() const
{
  return new BernsteinCopulaFactory(*this);
}

/* Compute the number of bins according to the inverse power rule */
UnsignedInteger BernsteinCopulaFactory::computeBinNumber(const NumericalSample & sample)
{
  return static_cast<UnsignedInteger>(1.0 + pow(sample.getSize(), 2.0 / (4.0 + sample.getDimension())));
}

struct BernsteinCopulaFactoryPolicy
{
  const NumericalSample input_;
  Mixture::DistributionCollection & output_;
  UnsignedInteger binNumber_;
  UnsignedInteger dimension_;

  BernsteinCopulaFactoryPolicy(const NumericalSample & input,
                               const UnsignedInteger binNumber,
                               Mixture::DistributionCollection & output)
    : input_(input)
    , output_(output)
    , binNumber_(binNumber)
    , dimension_(input.getDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const NumericalPoint nu(input_[i] * binNumber_);
      Mixture::DistributionCollection atomsKernel(dimension_);
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        atomsKernel[j] = Beta(std::floor(nu[j]) + 1.0, binNumber_ + 1.0, 0.0, 1.0);
      output_[i] = ComposedDistribution(atomsKernel);
    }
  }

}; /* end struct BernsteinCopulaFactoryPolicy */

/* Build a Bernstein copula based on the given sample. The bin number is computed according to the inverse power rule */
Distribution BernsteinCopulaFactory::build(const NumericalSample & sample)
{
  return build(sample, computeBinNumber(sample));
}

/* Build a Bernstein copula based on the given sample */
Distribution BernsteinCopulaFactory::buildParallel(const NumericalSample & empiricalCopulaSample,
    const UnsignedInteger binNumber)
{
  const UnsignedInteger size = empiricalCopulaSample.getSize();
  Mixture::DistributionCollection atomsMixture(size);
  BernsteinCopulaFactoryPolicy policy(empiricalCopulaSample, binNumber, atomsMixture);
  TBB::ParallelFor( 0, size, policy );
  Mixture result(atomsMixture);
  // Here we know that the mixture is a copula even if none of its atoms is.
  result.isCopula_ = true;
  return result;
}

Distribution BernsteinCopulaFactory::buildSequential(const NumericalSample & empiricalCopulaSample,
    const UnsignedInteger binNumber)
{
  const UnsignedInteger size = empiricalCopulaSample.getSize();
  const UnsignedInteger dimension = empiricalCopulaSample.getDimension();
  LOGINFO("BernsteinCopulaFactory - Create the resulting Bernstein copula");
  Mixture::DistributionCollection atomsMixture(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    {
      const NumericalPoint nu(empiricalCopulaSample[i] * binNumber);
      Mixture::DistributionCollection atomsKernel(dimension);
      for (UnsignedInteger j = 0; j < dimension; ++j)
        atomsKernel[j] = Beta(std::floor(nu[j]) + 1.0, binNumber + 1.0, 0.0, 1.0);
      atomsMixture[i] = ComposedDistribution(atomsKernel);
    }
  Mixture result(atomsMixture);
  // Here we know that the mixture is a copula even if none of its atoms is.
  result.isCopula_ = true;
  return result;
}

Distribution BernsteinCopulaFactory::build(const NumericalSample & sample,
    const UnsignedInteger binNumber)
{
  if (binNumber == 0) throw InvalidDimensionException(HERE) << "Error: the bin number must be positive for the BernsteinCopulaFactory";
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidDimensionException(HERE) << "Error: cannot build a copula using the Bernstein copula factory based on an empty sample";
  const UnsignedInteger dimension = sample.getDimension();
  LOGINFO("BernsteinCopulaFactory - Create the empirical copula sample");
  const NumericalSample empiricalCopulaSample(sample.rank() * NumericalPoint(dimension, (1.0 - SpecFunc::NumericalScalarEpsilon) / size));
  if (isParallel_)
    return buildParallel(empiricalCopulaSample, binNumber);
  return buildSequential(empiricalCopulaSample, binNumber);
}

/** Parallelization flag accessor */
void BernsteinCopulaFactory::setParallel(const Bool flag)
{
  isParallel_ = flag;
}

Bool BernsteinCopulaFactory::isParallel() const
{
  return isParallel_;
}


END_NAMESPACE_OPENTURNS
