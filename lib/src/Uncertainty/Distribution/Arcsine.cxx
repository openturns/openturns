//                                               -*- C++ -*-
/**
 *  @brief The Arcsine distribution
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Arcsine.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Arcsine)

static const Factory<Arcsine> Factory_Arcsine;


/* Default constructor */
Arcsine::Arcsine()
  : ContinuousDistribution()
  , a_(-1.0)
  , b_(1.0)
{
  setName("Arcsine");
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
Arcsine::Arcsine(const Scalar a,
                 const Scalar b)
  : ContinuousDistribution()
  , a_(a)
  , b_(b)
{
  if (!(a < b))
    throw InvalidArgumentException(HERE) << "in Arcsine : a must be smaller than b";
  setName("Arcsine");
  setDimension(1);
  computeRange();
}

/* Comparison operator */
Bool Arcsine::operator ==(const Arcsine & other) const
{
  if (this == &other) return true;
  return (a_ == other.a_) && (b_ == other.b_);
}

Bool Arcsine::equals(const DistributionImplementation & other) const
{
  const Arcsine* p_other = dynamic_cast<const Arcsine*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Arcsine::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Arcsine::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " a=" << a_
      << " b=" << b_;
  return oss;
}

String Arcsine::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(a = " << a_ << ", b = " << b_ << ")";
  return oss;
}

/* Virtual constructor */
Arcsine * Arcsine::clone() const
{
  return new Arcsine(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Arcsine::computeRange()
{
  setRange(Interval(a_, b_));
}

/* Get one realization of the distribution */
Point Arcsine::getRealization() const
{
  return Point(1, computeScalarQuantile(RandomGenerator::Generate()));
}

/* Get the DDF of the distribution */
Point Arcsine::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  Scalar ddf = 0.0;
  if ((a_ < x) && (x <= b_))
  {
    ddf = (x - 0.5 * (a_ + b_)) / (M_PI * (x - a_) * (b_ - x) * std::sqrt((x - a_) * (b_ - x)));
  }
  return Point(1, ddf);
}


/* Get the PDF of the distribution */
Scalar Arcsine::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if ((x <= a_) || (x >= b_)) return 0.0;
  return std::exp(computeLogPDF(point));
}

Scalar Arcsine::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if ((x <= a_) || (x >= b_)) return SpecFunc::LogMinScalar;
  return -std::log(M_PI) - 0.5 * (std::log(b_ - x) + std::log(x - a_));
}

/* Get the CDF of the distribution */
Scalar Arcsine::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (x <= a_) return 0.0;
  if (x >= b_) return 1.0;
  const Scalar value = std::asin((x - 0.5 * (a_ + b_)) / (0.5 * (b_ - a_))) / M_PI;
  return 0.5 + value;
}

Scalar Arcsine::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (x <= a_) return 1.0;
  if (x > b_) return 0.0;
  const Scalar value = std::asin((x - 0.5 * (a_ + b_)) / (0.5 * (b_ - a_))) / M_PI;
  return 0.5 - value;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Arcsine::computeCharacteristicFunction(const Scalar x) const
{
  //  return std::exp(Complex(0.0, a_)) * SpecFunc::HyperGeom_1_1(0.5, 1.0, (b_ - a_) * x);
  return std::exp(Complex(0.0, a_ + 0.5 * (b_ - a_) * x)) * j0(0.5 * (b_ - a_) * x);
}

/* Get the PDFGradient of the distribution */
Point Arcsine::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point pdfGradient(2);
  Point pdfGradient2(2);
  const Scalar x = point[0];

  if ((a_ < x) && (x <= b_))
  {
    pdfGradient[0] = -0.5 * (b_ - a_) * std::sqrt((b_ - x) * (x - a_)) / (M_PI * (x - b_) * (x - b_) * (x - a_) * (x - a_));
    pdfGradient[1] = (0.5 * (a_ + b_) - x) * std::sqrt((b_ - x) * (x - a_)) / (M_PI * (x - a_) * (x - a_) * (b_ - x) * (b_ - x));
    pdfGradient2[0] = -0.5 * pdfGradient[0] + 0.5 * pdfGradient[1];
    pdfGradient2[1] =  0.5 * pdfGradient[0] + 0.5 * pdfGradient[1];
  }
  return pdfGradient2;
}

/* Get the CDFGradient of the distribution */
Point Arcsine::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point cdfGradient(2);
  Point cdfGradient2(2);
  const Scalar x = point[0];
  if ((a_ < x) && (x <= b_))
  {
    cdfGradient[0] = (0.5 * (a_ + b_) - x) / (M_PI * 0.5 * (b_ - a_) * std::sqrt((b_ - x) * (x - a_)));
    cdfGradient[1]  = -1.0 / (M_PI * std::sqrt((b_ - x) * (x - a_)));
    cdfGradient2[0] = -0.5 * cdfGradient[0] + 0.5 * cdfGradient[1];
    cdfGradient2[1] =  0.5 * cdfGradient[0] + 0.5 * cdfGradient[1];
  }
  return cdfGradient2;
}

/* Get the quantile of the distribution */
Scalar Arcsine::computeScalarQuantile(const Scalar prob,
                                      const Bool tail) const
{
  const Scalar proba = tail ? 1.0 - prob : prob;
  const Scalar quantile = 0.5 * (b_ - a_) * std::sin(M_PI * (proba - 0.5)) + 0.5 * (a_ + b_);
  return quantile;
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar Arcsine::getRoughness() const
{
  throw NotDefinedException(HERE) << "roughness of Arcsine distribution is not L2-integrable";
}

/* Compute the entropy of the distribution */
Scalar Arcsine::computeEntropy() const
{
  return std::log(M_PI * (b_ - a_) / 4.0);
}

/* Compute the mean of the distribution */
void Arcsine::computeMean() const
{
  mean_ = Point(1, 0.5 * (a_ + b_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Arcsine::getStandardDeviation() const /*throw(NotDefinedException)*/
{
  return Point(1, 0.5 * (b_ - a_) * M_SQRT1_2);
}

/* Get the skewness of the distribution */
Point Arcsine::getSkewness() const /*throw(NotDefinedException)*/
{
  return Point(1, 0.0);
}

/* Get the kurtosis of the distribution */
Point Arcsine::getKurtosis() const /*throw(NotDefinedException)*/
{
  Scalar standardDeviation4 = getStandardDeviation()[0];
  standardDeviation4 *= standardDeviation4;
  standardDeviation4 *= standardDeviation4;
  Scalar a4 = 0.5 * (b_ - a_) * 0.5 * (b_ - a_);
  a4 *= a4;
  return Point(1, (3.0 / 8.0) * a4 / standardDeviation4);
}

/* Get the moments of the standardized distribution */
Point Arcsine::getStandardMoment(const UnsignedInteger n) const
{
  if (n % 2 == 1) return Point(1, 0.0);
  return Point(1, std::exp(SpecFunc::LogGamma(0.5 * n + 0.5) - SpecFunc::LogGamma(0.5 * n + 1)) / std::sqrt(M_PI));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Arcsine::getStandardRepresentative() const
{
  return new Arcsine(-1.0, 1.0);
}

/* Compute the covariance of the distribution */
void Arcsine::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const Scalar eta = getStandardDeviation()[0];
  covariance_(0, 0) = eta * eta;
  isAlreadyComputedCovariance_ = true;
}

Point Arcsine::getParameter() const
{
  return {a_, b_};
}

void Arcsine::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Arcsine(parameter[0], parameter[1]);
  setWeight(w);
}

Description Arcsine::getParameterDescription() const
{
  return {"a", "b"};
}

/* Check if the distribution is elliptical */
Bool Arcsine::isElliptical() const
{
  return true;
}

/* A accessor */
void Arcsine::setA(const Scalar a)
{
  if (a >= b_) throw InvalidArgumentException(HERE) << "in Arcsine : a must be smaller than b";
  if (a != a_)
  {
    a_ = a;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar Arcsine::getA() const
{
  return a_;
}


/* B accessor */
void Arcsine::setB(const Scalar b)
{
  if (a_ >= b) throw InvalidArgumentException(HERE) << "in Arcsine : a must be smaller than b";
  if (b != b_)
  {
    b_ = b;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar Arcsine::getB() const
{
  return b_;
}

void Arcsine::setAB(const Scalar a,
                    const Scalar b)
{
  if (a >= b)
    throw InvalidArgumentException(HERE) << "in Arcsine : a must be smaller than b";
  a_ = a;
  b_ = b;
  computeRange();
}

/* Method save() stores the object through the StorageManager */
void Arcsine::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "a_", a_ );
  adv.saveAttribute( "b_", b_ );
}

/* Method load() reloads the object from the StorageManager */
void Arcsine::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "a_", a_ );
  adv.loadAttribute( "b_", b_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS
