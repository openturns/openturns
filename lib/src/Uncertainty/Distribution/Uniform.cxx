//                                               -*- C++ -*-
/**
 *  @brief The Uniform distribution
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
#include "openturns/Uniform.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/Distribution.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Uniform)

static const Factory<Uniform> Factory_Uniform;

/* Default constructor */
Uniform::Uniform()
  : ContinuousDistribution()
  , a_(-1.0)
  , b_(1.0)
{
  setName( "Uniform" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Uniform::Uniform(const Scalar a,
                 const Scalar b)
  : ContinuousDistribution()
  , a_(a)
  , b_(b)
{
  setName( "Uniform" );
  if (b <= a) throw InvalidArgumentException(HERE) << "Error the lower bound a of a Uniform distribution must be less than its upper bound b, here a=" << a << " b=" << b;
  setDimension( 1 );
  computeRange();
}

/* Comparison operator */
Bool Uniform::operator ==(const Uniform & other) const
{
  if (this == &other) return true;
  return (a_ == other.a_) && (b_ == other.b_);
}

Bool Uniform::equals(const DistributionImplementation & other) const
{
  const Uniform* p_other = dynamic_cast<const Uniform*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Uniform::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Uniform::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " a=" << a_
      << " b=" << b_;
  return oss;
}

String Uniform::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(a = " << a_ << ", b = " << b_ << ")";
  return oss;
}

/* Virtual constructor */
Uniform * Uniform::clone() const
{
  return new Uniform(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Uniform::computeRange()
{
  setRange(Interval(a_, b_));
}


/* Get one realization of the distribution */
Point Uniform::getRealization() const
{
  return Point(1, a_ + (b_ - a_) * RandomGenerator::Generate());
}

/* Get a sample of the distribution */
Sample Uniform::getSample(const UnsignedInteger size) const
{
  SampleImplementation result(size, 1);
  result.setData(RandomGenerator::Generate(size));
  result *= Point(1, b_ - a_);
  result += Point(1, a_);
  result.setName(getName());
  result.setDescription(getDescription());
  return result;
}

/* Get the DDF of the distribution */
Point Uniform::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return Point(1, 0.0);
}


/* Get the PDF of the distribution */
Scalar Uniform::computePDF(const Scalar x) const
{
  if ((x <= a_) || (x > b_)) return 0.0;
  return 1.0 / (b_ - a_);
}

Scalar Uniform::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computePDF(point[0]);
}


/* Get the CDF of the distribution */
Scalar Uniform::computeCDF(const Scalar x) const
{
  if (x <= a_) return 0.0;
  if (x >= b_)  return 1.0;
  return (x - a_) / (b_ - a_);
}

Scalar Uniform::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computeCDF(point[0]);
}

Scalar Uniform::computeComplementaryCDF(const Scalar x) const
{
  if (x <= a_) return 1.0;
  if (x > b_)  return 0.0;
  return (b_ - x) / (b_ - a_);
}

Scalar Uniform::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computeComplementaryCDF(point[0]);
}

/* Get the product minimum volume interval containing a given probability of the distribution */
Interval Uniform::computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProb) const
{
  return computeBilateralConfidenceIntervalWithMarginalProbability(prob, marginalProb);
}

/* Get the product bilateral confidence interval containing a given probability of the distribution */
Interval Uniform::computeBilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProb) const
{
  marginalProb = prob;
  const Scalar m = 0.5 * (a_ + b_);
  const Scalar d = 0.5 * (b_ - a_);
  return Interval(m - prob * d, m + prob * d);
}

/* Get the minimum volume level set containing a given probability of the distribution */
LevelSet Uniform::computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & threshold) const
{
  const SymbolicFunction function("x", String(OSS() << std::setprecision(17) << 2.0 / (b_ - a_) << " * abs(x - (" << 0.5 * (a_ + b_) << "))"));
  threshold = prob;
  return LevelSet(function, LessOrEqual(), threshold);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Uniform::computeCharacteristicFunction(const Scalar x) const
{
  Complex result;
  const Scalar ax = a_ * x;
  const Scalar bx = b_ * x;
  if (std::abs(ax) + std::abs(bx) <= 1.0e-5) result = Complex(1.0 - (ax * ax + ax * bx + bx * bx) / 6.0, 0.5 * (ax + bx));
  else
  {
    const Scalar idenom = 1.0 / (bx - ax);
    result = Complex(idenom * (std::sin(bx) - std::sin(ax)), idenom * (std::cos(ax) - std::cos(bx)));
  }
  return result;
}

/* Get the PDFGradient of the distribution */
Point Uniform::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  Point pdfGradient(2, 0.0);
  if ((x < a_) || (x > b_)) return pdfGradient;
  const Scalar iAB = 1.0 / (b_ - a_);
  const Scalar iAB2 = iAB * iAB;
  pdfGradient[0] = iAB2;
  pdfGradient[1] = -iAB2;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point Uniform::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  Point cdfGradient(2, 0.0);
  if ((x < a_) || (x > b_)) return cdfGradient;
  const Scalar iAB = 1.0 / (b_ - a_);
  const Scalar iAB2 = iAB * iAB;
  cdfGradient[0] = (x - b_) * iAB2;
  cdfGradient[1] = (a_ - x) * iAB2;
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar Uniform::computeScalarQuantile(const Scalar prob,
                                      const Bool tail) const
{
  if (tail) return b_ - prob * (b_ - a_);
  return a_ + prob * (b_ - a_);
}

/* Compute the entropy of the distribution */
Scalar Uniform::computeEntropy() const
{
  return std::log(b_ - a_);
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar Uniform::getRoughness() const
{
  return 1.0 / (b_ - a_);
}

/* Compute the mean of the distribution */
void Uniform::computeMean() const
{
  mean_ = Point(1, 0.5 * (a_ + b_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Uniform::getStandardDeviation() const
{
  return Point(1, (b_ - a_) / std::sqrt(12.0));
}

/* Get the skewness of the distribution */
Point Uniform::getSkewness() const
{
  return Point(1, 0.0);
}

/* Get the kurtosis of the distribution */
Point Uniform::getKurtosis() const
{
  // 1.8 = 9/5
  return Point(1, 1.8);
}

/* Compute the covariance of the distribution */
void Uniform::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const Scalar eta = b_ - a_;
  covariance_(0, 0) = eta * eta / 12.0;
  isAlreadyComputedCovariance_ = true;
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Uniform::getStandardRepresentative() const
{
  return new Uniform(-1.0, 1.0);
}

/* Parameters value accessor */
Point Uniform::getParameter() const
{
  return {a_, b_};
}

void Uniform::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Uniform(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description Uniform::getParameterDescription() const
{
  return {"a", "b"};
}

/* Check if the distribution is elliptical */
Bool Uniform::isElliptical() const
{
  return true;
}

/* Check if the distribution is a copula */
Bool Uniform::isCopula() const
{
  return (a_ == 0.0) && (b_ == 1.0);
}


/* A accessor */
void Uniform::setA(const Scalar a)
{
  if (b_ <= a) throw InvalidArgumentException(HERE) << "Error the lower bound a of a Uniform distribution must be less than its upper bound b, here a=" << a << " b=" << b_;
  if (a != a_)
  {
    a_ = a;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar Uniform::getA() const
{
  return a_;
}


/* B accessor */
void Uniform::setB(const Scalar b)
{
  if (b <= a_) throw InvalidArgumentException(HERE) << "Error the upper bound b of a Uniform distribution must be greater than its lower bound a, here b=" << b << " a=" << a_;
  if (b != b_)
  {
    b_ = b;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar Uniform::getB() const
{
  return b_;
}

/* Method save() stores the object through the StorageManager */
void Uniform::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "a_", a_ );
  adv.saveAttribute( "b_", b_ );
}

/* Method load() reloads the object from the StorageManager */
void Uniform::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "a_", a_ );
  adv.loadAttribute( "b_", b_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
