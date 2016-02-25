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

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BernsteinCopulaFactory
 *
 * The class describes the probabilistic concept of BernsteinCopulaFactory.
 */

CLASSNAMEINIT(BernsteinCopulaFactory);

/* Default constructor */
BernsteinCopulaFactory::BernsteinCopulaFactory()
  : DistributionFactoryImplementation()
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
Distribution BernsteinCopulaFactory::build(const NumericalSample & sample,
					   const UnsignedInteger binNumber)
{
  if (binNumber == 0) throw InvalidDimensionException(HERE) << "Error: the bin number must be positive for the BernsteinCopulaFactory";
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidDimensionException(HERE) << "Error: cannot build a copula using the Bernstein copula factory based on an empty sample";
  const UnsignedInteger dimension(sample.getDimension());
  LOGINFO("BernsteinCopulaFactory - Create the empirical copula sample");
  const NumericalSample empiricalCopulaSample(sample.rank() * NumericalPoint(dimension, (1.0 - SpecFunc::NumericalScalarEpsilon) / size));
  Mixture::DistributionCollection atomsMixture(size);
  BernsteinCopulaFactoryPolicy policy(empiricalCopulaSample, binNumber, atomsMixture);
  LOGINFO("BernsteinCopulaFactory - Create the resulting Bernstein copula");
  TBB::ParallelFor( 0, size, policy );
  return Mixture(atomsMixture);
}

END_NAMESPACE_OPENTURNS
