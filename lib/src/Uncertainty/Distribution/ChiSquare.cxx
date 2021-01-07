//                                               -*- C++ -*-
/**
 *  @brief The ChiSquare distribution, ie the Gamma(nu/2,1/2) distribution
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
#include "openturns/ChiSquare.hxx"
#include "openturns/Gamma.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ChiSquare)

static const Factory<ChiSquare> Factory_ChiSquare;

/* Default constructor */
ChiSquare::ChiSquare()
  : ContinuousDistribution()
  , nu_(0.0)
  , normalizationFactor_(0.0)
{
  setName("ChiSquare");
  // This call also call update() and computeRange()
  setNu(1.0);
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
ChiSquare::ChiSquare(const Scalar nu)
  : ContinuousDistribution()
  , nu_(0.0)
  , normalizationFactor_(0.0)
{
  setName("ChiSquare");
  // This call set also call computeRange() and update().
  setNu(nu);
  setDimension(1);
}

/* Comparison operator */
Bool ChiSquare::operator ==(const ChiSquare & other) const
{
  if (this == &other) return true;
  return nu_ == other.nu_;
}

Bool ChiSquare::equals(const DistributionImplementation & other) const
{
  const ChiSquare* p_other = dynamic_cast<const ChiSquare*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String ChiSquare::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ChiSquare::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " nu=" << nu_;
  return oss;
}

String ChiSquare::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(nu = " << nu_ << ")";
  return oss;
}

/* Nu accessor */
void ChiSquare::setNu(const Scalar nu)
{
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Nu MUST be positive";
  if (nu != nu_)
  {
    nu_ = nu;
    computeRange();
    update();
  }
}

Scalar ChiSquare::getNu() const
{
  return nu_;
}


/* Virtual constructor */
ChiSquare * ChiSquare::clone() const
{
  return new ChiSquare(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void ChiSquare::computeRange()
{
  setRange(Gamma(0.5, 0.5 * nu_, 0.0).getRange());
}

/** Update the derivative attributes */
void ChiSquare::update()
{
  normalizationFactor_ = -0.5 * nu_ * M_LN2 - SpecFunc::LnGamma(0.5 * nu_);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
}


/* Get one realization of the distribution */
Point ChiSquare::getRealization() const
{
  return Point(1, 2.0 * DistFunc::rGamma(0.5 * nu_));
}


/* Get the DDF of the distribution */
Point ChiSquare::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (x <= 0.0) return Point(1, 0.0);
  return Point(1, ((0.5 * nu_ - 1.0) / x - 0.5) * computePDF(point));
}


/* Get the PDF of the distribution */
Scalar ChiSquare::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (x <= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

Scalar ChiSquare::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (x <= 0.0) return SpecFunc::LowestScalar;
  return normalizationFactor_ + (0.5 * nu_ - 1) * std::log(x) - 0.5 * x;
}

/* Get the CDF of the distribution */
Scalar ChiSquare::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  // No test here as the CDF is continuous for all nu_
  if (x <= 0.0) return 0.0;
  return DistFunc::pGamma(0.5 * nu_, 0.5 * x);
}

Scalar ChiSquare::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  // No test here as the CDF is continuous for all nu_
  if (x <= 0.0) return 1.0;
  return DistFunc::pGamma(0.5 * nu_, 0.5 * x, true);
}

/* Compute the entropy of the distribution */
Scalar ChiSquare::computeEntropy() const
{
  return 0.5 * nu_ + M_LN2 + SpecFunc::LogGamma(0.5 * nu_) + (1.0 - 0.5 * nu_) * SpecFunc::Psi(0.5 * nu_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex ChiSquare::computeCharacteristicFunction(const Scalar x) const
{
  return std::pow(Complex(1.0, -2.0 * x), -0.5 * nu_);
}

Complex ChiSquare::computeLogCharacteristicFunction(const Scalar x) const
{
  return -0.5 * nu_ * std::log(Complex(1.0, -2.0 * x));
}

/* Get the PDFGradient of the distribution */
Point ChiSquare::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point pdfGradient(1, 0.0);
  const Scalar x = point[0];
  if (x <= 0.0) return pdfGradient;
  Scalar pdf = computePDF(point);
  pdfGradient[0] = 0.5 * (std::log(0.5 * x) - SpecFunc::Psi(0.5 * nu_)) * pdf;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point ChiSquare::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point cdfGradient(1, 0.0);
  const Scalar x = point[0];
  if (x <= 0.0) return cdfGradient;
  Scalar eps = std::pow(cdfEpsilon_, 1.0 / 3.0);
  cdfGradient[0] = (DistFunc::pGamma(0.5 * (nu_ + eps), 0.5 * x) - DistFunc::pGamma(0.5 * (nu_ - eps), 0.5 * x)) / (2.0 * eps);
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar ChiSquare::computeScalarQuantile(const Scalar prob,
                                        const Bool tail) const
{
  return 2.0 * DistFunc::qGamma(0.5 * nu_, prob, tail);
}

/* Compute the mean of the distribution */
void ChiSquare::computeMean() const
{
  mean_ = Point(1, nu_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point ChiSquare::getStandardDeviation() const
{
  return Point(1, std::sqrt(2.0 * nu_));
}

/* Get the skewness of the distribution */
Point ChiSquare::getSkewness() const
{
  return Point(1, std::sqrt(8.0 / nu_));
}

/* Get the kurtosis of the distribution */
Point ChiSquare::getKurtosis() const
{
  return Point(1, 3.0 + 12.0 / nu_);
}

/* Compute the covariance of the distribution */
void ChiSquare::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = 2.0 * nu_;
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
Point ChiSquare::getStandardMoment(const UnsignedInteger n) const
{
  return Point(1, std::exp(n * M_LN2 + SpecFunc::LnGamma(n + 0.5 * nu_) - SpecFunc::LnGamma(0.5 * nu_)));
}

/* Parameters value accessor */
Point ChiSquare::getParameter() const
{
  return Point(1, nu_);
}

void ChiSquare::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = ChiSquare(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description ChiSquare::getParameterDescription() const
{
  return Description(1, "nu");
}

/* Method save() stores the object through the StorageManager */
void ChiSquare::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "nu_", nu_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void ChiSquare::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "nu_", nu_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  computeRange();
}




END_NAMESPACE_OPENTURNS
