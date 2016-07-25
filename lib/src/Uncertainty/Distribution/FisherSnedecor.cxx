//                                               -*- C++ -*-
/**
 *  @brief The Fisher-Snedecor distribution
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
#include "openturns/FisherSnedecor.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FisherSnedecor);

static const Factory<FisherSnedecor> Factory_FisherSnedecor;

/* Default constructor */
FisherSnedecor::FisherSnedecor()
  : ContinuousDistribution()
  , d1_(1.0)
  , d2_(5.0)
  , normalizationFactor_(0.0)
{
  setName( "FisherSnedecor" );
  setDimension( 1 );
  update();
  computeRange();
}

/* Parameters constructor */
FisherSnedecor::FisherSnedecor(const NumericalScalar d1,
                               const NumericalScalar d2)
  : ContinuousDistribution()
  , d1_(d1)
  , d2_(0.0)
  , normalizationFactor_(0.0)
{
  setName( "FisherSnedecor" );
  setD1(d1);
  // This call sets also the range
  setD2(d2);
  setDimension( 1 );
}

/* Comparison operator */
Bool FisherSnedecor::operator ==(const FisherSnedecor & other) const
{
  if (this == &other) return true;
  return (d1_ == other.d1_) && (d2_ == other.d2_);
}

Bool FisherSnedecor::equals(const DistributionImplementation & other) const
{
  const FisherSnedecor* p_other = dynamic_cast<const FisherSnedecor*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String FisherSnedecor::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FisherSnedecor::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " d1=" << d1_
      << " d2=" << d2_;
  return oss;
}

String FisherSnedecor::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(d1 = " << d1_ << ", d2 = " << d2_ << ")";
  return oss;
}

/* Virtual constructor */
FisherSnedecor * FisherSnedecor::clone() const
{
  return new FisherSnedecor(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void FisherSnedecor::computeRange()
{
  // Initialize the range with inverted bounds in order to use the generic
  // implementation of the computeScalarQuantile method to find the upper bound
  setRange(Interval(NumericalPoint(1, 0.0), NumericalPoint(1, -1.0), Interval::BoolCollection(1, true), Interval::BoolCollection(1, false)));
  // Now, compute the upper bound
  const NumericalPoint upperBound(computeUpperBound());
  setRange(Interval(NumericalPoint(1, 0.0), upperBound, Interval::BoolCollection(1, true), Interval::BoolCollection(1, false)));
}

/* Update the derivative attributes */
void FisherSnedecor::update()
{
  normalizationFactor_ = 0.5 * d1_ * std::log(d1_ / d2_) - SpecFunc::LnBeta(0.5 * d1_, 0.5 * d2_);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
}

/* Get one realization of the distribution */
NumericalPoint FisherSnedecor::getRealization() const
{
  return NumericalPoint(1, d2_ * DistFunc::rGamma(0.5 * d1_) / (d1_ * DistFunc::rGamma(0.5 * d2_)));
}

/* Get the PDF of the distribution */
NumericalScalar FisherSnedecor::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

NumericalScalar FisherSnedecor::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= 0.0) return -SpecFunc::MaxNumericalScalar;
  return normalizationFactor_ + (0.5 * d1_ - 1.0) * std::log(x) - 0.5 * (d1_ + d2_) * log1p(d1_ * x / d2_);
}

/* Get the CDF of the distribution */
NumericalScalar FisherSnedecor::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= 0) return 0.0;
  return DistFunc::pBeta(0.5 * d1_, 0.5 * d2_, d1_ * x / (d1_ * x + d2_));
}

/* Get the quantile of the distribution */
NumericalScalar FisherSnedecor::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if (getRange().getUpperBound()[0] < 0.0) return DistributionImplementation::computeScalarQuantile(prob, tail);
  const NumericalScalar p(tail ? 1.0 - prob : prob);
  const NumericalScalar q(DistFunc::qBeta(0.5 * d1_, 0.5 * d2_, p));
  if (q >= 1.0) return getRange().getUpperBound()[0];
  return d2_ * q / (d1_ * (1.0 - q));
}

/* Compute the mean of the distribution */
void FisherSnedecor::computeMean() const
{
  if (d2_ <= 2.0) throw NotDefinedException(HERE) << "Error: the mean is defined only when d2 > 2.";
  mean_ =  NumericalPoint(1, d2_ / (d2_ - 2));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint FisherSnedecor::getStandardDeviation() const
{
  return NumericalPoint(1, std::sqrt(getCovariance()(0, 0)));
}

/* Get the skewness of the distribution */
NumericalPoint FisherSnedecor::getSkewness() const
{
  if (d2_ <= 6.0) throw NotDefinedException(HERE) << "Error: the skewness is defined only when d2 > 6.";
  return NumericalPoint(1, (2.0 * d1_ + d2_ - 2.0) * std::sqrt(8.0 * (d2_ - 4.0)) / ((d2_ - 6.0) * std::sqrt(d1_ * (d1_ + d2_ - 2.0))));
}

/* Get the kurtosis of the distribution */
NumericalPoint FisherSnedecor::getKurtosis() const
{
  if (d2_ <= 8.0) throw NotDefinedException(HERE) << "Error: the kurtosis is defined only when d2 > 6.";
  return NumericalPoint(1, 3.0 * (d2_ - 4.0) * (16.0 + d2_ * (-16.0 + 4.0 * d2_) + d1_ * (-20.0 + d2_ * (8.0 + d2_) + d1_ * (10.0 + d2_))) / (d1_ * (d1_ + d2_ - 2.0) * (d2_ - 6.0) * (d2_ - 8.0)));
}

/* Get the moments of the standardized distribution */
NumericalPoint FisherSnedecor::getStandardMoment(const UnsignedInteger n) const
{
  if (2 * n >= d2_) throw NotDefinedException(HERE) << "Error: The raw moment of a FisherSnedecor distribution is defined only up to order d2/2, here n=" << n << " and d2=" << d2_;
  return NumericalPoint(1, std::exp(n * std::log(d2_ / d1_) + SpecFunc::LogGamma(0.5 * d1_ + n) + SpecFunc::LogGamma(0.5 * d2_ - n) - SpecFunc::LogGamma(0.5 * d1_) - SpecFunc::LogGamma(0.5 * d2_)));
}

/* Compute the covariance of the distribution */
void FisherSnedecor::computeCovariance() const
{
  if (d2_ <= 4.0) throw NotDefinedException(HERE) << "Error: the covariance is defined only when d2 > 4.";
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = 2.0 * d2_ * d2_ * (d1_ + d2_ - 2.0) / (d1_ * (d2_ - 4.0) * std::pow(d2_ - 2, 2));
  isAlreadyComputedMean_ = true;
}

/* Parameters value accessor */
NumericalPoint FisherSnedecor::getParameter() const
{
  NumericalPoint point(2);
  point[0] = d1_;
  point[1] = d2_;
  return point;
}

void FisherSnedecor::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = FisherSnedecor(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description FisherSnedecor::getParameterDescription() const
{
  Description description(2);
  description[0] = "d1";
  description[1] = "d2";
  return description;
}

/* D1 accessor */
void FisherSnedecor::setD1(const NumericalScalar d1)
{
  if (d1 <= 0.0) throw InvalidArgumentException(HERE) << "Error d1 of a FisherSnedecor distribution must be positive";
  if (d1_ != d1)
  {
    d1_ = d1;
    update();
    computeRange();
  }
}

NumericalScalar FisherSnedecor::getD1() const
{
  return d1_;
}


/* D2 accessor */
void FisherSnedecor::setD2(const NumericalScalar d2)
{
  if (d2 <= 0.0) throw InvalidArgumentException(HERE) << "Error d2 of a FisherSnedecor distribution must be positive";
  if (d2_ != d2)
  {
    d2_ = d2;
    update();
    computeRange();
  }
}

NumericalScalar FisherSnedecor::getD2() const
{
  return d2_;
}


/* Method save() stores the object through the StorageManager */
void FisherSnedecor::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "d1_", d1_ );
  adv.saveAttribute( "d2_", d2_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void FisherSnedecor::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "d1_", d1_ );
  adv.loadAttribute( "d2_", d2_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
