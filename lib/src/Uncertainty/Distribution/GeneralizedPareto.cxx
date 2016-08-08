//                                               -*- C++ -*-
/**
 *  @brief The GeneralizedPareto distribution
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
#include "openturns/GeneralizedPareto.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedPareto);

static const Factory<GeneralizedPareto> Factory_GeneralizedPareto;

/* Default constructor */
GeneralizedPareto::GeneralizedPareto()
  : ContinuousDistribution()
  , sigma_(1.0)
  , xi_(0.0)
{
  setName( "GeneralizedPareto" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
GeneralizedPareto::GeneralizedPareto(const NumericalScalar sigma,
                                     const NumericalScalar xi)
  : ContinuousDistribution()
  , sigma_(0.0)
  , xi_(xi)
{
  setName( "GeneralizedPareto" );
  // We set the dimension of the GeneralizedPareto distribution
  setDimension( 1 );
  // This call set also the range.
  setSigma(sigma);
}

/* Comparison operator */
Bool GeneralizedPareto::operator ==(const GeneralizedPareto & other) const
{
  if (this == &other) return true;
  return (sigma_ == other.sigma_) && (xi_ == other.xi_);
}

Bool GeneralizedPareto::equals(const DistributionImplementation & other) const
{
  const GeneralizedPareto* p_other = dynamic_cast<const GeneralizedPareto*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String GeneralizedPareto::__repr__() const
{
  OSS oss;
  oss << "class=" << GeneralizedPareto::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " sigma=" << sigma_
      << " xi=" << xi_;
  return oss;
}

String GeneralizedPareto::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(sigma = " << sigma_ << ", xi=" << xi_ << ")";
  return oss;
}

/* Virtual constructor */
GeneralizedPareto * GeneralizedPareto::clone() const
{
  return new GeneralizedPareto(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void GeneralizedPareto::computeRange()
{
  const NumericalPoint lowerBound(1, 0.0);
  const Interval::BoolCollection finiteLowerBound(1, true);
  NumericalPoint upperBound(1, 0.0);
  Interval::BoolCollection finiteUpperBound(1, false);
  if (xi_ >= 0.0) upperBound[0] = computeScalarQuantile(1.0 - SpecFunc::Precision);
  else
  {
    upperBound[0] = -sigma_ / xi_;
    finiteUpperBound[0] = true;
  }
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution */
NumericalPoint GeneralizedPareto::getRealization() const
{
  const NumericalScalar u = RandomGenerator::Generate();
  if (xi_ == 0.0) return NumericalPoint(1, -sigma_ * std::log(u));
  return NumericalPoint(1, sigma_ * expm1(-xi_ * std::log(u)) / xi_);
}


/* Get the DDF of the distribution */
NumericalPoint GeneralizedPareto::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const NumericalScalar z = point[0] / sigma_;
  NumericalPoint result(1);
  if (z < 0.0) return result;
  if (std::abs(std::sqrt(std::abs(xi_)) * z) < 1.0e-8)
  {
    result[0] = -0.5 * std::exp(-z) * (1.0 + xi_ * z * (0.5 * z - 1.0)) / (sigma_ * sigma_);
    return result;
  }
  if ((xi_ < 0.0) && (z >= -1.0 / xi_)) return result;
  result[0] = -(1.0 + xi_) * std::exp(-(2.0 + 1.0 / xi_) * log1p(xi_ * z)) / (sigma_ * sigma_);
  return result;
}


/* Get the PDF of the distribution */
NumericalScalar GeneralizedPareto::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const NumericalScalar z = point[0] / sigma_;
  if (z < 0.0) return 0.0;
  if (std::abs(z) * std::sqrt(std::abs(xi_)) < 1.0e-8) return std::exp(-z) * (1.0 + z * xi_ * (0.5 * z - 1.0)) / sigma_;
  if ((xi_ < 0.0) && (z >= -1.0 / xi_)) return 0.0;
  return std::exp(-(1.0 + 1.0 / xi_) * log1p(xi_ * z)) / sigma_;
}

NumericalScalar GeneralizedPareto::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const NumericalScalar z = point[0] / sigma_;
  if (z < 0.0) return -SpecFunc::MaxNumericalScalar;
  if (std::abs(std::sqrt(std::abs(xi_)) * z) < 1.0e-8) return -z + log1p(z * xi_ * (0.5 * z - 1.0)) - std::log(sigma_);
  if ((xi_ < 0.0) && (z >= -1.0 / xi_)) return -SpecFunc::MaxNumericalScalar;
  return -(1.0 + 1.0 / xi_) * log1p(xi_ * z) - std::log(sigma_);
}

/* Get the CDF of the distribution */
NumericalScalar GeneralizedPareto::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const NumericalScalar z = point[0] / sigma_;
  if (z <= 0.0) return 0.0;
  if (std::abs(std::sqrt(xi_) * z) < 1.0e-8) return -expm1(-z) - 0.5 * xi_ * z * z * std::exp(-z);
  if ((xi_ < 0.0) && (z > -1.0 / xi_)) return 1.0;
  return -expm1(-log1p(xi_ * z) / xi_);
}

/* Get the complementary CDF of the distribution */
NumericalScalar GeneralizedPareto::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const NumericalScalar z = point[0] / sigma_;
  if (z <= 0.0) return 1.0;
  if (std::abs(std::sqrt(xi_) * z) < 1.0e-8) return std::exp(-z) * (1.0 + 0.5 * xi_ * z * z);
  if ((xi_ < 0.0) && (z > -1.0 / xi_)) return 0.0;
  return std::exp(-log1p(xi_ * z) / xi_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex GeneralizedPareto::computeCharacteristicFunction(const NumericalScalar x) const
{
  if (xi_ == 0.0) return 1.0 / NumericalComplex(1.0, -x);
  return DistributionImplementation::computeCharacteristicFunction(x);
}

NumericalComplex GeneralizedPareto::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  if (xi_ == 0.0) return -std::log(NumericalComplex(1.0, -x));
  return std::log(computeCharacteristicFunction(x));
}

/* Get the PDFGradient of the distribution */
NumericalPoint GeneralizedPareto::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  NumericalPoint result(2);
  const NumericalScalar epsilon = 1.0e-5;
  result[0] = (GeneralizedPareto(sigma_ + epsilon, xi_).computePDF(point) - GeneralizedPareto(sigma_ - epsilon, xi_).computePDF(point)) / (2.0 * epsilon);
  result[1] = (GeneralizedPareto(sigma_, xi_ + epsilon).computePDF(point) - GeneralizedPareto(sigma_, xi_ - epsilon).computePDF(point)) / (2.0 * epsilon);
  return result;
}

/* Get the CDFGradient of the distribution */
NumericalPoint GeneralizedPareto::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalPoint result(2);
  const NumericalScalar epsilon = 1.0e-5;
  result[0] = (GeneralizedPareto(sigma_ + epsilon, xi_).computeCDF(point) - GeneralizedPareto(sigma_ - epsilon, xi_).computeCDF(point)) / (2.0 * epsilon);
  result[1] = (GeneralizedPareto(sigma_, xi_ + epsilon).computeCDF(point) - GeneralizedPareto(sigma_, xi_ - epsilon).computeCDF(point)) / (2.0 * epsilon);
  return result;
}

/* Get the quantile of the distribution */
NumericalScalar GeneralizedPareto::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if (xi_ == 0.0) return -sigma_ * (tail ? std::log(prob) : log1p(-prob));
  return sigma_ * expm1(-xi_ * (tail ? std::log(-prob) : log1p(-prob))) / xi_;
}

/* Compute the mean of the distribution */
void GeneralizedPareto::computeMean() const
{
  if (xi_ >= 1.0) throw NotDefinedException(HERE) << "Error: the mean is defined only for xi<1, here xi=" << xi_;
  mean_ = NumericalPoint(1, sigma_ / (1.0 - xi_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint GeneralizedPareto::getStandardDeviation() const
{
  return NumericalPoint(1, std::sqrt(getCovariance()(0, 0)));
}

/* Get the skewness of the distribution */
NumericalPoint GeneralizedPareto::getSkewness() const
{
  if (xi_ >= 1.0 / 3.0) throw NotDefinedException(HERE) << "Error: the skewness is defined only for xi<1/3, here xi=" << xi_;
  return NumericalPoint(1, 2.0 * (1.0 + xi_) * std::sqrt(1.0 - 2.0 * xi_) / (1.0 - 3.0 * xi_));
}

/* Get the kurtosis of the distribution */
NumericalPoint GeneralizedPareto::getKurtosis() const
{
  if (xi_ >= 1.0 / 4.0) throw NotDefinedException(HERE) << "Error: the kurtosis is defined only for xi<1/4, here xi=" << xi_;
  return NumericalPoint(1, 3.0 * (1.0 - 2.0 * xi_) * (3.0 + xi_ * (1.0 + 2.0 * xi_)) / ((1.0 - 4.0 * xi_) * (1.0 - 3.0 * xi_)));
}

/* Get the moments of the standardized distribution */
NumericalPoint GeneralizedPareto::getStandardMoment(const UnsignedInteger n) const
{
  if (n == 0) return NumericalPoint(1, 1.0);
  if (xi_ == 0.0) return NumericalPoint(1, SpecFunc::Gamma(n + 1.0));
  if (xi_ > 0.0)
  {
    if (n < trunc(1.0 / xi_)) return NumericalPoint(1, std::exp(-std::log(xi_) * (n + 1.0) + SpecFunc::LogGamma(1.0 / xi_ - n) + SpecFunc::LogGamma(n + 1.0) - SpecFunc::LogGamma(1.0 + 1.0 / xi_)));
    throw NotDefinedException(HERE) << "Error: the standard moments are defined up to order " << trunc(1.0 / xi_) << " and n=" << n;
  }
  return NumericalPoint(1, std::exp(-(n + 1.0) * std::log(-xi_) + SpecFunc::LogGamma(-1.0 / xi_) + SpecFunc::LogGamma(n + 1.0) - SpecFunc::LogGamma(-1.0 / xi_ + n + 1.0)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
GeneralizedPareto::Implementation GeneralizedPareto::getStandardRepresentative() const
{
  return GeneralizedPareto(1.0, xi_).clone();
}

/* Compute the covariance of the distribution */
void GeneralizedPareto::computeCovariance() const
{
  if (xi_ >= 0.5) throw NotDefinedException(HERE) << "Error: the covariance is defined only for xi<1/2, here xi=" << xi_;
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = sigma_ * sigma_ / ((1.0 - 2.0 * xi_) * (1.0 - xi_) * (1.0 - xi_));
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value  accessor */
NumericalPoint GeneralizedPareto::getParameter() const
{
  NumericalPoint point(2);
  point[0] = sigma_;
  point[1] = xi_;
  return point;
}

void GeneralizedPareto::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const NumericalScalar w = getWeight();
  *this = GeneralizedPareto(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description GeneralizedPareto::getParameterDescription() const
{
  Description description(2);
  description[0] = "sigma";
  description[1] = "xi";
  return description;
}

/* Sigma accessor */
void GeneralizedPareto::setSigma(const NumericalScalar sigma)
{
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "Sigma MUST be positive";
  if (sigma != sigma_)
  {
    sigma_ = sigma;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Sigma accessor */
NumericalScalar GeneralizedPareto::getSigma() const
{
  return sigma_;
}

/* Xi accessor */
void GeneralizedPareto::setXi(const NumericalScalar xi)
{
  if (xi != xi_)
  {
    xi_ = xi;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    // The mean does not depend on xi
    computeRange();
  }
}

/* Xi accessor */
NumericalScalar GeneralizedPareto::getXi() const
{
  return xi_;
}

/* Method save() stores the object through the StorageManager */
void GeneralizedPareto::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "sigma_", sigma_ );
  adv.saveAttribute( "xi_", xi_ );
}

/* Method load() reloads the object from the StorageManager */
void GeneralizedPareto::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "sigma_", sigma_ );
  adv.loadAttribute( "xi_", xi_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS
