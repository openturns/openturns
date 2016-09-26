//                                               -*- C++ -*-
/**
 *  @brief The Trapezoidal distribution
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
#include <cmath>
#include "openturns/Trapezoidal.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Trapezoidal);

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
Trapezoidal::Trapezoidal(const NumericalScalar a,
                         const NumericalScalar b,
                         const NumericalScalar c,
                         const NumericalScalar d)
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

String Trapezoidal::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(a = " << a_ << ", b = " << b_ << ", c = " << c_ << ", d = " << d_ << ")";
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
NumericalPoint Trapezoidal::getRealization() const
{
  return NumericalPoint(1, computeScalarQuantile(RandomGenerator::Generate()));
}


/* Get the DDF of the distribution */
NumericalPoint Trapezoidal::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];

  if ((a_ < x) && (x < b_)) return NumericalPoint(1, h_ / (b_ - a_));
  return NumericalPoint(1, -h_ / (d_ - c_));
}


/* Get the PDF of the distribution */
NumericalScalar Trapezoidal::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];

  if (x <= a_) return 0.0;
  // Will never go there if a_ == b_
  if (x < b_) return h_ * (x - a_) / (b_ - a_);
  if (x < c_) return h_;
  // Will never go there if c_ == d_
  if (x < d_) return h_ * (d_ - x) / (d_ - c_);
  return 0.0;
}

/* Get the CDF of the distribution */
NumericalScalar Trapezoidal::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if (x <= a_) return 0.0;
  // Will never go there if a_ == b_
  if (x < b_) return 0.5 * h_ * ( (x - a_) * (x - a_) / (b_ - a_) );
  if (x < c_) return 0.5 * h_ * (2.0 * x - a_ - b_);
  // Will never go there if c_ == d_
  if (x < d_) return 1.0 - 0.5 * h_ * (x - d_) * (x - d_) / (d_ - c_);
  return 1.0;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex Trapezoidal::computeCharacteristicFunction(const NumericalScalar u) const
{
  const NumericalScalar au = a_ * u;
  const NumericalScalar bu = b_ * u;
  const NumericalScalar cu = c_ * u;
  const NumericalScalar du = d_ * u;

  NumericalScalar real = 0.0;
  NumericalScalar imag = 0.0;
  NumericalComplex phi;

  if (std::abs(u) <= 1.0e-10) phi = NumericalComplex((-a_ + 3.0 * b_ - 3.0 * c_ + d_) * h_ / 2.0, 1.0);
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
    phi = NumericalComplex (real, imag);
  }
  return phi;
}

/* Get the PDFGradient of the distribution */
NumericalPoint Trapezoidal::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  NumericalPoint pdfGradient(4, 0.0);
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


/* Get the CDFGradient of the distribution */
NumericalPoint Trapezoidal::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  NumericalPoint cdfGradient(4, 0.0);
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
NumericalScalar Trapezoidal::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  const NumericalScalar c1 = 0.5 * (b_ - a_) * h_;
  const NumericalScalar c2 = c1 + (c_ - b_) * h_;
  const NumericalScalar q = tail ? 1.0 - prob : prob;
  // q in (0, c1)
  if (q <= c1) return a_ + std::sqrt(2.0 * (b_ - a_) * q / h_);
  // q in (c1, c2)
  if (q <= c2) return b_ + (q - c1) / h_;
  // q in (c2, 1)
  return d_ - std::sqrt(2.0 * (d_ - c_) * (1.0 - q) / h_);
}

/* Get the roughness, i.e. the L2-norm of the PDF */
NumericalScalar Trapezoidal::getRoughness() const
{
  return -(a_ + 2.0 * b_ - 2.0 * c_ - d_) * h_ * h_ / 3.0;
}

/* Compute the mean of the distribution */
void Trapezoidal::computeMean() const
{
  mean_ = NumericalPoint(1, (-b_ * b_ - b_ * a_ - a_ * a_ + c_ * c_ + c_ * d_ + d_ * d_) * h_ / 6.0);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint Trapezoidal::getStandardDeviation() const /*throw(NotDefinedException)*/
{
  return NumericalPoint(1, getSigma());
}

/* Get the skewness of the distribution */
NumericalPoint Trapezoidal::getSkewness() const /*throw(NotDefinedException)*/
{
  const NumericalScalar a2 = a_ * a_;
  const NumericalScalar a3 = a2 * a_;
  const NumericalScalar a4 = a3 * a_;

  const NumericalScalar b2 = b_ * b_;
  const NumericalScalar b3 = b2 * b_;
  const NumericalScalar b4 = b3 * b_;

  const NumericalScalar c2 = c_ * c_;
  const NumericalScalar c3 = c2 * c_;
  const NumericalScalar c4 = c3 * c_;

  const NumericalScalar d2 = d_ * d_;
  const NumericalScalar d3 = d2 * d_;
  const NumericalScalar d4 = d3 * d_;

  NumericalScalar thirdMoment = (d_ - c_ - b_ + a_) * (d_ - c_ + b_ - a_) * (2 * d4 + 10 * c_ * d3 - 9 * b_ * d3 - 9 * a_ * d3 + 21 * c2 * d2 - 36 * b_ * c_ * d2 - 36 * a_ * c_ * d2 + 14 * b2 * d2 + 35 * a_ * b_ * d2 + 14 * a2 * d2 + 10 * c3 * d_ - 36 * b_ * c2 * d_ - 36 * a_ * c2 * d_ + 35 * b2 * c_ * d_ + 74 * a_ * b_ * c_ * d_ + 35 * a2 * c_ * d_ - 9 * b3 * d_ - 36 * a_ * b2 * d_ - 36 * a2 * b_ * d_ - 9 * a3 * d_ + 2 * c4 - 9 * b_ * c3 - 9 * a_ * c3 + 14 * b2 * c2 + 35 * a_ * b_ * c2 + 14 * a2 * c2 - 9 * b3 * c_ - 36 * a_ * b2 * c_ - 36 * a2 * b_ * c_ - 9 * a3 * c_ + 2 * b4 + 10 * a_ * b3 + 21 * a2 * b2 + 10 * a3 * b_ + 2 * a4) * h_ * h_ * h_ / 2160.0;

  // retrieve sigma^3
  NumericalScalar standardDeviation3 = getSigma();
  standardDeviation3 = standardDeviation3 * standardDeviation3 * standardDeviation3;

  return NumericalPoint(1, thirdMoment / standardDeviation3);
}

/* Get the kurtosis of the distribution */
NumericalPoint Trapezoidal::getKurtosis() const /*throw(NotDefinedException)*/
{
  const NumericalScalar a2 = a_ * a_;
  const NumericalScalar a3 = a2 * a_;
  const NumericalScalar a4 = a3 * a_;

  const NumericalScalar b2 = b_ * b_;
  const NumericalScalar b3 = b2 * b_;
  const NumericalScalar b4 = b3 * b_;

  const NumericalScalar c2 = c_ * c_;
  const NumericalScalar c3 = c2 * c_;
  const NumericalScalar c4 = c3 * c_;

  const NumericalScalar d2 = d_ * d_;
  const NumericalScalar d3 = d2 * d_;
  const NumericalScalar d4 = d3 * d_;

  const NumericalScalar h2 = h_ * h_;

  NumericalScalar fourthMoment = (d2 + c_ * d_ - 2 * b_ * d_ - a_ * d_ + c2 - b_ * c_ - 2 * a_ * c_ + b2 + a_ * b_ + a2) * (d2 + c_ * d_ - b_ * d_ - 2 * a_ * d_ + c2 - 2 * b_ * c_ - a_ * c_ + b2 + a_ * b_ + a2) * (d4 + 2 * c_ * d3 - 3 * b_ * d3 - 3 * a_ * d3 - 3 * c2 * d2 + 4 * b2 * d2 + a_ * b_ * d2 + 4 * a2 * d2 + 2 * c3 * d_ + b2 * c_ * d_ - 2 * a_ * b_ * c_ * d_ + a2 * c_ * d_ - 3 * b3 * d_ - 3 * a3 * d_ + c4 - 3 * b_ * c3 - 3 * a_ * c3 + 4 * b2 * c2 + a_ * b_ * c2 + 4 * a2 * c2 - 3 * b3 * c_ - 3 * a3 * c_ + b4 + 2 * a_ * b3 - 3 * a2 * b2 + 2 * a3 * b_ + a4) * h2 * h2 / 2160.0;

  // retrieve sigma^4
  NumericalScalar standardDeviation4 = getSigma();
  standardDeviation4 *= standardDeviation4;
  standardDeviation4 *= standardDeviation4;

  return NumericalPoint(1, fourthMoment / standardDeviation4);
}

/* Get the moments of the standardized distribution */
NumericalPoint Trapezoidal::getStandardMoment(const UnsignedInteger n) const
{
  if (n == 0) return NumericalPoint(1, 1.0);
  const NumericalScalar beta = 1.0 - 2.0 * (d_ - b_) / (d_ - a_);
  const NumericalScalar gamma = 1.0 - 2.0 * (d_ - c_) / (d_ - a_);
  const NumericalScalar eta = 2.0 / (2.0 - beta + gamma);
  const NumericalScalar betaPow = std::pow(beta, n + 1.0);
  const NumericalScalar gammaPow = std::pow(gamma, n + 1.0);
  NumericalScalar value = eta * (gammaPow - betaPow) / (n + 1);
  if (beta > -1.0)
  {
    if (n % 2 == 0) value += (eta / (beta + 1.0)) * ((beta * betaPow - 1.0) / (n + 2) + (betaPow + 1.0) / (n + 1));
    else value += (eta / (beta + 1.0)) * ((beta * betaPow + 1.0) / (n + 2) + (betaPow - 1.0) / (n + 1));
  }
  if (gamma <  1.0) value -= (eta / (1.0 - gamma)) * ((1.0 - gamma * gammaPow) / (n + 2) - (1.0 - gammaPow) / (n + 1));
  return NumericalPoint(1, value);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Trapezoidal::Implementation Trapezoidal::getStandardRepresentative() const
{
  return Trapezoidal(-1.0, 1.0 - 2.0 * (d_ - b_) / (d_ - a_), 1.0 - 2.0 * (d_ - c_) / (d_ - a_), 1.0).clone();
}

/* Compute the covariance of the distribution */
void Trapezoidal::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  NumericalScalar standardDeviation = getSigma();
  covariance_(0, 0) = standardDeviation * standardDeviation;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
NumericalPoint Trapezoidal::getParameter() const
{
  NumericalPoint point(4);
  point[0] = a_;
  point[1] = b_;
  point[2] = c_;
  point[3] = d_;
  return point;
}

void Trapezoidal::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 4) throw InvalidArgumentException(HERE) << "Error: expected 4 values, got " << parameter.getSize();
  const NumericalScalar w = getWeight();
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
  return std::abs(b_ - a_ - (d_ - c_)) < ResourceMap::GetAsNumericalScalar("Distribution-DefaultQuantileEpsilon");
}

/* Sigma accessor */
NumericalScalar Trapezoidal::getSigma() const
{
  const NumericalScalar a2 = a_ * a_;
  const NumericalScalar a3 = a2 * a_;
  const NumericalScalar a4 = a3 * a_;

  const NumericalScalar b2 = b_ * b_;
  const NumericalScalar b3 = b2 * b_;
  const NumericalScalar b4 = b3 * b_;

  const NumericalScalar c2 = c_ * c_;
  const NumericalScalar c3 = c2 * c_;
  const NumericalScalar c4 = c3 * c_;

  const NumericalScalar d2 = d_ * d_;
  const NumericalScalar d3 = d2 * d_;
  const NumericalScalar d4 = d3 * d_;

  NumericalScalar SecondMoment = (d4 + 2 * c_ * d3 - 3 * b_ * d3 - 3 * a_ * d3 - 3 * b_ * c_ * d2 - 3 * a_ * c_ * d2 + 4 * b2 * d2 + 4 * a_ * b_ * d2 + 4 * a2 * d2 + 2 * c3 * d_ - 3 * b_ * c2 * d_ - 3 * a_ * c2 * d_ + 4 * b2 * c_ * d_ + 4 * a_ * b_ * c_ * d_ + 4 * a2 * c_ * d_ - 3 * b3 * d_ - 3 * a_ * b2 * d_ - 3 * a2 * b_ * d_ - 3 * a3 * d_ + c4 - 3 * b_ * c3 - 3 * a_ * c3 + 4 * b2 * c2 + 4 * a_ * b_ * c2 + 4 * a2 * c2 - 3 * b3 * c_ - 3 * a_ * b2 * c_ - 3 * a2 * b_ * c_ - 3 * a3 * c_ + b4 + 2 * a_ * b3 + 2 * a3 * b_ + a4) * h_ * h_ / 72.0;

  return std::sqrt(SecondMoment);
}

/* A accessor */
void Trapezoidal::setA(const NumericalScalar a)
{
  if (a != a_)
  {
    a_ = a;
    update();
  }
}

NumericalScalar Trapezoidal::getA() const
{
  return a_;
}


/* B accessor */
void Trapezoidal::setB(const NumericalScalar b)
{
  if (b != b_)
  {
    b_ = b;
    update();
  }
}

NumericalScalar Trapezoidal::getB() const
{
  return b_;
}

/* C accessor */
void Trapezoidal::setC(const NumericalScalar c)
{
  if (c != c_)
  {
    c_ = c;
    update();
  }
}

NumericalScalar Trapezoidal::getC() const
{
  return c_;
}

/* D accessor */
void Trapezoidal::setD(const NumericalScalar d)
{
  if (d != d_)
  {
    d_ = d;
    update();
  }
}

NumericalScalar Trapezoidal::getD() const
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
NumericalPoint Trapezoidal::getSingularities() const
{
  NumericalPoint singularities(0);
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
