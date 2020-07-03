//                                               -*- C++ -*-
/**
 *  @brief The Logistic distribution
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
#include "openturns/Logistic.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Logistic)

static const Factory<Logistic> Factory_Logistic;

/* Default constructor */
Logistic::Logistic()
  : ContinuousDistribution()
  , mu_(0.0)
  , beta_(1.0)
{
  setName( "Logistic" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Logistic::Logistic(const Scalar mu,
                   const Scalar beta)
  : ContinuousDistribution()
  , mu_(mu)
  , beta_(0.0)
{
  setName( "Logistic" );
  // This call set also the range.
  setBeta(beta);
  setDimension( 1 );
}

/* Comparison operator */
Bool Logistic::operator ==(const Logistic & other) const
{
  if (this == &other) return true;
  return (mu_ == other.mu_) && (beta_ == other.beta_);
}

Bool Logistic::equals(const DistributionImplementation & other) const
{
  const Logistic* p_other = dynamic_cast<const Logistic*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Logistic::__repr__() const
{
  OSS oss;
  oss << "class=" << Logistic::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " mu=" << mu_
      << " beta=" << beta_;
  return oss;
}

String Logistic::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(mu = " << mu_ << ", beta = " << beta_ << ")";
  return oss;
}

/* Virtual constructor */
Logistic * Logistic::clone() const
{
  return new Logistic(*this);
}

/* Get one realization of the distribution */
Point Logistic::getRealization() const
{
  Scalar prob = RandomGenerator::Generate();
  return Point(1, mu_ + beta_ * std::log(prob / (1.0 - prob)));
}


/* Get the DDF of the distribution */
Point Logistic::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Scalar expX = std::exp((point[0] - mu_) / beta_);
  Scalar betaExpX = beta_ * (1.0 + expX);
  return Point(1, beta_ * expX * (1.0 - expX) / (betaExpX * betaExpX * betaExpX));
}


/* Get the PDF of the distribution */
Scalar Logistic::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const Scalar z = (point[0] - mu_) / beta_;
  if (z > 12.38075336)
  {
    const Scalar expMZ = std::exp(-z);
    return expMZ * (1.0 - 2.0 * expMZ) / beta_;
  }
  if (z < -12.38075336)
  {
    const Scalar expZ = std::exp(z);
    return expZ * (1.0 - 2.0 * expZ) / beta_;
  }
  const Scalar expMZ = std::exp(-z);
  return expMZ / (beta_ * std::pow(1.0 + expMZ, 2.0));
}

Scalar Logistic::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar z = (point[0] - mu_) / beta_;
  if (z > 12.38075336) return -z + log1p(-2.0 * std::exp(-z)) - std::log(beta_);
  if (z < -12.38075336) return z + log1p(-2.0 * std::exp(z)) - std::log(beta_);
  return -z - std::log(beta_) - 2.0 * log1p(std::exp(-z));
}

/* Get the CDF of the distribution */
Scalar Logistic::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar z = (point[0] - mu_) / beta_;
  if (z > 12.01454911)
  {
    const Scalar expMZ = std::exp(-z);
    return 1.0 - expMZ * (1.0 - expMZ);
  }
  if (z < -12.01454911)
  {
    const Scalar expZ = std::exp(z);
    return expZ * (1.0 - expZ);
  }
  return 1.0 / (1.0 + std::exp(-z));
}

Scalar Logistic::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar z = (point[0] - mu_) / beta_;
  if (z > 12.01454911)
  {
    const Scalar expMZ = std::exp(-z);
    return expMZ * (1.0 - expMZ);
  }
  if (z < -12.01454911)
  {
    const Scalar expZ = std::exp(z);
    return 1.0 - expZ * (1.0 - expZ);
  }
  return 1.0 / (1.0 + std::exp(z));
}

/* Compute the entropy of the distribution */
Scalar Logistic::computeEntropy() const
{
  return 2.0 + std::log(beta_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Logistic::computeCharacteristicFunction(const Scalar x) const
{
  const Scalar piBetaU = M_PI * beta_ * x;
  return std::exp(Complex(0.0, x * mu_)) * piBetaU / std::sinh(piBetaU);
}

Complex Logistic::computeLogCharacteristicFunction(const Scalar x) const
{
  const Scalar piBetaU = M_PI * beta_ * x;
  return Complex(0.0, x * mu_) + std::log(piBetaU) - std::log(std::sinh(piBetaU));
}

/* Get the PDFGradient of the distribution */
Point Logistic::computePDFGradient(const Point & point) const
{
  Scalar x = (point[0] - mu_) / beta_;
  Scalar expX = std::exp(x);
  Scalar betaExpX = beta_ * (1.0 + expX);
  Point pdfGradient(2);
  pdfGradient[0] = beta_ * expX * (expX - 1.0) / (betaExpX * betaExpX * betaExpX);
  pdfGradient[1] = pdfGradient[0] * x - expX / (betaExpX * betaExpX);
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point Logistic::computeCDFGradient(const Point & point) const
{
  Scalar x = (point[0] - mu_) / beta_;
  Scalar expX = std::exp(x);
  Scalar betaExpX = beta_ * (1.0 + expX);
  Point cdfGradient(2);
  cdfGradient[0] = -beta_ * expX / (betaExpX * betaExpX);
  cdfGradient[1] = cdfGradient[0] * x;
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar Logistic::computeScalarQuantile(const Scalar prob,
                                       const Bool tail) const
{
  if (tail) return mu_ + beta_ * std::log((1.0 - prob) / prob);
  return mu_ + beta_ * std::log(prob / (1.0 - prob));
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar Logistic::getRoughness() const
{
  // 0.1666666666666666666666667 = 1 / 6
  return 0.1666666666666666666666667 / beta_;
}

/* Compute the mean of the distribution */
void Logistic::computeMean() const
{
  mean_ = Point(1, mu_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Logistic::getStandardDeviation() const
{
  return Point(1, SpecFunc::PI_SQRT3 * beta_);
}

/* Get the skewness of the distribution */
Point Logistic::getSkewness() const
{
  return Point(1, 0.0);
}

/* Get the kurtosis of the distribution */
Point Logistic::getKurtosis() const
{
  // 3+6/5 = 4.2
  return Point(1, 4.2);
}

/* Compute the covariance of the distribution */
void Logistic::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = SpecFunc::PI2_3 * beta_ * beta_;
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
Point Logistic::getStandardMoment(const UnsignedInteger n) const
{
  if (n % 2 == 1) return Point(1, 0.0);
  if (n ==  2) return Point(1, 3.2898681336964528730e00);
  if (n ==  4) return Point(1, 4.5457575815867804043e01);
  if (n ==  6) return Point(1, 1.4191935714683065499e03);
  if (n ==  8) return Point(1, 8.0336229269397526593e04);
  if (n == 10) return Point(1, 7.2506291303452157147e06);
  if (n == 12) return Point(1, 9.5777106159240860718e08);
  if (n == 14) return Point(1, 1.7434597634941644924e11);
  if (n == 16) return Point(1, 4.1844942225739403485e13);
  if (n == 18) return Point(1, 1.2804698598089257967e16);
  if (n == 20) return Point(1, 4.8657993773520842256e18);
  if (n == 22) return Point(1, 2.2480009196614112639e21);
  if (n == 24) return Point(1, 1.2408967295076549681e24);
  if (n == 26) return Point(1, 8.0658291023450627114e26);
  if (n == 28) return Point(1, 6.0977668695185918278e29);
  if (n == 30) return Point(1, 5.3050571913031274079e32);
  if (n == 32) return Point(1, 5.2626167374485750005e35);
  if (n == 34) return Point(1, 5.9046559804483869578e38);
  if (n == 36) return Point(1, 7.4398665356897600917e41);
  if (n == 38) return Point(1, 1.0460452349293967338e45);
  if (n == 40) return Point(1, 1.6318305664943113275e48);
  if (n == 42) return Point(1, 2.8100122355051208743e51);
  if (n == 44) return Point(1, 5.3165431495765953256e54);
  if (n == 46) return Point(1, 1.1005244319624021505e58);
  if (n == 48) return Point(1, 2.4827831185072057135e61);
  if (n == 50) return Point(1, 6.0828186403426702060e64);
  return DistributionImplementation::getStandardMoment(n);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Logistic::getStandardRepresentative() const
{
  return new Logistic(0.0, 1.0);
}

/* Parameters value accessor */
Point Logistic::getParameter() const
{
  Point point(2);
  point[0] = mu_;
  point[1] = beta_;
  return point;
}

void Logistic::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Logistic(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description Logistic::getParameterDescription() const
{
  Description description(2);
  description[0] = "mu";
  description[1] = "beta";
  return description;
}

/* Check if the distribution is elliptical */
Bool Logistic::isElliptical() const
{
  return true;
}

/* Alpha accessor */
void Logistic::setMu(const Scalar mu)
{
  if (mu != mu_)
  {
    mu_ = mu;
    isAlreadyComputedMean_ = false;
    // The covariancedoes not depend on alpha
    computeRange();
  }
}

Scalar Logistic::getMu() const
{
  return mu_;
}

/* Beta accessor */
void Logistic::setBeta(const Scalar beta)
{
  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Beta MUST be positive, here beta=" << beta;
  if (beta != beta_)
  {
    beta_ = beta;
    isAlreadyComputedCovariance_ = false;
    // The mean does not depend on beta
    computeRange();
  }
}

Scalar Logistic::getBeta() const
{
  return beta_;
}

/* Method save() stores the object through the StorageManager */
void Logistic::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "beta_", beta_ );
}

/* Method load() reloads the object from the StorageManager */
void Logistic::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  if (adv.hasAttribute("alpha_")) // old name
    adv.loadAttribute( "alpha_", mu_ );
  else
    adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "beta_", beta_ );
  computeRange();
}



END_NAMESPACE_OPENTURNS
