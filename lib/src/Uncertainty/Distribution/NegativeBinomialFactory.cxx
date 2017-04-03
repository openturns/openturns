//                                               -*- C++ -*-
/**
 *  @brief Factory for NegativeBinomial distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/NegativeBinomialFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/Brent.hxx"
#include "openturns/DiscreteDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NegativeBinomialFactory);

static const Factory<NegativeBinomialFactory> Factory_NegativeBinomialFactory;

/* Default constructor */
NegativeBinomialFactory::NegativeBinomialFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
NegativeBinomialFactory * NegativeBinomialFactory::clone() const
{
  return new NegativeBinomialFactory(*this);
}


/* Here is the interface that all derived class must implement */

struct NegativeBinomialFactoryParameterConstraint
{
  /** Constructor from a sample and a derivative factor estimate */
  NegativeBinomialFactoryParameterConstraint(const Sample & sample,
      const NumericalScalar mean):
    sample_(sample),
    mean_(mean)
  {
    // Nothing to do
  };

  NumericalPoint computeConstraint(const NumericalPoint & parameter) const
  {
    const NumericalScalar r = parameter[0];
    if (!(r > 0.0)) throw InvalidArgumentException(HERE) << "Error: the r parameter must be positive.";
    const UnsignedInteger size = sample_.getSize();
    /* \sum_{i=1}^N \psi(x_i + r) */
    NumericalScalar sumPsi = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i) sumPsi += SpecFunc::Psi(sample_[i][0] + r);
    const NumericalScalar value = sumPsi + size * (std::log(r / (r + mean_)) - SpecFunc::Psi(r));
    return NumericalPoint(1, value);
  }

  // The data
  const Sample & sample_;
  // Mean value
  NumericalScalar mean_;
};

NegativeBinomialFactory::Implementation NegativeBinomialFactory::build(const Sample & sample) const
{
  return buildAsNegativeBinomial(sample).clone();
}

NegativeBinomialFactory::Implementation NegativeBinomialFactory::build(const NumericalPoint & parameters) const
{
  return buildAsNegativeBinomial(parameters).clone();
}

NegativeBinomialFactory::Implementation NegativeBinomialFactory::build() const
{
  return buildAsNegativeBinomial().clone();
}

NegativeBinomial NegativeBinomialFactory::buildAsNegativeBinomial(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a NegativeBinomial distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a NegativeBinomial distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  NumericalScalar mean = 0.0;
  NumericalScalar var = 0.0;
  const NumericalScalar supportEpsilon = ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon");
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar x = sample[i][0];
    const int iX(static_cast<int>(round(x)));
    // The sample must be made of nonnegative integral values
    if (std::abs(x - iX) > supportEpsilon || (iX < 0)) throw InvalidArgumentException(HERE) << "Error: can build a NegativeBinomial distribution only from a sample made of nonnegative integers, here x=" << x;
    var = i * var / (i + 1.0) + (1.0 - 1.0 / (i + 1.0)) * (mean - x) * (mean - x) / (i + 1.0);
    mean = (x + i * mean) / (i + 1.0);
  }
  // Build the constraint
  NegativeBinomialFactoryParameterConstraint constraint(sample, mean);
  const Function f(bindMethod<NegativeBinomialFactoryParameterConstraint, NumericalPoint, NumericalPoint>(constraint, &NegativeBinomialFactoryParameterConstraint::computeConstraint, 1, 1));
  // Find a bracketing interval using the moment estimate
  NumericalScalar a = 1.0;
  NumericalScalar b = 2.0;
  // Try to improve the starting point of the bracketing using the moment estimate of r
  if (var > mean)
  {
    const NumericalScalar rMoment = mean * mean / (var - mean);
    a = 0.5 * rMoment;
    b = 2.0 * rMoment;
  }
  NumericalScalar fA = f(NumericalPoint(1, a))[0];
  NumericalScalar fB = f(NumericalPoint(1, b))[0];
  // While f has the same sign at the two bounds, update the interval
  while ((fA * fB > 0.0))
  {
    a = 0.5 * a;
    fA = f(NumericalPoint(1, a))[0];
    if (fA * fB <= 0.0) break;
    b = 2.0 * b;
    fB = f(NumericalPoint(1, b))[0];
  }
  // Solve the constraint equation
  Brent solver(ResourceMap::GetAsNumericalScalar("NegativeBinomialFactory-AbsolutePrecision"), ResourceMap::GetAsNumericalScalar("NegativeBinomialFactory-RelativePrecision"), ResourceMap::GetAsNumericalScalar("NegativeBinomialFactory-ResidualPrecision"), ResourceMap::GetAsUnsignedInteger("NegativeBinomialFactory-MaximumIteration"));
  // R estimate
  const NumericalScalar r = solver.solve(f, 0.0, a, b, fA, fB);
  // Corresponding p estimate
  const NumericalScalar p = 1.0 / (r / mean + 1.0);
  NegativeBinomial result(r, p);
  result.setDescription(sample.getDescription());
  return result;
}

NegativeBinomial NegativeBinomialFactory::buildAsNegativeBinomial(const NumericalPoint & parameters) const
{
  try
  {
    NegativeBinomial distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a NegativeBinomial distribution from the given parameters";
  }
}

NegativeBinomial NegativeBinomialFactory::buildAsNegativeBinomial() const
{
  return NegativeBinomial();
}

END_NAMESPACE_OPENTURNS
