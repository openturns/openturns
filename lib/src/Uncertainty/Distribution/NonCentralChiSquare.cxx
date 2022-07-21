//                                               -*- C++ -*-
/**
 *  @brief The NonCentralChiSquare distribution
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/NonCentralChiSquare.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NonCentralChiSquare)

static const Factory<NonCentralChiSquare> Factory_NonCentralChiSquare;


/* Default constructor */
NonCentralChiSquare::NonCentralChiSquare(const Scalar nu,
    const Scalar lambda)
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

Bool NonCentralChiSquare::equals(const DistributionImplementation & other) const
{
  const NonCentralChiSquare* p_other = dynamic_cast<const NonCentralChiSquare*>(&other);
  return p_other && (*this == *p_other);
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

String NonCentralChiSquare::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(nu = " << nu_ << ", lambda = " << lambda_ << ")";
  return oss;
}

/* Virtual constructor */
NonCentralChiSquare * NonCentralChiSquare::clone() const
{
  return new NonCentralChiSquare(*this);
}

/* Get one realization of the distribution */
Point NonCentralChiSquare::getRealization() const
{
  return DistFunc::rNonCentralChiSquare(nu_, lambda_, 1);
}

/* Get the PDF of the distribution */
Scalar NonCentralChiSquare::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return DistFunc::dNonCentralChiSquare(nu_, lambda_, point[0], pdfEpsilon_, maximumIteration_);
}

/* Get the CDF of the distribution */
Scalar NonCentralChiSquare::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if (point[0] <= 0.0) return 0.0;
  return DistFunc::pNonCentralChiSquare(nu_, lambda_, point[0], false, cdfEpsilon_, maximumIteration_);
}

/** Get the PDFGradient of the distribution */
Point NonCentralChiSquare::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Scalar eps = std::pow(pdfEpsilon_, 1.0 / 3.0);
  Point pdfGradient(2);
  pdfGradient[0] = (DistFunc::dNonCentralChiSquare(nu_ + eps, lambda_, point[0], pdfEpsilon_, maximumIteration_) - DistFunc::dNonCentralChiSquare(nu_ - eps, lambda_, point[0], pdfEpsilon_, maximumIteration_)) / (2.0 * eps);
  pdfGradient[1] = (DistFunc::dNonCentralChiSquare(nu_, lambda_ + eps, point[0], pdfEpsilon_, maximumIteration_) - DistFunc::dNonCentralChiSquare(nu_, lambda_ - eps, point[0], pdfEpsilon_, maximumIteration_)) / (2.0 * eps);
  return pdfGradient;
}

/** Get the CDFGradient of the distribution */
Point NonCentralChiSquare::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Scalar eps = std::pow(cdfEpsilon_, 1.0 / 3.0);
  Point cdfGradient(2);
  cdfGradient[0] = (DistFunc::pNonCentralChiSquare(nu_ + eps, lambda_, point[0], false, cdfEpsilon_, maximumIteration_) - DistFunc::pNonCentralChiSquare(nu_ - eps, lambda_, point[0], false, cdfEpsilon_, maximumIteration_)) / (2.0 * eps);
  cdfGradient[1] = (DistFunc::pNonCentralChiSquare(nu_, lambda_ + eps, point[0], false, cdfEpsilon_, maximumIteration_) - DistFunc::pNonCentralChiSquare(nu_, lambda_ - eps, point[0], false, cdfEpsilon_, maximumIteration_)) / (2.0 * eps);
  return cdfGradient;
}

/* Compute the mean of the distribution */
void NonCentralChiSquare::computeMean() const
{
  mean_ = Point(1, nu_ + lambda_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point NonCentralChiSquare::getStandardDeviation() const
{
  return Point(1, std::sqrt(2.0 * (nu_ + 2.0 * lambda_)));
}

/* Get the skewness of the distribution */
Point NonCentralChiSquare::getSkewness() const
{
  if (nu_ == 0.0) throw NotDefinedException(HERE) << "Error: the skewness is not defined for nu=0.";
  if (lambda_ == 0.0) return Point(1, 2.0 * M_SQRT2 / std::sqrt(nu_));
  return Point(1, (nu_ + 3.0 * lambda_) * std::pow(2.0 / (nu_ + 2.0 * lambda_), 1.5));
}

/* Get the kurtosis of the distribution */
Point NonCentralChiSquare::getKurtosis() const
{
  if (nu_ == 0.0) throw NotDefinedException(HERE) << "Error: the kurtosis is not defined for nu=0.";
  if (lambda_ == 0.0) return Point(1, 3.0 + 12.0 / nu_);
  return Point(1, 3.0 + 12.0 * (nu_ + 4.0 * lambda_) / std::pow(nu_ + 2.0 * lambda_, 2.0));
}

/* Get the moments of the standardized distribution */
Point NonCentralChiSquare::getStandardMoment(const UnsignedInteger n) const
{
  UniVariatePolynomial p(Point(1, 1.0));
  Point derivativeFactor(3);
  derivativeFactor[0] = 1.0;
  derivativeFactor[1] = -4.0;
  derivativeFactor[2] = 4.0;
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    Point polynomialFactor(2);
    polynomialFactor[0] = lambda_ + 4.0 * k + nu_;
    polynomialFactor[1] = -8.0 * k - 2.0 * nu_;
    p = p.derivate() * UniVariatePolynomial(derivativeFactor) + p * UniVariatePolynomial(polynomialFactor);
  }
  return Point(1, p.getCoefficients()[0]);
}

/* Compute the covariance of the distribution */
void NonCentralChiSquare::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) =  2.0 * (nu_ + 2.0 * lambda_);
  isAlreadyComputedCovariance_ = true;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex NonCentralChiSquare::computeCharacteristicFunction(const Scalar x) const
{
  return std::exp(computeLogCharacteristicFunction(x));
}

Complex NonCentralChiSquare::computeLogCharacteristicFunction(const Scalar x) const
{
  const Complex denominator(1.0, -2.0 * x);
  return Complex(0.0, lambda_ * x) / denominator - 0.5 * nu_ * std::log(denominator);
}

Point NonCentralChiSquare::getParameter() const
{
  return {nu_, lambda_};
}

void NonCentralChiSquare::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = NonCentralChiSquare(parameter[0], parameter[1]);
  setWeight(w);
}

Description NonCentralChiSquare::getParameterDescription() const
{
  return {"nu", "lambda"};
}

/* Nu accessor */
void NonCentralChiSquare::setNu(const Scalar nu)
{
  if (!(nu >= 0.0)) throw InvalidArgumentException(HERE) << "Nu MUST be strictly positive";
  if (nu != nu_)
  {
    nu_ = nu;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

void NonCentralChiSquare::setNuLambda(const Scalar nu,
                                      const Scalar lambda)
{
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Nu MUST be positive";
  if (!(lambda >= 0.0)) throw InvalidArgumentException(HERE) << "Lambda MUST be nonnegative";
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
Scalar NonCentralChiSquare::getNu() const
{
  return nu_;
}


/* Lambda accessor */
void NonCentralChiSquare::setLambda(const Scalar lambda)
{
  if (!(lambda >= 0.0)) throw InvalidArgumentException(HERE) << "Lambda MUST be nonnegative";
  if (lambda != lambda_)
  {
    lambda_ = lambda;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Lambda accessor */
Scalar NonCentralChiSquare::getLambda() const
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
