//                                               -*- C++ -*-
/**
 *  @brief The Pareto distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Pareto.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Pareto)

static const Factory<Pareto> Factory_Pareto;

/* Default constructor */
Pareto::Pareto()
  : ContinuousDistribution()
  , beta_(1.0)
  , alpha_(1.0)
  , gamma_(0.0)
{
  setName( "Pareto" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Pareto::Pareto(const Scalar beta,
               const Scalar alpha,
               const Scalar gamma)
  : ContinuousDistribution()
  , beta_(beta)
  , alpha_(1.0)
  , gamma_(gamma)
{
  setName( "Pareto" );
  // We set the dimension of the Pareto distribution
  setDimension( 1 );
  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Beta must be positive, here beta=" << beta;
  // This call set also the range.
  setAlpha(alpha);
}

/* Comparison operator */
Bool Pareto::operator ==(const Pareto & other) const
{
  if (this == &other) return true;
  return (beta_ == other.beta_) && (alpha_ == other.alpha_) && (gamma_ == other.gamma_);
}

Bool Pareto::equals(const DistributionImplementation & other) const
{
  const Pareto* p_other = dynamic_cast<const Pareto*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Pareto::__repr__() const
{
  OSS oss;
  oss << "class=" << Pareto::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " beta=" << beta_
      << " alpha=" << alpha_
      << " gamma=" << gamma_;
  return oss;
}

String Pareto::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(beta = " << beta_ << ", alpha=" << alpha_ << ", gamma=" << gamma_ << ")";
  return oss;
}

/* Virtual constructor */
Pareto * Pareto::clone() const
{
  return new Pareto(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Pareto::computeRange()
{
  const Point lowerBound(1, gamma_ + beta_);
  const Interval::BoolCollection finiteLowerBound(1, true);
  Point upperBound(1, computeScalarQuantile(1.0 - SpecFunc::Precision));
  Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution */
Point Pareto::getRealization() const
{
  const Scalar u = RandomGenerator::Generate();
  return Point(1, computeScalarQuantile(u));
}


/* Get the DDF of the distribution */
Point Pareto::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const Scalar x = point[0] - gamma_;
  Point result(1);
  if (x < beta_) return result;
  result[0] = -alpha_ * (1.0 + alpha_) / (beta_ * beta_) * std::pow(beta_ / x, 2.0 + alpha_);
  return result;
}


/* Get the PDF of the distribution */
Scalar Pareto::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const Scalar x = point[0] - gamma_;
  if (x < beta_) return 0.0;
  return alpha_ / beta_ * std::pow(beta_ / x, 1.0 + alpha_);
}

Scalar Pareto::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const Scalar x = point[0] - gamma_;
  if (x < beta_) return SpecFunc::LowestScalar;
  return std::log(alpha_ / beta_) + (1.0 + alpha_) * std::log(beta_ / x);
}

/* Get the CDF of the distribution */
Scalar Pareto::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const Scalar x = point[0] - gamma_;
  if (x < beta_) return 0.0;
  return 1.0 - std::pow(beta_ / x, alpha_);
}

/* Get the complementary CDF of the distribution */
Scalar Pareto::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const Scalar x = point[0] - gamma_;
  if (x < beta_) return 0.0;
  return std::pow(beta_ / x, alpha_);
}

/** Get the product minimum volume interval containing a given probability of the distribution */
Interval Pareto::computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProb) const
{
  return computeUnilateralConfidenceIntervalWithMarginalProbability(prob, false, marginalProb);
}

/** Get the minimum volume level set containing a given probability of the distribution */
LevelSet Pareto::computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & threshold) const
{
  const Interval interval(computeMinimumVolumeInterval(prob));
  Function minimumVolumeLevelSetFunction(MinimumVolumeLevelSetEvaluation(clone()).clone());
  minimumVolumeLevelSetFunction.setGradient(MinimumVolumeLevelSetGradient(clone()).clone());
  Scalar minusLogPDFThreshold = -computeLogPDF(interval.getUpperBound()[0]);
  threshold = std::exp(-minusLogPDFThreshold);
  return LevelSet(minimumVolumeLevelSetFunction, LessOrEqual(), minusLogPDFThreshold);
}

/* Compute the entropy of the distribution */
Scalar Pareto::computeEntropy() const
{
  return std::log(beta_ / alpha_) + 1.0 / alpha_ + 1.0;
}

/* Get the PDFGradient of the distribution */
Point Pareto::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  Point result(3);
  const Scalar x = point[0] - gamma_;
  if (x < beta_) return result;
  result[0] = alpha_ * alpha_ * std::pow(beta_, alpha_ - 1.0) * std::pow(x, -alpha_ - 1.0);
  result[1] = std::pow(beta_, alpha_) * std::pow(x, -alpha_ - 1.0) * (alpha_ * std::log(beta_) - alpha_ * std::log(x) + 1);
  result[2] = alpha_ * std::pow(beta_, alpha_) * (alpha_ + 1.0) * std::pow(x, -alpha_ - 2.0);
  return result;
}

/* Get the CDFGradient of the distribution */
Point Pareto::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point result(3);
  const Scalar x = point[0] - gamma_;
  if (x < beta_) return result;
  result[0] =  - alpha_ * std::pow(beta_ / x, alpha_) / beta_;
  result[1] = - std::pow(beta_ / x, alpha_) * std::log(beta_ / x);
  result[2] = alpha_ * std::pow(-beta_ / x, alpha_) / x;
  return result;
}

/* Get the quantile of the distribution */
Scalar Pareto::computeScalarQuantile(const Scalar prob,
                                     const Bool tail) const
{
  const Scalar q = tail ? 1.0 - prob : prob;
  return gamma_ + beta_ / std::pow(1.0 - q, 1.0 / alpha_);
}

/* Compute the mean of the distribution */
void Pareto::computeMean() const
{
  if (!(alpha_ > 1.0)) throw NotDefinedException(HERE) << "Error: the mean is defined only for alpha>1, here alpha=" << alpha_;
  mean_ = Point(1, gamma_ + (beta_ * alpha_) / (alpha_ - 1.0));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Pareto::getStandardDeviation() const
{
  return Point(1, std::sqrt(getCovariance()(0, 0)));
}

/* Get the skewness of the distribution */
Point Pareto::getSkewness() const
{
  if (!(alpha_ > 3.0)) throw NotDefinedException(HERE) << "Error: the skewness is defined only for alpha>3, here alpha=" << alpha_;
  return Point(1, 2.0 * (1.0 + alpha_) / (alpha_ - 3.0) * std::sqrt((alpha_ - 2.0) / alpha_));
}

/* Get the kurtosis of the distribution */
Point Pareto::getKurtosis() const
{
  if (!(alpha_ > 4.0)) throw NotDefinedException(HERE) << "Error: the kurtosis is defined only for alpha>4, here alpha=" << alpha_;
  return Point(1, 3.0 * (3.0 * alpha_ * alpha_ * alpha_ - 5.0 * alpha_ * alpha_ - 4.0) / (alpha_ * (alpha_ - 3.0) * (alpha_ - 4.0)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Pareto::getStandardRepresentative() const
{
  return new Pareto(1.0, alpha_, 0.0);
}

/* Compute the covariance of the distribution */
void Pareto::computeCovariance() const
{
  if (!(alpha_ > 2.0)) throw NotDefinedException(HERE) << "Error: the covariance is defined only for alpha>2, here alpha=" << alpha_;
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = beta_ * beta_ * alpha_ / ((alpha_ - 1.0) * (alpha_ - 1.0) * (alpha_ - 2.0));
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value  accessor */
Point Pareto::getParameter() const
{
  Point point(3);
  point[0] = beta_;
  point[1] = alpha_;
  point[2] = gamma_;
  return point;
}

void Pareto::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Pareto(parameter[0], parameter[1], parameter[2]);
  setWeight(w);
}

/* Parameters description accessor */
Description Pareto::getParameterDescription() const
{
  Description description(3);
  description[0] = "beta";
  description[1] = "alpha";
  description[2] = "gamma";
  return description;
}

/* Sigma accessor */
void Pareto::setBeta(const Scalar beta)
{
  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Beta MUST be positive";
  if (beta != beta_)
  {
    beta_ = beta;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Sigma accessor */
Scalar Pareto::getBeta() const
{
  return beta_;
}

/* Xi accessor */
void Pareto::setAlpha(const Scalar alpha)
{
  if (!(alpha > 0.0)) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (alpha != alpha_)
  {
    alpha_ = alpha;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    // The mean does not depend on xi
    computeRange();
  }
}

/* Xi accessor */
Scalar Pareto::getAlpha() const
{
  return alpha_;
}

/* Xi accessor */
void Pareto::setGamma(const Scalar gamma)
{
  if (gamma != gamma_)
  {
    gamma_ = gamma;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    // The mean does not depend on xi
    computeRange();
  }
}

/* Xi accessor */
Scalar Pareto::getGamma() const
{
  return gamma_;
}

/* Method save() stores the object through the StorageManager */
void Pareto::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

/* Method load() reloads the object from the StorageManager */
void Pareto::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "beta_", beta_ );
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "gamma_", gamma_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS
