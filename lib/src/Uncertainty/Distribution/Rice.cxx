//                                               -*- C++ -*-
/**
 *  @brief The Rice distribution
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
#include "openturns/Rice.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Rice);

static const Factory<Rice> Factory_Rice;

/* Default constructor */
Rice::Rice()
  : ContinuousDistribution()
  , sigma_(-1.0)
  , nu_(-1.0)
  , maximumIteration_(ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"))
{
  setName("Rice");
  setNu(0.0);
  setSigma(1.0);
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
Rice::Rice(const NumericalScalar sigma,
           const NumericalScalar nu)
  : ContinuousDistribution()
  , sigma_(0.0)
  , nu_(nu)
  , maximumIteration_(ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"))
{
  setName("Rice");
  setNu(nu);
  // This call sets also the range
  setSigma(sigma);
  setDimension(1);
}

/* Comparison operator */
Bool Rice::operator ==(const Rice & other) const
{
  if (this == &other) return true;
  return (sigma_ == other.sigma_) && (nu_ == other.nu_);
}

Bool Rice::equals(const DistributionImplementation & other) const
{
  const Rice* p_other = dynamic_cast<const Rice*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Rice::__repr__() const
{
  OSS oss;
  oss << "class=" << Rice::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " sigma=" << sigma_
      << " nu=" << nu_;
  return oss;
}

String Rice::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(sigma = " << sigma_ << ", nu = " << nu_ << ")";
  return oss;
}

/* Virtual constructor */
Rice * Rice::clone() const
{
  return new Rice(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Rice::computeRange()
{
  const NumericalPoint lowerBound(1, 0.0 );
  const NumericalPoint upperBound(1, computeScalarQuantile(cdfEpsilon_, true));
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}


/* Get one realization of the distribution */
NumericalPoint Rice::getRealization() const
{
  const NumericalScalar x = sigma_ * DistFunc::rNormal() + nu_;
  const NumericalScalar y = sigma_ * DistFunc::rNormal();
  return NumericalPoint(1.0, std::sqrt(x * x + y * y));
}


/* Get the PDF of the distribution */
NumericalScalar Rice::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if (x <= 0.0) return 0.0;
  const NumericalScalar xScaled = x / sigma_;
  const NumericalScalar nuScaled = nu_ / sigma_;
  return xScaled / sigma_ * std::exp(-0.5 * (xScaled * xScaled + nuScaled * nuScaled) + SpecFunc::LogBesselI0(xScaled * nuScaled));
}


/* Get the logarithm of the PDF of the distribution */
NumericalScalar Rice::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if (x <= 0.0) return -SpecFunc::MaxNumericalScalar;
  const NumericalScalar xScaled = x / sigma_;
  const NumericalScalar nuScaled = nu_ / sigma_;
  return std::log(xScaled / sigma_) - 0.5 * (xScaled * xScaled + nuScaled * nuScaled) + SpecFunc::LogBesselI0(xScaled * nuScaled);
}


/* Get the CDF of the distribution */
NumericalScalar Rice::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if (point[0] <= 0.0) return 0.0;
  const NumericalScalar lambda = std::pow(nu_ / sigma_, 2);
  const NumericalScalar y = std::pow(point[0] / sigma_, 2);
  return DistFunc::pNonCentralChiSquare(2, lambda , y, false, pdfEpsilon_, maximumIteration_);
}

NumericalScalar Rice::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if (point[0] <= 0.0) return 1.0;
  const NumericalScalar lambda = std::pow(nu_ / sigma_, 2);
  const NumericalScalar y = std::pow(point[0] / sigma_, 2);
  return DistFunc::pNonCentralChiSquare(2, lambda , y, true, pdfEpsilon_, maximumIteration_);
}

/* Compute the mean of the distribution */
void Rice::computeMean() const
{
  //1.253314137315500251207882 = sqrt(pi/2)
  const NumericalScalar x = -0.5 * std::pow(nu_ / sigma_, 2);
  mean_ = NumericalPoint(1, sigma_ * 1.253314137315500251207882 * SpecFunc::HyperGeom_1_1(-0.5, 1, x));
}

/* Get the standard deviation of the distribution */
NumericalPoint Rice::getStandardDeviation() const
{
  if (!isAlreadyComputedCovariance_) computeCovariance();
  return NumericalPoint(1, std::sqrt(covariance_(0, 0)));
}

/* Get the moments of the standardized distribution */
NumericalPoint Rice::getStandardMoment(const UnsignedInteger n) const
{
  if (n == 0) return NumericalPoint(1, 1.0);
  const NumericalScalar sigma2 = sigma_ * sigma_;
  return NumericalPoint(1, std::pow(2.0 * sigma2, 0.5 * n) * SpecFunc::Gamma(1.0 + 0.5 * n) * SpecFunc::HyperGeom_1_1(-0.5 * n, 1.0, -0.5 * nu_ * nu_ / sigma2));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Rice::Implementation Rice::getStandardRepresentative() const
{
  return clone();
}


/* Compute the covariance of the distribution */
void Rice::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  NumericalScalar covariance = 0.0;
  const NumericalScalar mu = getMean()[0];
  covariance = 2.0 * sigma_ * sigma_ + (nu_ - mu) * (nu_ + mu);
  covariance_(0, 0) = covariance;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
NumericalPoint Rice::getParameter() const
{
  NumericalPoint point(2);
  point[0] = sigma_;
  point[1] = nu_;
  return point;
}

void Rice::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = Rice(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description Rice::getParameterDescription() const
{
  Description description(2);
  description[0] = "sigma";
  description[1] = "nu";
  return description;
}

/* Sigma accessor */
void Rice::setSigma(const NumericalScalar sigma)
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

NumericalScalar Rice::getSigma() const
{
  return sigma_;
}

/* Nu accessor */
void Rice::setNu(const NumericalScalar nu)
{
  if (nu < 0.0) throw InvalidArgumentException(HERE) << "Nu MUST be positive";
  if (nu != nu_)
  {
    nu_ = nu;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on nu
    computeRange();
  }
}

NumericalScalar Rice::getNu() const
{
  return nu_;
}

/* Maximum iterations accessor */
void Rice::setMaximumIteration(const UnsignedInteger maximumIteration)
{
  maximumIteration_ = maximumIteration;
}

UnsignedInteger Rice::getMaximumIteration() const
{
  return maximumIteration_;
}

/* Method save() stores the object through the StorageManager */
void Rice::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "sigma_", sigma_ );
  adv.saveAttribute( "nu_", nu_ );
  adv.saveAttribute( "maximumIteration_", maximumIteration_ );
}

/* Method load() reloads the object from the StorageManager */
void Rice::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "sigma_", sigma_ );
  adv.loadAttribute( "nu_", nu_ );
  adv.loadAttribute( "maximumIteration_", maximumIteration_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
