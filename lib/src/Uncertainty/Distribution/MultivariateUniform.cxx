//                                               -*- C++ -*-
/**
 *  @brief The MultivariateUniform distribution
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
#include "openturns/MultivariateUniform.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MultivariateUniform)

static const Factory<MultivariateUniform> Factory_MultivariateUniform;

/* Default constructor */
MultivariateUniform::MultivariateUniform()
  : DistributionImplementation()
  , a_(1, -1.0)
  , b_(1, 1.0)
{
  setName("MultivariateUniform");
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
MultivariateUniform::MultivariateUniform(const Point & a,
    const Point & b)
  : DistributionImplementation()
  , a_(a)
  , b_(b)
{
  setName("MultivariateUniform");
  const UnsignedInteger dimension = a.getSize();
  if (b.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the vectors a and b must have the same dimension, here a=" << a << " and b=" << b;
  if (dimension < 1) throw InvalidArgumentException(HERE) << "Error: the dimension must be at least 1, here dimension=" << dimension;
  for (UnsignedInteger i = 0; i < dimension; ++i)
    if (!(a[i] < b[i])) throw InvalidArgumentException(HERE) << "Error: we must have a[i] < b[i] for all i, here a[" << i << "]=" << a[i] << " and b[" << i << "]=" << b[i];
  setDimension(dimension);
  computeRange();
}

/* Comparison operator */
Bool MultivariateUniform::operator ==(const MultivariateUniform & other) const
{
  if (this == &other) return true;
  return (a_ == other.a_) && (b_ == other.b_);
}

Bool MultivariateUniform::equals(const DistributionImplementation & other) const
{
  const MultivariateUniform* p_other = dynamic_cast<const MultivariateUniform*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String MultivariateUniform::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MultivariateUniform::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " a=" << a_
      << " b=" << b_;
  return oss;
}

String MultivariateUniform::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(a = " << a_.__str__() << ", b = " << b_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
MultivariateUniform * MultivariateUniform::clone() const
{
  return new MultivariateUniform(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void MultivariateUniform::computeRange()
{
  setRange(Interval(a_, b_));
}


/* Get one realization of the distribution */
Point MultivariateUniform::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  Point realization(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    realization[i] = a_[i] + (b_[i] - a_[i]) * RandomGenerator::Generate();
  return realization;
}

/* Get the DDF of the distribution */
Point MultivariateUniform::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  // DDF is zero everywhere the PDF is defined (except at boundaries)
  return Point(dimension, 0.0);
}


/* Get the PDF of the distribution */
Scalar MultivariateUniform::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar x = point[i];
    if ((x < a_[i]) || (x > b_[i])) return 0.0;
  }
  Scalar pdf = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
    pdf /= (b_[i] - a_[i]);
  return pdf;
}

Scalar MultivariateUniform::computeLogPDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar x = point[i];
    if ((x < a_[i]) || (x > b_[i])) return SpecFunc::LowestScalar;
  }
  Scalar logPDF = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
    logPDF -= std::log(b_[i] - a_[i]);
  return logPDF;
}


/* Get the CDF of the distribution */
Scalar MultivariateUniform::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  Scalar cdf = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar x = point[i];
    if (x <= a_[i]) return 0.0;
    if (x >= b_[i]) continue;
    cdf *= (x - a_[i]) / (b_[i] - a_[i]);
  }
  return cdf;
}


/* Get the survival function of the distribution */
Scalar MultivariateUniform::computeSurvivalFunction(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  Scalar survival = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar x = point[i];
    if (x >= b_[i]) return 0.0;
    if (x <= a_[i]) continue;
    survival *= (b_[i] - x) / (b_[i] - a_[i]);
  }
  return survival;
}


/* Get the probability content of an interval */
Scalar MultivariateUniform::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  Scalar probability = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar aI = a_[i];
    const Scalar bI = b_[i];
    const Scalar lower = std::max(aI, interval.getLowerBound()[i]);
    const Scalar upper = std::min(bI, interval.getUpperBound()[i]);
    if (upper <= lower) return 0.0;
    probability *= (upper - lower) / (bI - aI);
  }
  return probability;
}


/* Get the quantile of the distribution */
Point MultivariateUniform::computeQuantile(const Scalar prob,
    const Bool tail,
    Scalar & marginalProb) const
{
  if (!((prob >= 0.0) && (prob <= 1.0)))
    throw InvalidArgumentException(HERE) << "computeQuantile expected prob to belong to [0,1], but is " << prob;
  const UnsignedInteger dimension = getDimension();
  Point quantile(dimension);
  marginalProb = prob;
  if (tail)
  {
    for (UnsignedInteger i = 0; i < dimension; ++i)
      quantile[i] = b_[i] - prob * (b_[i] - a_[i]);
  }
  else
  {
    for (UnsignedInteger i = 0; i < dimension; ++i)
      quantile[i] = a_[i] + prob * (b_[i] - a_[i]);
  }
  return quantile;
}


/* Compute the entropy of the distribution */
Scalar MultivariateUniform::computeEntropy() const
{
  const UnsignedInteger dimension = getDimension();
  Scalar entropy = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
    entropy += std::log(b_[i] - a_[i]);
  return entropy;
}


/* Get the standard deviation of the distribution */
Point MultivariateUniform::getStandardDeviation() const
{
  const UnsignedInteger dimension = getDimension();
  Point sigma(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    sigma[i] = (b_[i] - a_[i]) / std::sqrt(12.0);
  return sigma;
}

/* Get the skewness of the distribution */
Point MultivariateUniform::getSkewness() const
{
  return Point(getDimension(), 0.0);
}

/* Get the kurtosis of the distribution */
Point MultivariateUniform::getKurtosis() const
{
  return Point(getDimension(), 1.8);
}

/* Compute the mean of the distribution */
void MultivariateUniform::computeMean() const
{
  const UnsignedInteger dimension = getDimension();
  mean_ = Point(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    mean_[i] = 0.5 * (a_[i] + b_[i]);
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void MultivariateUniform::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar eta = b_[i] - a_[i];
    covariance_(i, i) = eta * eta / 12.0;
  }
  isAlreadyComputedCovariance_ = true;
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution MultivariateUniform::getStandardRepresentative() const
{
  MultivariateUniform standard(Point(getDimension(), -1.0), Point(getDimension(), 1.0));
  standard.setDescription(getDescription());
  return standard;
}

/* Parameters value accessor */
Point MultivariateUniform::getParameter() const
{
  const UnsignedInteger dimension = getDimension();
  Point parameter(2 * dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    parameter[i] = a_[i];
    parameter[dimension + i] = b_[i];
  }
  return parameter;
}

void MultivariateUniform::setParameter(const Point & parameter)
{
  const UnsignedInteger dimension = getDimension();
  if (parameter.getSize() != 2 * dimension) throw InvalidArgumentException(HERE) << "Error: expected a point of size=dimension*2, got " << parameter.getSize();
  Point a(dimension);
  Point b(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    a[i] = parameter[i];
    b[i] = parameter[dimension + i];
  }
  const Scalar w = getWeight();
  *this = MultivariateUniform(a, b);
  setWeight(w);
}

/* Parameters description accessor */
Description MultivariateUniform::getParameterDescription() const
{
  const UnsignedInteger dimension = getDimension();
  Description description(2 * dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    description[i] = (OSS() << "a_" << i);
    description[dimension + i] = (OSS() << "b_" << i);
  }
  return description;
}

/* Check if the distribution is elliptical */
Bool MultivariateUniform::isElliptical() const
{
  return (getDimension() == 1);
}

/* Check if the distribution is a copula */
Bool MultivariateUniform::isCopula() const
{
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
    if ((a_[i] != 0.0) || (b_[i] != 1.0)) return false;
  return true;
}

/* Tell if the distribution has independent copula */
Bool MultivariateUniform::hasIndependentCopula() const
{
  return true;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution MultivariateUniform::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  const UnsignedInteger outputDimension = indices.getSize();
  if (outputDimension == 1)
  {
    const UnsignedInteger index = indices[0];
    Uniform marginal(a_[index], b_[index]);
    marginal.setDescription({getDescription()[index]});
    return marginal;
  }
  Point a(outputDimension);
  Point b(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger index = indices[i];
    a[i] = a_[index];
    b[i] = b_[index];
  }
  MultivariateUniform* marginal(new MultivariateUniform(a, b));
  marginal->setDescription(getDescription().select(indices));
  return marginal;
}


/* A accessor */
void MultivariateUniform::setA(const Point & a)
{
  if (a.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a vector of dimension " << getDimension() << ", got " << a.getDimension();
  for (UnsignedInteger i = 0; i < getDimension(); ++i)
    if (!(b_[i] > a[i])) throw InvalidArgumentException(HERE) << "Error: we must have a[i] < b[i] for all i";
  if (a != a_)
  {
    a_ = a;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Point MultivariateUniform::getA() const
{
  return a_;
}


/* B accessor */
void MultivariateUniform::setB(const Point & b)
{
  if (b.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a vector of dimension " << getDimension() << ", got " << b.getDimension();
  for (UnsignedInteger i = 0; i < getDimension(); ++i)
    if (!(b[i] > a_[i])) throw InvalidArgumentException(HERE) << "Error: we must have b[i] > a[i] for all i";
  if (b != b_)
  {
    b_ = b;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Point MultivariateUniform::getB() const
{
  return b_;
}

/* Method save() stores the object through the StorageManager */
void MultivariateUniform::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "a_", a_ );
  adv.saveAttribute( "b_", b_ );
}

/* Method load() reloads the object from the StorageManager */
void MultivariateUniform::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "a_", a_ );
  adv.loadAttribute( "b_", b_ );
  computeRange();
}

/* Get the quantile of the distribution (1D) */
Scalar MultivariateUniform::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (getDimension() != 1) throw NotDefinedException(HERE) << "Error: the scalar quantile is not defined for dimension=" << getDimension() << ", use computeQuantile instead.";
  if (!((prob >= 0.0) && (prob <= 1.0)))
    throw InvalidArgumentException(HERE) << "computeScalarQuantile expected prob to belong to [0,1], but is " << prob;
  if (tail) return b_[0] - prob * (b_[0] - a_[0]);
  return a_[0] + prob * (b_[0] - a_[0]);
}

END_NAMESPACE_OPENTURNS
