//                                               -*- C++ -*-
/**
 *  @brief The Arcsine distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "Arcsine.hxx"
#include "RandomGenerator.hxx"
#include "SpecFunc.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Arcsine);

static const Factory<Arcsine> RegisteredFactory;


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
Arcsine::Arcsine(const NumericalScalar arg1,
                 const NumericalScalar arg2,
                 const ParameterSet set)
  : ContinuousDistribution()
  , a_(-1.0)
  , b_(1.0)
{
  setName("Arcsine");
  switch (set)
  {
    case AB:
      setAB(arg1, arg2);
      break;

    case MUSIGMA:
      setMuSigma(arg1, arg2);
      break;

    default:
      throw InvalidArgumentException(HERE) << "Invalid parameter set argument";
  }
  setDimension(1);
}

/* Comparison operator */
Bool Arcsine::operator ==(const Arcsine & other) const
{
  if (this == &other) return true;
  return (a_ == other.a_) && (b_ == other.b_);
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

String Arcsine::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(a = " << a_ << ", b = " << b_ << ")";
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
NumericalPoint Arcsine::getRealization() const
{
  return NumericalPoint(1, computeScalarQuantile(RandomGenerator::Generate()));
}

/* Get the DDF of the distribution */
NumericalPoint Arcsine::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  NumericalScalar ddf(0.0);
  if ((a_ < x) && (x <= b_))
  {
    ddf = (x - 0.5 * (a_ + b_)) / (M_PI * (x - a_) * (b_ - x) * std::sqrt((x - a_) * (b_ - x)));
  }
  return NumericalPoint(1, ddf);
}


/* Get the PDF of the distribution */
NumericalScalar Arcsine::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if ((x <= a_) || (x >= b_)) return 0.0;
  return std::exp(computeLogPDF(point));
}

NumericalScalar Arcsine::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if ((x <= a_) || (x >= b_)) return -SpecFunc::MaxNumericalScalar;
  return -std::log(M_PI) - 0.5 * (std::log(b_ - x) + std::log(x - a_));
}

/* Get the CDF of the distribution */
NumericalScalar Arcsine::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= a_) return 0.0;
  if (x >= b_) return 1.0;
  const NumericalScalar value(std::asin((x - 0.5 * (a_ + b_)) / (0.5 * (b_ - a_))) / M_PI);
  return 0.5 + value;
}

NumericalScalar Arcsine::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= a_) return 1.0;
  if (x > b_) return 0.0;
  const NumericalScalar value(std::asin((x - 0.5 * (a_ + b_)) / (0.5 * (b_ - a_))) / M_PI);
  return 0.5 - value;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex Arcsine::computeCharacteristicFunction(const NumericalScalar x) const
{
  //  return std::exp(NumericalComplex(0.0, a_)) * SpecFunc::HyperGeom_1_1(0.5, 1.0, (b_ - a_) * x);
  return std::exp(NumericalComplex(0.0, a_ + 0.5 * (b_ - a_) * x)) * j0(0.5 * (b_ - a_) * x);
}

/* Get the PDFGradient of the distribution */
NumericalPoint Arcsine::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalPoint pdfGradient(2);
  NumericalPoint pdfGradient2(2);
  const NumericalScalar x(point[0]);

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
NumericalPoint Arcsine::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalPoint cdfGradient(2);
  NumericalPoint cdfGradient2(2);
  const NumericalScalar x(point[0]);
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
NumericalScalar Arcsine::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  const NumericalScalar proba(tail ? 1.0 - prob : prob);
  const NumericalScalar quantile(0.5 * (b_ - a_) * std::sin(M_PI * (proba - 0.5)) + 0.5 * (a_ + b_));
  return quantile;
}

/* Get the roughness, i.e. the L2-norm of the PDF */
NumericalScalar Arcsine::getRoughness() const
{
  throw NotDefinedException(HERE) << "roughness of Arcsine distribution is not L2-integrable";
}

/* Compute the mean of the distribution */
void Arcsine::computeMean() const
{
  mean_ = NumericalPoint(1, getMu());
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint Arcsine::getStandardDeviation() const /*throw(NotDefinedException)*/
{
  return NumericalPoint(1, getSigma());
}

/* Get the skewness of the distribution */
NumericalPoint Arcsine::getSkewness() const /*throw(NotDefinedException)*/
{
  return NumericalPoint(1, 0.0);
}

/* Get the kurtosis of the distribution */
NumericalPoint Arcsine::getKurtosis() const /*throw(NotDefinedException)*/
{
  NumericalScalar standardDeviation4(getSigma());
  standardDeviation4 *= standardDeviation4;
  standardDeviation4 *= standardDeviation4;
  NumericalScalar a4(0.5 * (b_ - a_) * 0.5 * (b_ - a_));
  a4 *= a4;
  return NumericalPoint(1, (3.0 / 8.0) * a4 / standardDeviation4);
}

/* Get the moments of the standardized distribution */
NumericalPoint Arcsine::getStandardMoment(const UnsignedInteger n) const
{
  if (n % 2 == 1) return NumericalPoint(1, 0.0);
  return NumericalPoint(1, std::exp(SpecFunc::LogGamma(0.5 * n + 0.5) - SpecFunc::LogGamma(0.5 * n + 1)) / std::sqrt(M_PI));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Arcsine::Implementation Arcsine::getStandardRepresentative() const
{
  return Arcsine(-1.0, 1.0).clone();
}

/* Compute the covariance of the distribution */
void Arcsine::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const NumericalScalar eta(getSigma());
  covariance_(0, 0) = eta * eta;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
Arcsine::NumericalPointWithDescriptionCollection Arcsine::getParametersCollection() const
{
  NumericalPointWithDescriptionCollection parameters(1);
  NumericalPointWithDescription point(2);
  Description description(point.getDimension());
  point[0] = a_;
  point[1] = b_;
  description[0] = "a";
  description[1] = "b";
  point.setDescription(description);
  point.setName(getDescription()[0]);
  parameters[0] = point;
  return parameters;
}

void Arcsine::setParametersCollection(const NumericalPointCollection & parametersCollection)
{
  const NumericalScalar w(getWeight());
  *this = Arcsine(parametersCollection[0][0], parametersCollection[0][1]);
  setWeight(w);
}


/* A accessor */
void Arcsine::setA(const NumericalScalar a)
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

NumericalScalar Arcsine::getA() const
{
  return a_;
}


/* B accessor */
void Arcsine::setB(const NumericalScalar b)
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

NumericalScalar Arcsine::getB() const
{
  return b_;
}

void Arcsine::setAB(const NumericalScalar a,
                    const NumericalScalar b)
{
  if (a >= b)
    throw InvalidArgumentException(HERE) << "in Arcsine : a must be smaller than b";
  a_ = a;
  b_ = b;
  computeRange();
}

/* Mu accessor */
void Arcsine::setMu(const NumericalScalar mu)
{
  const NumericalScalar sigma(getSigma());
  setAB(mu - sigma * M_SQRT1_2, mu + sigma * M_SQRT1_2);
}

NumericalScalar Arcsine::getMu() const
{
  return 0.5 * (a_ + b_);
}


/* Sigma accessor */
void Arcsine::setSigma(const NumericalScalar sigma)
{
  const NumericalScalar mu(getMu());
  setAB(mu - sigma * M_SQRT2, mu + sigma * M_SQRT2);
}

NumericalScalar Arcsine::getSigma() const
{
  return 0.5 * (b_ - a_) * M_SQRT1_2;
}

void Arcsine::setMuSigma(const NumericalScalar mu,
                         const NumericalScalar sigma)
{
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "in Arcsine : sigma must be positive";
  setAB(mu - sigma * std::sqrt(2.0), mu + sigma * std::sqrt(2.0));
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
