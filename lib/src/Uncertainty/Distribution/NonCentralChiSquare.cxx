//                                               -*- C++ -*-
/**
 *  @brief The NonCentralChiSquare distribution
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
#include "openturns/NonCentralChiSquare.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NonCentralChiSquare);

static const Factory<NonCentralChiSquare> RegisteredFactory;


/* Default constructor */
NonCentralChiSquare::NonCentralChiSquare(const NumericalScalar nu,
    const NumericalScalar lambda)
  : ContinuousDistribution()
  , nu_(0.0)
  , lambda_(0.0)
  , maximumIteration_(ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"))
{
  setName("NonCentralChiSquare");
  // This call set also the range.
  setNuLambda(nu, lambda);
  setDimension(1);
}

/* Comparison operator */
Bool NonCentralChiSquare::operator ==(const NonCentralChiSquare & other) const
{
  if (this == &other) return true;
  return (nu_ == other.nu_) && (lambda_ == other.lambda_);
}

/* String converter */
String NonCentralChiSquare::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NonCentralChiSquare::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " nu=" << nu_
      << " lambda=" << lambda_;
  return oss;
}

String NonCentralChiSquare::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(nu = " << nu_ << ", lambda = " << lambda_ << ")";
  return oss;
}

/* Virtual constructor */
NonCentralChiSquare * NonCentralChiSquare::clone() const
{
  return new NonCentralChiSquare(*this);
}

/* Get one realization of the distribution */
NumericalPoint NonCentralChiSquare::getRealization() const
{
  return DistFunc::rNonCentralChiSquare(nu_, lambda_, 1);
}

/* Get the PDF of the distribution */
NumericalScalar NonCentralChiSquare::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return DistFunc::dNonCentralChiSquare(nu_, lambda_, point[0], pdfEpsilon_, maximumIteration_);
}

/* Get the CDF of the distribution */
NumericalScalar NonCentralChiSquare::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if (point[0] <= 0.0) return 0.0;
  return DistFunc::pNonCentralChiSquare(nu_, lambda_, point[0], false, cdfEpsilon_, maximumIteration_);
}

/** Get the PDFGradient of the distribution */
NumericalPoint NonCentralChiSquare::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalScalar eps(std::pow(pdfEpsilon_, 1.0 / 3.0));
  NumericalPoint pdfGradient(2);
  pdfGradient[0] = (DistFunc::dNonCentralChiSquare(nu_ + eps, lambda_, point[0], pdfEpsilon_, maximumIteration_) - DistFunc::dNonCentralChiSquare(nu_ - eps, lambda_, point[0], pdfEpsilon_, maximumIteration_)) / (2.0 * eps);
  pdfGradient[1] = (DistFunc::dNonCentralChiSquare(nu_, lambda_ + eps, point[0], pdfEpsilon_, maximumIteration_) - DistFunc::dNonCentralChiSquare(nu_, lambda_ - eps, point[0], pdfEpsilon_, maximumIteration_)) / (2.0 * eps);
  return pdfGradient;
}

/** Get the CDFGradient of the distribution */
NumericalPoint NonCentralChiSquare::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalScalar eps(std::pow(cdfEpsilon_, 1.0 / 3.0));
  NumericalPoint cdfGradient(2);
  cdfGradient[0] = (DistFunc::pNonCentralChiSquare(nu_ + eps, lambda_, point[0], false, cdfEpsilon_, maximumIteration_) - DistFunc::pNonCentralChiSquare(nu_ - eps, lambda_, point[0], false, cdfEpsilon_, maximumIteration_)) / (2.0 * eps);
  cdfGradient[1] = (DistFunc::pNonCentralChiSquare(nu_, lambda_ + eps, point[0], false, cdfEpsilon_, maximumIteration_) - DistFunc::pNonCentralChiSquare(nu_, lambda_ - eps, point[0], false, cdfEpsilon_, maximumIteration_)) / (2.0 * eps);
  return cdfGradient;
}

/* Compute the mean of the distribution */
void NonCentralChiSquare::computeMean() const
{
  mean_ = NumericalPoint(1, nu_ + lambda_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint NonCentralChiSquare::getStandardDeviation() const
{
  return NumericalPoint(1, std::sqrt(2.0 * (nu_ + 2.0 * lambda_)));
}

/* Get the skewness of the distribution */
NumericalPoint NonCentralChiSquare::getSkewness() const
{
  if (nu_ == 0.0) throw NotDefinedException(HERE) << "Error: the skewness is not defined for nu=0.";
  if (lambda_ == 0.0) return NumericalPoint(1, 2.0 * M_SQRT2 / std::sqrt(nu_));
  return NumericalPoint(1, (nu_ + 3.0 * lambda_) * std::pow(2.0 / (nu_ + 2.0 * lambda_), 1.5));
}

/* Get the kurtosis of the distribution */
NumericalPoint NonCentralChiSquare::getKurtosis() const
{
  if (nu_ == 0.0) throw NotDefinedException(HERE) << "Error: the kurtosis is not defined for nu=0.";
  if (lambda_ == 0.0) return NumericalPoint(1, 3.0 + 12.0 / nu_);
  return NumericalPoint(1, 3.0 + 12.0 * (nu_ + 4.0 * lambda_) / std::pow(nu_ + 2.0 * lambda_, 2.0));
}

/* Get the moments of the standardized distribution */
NumericalPoint NonCentralChiSquare::getStandardMoment(const UnsignedInteger n) const
{
  UniVariatePolynomial p(NumericalPoint(1, 1.0));
  NumericalPoint derivativeFactor(3);
  derivativeFactor[0] = 1.0;
  derivativeFactor[1] = -4.0;
  derivativeFactor[2] = 4.0;
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    NumericalPoint polynomialFactor(2);
    polynomialFactor[0] = lambda_ + 4.0 * k + nu_;
    polynomialFactor[1] = -8.0 * k - 2.0 * nu_;
    p = p.derivate() * UniVariatePolynomial(derivativeFactor) + p * UniVariatePolynomial(polynomialFactor);
  }
  return NumericalPoint(1, p.getCoefficients()[0]);
}

/* Compute the covariance of the distribution */
void NonCentralChiSquare::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) =  2.0 * (nu_ + 2.0 * lambda_);
  isAlreadyComputedCovariance_ = true;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex NonCentralChiSquare::computeCharacteristicFunction(const NumericalScalar x) const
{
  return std::exp(computeLogCharacteristicFunction(x));
}

NumericalComplex NonCentralChiSquare::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  const NumericalComplex denominator(1.0, -2.0 * x);
  return NumericalComplex(0.0, lambda_ * x) / denominator - 0.5 * nu_ * std::log(denominator);
}

NumericalPoint NonCentralChiSquare::getParameter() const
{
  NumericalPoint point(2);
  point[0] = nu_;
  point[1] = lambda_;
  return point;
}

void NonCentralChiSquare::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = NonCentralChiSquare(parameter[0], parameter[1]);
  setWeight(w);
}

Description NonCentralChiSquare::getParameterDescription() const
{
  Description description(2);
  description[0] = "nu";
  description[1] = "lambda";
  return description;
}

/* Nu accessor */
void NonCentralChiSquare::setNu(const NumericalScalar nu)
{
  if (nu < 0.0) throw InvalidArgumentException(HERE) << "Nu MUST be strictly positive";
  if (nu != nu_)
  {
    nu_ = nu;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

void NonCentralChiSquare::setNuLambda(const NumericalScalar nu,
                                      const NumericalScalar lambda)
{
  if (nu <= 0.0) throw InvalidArgumentException(HERE) << "Nu MUST be positive";
  if (lambda < 0.0) throw InvalidArgumentException(HERE) << "Lambda MUST be nonnegative";
  if ((nu != nu_) || (lambda != lambda_))
  {
    nu_ = nu;
    lambda_ = lambda;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Nu accessor */
NumericalScalar NonCentralChiSquare::getNu() const
{
  return nu_;
}


/* Lambda accessor */
void NonCentralChiSquare::setLambda(const NumericalScalar lambda)
{
  if (lambda < 0.0) throw InvalidArgumentException(HERE) << "Lambda MUST be nonnegative";
  if (lambda != lambda_)
  {
    lambda_ = lambda;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Lambda accessor */
NumericalScalar NonCentralChiSquare::getLambda() const
{
  return lambda_;
}

/* Maximum iterations accessor */
void NonCentralChiSquare::setMaximumIteration(const UnsignedInteger maximumIteration)
{
  maximumIteration_ = maximumIteration;
}

UnsignedInteger NonCentralChiSquare::getMaximumIteration() const
{
  return maximumIteration_;
}

/* Method save() stores the object through the StorageManager */
void NonCentralChiSquare::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "nu_", nu_ );
  adv.saveAttribute( "lambda_", lambda_ );
  adv.saveAttribute( "maximumIteration_", maximumIteration_ );
}

/* Method load() reloads the object from the StorageManager */
void NonCentralChiSquare::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "nu_", nu_ );
  adv.loadAttribute( "lambda_", lambda_ );
  adv.loadAttribute( "maximumIteration_", maximumIteration_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
