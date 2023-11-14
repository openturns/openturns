//                                               -*- C++ -*-
/**
 *  @brief Factory for NegativeBinomial distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/NegativeBinomialFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/Brent.hxx"
#include "openturns/DiscreteDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NegativeBinomialFactory)

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
      const Scalar mean):
    sample_(sample),
    mean_(mean)
  {
    // Nothing to do
  }

  Point computeConstraint(const Point & parameter) const
  {
    const Scalar r = parameter[0];
    if (!(r > 0.0)) throw InvalidArgumentException(HERE) << "Error: the r parameter must be positive.";
    const UnsignedInteger size = sample_.getSize();
    /* \sum_{i=1}^N \psi(x_i + r) */
    Scalar sumPsi = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i) sumPsi += SpecFunc::Psi(sample_(i, 0) + r);
    const Scalar value = sumPsi + size * (std::log(r / (r + mean_)) - SpecFunc::Psi(r));
    return Point(1, value);
  }

  // The data
  const Sample & sample_;
  // Mean value
  Scalar mean_;
};

Distribution NegativeBinomialFactory::build(const Sample & sample) const
{
  return buildAsNegativeBinomial(sample).clone();
}

Distribution NegativeBinomialFactory::build(const Point & parameters) const
{
  return buildAsNegativeBinomial(parameters).clone();
}

Distribution NegativeBinomialFactory::build() const
{
  return buildAsNegativeBinomial().clone();
}

NegativeBinomial NegativeBinomialFactory::buildAsNegativeBinomial(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a NegativeBinomial distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a NegativeBinomial distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  Scalar mean = 0.0;
  Scalar var = 0.0;
  const Scalar supportEpsilon = ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon");
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar x = sample(i, 0);
    if (!SpecFunc::IsNormal(x)) throw InvalidArgumentException(HERE) << "Error: cannot build a NegativeBinomial distribution if data contains NaN or Inf";
    const int iX(static_cast<int>(round(x)));
    // The sample must be made of nonnegative integral values
    if (std::abs(x - iX) > supportEpsilon || (iX < 0)) throw InvalidArgumentException(HERE) << "Error: can build a NegativeBinomial distribution only from a sample made of nonnegative integers, here x=" << x;
    var = i * var / (i + 1.0) + (1.0 - 1.0 / (i + 1.0)) * (mean - x) * (mean - x) / (i + 1.0);
    mean = (x + i * mean) / (i + 1.0);
  }
  // Build the constraint
  NegativeBinomialFactoryParameterConstraint constraint(sample, mean);
  const Function f(bindMethod<NegativeBinomialFactoryParameterConstraint, Point, Point>(constraint, &NegativeBinomialFactoryParameterConstraint::computeConstraint, 1, 1));
  // Find a bracketing interval using the moment estimate
  Scalar a = 1.0;
  Scalar b = 2.0;
  // Try to improve the starting point of the bracketing using the moment estimate of r
  if (var > mean)
  {
    const Scalar rMoment = mean * mean / (var - mean);
    a = 0.5 * rMoment;
    b = 2.0 * rMoment;
  }
  Scalar fA = f(Point(1, a))[0];
  Scalar fB = f(Point(1, b))[0];
  // While f has the same sign at the two bounds, update the interval
  while ((fA * fB > 0.0))
  {
    a = 0.5 * a;
    fA = f(Point(1, a))[0];
    if (fA * fB <= 0.0) break;
    b = 2.0 * b;
    fB = f(Point(1, b))[0];
  }
  // Solve the constraint equation
  Brent solver(ResourceMap::GetAsScalar("NegativeBinomialFactory-AbsolutePrecision"), ResourceMap::GetAsScalar("NegativeBinomialFactory-RelativePrecision"), ResourceMap::GetAsScalar("NegativeBinomialFactory-ResidualPrecision"), ResourceMap::GetAsUnsignedInteger("NegativeBinomialFactory-MaximumIteration"));
  // R estimate
  const Scalar r = solver.solve(f, 0.0, a, b, fA, fB);
  // Corresponding p estimate
  const Scalar p = 1.0 / (r / mean + 1.0);
  NegativeBinomial result(r, p);
  result.setDescription(sample.getDescription());
  return result;
}

NegativeBinomial NegativeBinomialFactory::buildAsNegativeBinomial(const Point & parameters) const
{
  try
  {
    NegativeBinomial distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a NegativeBinomial distribution from the given parameters";
  }
}

NegativeBinomial NegativeBinomialFactory::buildAsNegativeBinomial() const
{
  return NegativeBinomial();
}

END_NAMESPACE_OPENTURNS
