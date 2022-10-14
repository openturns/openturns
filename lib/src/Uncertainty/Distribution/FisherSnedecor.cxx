//                                               -*- C++ -*-
/**
 *  @brief The Fisher-Snedecor distribution
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
#include "openturns/FisherSnedecor.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FisherSnedecor)

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
FisherSnedecor::FisherSnedecor(const Scalar d1,
                               const Scalar d2)
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

String FisherSnedecor::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(d1 = " << d1_ << ", d2 = " << d2_ << ")";
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
  setRange(Interval(Point(1, 0.0), Point(1, -1.0), Interval::BoolCollection(1, true), Interval::BoolCollection(1, false)));
  // Now, compute the upper bound
  const Point upperBound(computeUpperBound());
  setRange(Interval(Point(1, 0.0), upperBound, Interval::BoolCollection(1, true), Interval::BoolCollection(1, false)));
}

/* Update the derivative attributes */
void FisherSnedecor::update()
{
  normalizationFactor_ = 0.5 * d1_ * std::log(d1_ / d2_) - SpecFunc::LnBeta(0.5 * d1_, 0.5 * d2_);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
}

/* Get one realization of the distribution */
Point FisherSnedecor::getRealization() const
{
  return Point(1, d2_ * DistFunc::rGamma(0.5 * d1_) / (d1_ * DistFunc::rGamma(0.5 * d2_)));
}

/* Get the PDF of the distribution */
Scalar FisherSnedecor::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (x <= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

Scalar FisherSnedecor::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (x <= 0.0) return SpecFunc::LowestScalar;
  return normalizationFactor_ + (0.5 * d1_ - 1.0) * std::log(x) - 0.5 * (d1_ + d2_) * log1p(d1_ * x / d2_);
}

Point FisherSnedecor::computeLogPDFGradient(const Point & point) const
{
  const Scalar x = point[0];
  Point logPdfGradient(2, 0.0);
  if (!(x > 0.0)) return logPdfGradient;
  const Scalar d1xd2 = d1_ * x + d2_;
  // First derivate the normlizationFactor as function of d1_, d2_ (see expression above in LogPDF)
  // As the term is a combinations of LnBeta(d1/2, d2/2) := log(Beta(d1/2, d2/2), dLnBeta = dBeta/Beta
  // As dBeta(x,y) = B(x,y) * (DiGamma(X) - DiGamma(x+y)) (see  https://en.wikipedia.org/wiki/Beta_function#Derivatives)
  // it follows that d(LnBeta(x,y)) = dBeta(x,y) / Beta(x,y) = DiGamma(X) - DiGamma(x+y)
  // Rest is very easy to derivate
  logPdfGradient[0] = 0.5 * ( std::log(d1_ * x / d1xd2) + 1.0 - SpecFunc::DiGamma(0.5 * d1_) + SpecFunc::DiGamma(0.5 * d1_ + 0.5 * d2_) - (d1_ + d2_) * x / d1xd2);
  logPdfGradient[1] = 0.5 * (-d1_  / d2_ - SpecFunc::DiGamma(0.5 * d2_) + SpecFunc::DiGamma(0.5 * d1_ + 0.5 * d2_) - log1p(d1_ * x / d2_) + (d1_  + d2_) * (d1_ * x  / d2_) / d1xd2);
  return logPdfGradient;
}

Point FisherSnedecor::computePDFGradient(const Point & point) const
{
  // PDF(x) =  exp(LogPDF(x)) thus PDF(x)' = LogPDF(x)' * exp(LogPDF(x))
  Point PdfGradient(computeLogPDFGradient(point));
  Scalar pdf = computePDF(point);
  return PdfGradient * pdf;
}

/* Get the CDF of the distribution */
Scalar FisherSnedecor::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (x <= 0) return 0.0;
  return DistFunc::pBeta(0.5 * d1_, 0.5 * d2_, d1_ * x / (d1_ * x + d2_));
}

/* Get the quantile of the distribution */
Scalar FisherSnedecor::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (getRange().getUpperBound()[0] < 0.0) return DistributionImplementation::computeScalarQuantile(prob, tail);
  const Scalar p = tail ? 1.0 - prob : prob;
  const Scalar q = DistFunc::qBeta(0.5 * d1_, 0.5 * d2_, p);
  if (q >= 1.0) return getRange().getUpperBound()[0];
  return d2_ * q / (d1_ * (1.0 - q));
}

/* Compute the mean of the distribution */
void FisherSnedecor::computeMean() const
{
  if (!(d2_ > 2.0)) throw NotDefinedException(HERE) << "Error: the mean is defined only when d2 > 2.";
  mean_ =  Point(1, d2_ / (d2_ - 2));
  isAlreadyComputedMean_ = true;
}

/* Compute the entropy of the distribution */
Scalar FisherSnedecor::computeEntropy() const
{
  return std::log(d2_ / d1_) + SpecFunc::LogBeta(0.5 * d1_, 0.5 * d2_) + (1.0 - 0.5 * d1_) * SpecFunc::Psi(0.5 * d1_) + (0.5 * (d1_ + d2_)) * SpecFunc::Psi(0.5 * (d1_ + d2_)) - (1.0 + 0.5 * d2_) * SpecFunc::Psi(0.5 * d2_);
}

/* Get the standard deviation of the distribution */
Point FisherSnedecor::getStandardDeviation() const
{
  return Point(1, std::sqrt(getCovariance()(0, 0)));
}

/* Get the skewness of the distribution */
Point FisherSnedecor::getSkewness() const
{
  if (!(d2_ > 6.0)) throw NotDefinedException(HERE) << "Error: the skewness is defined only when d2 > 6.";
  return Point(1, (2.0 * d1_ + d2_ - 2.0) * std::sqrt(8.0 * (d2_ - 4.0)) / ((d2_ - 6.0) * std::sqrt(d1_ * (d1_ + d2_ - 2.0))));
}

/* Get the kurtosis of the distribution */
Point FisherSnedecor::getKurtosis() const
{
  if (!(d2_ > 8.0)) throw NotDefinedException(HERE) << "Error: the kurtosis is defined only when d2 > 6.";
  return Point(1, 3.0 * (d2_ - 4.0) * (16.0 + d2_ * (-16.0 + 4.0 * d2_) + d1_ * (-20.0 + d2_ * (8.0 + d2_) + d1_ * (10.0 + d2_))) / (d1_ * (d1_ + d2_ - 2.0) * (d2_ - 6.0) * (d2_ - 8.0)));
}

/* Compute the covariance of the distribution */
void FisherSnedecor::computeCovariance() const
{
  if (!(d2_ > 4.0)) throw NotDefinedException(HERE) << "Error: the covariance is defined only when d2 > 4.";
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = 2.0 * d2_ * d2_ * (d1_ + d2_ - 2.0) / (d1_ * (d2_ - 4.0) * std::pow(d2_ - 2, 2));
  isAlreadyComputedMean_ = true;
}

/* Parameters value accessor */
Point FisherSnedecor::getParameter() const
{
  Point point(2);
  point[0] = d1_;
  point[1] = d2_;
  return point;
}

void FisherSnedecor::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
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
void FisherSnedecor::setD1(const Scalar d1)
{
  if (!(d1 > 0.0)) throw InvalidArgumentException(HERE) << "Error d1 of a FisherSnedecor distribution must be positive";
  if (d1_ != d1)
  {
    d1_ = d1;
    update();
    computeRange();
  }
}

Scalar FisherSnedecor::getD1() const
{
  return d1_;
}


/* D2 accessor */
void FisherSnedecor::setD2(const Scalar d2)
{
  if (!(d2 > 0.0)) throw InvalidArgumentException(HERE) << "Error d2 of a FisherSnedecor distribution must be positive";
  if (d2_ != d2)
  {
    d2_ = d2;
    update();
    computeRange();
  }
}

Scalar FisherSnedecor::getD2() const
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
