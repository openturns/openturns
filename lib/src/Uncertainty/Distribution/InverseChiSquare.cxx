//                                               -*- C++ -*-
/**
 *  @brief The InverseChiSquare distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/InverseChiSquare.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseChiSquare)

static const Factory<InverseChiSquare> Factory_InverseChiSquare;

/* Default constructor */
InverseChiSquare::InverseChiSquare()
  : ContinuousDistribution()
  , nu_(1.0)
  , normalizationFactor_(0.0)
{
  setName("InverseChiSquare");
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
InverseChiSquare::InverseChiSquare(const Scalar nu)
  : ContinuousDistribution()
  , nu_(0.0)
  , normalizationFactor_(0.0)
{
  setName("InverseChiSquare");
  setNu(nu);
  setDimension(1);
}

/* Comparison operator */
Bool InverseChiSquare::operator ==(const InverseChiSquare & other) const
{
  if (this == &other) return true;
  return nu_ == other.nu_;
}

Bool InverseChiSquare::equals(const DistributionImplementation & other) const
{
  const InverseChiSquare* p_other = dynamic_cast<const InverseChiSquare*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String InverseChiSquare::__repr__() const
{
  OSS oss;
  oss << "class=" << InverseChiSquare::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " nu=" << nu_;
  return oss;
}

String InverseChiSquare::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(nu = " << nu_ << ")";
  return oss;
}

/* K accessor */
void InverseChiSquare::setNu(const Scalar nu)
{
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Nu MUST be positive";
  if (nu != nu_)
  {
    nu_ = nu;
    computeRange();
    update();
  }
}

Scalar InverseChiSquare::getNu() const
{
  return nu_;
}

/* Virtual constructor */
InverseChiSquare * InverseChiSquare::clone() const
{
  return new InverseChiSquare(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void InverseChiSquare::computeRange()
{
  const Point lowerBound(1, 0.0);
  const Point upperBound(computeUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/** Update the derivative attributes */
void InverseChiSquare::update()
{
  // For large k we use the following normalization factor:
  // normalizationFactor = log(2*k^{k-1}/Gamma(k))
  //                     = log(2) + (k+1)log(k) - log(Gamma(k))
  // which is expanded wrt k
  const Scalar k = 0.5 * nu_;
  if (k >= 6.9707081224932495879)
  {
    static const Scalar alpha[10] = {0.91893853320467274177, 0.83333333333333333333e-1, -0.27777777777777777778e-2, 0.79365079365079365079e-3, -0.59523809523809523810e-3, 0.84175084175084175084e-3, -0.19175269175269175269e-2, 0.64102564102564102564e-2, -0.29550653594771241830e-1, 0.17964437236883057316};
    const Scalar ik = 1.0 / k;
    const Scalar ik2 = ik * ik;
    normalizationFactor_ = std::log(2.0) + k + 1.5 * std::log(k) - (alpha[0] + ik * (alpha[1] + ik2 * (alpha[2] + ik2 * (alpha[3] + ik2 * (alpha[4] + ik2 * (alpha[5] + ik2 * (alpha[6] + ik2 * (alpha[7] + ik2 * (alpha[8] + ik2 * alpha[9])))))))));
  }
  // For small k, the normalization factor is:
  // normalizationFactor = log(2/Gamma(k))
  //                     = log(2) - log(Gamma(k))
  else normalizationFactor_ = std::log(2.0) - SpecFunc::LnGamma(k);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
}


/* Get one realization of the distribution */
Point InverseChiSquare::getRealization() const
{
  return Point(1, 1.0 / (2.0 * DistFunc::rGamma(0.5 * nu_)));
}


/* Get the DDF of the distribution */
Point InverseChiSquare::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (x <= 0.0) return Point(1, 0.0);
  return Point(1, (1.0 / (2.0 * x) - (0.5 * nu_ + 1.0)) * computePDF(point) / x);
}


/* Get the PDF of the distribution */
Scalar InverseChiSquare::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if (point[0] <= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

Scalar InverseChiSquare::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  // From textbook, we have log(PDF(x)) =  log(lambda)-log(Gamma(k))-(k+1)*log(lambda*x)-1/(lambda*x)
  const Scalar u = 2.0 * point[0];
  if (u <= 0.0) return SpecFunc::LogMinScalar;
  // Use asymptotic expansion for large k
  // Here log(PDF(x)) = L - (k-1)*log(k)-(k+1)*log(2*x)-1/(2*x)
  const Scalar k = 0.5 * nu_;
  if (k >= 6.9707081224932495879) return normalizationFactor_ - (k + 1.0) * std::log(k * u) - 1.0 / u;
  return normalizationFactor_ - (k + 1.0) * std::log(u) - 1.0 / u;
}

/* Get the CDF of the distribution */
Scalar InverseChiSquare::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  // No test here as the CDF is continuous for all k
  if (x <= 0.0) return 0.0;
  return DistFunc::pGamma(0.5 * nu_, 0.5 / x, true);
}

Scalar InverseChiSquare::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  // No test here as the CDF is continuous for all k
  if (x <= 0.0) return 1.0;
  return DistFunc::pGamma(0.5 * nu_, 0.5 / x);
}

/* Compute the entropy of the distribution */
Scalar InverseChiSquare::computeEntropy() const
{
  return 0.5 * nu_ - M_LN2 + SpecFunc::LogGamma(0.5 * nu_) - (1.0 + 0.5 * nu_) * SpecFunc::Psi(0.5 * nu_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex InverseChiSquare::computeCharacteristicFunction(const Scalar x) const
{
  return DistributionImplementation::computeCharacteristicFunction(x);
}

Complex InverseChiSquare::computeLogCharacteristicFunction(const Scalar x) const
{
  return DistributionImplementation::computeLogCharacteristicFunction(x);
}

/* Get the PDFGradient of the distribution */
Point InverseChiSquare::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point pdfGradient(2);
  const Scalar x = point[0];
  if (x <= 0.0) return pdfGradient;
  const Scalar pdf = computePDF(point);
  pdfGradient[0] = -(std::log(2.0) + std::log(x) + SpecFunc::DiGamma(0.5 * nu_)) * pdf;
  pdfGradient[1] = 0.5 * (0.5 / x - nu_) * pdf;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point InverseChiSquare::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point cdfGradient(2, 0.0);
  const Scalar x = point[0];
  if (x <= 0.0) return cdfGradient;
  const Scalar lambdaXInverse = 0.5 / x;
  const Scalar pdf = computePDF(x);
  const Scalar eps = std::pow(cdfEpsilon_, 1.0 / 3.0);
  cdfGradient[0] = (DistFunc::pGamma(0.5 * nu_ + eps, lambdaXInverse, true) - DistFunc::pGamma(0.5 * nu_ - eps, lambdaXInverse, true)) / (2.0 * eps);
  cdfGradient[1] = 0.5 * pdf * x;
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar InverseChiSquare::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  return 0.5 / DistFunc::qGamma(0.5 * nu_, prob, !tail);
}

/* Compute the mean of the distribution */
void InverseChiSquare::computeMean() const
{
  if (!(nu_ > 2.0)) throw NotDefinedException(HERE) << "InverseChiSquare mean is defined only for nu > 2, here nu=" << nu_;
  mean_ = Point(1, 1.0 / (nu_ - 2.0));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point InverseChiSquare::getStandardDeviation() const
{
  if (!(nu_ > 4.0)) throw NotDefinedException(HERE) << "InverseChiSquare standard deviation is defined only for nu > 4, here nu=" << nu_;
  return Point(1, std::sqrt(getCovariance()(0, 0)));
}

/* Get the skewness of the distribution */
Point InverseChiSquare::getSkewness() const
{
  if (!(nu_ > 6.0)) throw NotDefinedException(HERE) << "InverseChiSquare skewness is defined only for nu > 6, here nu=" << nu_;
  return Point(1, 8.0 * std::sqrt(0.5 * nu_ - 2.0) / (nu_ - 6.0));
}

/* Get the kurtosis of the distribution */
Point InverseChiSquare::getKurtosis() const
{
  if (!(nu_ > 8.0)) throw NotDefinedException(HERE) << "InverseChiSquare kurtosis is defined only for nu > 8, here nu=" << nu_;
  return Point(1, 12.0 * (0.5 * nu_ * (0.5 * nu_ + 3.0) - 10.0) / ((nu_ - 6.0) * (nu_ - 8.0)));
}

/* Get the moments of the standardized distribution */
Point InverseChiSquare::getStandardMoment(const UnsignedInteger n) const
{
  if (nu_ <= 2.0 * n) throw NotDefinedException(HERE) << "InverseChiSquare standard moment of order " << 2.0 * n << " is defined only for nu > " << 2.0 * n << ", here k=" << nu_;
  return Point(1, std::exp(SpecFunc::LogGamma(0.5 * nu_ - n) - SpecFunc::LogGamma(0.5 * nu_)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution InverseChiSquare::getStandardRepresentative() const
{
  return new InverseChiSquare(nu_);
}

/* Compute the covariance of the distribution */
void InverseChiSquare::computeCovariance() const
{
  if (!(nu_ > 4.0)) throw NotDefinedException(HERE) << "InverseChiSquare covariance is defined only for nu > 4, here nu=" << nu_;
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = 2.0 / ((nu_ - 2.0) * (nu_ - 2.0) * (nu_ - 4.0));
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
Point InverseChiSquare::getParameter() const
{
  return Point(1, nu_);
}

void InverseChiSquare::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = InverseChiSquare(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description InverseChiSquare::getParameterDescription() const
{
  return Description(1, "nu");
}

/* Method save() stores the object through the StorageManager */
void InverseChiSquare::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "nu_", nu_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseChiSquare::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "nu_", nu_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
