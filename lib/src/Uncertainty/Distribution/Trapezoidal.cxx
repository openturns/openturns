//                                               -*- C++ -*-
/**
 *  @brief The Trapezoidal distribution
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
#include <cmath>
#include "openturns/Trapezoidal.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Trapezoidal)

static const Factory<Trapezoidal> Factory_Trapezoidal;

/* Default constructor */
Trapezoidal::Trapezoidal()
  : ContinuousDistribution()
  , a_(-2.0)
  , b_(-1.0)
  , c_(1.0)
  , d_(2.0)
{
  setName("Trapezoidal");
  update();
}

/* Parameters constructor */
Trapezoidal::Trapezoidal(const Scalar a,
                         const Scalar b,
                         const Scalar c,
                         const Scalar d)
  : ContinuousDistribution()
  , a_(a)
  , b_(b)
  , c_(c)
  , d_(d)
{
  setName("Trapezoidal");
  update();
}

/* Comparison operator */
Bool Trapezoidal::operator ==(const Trapezoidal & other) const
{
  if (this == &other) return true;
  return (a_ == other.a_) && (b_ == other.b_) && (c_ == other.c_) && (d_ == other.d_);
}

Bool Trapezoidal::equals(const DistributionImplementation & other) const
{
  const Trapezoidal* p_other = dynamic_cast<const Trapezoidal*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Trapezoidal::__repr__() const
{
  OSS oss;
  oss << "class=" << Trapezoidal::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " a=" << a_
      << " b=" << b_
      << " c=" << c_
      << " d=" << d_
      << " h=" << h_;
  return oss;
}

String Trapezoidal::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(a = " << a_ << ", b = " << b_ << ", c = " << c_ << ", d = " << d_ << ")";
  return oss;
}

/* Virtual constructor */
Trapezoidal * Trapezoidal::clone() const
{
  return new Trapezoidal(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Trapezoidal::computeRange()
{
  setRange(Interval(a_, d_));
}


/* Get one realization of the distribution */
Point Trapezoidal::getRealization() const
{
  return Point(1, computeScalarQuantile(RandomGenerator::Generate()));
}


/* Get the DDF of the distribution */
Point Trapezoidal::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];

  if ((a_ < x) && (x < b_)) return Point(1, h_ / (b_ - a_));
  if ((c_ < x) && (x < d_)) return Point(1, -h_ / (d_ - c_));
  return Point(1, 0.0);
}


/* Get the PDF of the distribution */
Scalar Trapezoidal::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];

  if (x <= a_) return 0.0;
  // Will never go there if a_ == b_
  if (x < b_) return h_ * (x - a_) / (b_ - a_);
  if (x < c_) return h_;
  // Will never go there if c_ == d_
  if (x < d_) return h_ * (d_ - x) / (d_ - c_);
  return 0.0;
}

/* Get the CDF of the distribution */
Scalar Trapezoidal::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (x <= a_) return 0.0;
  // Will never go there if a_ == b_
  if (x < b_) return 0.5 * h_ * ( (x - a_) * (x - a_) / (b_ - a_) );
  if (x < c_) return 0.5 * h_ * (2.0 * x - a_ - b_);
  // Will never go there if c_ == d_
  if (x < d_) return 1.0 - 0.5 * h_ * (x - d_) * (x - d_) / (d_ - c_);
  return 1.0;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Trapezoidal::computeCharacteristicFunction(const Scalar u) const
{
  const Scalar au = a_ * u;
  const Scalar bu = b_ * u;
  const Scalar cu = c_ * u;
  const Scalar du = d_ * u;

  Scalar real = 0.0;
  Scalar imag = 0.0;
  Complex phi;
  const Scalar u2 = u * u;
  const Scalar u3 = u2 * u;
  const Scalar u4 = u2 * u2;
  const Scalar a2 = a_ * a_;
  const Scalar a3 = a2 * a_;
  const Scalar a4 = a2 * a2;
  const Scalar c2 = c_ * c_;
  const Scalar c3 = c2 * c_;
  const Scalar c4 = c2 * c2;
  // Use an even power to check if the series can be used
  const Scalar factor = h_ * ((d_ + c_) * (c2 + d_ * (c_ + d_)) * (c2 + d_ * (d_ - c_)) - (b_ + a_) * (a2 + b_ * (a_ + b_)) * (a2 + b_ * (b_ - a_))) * u4 / 720.0;
  if (std::abs(factor) < SpecFunc::ScalarEpsilon) phi = Complex(1.0 + ((a_ + b_) * (a2 + b_ * b_) - (c_ + d_) * (c2 + d_ * d_)) * h_ * u2 / 24.0 + factor, (c2 + d_ * (c_ + d_) - (a2 + b_ * (a_ + b_))) * h_ * u / 6.0 + (a4 + b_ * (a3 + b_ * (a2 + b_ * (a_ + b_))) - (c4 + d_ * (c3 + d_ * (c2 + d_ * (c_ + d_))))) * h_ * u3 / 120.0);
  else
  {
    if (a_ < b_)
    {
      real += ((b_ - a_) * u * std::sin(bu) + std::cos(bu) - std::cos(au)) * h_ / ((b_ - a_) * u * u);
      imag += (std::sin(bu) + (a_ - b_) * u * std::cos(bu) - std::sin(au)) * h_ / ((b_ - a_) * u * u);
    }
    real +=  (std::sin(cu) - std::sin(bu)) * h_ / u;
    imag += -(std::cos(cu) - std::cos(bu)) * h_ / u;
    if(c_ < d_)
    {
      real += -(std::cos(du) + (d_ - c_) * u * std::sin(cu) - std::cos(cu)) * h_ / ((d_ - c_) * u * u);
      imag += -(std::sin(du) - std::sin(cu) - (d_ - c_) * u * std::cos(cu)) * h_ / ((d_ - c_) * u * u);
    }
    phi = Complex (real, imag);
  }
  return phi;
}

/* Get the PDFGradient of the distribution */
Point Trapezoidal::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if ((a_ == b_) || (b_ == c_) || (c_ == d_)) throw NotDefinedException(HERE) << "Error: cannot compute the PDF gradient of a Trapezoidal distribution when a=b or b=c or c=d, here a=" << a_ << ", b=" << b_ << ", c=" << c_ << " and d=" << d_;
  const Scalar x = point[0];
  Point pdfGradient(4, 0.0);
  if ((a_ < x) && (x < b_))
  {
    pdfGradient[0] = -(d_ * b_ - d_ * x - a_ * a_ + 2.0 * a_ * x - b_ * b_ + b_ * c_ - c_ * x) * h_ * h_ / (2.0 * (a_ - b_) * (a_ - b_));
    pdfGradient[1] = (2.0 * b_ - d_ - c_) * (x - a_) * h_ * h_ / (2.0 * (a_ - b_) * (a_ - b_));
    pdfGradient[2] = 0.5 * (x - a_) * h_ * h_ / (a_ - b_);
    pdfGradient[3] = 0.5 * (x - a_) * h_ * h_ / (a_ - b_);
  }
  else if ((b_ <= x) && (x <= c_))
  {
    pdfGradient[0] =  h_ * h_ / 2.0;
    pdfGradient[1] =  h_ * h_ / 2.0;
    pdfGradient[2] = -h_ * h_ / 2.0;
    pdfGradient[3] = -h_ * h_ / 2.0;
  }
  else if ((c_ < x) && (x < d_))
  {
    pdfGradient[0] = 0.5 * (d_ - x) * h_ * h_ / (d_ - c_);
    pdfGradient[1] = 0.5 * (d_ - x) * h_ * h_ / (d_ - c_);
    pdfGradient[2] = 0.5 * (2.0 * c_ - b_ - a_) * (d_ - x) * h_ * h_ / ((d_ - c_) * (d_ - c_));
    pdfGradient[3] = 0.5 * (b_ * c_ - b_ * x - c_ * c_ + c_ * a_ - d_ * d_ + 2.0 * d_ * x - a_ * x) * h_ * h_ / ((c_ - d_) * (c_ - d_));
  }
  return pdfGradient;
}

/* Get the logPDFGradient of the distribution */
Point Trapezoidal::computeLogPDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if ((a_ == b_) || (b_ == c_) || (c_ == d_)) throw NotDefinedException(HERE) << "Error: cannot compute the log PDF gradient of a Trapezoidal distribution when a=b or b=c or c=d, here a=" << a_ << ", b=" << b_ << ", c=" << c_ << " and d=" << d_;
  const Scalar x = point[0];
  Point logPdfGradient(4, 0.0);
  if ((a_ < x) && (x < b_))
  {
    logPdfGradient[0] = h_ / 2.0 - 1.0 / (x - a_) + 1.0 / (b_ - a_);
    logPdfGradient[1] = h_ / 2.0 - 1.0 / (b_ - a_);
    logPdfGradient[2] = -h_ / 2.0;
    logPdfGradient[3] = -h_ / 2.0;
  }
  else if ((b_ <= x) && (x <= c_))
  {
    logPdfGradient[0] =  h_ / 2.0;
    logPdfGradient[1] =  h_ / 2.0;
    logPdfGradient[2] = -h_ / 2.0;
    logPdfGradient[3] = -h_ / 2.0;
  }
  else if ((c_ < x) && (x < d_))
  {
    logPdfGradient[0] = h_ / 2.0;
    logPdfGradient[1] = h_ / 2.0;
    logPdfGradient[2] = -h_ / 2.0 + 1.0 / (d_ - c_);
    logPdfGradient[3] = -h_ / 2.0 + 1.0 / (d_ - x) - 1.0 / (d_ - c_);
  }
  return logPdfGradient;
}


/* Get the CDFGradient of the distribution */
Point Trapezoidal::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if ((a_ == b_) || (b_ == c_) || (c_ == d_)) throw NotDefinedException(HERE) << "Error: cannot compute the CDF gradient of a Trapezoidal distribution when a=b or b=c or c=d, here a=" << a_ << ", b=" << b_ << ", c=" << c_ << " and d=" << d_;
  const Scalar x = point[0];
  Point cdfGradient(4, 0.0);
  if ((a_ < x) && (x < b_))
  {
    cdfGradient[0] =  0.25 * (x - a_) * (c_ * a_ - 2.0 * c_ * b_ + c_ * x - 2.0 * a_ * x + a_ * d_ + 2.0 * b_ * b_ - 2.0 * b_ * d_ + x * d_) * h_ * h_ / ((a_ - b_) * (a_ - b_));
    cdfGradient[1] = -0.25 * (a_ - x) * (a_ - x) * (c_ + d_ - 2.0 * b_) * h_ * h_ / ((a_ - b_) * (a_ - b_));
    cdfGradient[2] =  0.25 * (a_ - x) * (a_ - x) * h_ * h_ / (a_ - b_);
    cdfGradient[3] =  0.25 * (a_ - x) * (a_ - x) * h_ * h_ / (a_ - b_);
  }
  else if ((b_ <= x) && (x <= c_))
  {
    cdfGradient[0] = 0.25 * (-c_ - d_ + 2.0 * x) * h_ * h_;
    cdfGradient[1] = 0.25 * (-c_ - d_ + 2.0 * x) * h_ * h_;
    cdfGradient[2] = 0.25 * (a_ + b_ - 2.0 * x) * h_ * h_;
    cdfGradient[3] = 0.25 * (a_ + b_ - 2.0 * x) * h_ * h_;
  }
  else if ((c_ < x) && (x < d_))
  {
    cdfGradient[0] = -0.25 * (d_ - x) * (d_ - x) * h_ * h_ / (d_ - c_);
    cdfGradient[1] = -0.25 * (d_ - x) * (d_ - x) * h_ * h_ / (d_ - c_);
    cdfGradient[2] =  0.25 * (d_ - x) * (d_ - x) * (a_ + b_ - 2.0 * c_) * h_ * h_ / ((d_ - c_) * (d_ - c_));
    cdfGradient[3] = -0.25 * (d_ - x) * (2.0 * a_ * c_ - a_ * d_ - a_ * x + 2.0 * b_ * c_ - b_ * x - 2.0 * c_ * c_ + 2.0 * d_ * x) * h_ * h_ / ((d_ - c_) * (d_ - c_));
  }
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar Trapezoidal::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  const Scalar c1 = 0.5 * (b_ - a_) * h_;
  const Scalar c2 = c1 + (c_ - b_) * h_;
  const Scalar q = tail ? 1.0 - prob : prob;
  // q in (0, c1)
  if (q <= c1) return a_ + std::sqrt(2.0 * (b_ - a_) * q / h_);
  // q in (c1, c2)
  if (q <= c2) return b_ + (q - c1) / h_;
  // q in (c2, 1)
  return d_ - std::sqrt(2.0 * (d_ - c_) * (1.0 - q) / h_);
}

Scalar Trapezoidal::computeProbability(const Interval & interval) const
{
  if (interval.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "computeProbability expected an interval of dimension=" << dimension_ << ", got dimension=" << interval.getDimension();
  return computeProbabilityGeneral1D(interval.getLowerBound()[0], interval.getUpperBound()[0]);
}

/* Compute the entropy of the distribution */
Scalar Trapezoidal::computeEntropy() const
{
  return 0.25 * h_ * ((b_ - a_) + (d_ - c_)) - std::log(h_);
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar Trapezoidal::getRoughness() const
{
  return -(a_ + 2.0 * b_ - 2.0 * c_ - d_) * h_ * h_ / 3.0;
}

/* Compute the mean of the distribution */
void Trapezoidal::computeMean() const
{
  mean_ = Point(1, (-b_ * b_ - b_ * a_ - a_ * a_ + c_ * c_ + c_ * d_ + d_ * d_) * h_ / 6.0);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Trapezoidal::getStandardDeviation() const /*throw(NotDefinedException)*/
{
  return Point(1, getSigma());
}

/* Get the skewness of the distribution */
Point Trapezoidal::getSkewness() const /*throw(NotDefinedException)*/
{
  const Scalar a2 = a_ * a_;
  const Scalar a3 = a2 * a_;
  const Scalar a4 = a3 * a_;

  const Scalar b2 = b_ * b_;
  const Scalar b3 = b2 * b_;
  const Scalar b4 = b3 * b_;

  const Scalar c2 = c_ * c_;
  const Scalar c3 = c2 * c_;
  const Scalar c4 = c3 * c_;

  const Scalar d2 = d_ * d_;
  const Scalar d3 = d2 * d_;
  const Scalar d4 = d3 * d_;

  Scalar thirdMoment = (d_ - c_ - b_ + a_) * (d_ - c_ + b_ - a_) * (2 * d4 + 10 * c_ * d3 - 9 * b_ * d3 - 9 * a_ * d3 + 21 * c2 * d2 - 36 * b_ * c_ * d2 - 36 * a_ * c_ * d2 + 14 * b2 * d2 + 35 * a_ * b_ * d2 + 14 * a2 * d2 + 10 * c3 * d_ - 36 * b_ * c2 * d_ - 36 * a_ * c2 * d_ + 35 * b2 * c_ * d_ + 74 * a_ * b_ * c_ * d_ + 35 * a2 * c_ * d_ - 9 * b3 * d_ - 36 * a_ * b2 * d_ - 36 * a2 * b_ * d_ - 9 * a3 * d_ + 2 * c4 - 9 * b_ * c3 - 9 * a_ * c3 + 14 * b2 * c2 + 35 * a_ * b_ * c2 + 14 * a2 * c2 - 9 * b3 * c_ - 36 * a_ * b2 * c_ - 36 * a2 * b_ * c_ - 9 * a3 * c_ + 2 * b4 + 10 * a_ * b3 + 21 * a2 * b2 + 10 * a3 * b_ + 2 * a4) * h_ * h_ * h_ / 2160.0;

  // retrieve sigma^3
  Scalar standardDeviation3 = getSigma();
  standardDeviation3 = standardDeviation3 * standardDeviation3 * standardDeviation3;

  return Point(1, thirdMoment / standardDeviation3);
}

/* Get the kurtosis of the distribution */
Point Trapezoidal::getKurtosis() const /*throw(NotDefinedException)*/
{
  const Scalar a2 = a_ * a_;
  const Scalar a3 = a2 * a_;
  const Scalar a4 = a3 * a_;

  const Scalar b2 = b_ * b_;
  const Scalar b3 = b2 * b_;
  const Scalar b4 = b3 * b_;

  const Scalar c2 = c_ * c_;
  const Scalar c3 = c2 * c_;
  const Scalar c4 = c3 * c_;

  const Scalar d2 = d_ * d_;
  const Scalar d3 = d2 * d_;
  const Scalar d4 = d3 * d_;

  const Scalar h2 = h_ * h_;

  Scalar fourthMoment = (d2 + c_ * d_ - 2 * b_ * d_ - a_ * d_ + c2 - b_ * c_ - 2 * a_ * c_ + b2 + a_ * b_ + a2) * (d2 + c_ * d_ - b_ * d_ - 2 * a_ * d_ + c2 - 2 * b_ * c_ - a_ * c_ + b2 + a_ * b_ + a2) * (d4 + 2 * c_ * d3 - 3 * b_ * d3 - 3 * a_ * d3 - 3 * c2 * d2 + 4 * b2 * d2 + a_ * b_ * d2 + 4 * a2 * d2 + 2 * c3 * d_ + b2 * c_ * d_ - 2 * a_ * b_ * c_ * d_ + a2 * c_ * d_ - 3 * b3 * d_ - 3 * a3 * d_ + c4 - 3 * b_ * c3 - 3 * a_ * c3 + 4 * b2 * c2 + a_ * b_ * c2 + 4 * a2 * c2 - 3 * b3 * c_ - 3 * a3 * c_ + b4 + 2 * a_ * b3 - 3 * a2 * b2 + 2 * a3 * b_ + a4) * h2 * h2 / 2160.0;

  // retrieve sigma^4
  Scalar standardDeviation4 = getSigma();
  standardDeviation4 *= standardDeviation4;
  standardDeviation4 *= standardDeviation4;

  return Point(1, fourthMoment / standardDeviation4);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Trapezoidal::getStandardRepresentative() const
{
  Trapezoidal standard(-1.0, 1.0 - 2.0 * (d_ - b_) / (d_ - a_), 1.0 - 2.0 * (d_ - c_) / (d_ - a_), 1.0);
  standard.setDescription(getDescription());
  return standard;
}

/* Compute the covariance of the distribution */
void Trapezoidal::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  Scalar standardDeviation = getSigma();
  covariance_(0, 0) = standardDeviation * standardDeviation;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
Point Trapezoidal::getParameter() const
{
  Point point(4);
  point[0] = a_;
  point[1] = b_;
  point[2] = c_;
  point[3] = d_;
  return point;
}

void Trapezoidal::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 4) throw InvalidArgumentException(HERE) << "Error: expected 4 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Trapezoidal(parameter[0], parameter[1], parameter[2], parameter[3]);
  setWeight(w);
}

/* Parameters description accessor */
Description Trapezoidal::getParameterDescription() const
{
  Description description(4);
  description[0] = "a";
  description[1] = "b";
  description[2] = "c";
  description[3] = "d";
  return description;
}

/* Check if the distribution is elliptical */
Bool Trapezoidal::isElliptical() const
{
  return std::abs(b_ - a_ - (d_ - c_)) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
}

/* Sigma accessor */
Scalar Trapezoidal::getSigma() const
{
  const Scalar a2 = a_ * a_;
  const Scalar a3 = a2 * a_;
  const Scalar a4 = a3 * a_;

  const Scalar b2 = b_ * b_;
  const Scalar b3 = b2 * b_;
  const Scalar b4 = b3 * b_;

  const Scalar c2 = c_ * c_;
  const Scalar c3 = c2 * c_;
  const Scalar c4 = c3 * c_;

  const Scalar d2 = d_ * d_;
  const Scalar d3 = d2 * d_;
  const Scalar d4 = d3 * d_;

  Scalar SecondMoment = (d4 + 2 * c_ * d3 - 3 * b_ * d3 - 3 * a_ * d3 - 3 * b_ * c_ * d2 - 3 * a_ * c_ * d2 + 4 * b2 * d2 + 4 * a_ * b_ * d2 + 4 * a2 * d2 + 2 * c3 * d_ - 3 * b_ * c2 * d_ - 3 * a_ * c2 * d_ + 4 * b2 * c_ * d_ + 4 * a_ * b_ * c_ * d_ + 4 * a2 * c_ * d_ - 3 * b3 * d_ - 3 * a_ * b2 * d_ - 3 * a2 * b_ * d_ - 3 * a3 * d_ + c4 - 3 * b_ * c3 - 3 * a_ * c3 + 4 * b2 * c2 + 4 * a_ * b_ * c2 + 4 * a2 * c2 - 3 * b3 * c_ - 3 * a_ * b2 * c_ - 3 * a2 * b_ * c_ - 3 * a3 * c_ + b4 + 2 * a_ * b3 + 2 * a3 * b_ + a4) * h_ * h_ / 72.0;

  return std::sqrt(SecondMoment);
}

/* A accessor */
void Trapezoidal::setA(const Scalar a)
{
  if (a != a_)
  {
    a_ = a;
    update();
  }
}

Scalar Trapezoidal::getA() const
{
  return a_;
}


/* B accessor */
void Trapezoidal::setB(const Scalar b)
{
  if (b != b_)
  {
    b_ = b;
    update();
  }
}

Scalar Trapezoidal::getB() const
{
  return b_;
}

/* C accessor */
void Trapezoidal::setC(const Scalar c)
{
  if (c != c_)
  {
    c_ = c;
    update();
  }
}

Scalar Trapezoidal::getC() const
{
  return c_;
}

/* D accessor */
void Trapezoidal::setD(const Scalar d)
{
  if (d != d_)
  {
    d_ = d;
    update();
  }
}

Scalar Trapezoidal::getD() const
{
  return d_;
}

/* Update attributes */
void Trapezoidal::update()
{
  setDimension(1);
  if (d_ <= a_) throw InvalidArgumentException(HERE) << " in Trapezoidal : distribution parameter d must be greater than argument a, here a=" << a_ << " d=" << d_;
  if (b_ < a_) throw InvalidArgumentException(HERE) << " in Trapezoidal : distribution parameter b must be greater or equal to argument a, here a=" << a_ << " b=" << b_;
  if (c_ < b_) throw InvalidArgumentException(HERE) << " in Trapezoidal : distribution parameter c must be greater than argument b, here b=" << b_ << " c=" << c_;
  if (d_ < c_) throw InvalidArgumentException(HERE) << " in Trapezoidal : distribution parameter d must be greater or equal to argument c, here c=" << c_ << " d=" << d_;
  h_ = 2. / (c_ - a_ + d_ - b_);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

/* Get the PDF singularities inside of the range - 1D only */
Point Trapezoidal::getSingularities() const
{
  Point singularities(0);
  if (a_ < b_ && b_ < d_) singularities.add(b_);
  if (b_ < c_ && c_ < d_) singularities.add(c_);
  return singularities;
}

/* Method save() stores the object through the StorageManager */
void Trapezoidal::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute("a_", a_);
  adv.saveAttribute("b_", b_);
  adv.saveAttribute("c_", c_);
  adv.saveAttribute("d_", d_);
}

/* Method load() reloads the object from the StorageManager */
void Trapezoidal::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute("a_", a_);
  adv.loadAttribute("b_", b_);
  adv.loadAttribute("c_", c_);
  adv.loadAttribute("d_", d_);
  update();
}

END_NAMESPACE_OPENTURNS
