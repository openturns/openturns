//                                               -*- C++ -*-
/**
 *  @brief The SmoothedUniform distribution
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SmoothedUniform.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/Normal.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SmoothedUniform)

static const Factory<SmoothedUniform> Factory_SmoothedUniform;

/* Default constructor */
SmoothedUniform::SmoothedUniform()
  : RandomMixture()
  , a_(-1.0)
  , b_(1.0)
  , sigma_(1.0)
{
  setName( "SmoothedUniform" );
  setDimension( 1 );
  DistributionCollection coll(2);
  coll[0] = Uniform(a_, b_);
  coll[1] = Normal(0.0, sigma_);
  setDistributionCollectionAndWeights(coll, Matrix(1, 2, Point(2, 1.0)), false);
  computeRange();
}

/* Parameters constructor */
SmoothedUniform::SmoothedUniform(const Scalar a,
                                 const Scalar b,
                                 const Scalar sigma)
  : RandomMixture()
  , a_(a)
  , b_(b)
  , sigma_(sigma)
{
  setName( "SmoothedUniform" );
  setDimension( 1 );
  if (b <= a) throw InvalidArgumentException(HERE) << "Error the upper bound b of the Uniform component of a SmoothedUniform distribution must be greater than its lower bound a, here a=" << a << " b=" << b;
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "Error the standard deviation of the Normal component of a SmoothedUniform must be positive, here sigma=" << sigma;
  DistributionCollection coll(2);
  coll[0] = Uniform(a_, b_);
  coll[1] = Normal(0.0, sigma_);
  setDistributionCollectionAndWeights(coll, Matrix(1, 2, Point(2, 1.0)), false);
  computeRange();
}

/* Comparison operator */
Bool SmoothedUniform::operator ==(const SmoothedUniform & other) const
{
  if (this == &other) return true;
  return (a_ == other.a_) && (b_ == other.b_) && (sigma_ == other.sigma_);
}

Bool SmoothedUniform::equals(const DistributionImplementation & other) const
{
  const SmoothedUniform* p_other = dynamic_cast<const SmoothedUniform*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String SmoothedUniform::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SmoothedUniform::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " a=" << a_
      << " b=" << b_
      << " sigma=" << sigma_;
  return oss;
}

String SmoothedUniform::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(a = " << a_ << ", b = " << b_ << ", sigma = " << sigma_ << ")";
  return oss;
}

/* Virtual constructor */
SmoothedUniform * SmoothedUniform::clone() const
{
  return new SmoothedUniform(*this);
}

/* Get one realization of the distribution */
Point SmoothedUniform::getRealization() const
{
  return Point(1, a_ + (b_ - a_) * RandomGenerator::Generate() + sigma_ * DistFunc::rNormal());
}

/* Get a sample of the distribution */
Sample SmoothedUniform::getSample(const UnsignedInteger size) const
{
  SampleImplementation result(size, 1);
  const Scalar ba = b_ - a_;
  for (UnsignedInteger i = 0; i < size; ++i)
    result(i, 0) = a_ + ba * RandomGenerator::Generate() + sigma_ * DistFunc::rNormal();
  return result;
}

/* Get the DDF of the distribution */
Point SmoothedUniform::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return Point(1, 0.0);
}


/* Get the PDF of the distribution */
Scalar SmoothedUniform::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  const Scalar ax = (a_ - x) / sigma_;
  const Scalar bx = (b_ - x) / sigma_;
  const Scalar ba = b_ - a_;
  const Scalar PhiAX = DistFunc::pNormal(ax);
  const Scalar PhiBX = DistFunc::pNormal(bx);
  return (PhiBX - PhiAX) / ba;
}


/* Get the CDF of the distribution */
Scalar SmoothedUniform::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  const Scalar ax = (a_ - x) / sigma_;
  const Scalar bx = (b_ - x) / sigma_;
  const Scalar ba = b_ - a_;
  const Scalar phiAX = DistFunc::dNormal(ax);
  const Scalar phiBX = DistFunc::dNormal(bx);
  const Scalar expPart = phiAX - phiBX;
  const Scalar PhiAX = DistFunc::pNormal(ax);
  const Scalar PhiBX = DistFunc::pNormal(bx);
  const Scalar phiPart = PhiAX * ax - PhiBX * bx;
  return 1.0 + sigma_ * (expPart + phiPart) / ba;
}

Scalar SmoothedUniform::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  const Scalar ax = (a_ - x) / sigma_;
  const Scalar bx = (b_ - x) / sigma_;
  const Scalar ba = b_ - a_;
  const Scalar phiAX = DistFunc::dNormal(ax);
  const Scalar phiBX = DistFunc::dNormal(bx);
  const Scalar expPart = phiAX - phiBX;
  const Scalar PhiAX = DistFunc::pNormal(ax);
  const Scalar PhiBX = DistFunc::pNormal(bx);
  const Scalar phiPart = PhiAX * ax - PhiBX * bx;
  return -sigma_ * (expPart + phiPart) / ba;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex SmoothedUniform::computeCharacteristicFunction(const Scalar x) const
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
  return result * std::exp(-0.5 * sigma_ * sigma_ * x * x);
}

/* Get the PDFGradient of the distribution */
Point SmoothedUniform::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  const Scalar ax = (a_ - x) / sigma_;
  const Scalar bx = (b_ - x) / sigma_;
  const Scalar ba = b_ - a_;
  const Scalar phiAX = DistFunc::dNormal(ax);
  const Scalar phiBX = DistFunc::dNormal(bx);
  const Scalar PhiAX = DistFunc::pNormal(ax);
  const Scalar PhiBX = DistFunc::pNormal(bx);
  Point pdfGradient(3, 0.0);
  pdfGradient[0] =      -(phiAX / sigma_ - (PhiBX - PhiAX) / ba) / ba;
  pdfGradient[1] =       (phiBX / sigma_ - (PhiBX - PhiAX) / ba) / ba;
  pdfGradient[2] = -0.5 * (phiBX * bx - phiAX * ax) / sigma_;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point SmoothedUniform::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  const Scalar ax = (a_ - x) / sigma_;
  const Scalar bx = (b_ - x) / sigma_;
  const Scalar ba = b_ - a_;
  const Scalar phiAX = DistFunc::dNormal(ax);
  const Scalar phiBX = DistFunc::dNormal(bx);
  const Scalar PhiAX = DistFunc::pNormal(ax);
  const Scalar PhiBX = DistFunc::pNormal(bx);
  Point cdfGradient(3, 0.0);
  cdfGradient[0] =  sigma_ * (bx * (PhiAX - PhiBX) + phiAX - phiBX) / (ba * ba);
  cdfGradient[1] = -sigma_ * (ax * (PhiAX - PhiBX) + phiAX - phiBX) / (ba * ba);
  cdfGradient[2] =  (phiAX - phiBX) / ba;
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar SmoothedUniform::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  const Scalar q = (tail ? 1.0 - prob : prob);
  const Scalar xMin = range_.getLowerBound()[0];
  if (q <= 0.0) return xMin;
  const Scalar xMax = range_.getUpperBound()[0];
  if (q >= 1.0) return xMax;
  const Scalar ba = b_ - a_;
  Scalar x = (sigma_ >= 10.0 * ba ? 0.5 * (a_ + b_) + sigma_ * DistFunc::qNormal(q) : a_ + q * ba);
  const Scalar delta = xMax - xMin;
  Scalar dx = delta;
  const Scalar epsilon = cdfEpsilon_ * delta;
  for (UnsignedInteger i = 0; i < 16 && std::abs(dx) > epsilon; ++i)
  {
    const Scalar ax = (a_ - x) / sigma_;
    const Scalar bx = (b_ - x) / sigma_;
    const Scalar phiAX = DistFunc::dNormal(ax);
    const Scalar phiBX = DistFunc::dNormal(bx);
    const Scalar PhiBX = DistFunc::pNormal(bx);
    const Scalar PhiAX = DistFunc::pNormal(ax);
    const Scalar pdf = (PhiBX - PhiAX) / ba;
    const Scalar cdf = sigma_ * (phiAX - phiBX + PhiAX * ax - PhiBX * bx) / ba + 1.0;
    dx = (q - cdf) / pdf;
    x += dx;
  }
  // If Newton's iteration failed to converge (only due to cumulated rounding effects)
  if (std::abs(dx) > epsilon) return DistributionImplementation::computeScalarQuantile(prob, tail);
  return x;
}

/* Compute the mean of the distribution */
void SmoothedUniform::computeMean() const
{
  mean_ = Point(1, 0.5 * (a_ + b_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point SmoothedUniform::getStandardDeviation() const
{
  return Point(1, std::sqrt((b_ - a_) * (b_ - a_) / 12.0 + sigma_ * sigma_));
}

/* Get the skewness of the distribution */
Point SmoothedUniform::getSkewness() const
{
  return Point(1, 0.0);
}

/* Get the kurtosis of the distribution */
Point SmoothedUniform::getKurtosis() const
{
  const Scalar den = 12.0 * sigma_ * sigma_ / (b_ - a_) / (b_ - a_) + 1.0;
  return Point(1, 3.0 - 1.2 / den / den);
}

/* Compute the covariance of the distribution */
void SmoothedUniform::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = (b_ - a_) * (b_ - a_) / 12.0 + sigma_ * sigma_;
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
Point SmoothedUniform::getStandardMoment(const UnsignedInteger n) const
{
  if (n % 2 == 1) return Point(1, 0.0);
  return Point(1, 1.0 / (n + 1));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution SmoothedUniform::getStandardRepresentative() const
{
  return new SmoothedUniform(-1.0, 1.0, 1.0);
}

/* Check if the distribution is elliptical */
Bool SmoothedUniform::isElliptical() const
{
  return true;
}

/* Parameters value accessor */
Point SmoothedUniform::getParameter() const
{
  Point point(3);
  point[0] = a_;
  point[1] = b_;
  point[2] = sigma_;
  return point;
}

void SmoothedUniform::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = SmoothedUniform(parameter[0], parameter[1], parameter[2]);
  setWeight(w);
}

/* Parameters description accessor */
Description SmoothedUniform::getParameterDescription() const
{
  Description description(3);
  description[0] = "a";
  description[1] = "b";
  description[2] = "sigma";
  return description;
}

SmoothedUniform::PointWithDescriptionCollection SmoothedUniform::getParametersCollection() const
{
  PointWithDescription parameter(getParameter());
  parameter.setDescription(getParameterDescription());
  parameter.setName(getName());
  return PointWithDescriptionCollection(1, parameter);
}

void SmoothedUniform::setParametersCollection(const PointCollection & parametersCollection)
{
  if (parametersCollection.getSize() != 1) throw InvalidArgumentException(HERE) << "Parameters must be of size 1";
  setParameter(parametersCollection[0]);
}

/* A accessor */
void SmoothedUniform::setA(const Scalar a)
{
  if (b_ <= a) throw InvalidArgumentException(HERE) << "Error the lower bound a of a SmoothedUniform distribution must be less than its upper bound b, here a=" << a << " b=" << b_;
  if (a != a_)
  {
    a_ = a;
    DistributionCollection coll(getDistributionCollection());
    coll[0] = Uniform(a_, b_);
    setDistributionCollectionAndWeights(coll, Matrix(1, 2, Point(2, 1.0)), false);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar SmoothedUniform::getA() const
{
  return a_;
}


/* B accessor */
void SmoothedUniform::setB(const Scalar b)
{
  if (b <= a_) throw InvalidArgumentException(HERE) << "Error the upper bound b of the Uniform component of a SmoothedUniform distribution must be greater than its lower bound a, here a=" << a_ << " b=" << b;
  if (b != b_)
  {
    b_ = b;
    DistributionCollection coll(getDistributionCollection());
    coll[0] = Uniform(a_, b_);
    setDistributionCollectionAndWeights(coll, Matrix(1, 2, Point(2, 1.0)), false);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar SmoothedUniform::getB() const
{
  return b_;
}

/* Sigma accessor */
void SmoothedUniform::setSigma(const Scalar sigma)
{
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "Error the standard deviation of the Normal component of a SmoothedUniform must be positive, here sigma=" << sigma;
  if (sigma != sigma_)
  {
    sigma_ = sigma;
    DistributionCollection coll(getDistributionCollection());
    coll[1] = Normal(0.0, sigma_);
    setDistributionCollectionAndWeights(coll, Matrix(1, 2, Point(2, 1.0)), false);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar SmoothedUniform::getSigma() const
{
  return sigma_;
}

/* Method save() stores the object through the StorageManager */
void SmoothedUniform::save(Advocate & adv) const
{
  RandomMixture::save(adv);
  adv.saveAttribute( "a_", a_ );
  adv.saveAttribute( "b_", b_ );
  adv.saveAttribute( "sigma_", sigma_ );
}

/* Method load() reloads the object from the StorageManager */
void SmoothedUniform::load(Advocate & adv)
{
  RandomMixture::load(adv);
  adv.loadAttribute( "a_", a_ );
  adv.loadAttribute( "b_", b_ );
  adv.loadAttribute( "sigma_", sigma_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
