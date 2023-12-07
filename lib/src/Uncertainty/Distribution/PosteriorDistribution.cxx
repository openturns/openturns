//                                               -*- C++ -*-
/**
 *  @brief The PosteriorDistribution distribution
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
#include "openturns/PosteriorDistribution.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/GaussLegendre.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(PosteriorDistribution)

static const Factory<PosteriorDistribution> Factory_PosteriorDistribution;


/* Default constructor */
PosteriorDistribution::PosteriorDistribution()
  : ContinuousDistribution(),
    conditionalDistribution_(),
    observations_()
{
  setName("PosteriorDistribution");
  // First, set the observations
  observations_ = Sample(1, conditionalDistribution_.getConditionedDistribution().getMean());
  // Then, set the conditional distribution. It also set the dimension.
  setConditionalDistribution(conditionalDistribution_);
  computeRange();
}

/* Parameters constructor */
PosteriorDistribution::PosteriorDistribution(const ConditionalDistribution & conditionalDistribution,
    const Sample & observations)
  : ContinuousDistribution(),
    conditionalDistribution_(),
    observations_(observations)
{
  setName("PosteriorDistribution");
  if (observations.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a posterior distribution with no observation.";
  setConditionalDistribution(conditionalDistribution);
}

/* Comparison operator */
Bool PosteriorDistribution::operator ==(const PosteriorDistribution & other) const
{
  if (this == &other) return true;
  return (conditionalDistribution_ == other.conditionalDistribution_) && (observations_ == other.observations_);
}

Bool PosteriorDistribution::equals(const DistributionImplementation & other) const
{
  const PosteriorDistribution* p_other = dynamic_cast<const PosteriorDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String PosteriorDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << PosteriorDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " conditional distribution=" << conditionalDistribution_
      << " observations=" << observations_;
  return oss;
}

String PosteriorDistribution::__str__(const String & offset) const
{
  OSS oss;
  oss << getClassName() << "(conditional distribution = " << conditionalDistribution_.__str__() << ", observations =\n" << offset << observations_.__str__(offset) << ")";
  return oss;
}

/* Virtual constructor */
PosteriorDistribution * PosteriorDistribution::clone() const
{
  return new PosteriorDistribution(*this);
}

/* Compute the likelihood of the observations */
Point PosteriorDistribution::computeLikelihood(const Point & theta) const
{
  return Point(1, std::exp(computeLogLikelihood(theta)));
}

/* Compute the log-likelihood of the observations */
Scalar PosteriorDistribution::computeLogLikelihood(const Point & theta) const
{
  Distribution conditionedDistribution(conditionalDistribution_.getConditionedDistribution());
  conditionedDistribution.setParameter(theta);
  Scalar logLikelihood = 0.0;
  const UnsignedInteger size = observations_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar atomicValue = conditionedDistribution.computeLogPDF(observations_[i]);
    logLikelihood += atomicValue;
  }
  return logLikelihood;
}

/* Get the PDF of the distribution */
Scalar PosteriorDistribution::computePDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  const Scalar value = conditionalDistribution_.getConditioningDistribution().computeLogPDF(point) - logNormalizationFactor_ + computeLogLikelihood(point);
  return std::exp(value);
}


/* Get the CDF of the distribution */
Scalar PosteriorDistribution::computeCDF(const Point & point) const
{
  // FIXME: computeExpectation seems to incorrectly compute the CDF of the prior
  if (conditionalDistribution_.getConditioningDistribution().isContinuous())
    return ContinuousDistribution::computeCDF(point);

  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  Description inputDescription(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i) inputDescription[i] = String(OSS() << "x" << i);
  const SymbolicFunction f(inputDescription, Description(1, "1"));
  const Scalar cdf = conditionalDistribution_.computeExpectation(f, point)[0];
  return cdf;
}

/* Parameters value and description accessor */
PosteriorDistribution::PointWithDescriptionCollection PosteriorDistribution::getParametersCollection() const
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::getParametersCollection() const";
}

void PosteriorDistribution::setParametersCollection(const PointCollection & )
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::setParametersCollection(const PointCollection & parametersCollection)";
}


/* Conditional distribution accessor */
void PosteriorDistribution::setConditionalDistribution(const ConditionalDistribution & conditionalDistribution)
{
  if (observations_.getDimension() != conditionalDistribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the conditioned distribution defining the conditional distribution must have the same dimension as the observations.";
  conditionalDistribution_ = conditionalDistribution;
  setDimension(conditionalDistribution.getConditioningDistribution().getDimension());
  logNormalizationFactor_ = 0.0;
  const UnsignedInteger size = observations_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    logNormalizationFactor_ += conditionalDistribution_.computeLogPDF(observations_[i]);
  computeRange();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;

  // FIXME: tweak normalization factor
  if (conditionalDistribution_.getConditioningDistribution().isContinuous())
  {
    GaussLegendre integrationAlgorithm(getDimension());
    const Scalar iPDF = integrationAlgorithm.integrate(PDFWrapper(this), getRange())[0];
    logNormalizationFactor_ += std::log(iPDF);
  }
}

ConditionalDistribution PosteriorDistribution::getConditionalDistribution() const
{
  return conditionalDistribution_;
}


/* Observations accessor */
void PosteriorDistribution::setObservations(const Sample & observations)
{
  if (observations.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot use a posterior distribution with no observation.";
  if (observations.getDimension() != conditionalDistribution_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the conditioned distribution defining the conditional distribution must have the same dimension as the observations.";
  observations_ = observations;
  setConditionalDistribution(conditionalDistribution_);
}

Sample PosteriorDistribution::getObservations() const
{
  return observations_;
}

/* Log normalization factor accessor */
Scalar PosteriorDistribution::getLogNormalizationFactor() const
{
  return logNormalizationFactor_;
}

/* Compute the numerical range of the distribution given the parameters values */
void PosteriorDistribution::computeRange()
{
  setRange(conditionalDistribution_.conditioningDistribution_.getRange());
}

class PosteriorDistributionLikelihoodEvaluation : public EvaluationImplementation
{
public:
  PosteriorDistributionLikelihoodEvaluation(const PosteriorDistribution & distribution) : EvaluationImplementation(), distribution_(distribution) {}
  PosteriorDistributionLikelihoodEvaluation * clone() const override
  {
    return new PosteriorDistributionLikelihoodEvaluation(*this);
  }
  UnsignedInteger getInputDimension() const override
  {
    return distribution_.getDimension();
  }
  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator()(const Point & inP) const override
  {
    return distribution_.computeLikelihood(inP);
  }

private:
  const PosteriorDistribution & distribution_;
};

/* Compute the mean of the distribution */
void PosteriorDistribution::computeMean() const
{
  Description inputDescription(Description::BuildDefault(getDimension(), "x"));
  const SymbolicFunction meanFunction(inputDescription, inputDescription);
  const Function likelihood(PosteriorDistributionLikelihoodEvaluation(*this));
  mean_ = conditionalDistribution_.computeExpectation(likelihood * meanFunction, getRange().getUpperBound()) / std::exp(logNormalizationFactor_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point PosteriorDistribution::getStandardDeviation() const
{
  // To insure that the covariance has been computed
  getCovariance();
  Point sigma(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i) sigma[i] = std::sqrt(covariance_(i, i));
  return sigma;
}

/* Get the skewness of the distribution */
Point PosteriorDistribution::getSkewness() const
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::getSkewness() const";
}

/* Get the kurtosis of the distribution */
Point PosteriorDistribution::getKurtosis() const
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::getKurtosis() const";
}

/* Compute the covariance of the distribution */
void PosteriorDistribution::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  // To insure that the mean has been computed
  getMean();
  Description inputDescription(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) inputDescription[i] = String(OSS() << "x" << i);
  Description formulas((dimension * (dimension + 1)) / 2);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar muI = mean_[i];
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      const Scalar muJ = mean_[j];
      formulas[index] = String(OSS() << "(x" << i << "-" << muI << ")*(x" << j << "-" << muJ << ")");
      ++index;
    }
  }
  const SymbolicFunction covarianceFunction(inputDescription, formulas);
  const Function likelihood(PosteriorDistributionLikelihoodEvaluation(*this));
  const Point result(conditionalDistribution_.computeExpectation(likelihood * covarianceFunction, getRange().getUpperBound()) / std::exp(logNormalizationFactor_));
  index = 0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      covariance_(i, j) = result[index];
      ++index;
    }
  isAlreadyComputedCovariance_ = true;
}

/* Method save() stores the object through the StorageManager */
void PosteriorDistribution::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "conditionalDistribution_", conditionalDistribution_ );
  adv.saveAttribute( "observations_", observations_ );
  adv.saveAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void PosteriorDistribution::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "conditionalDistribution_", conditionalDistribution_ );
  adv.loadAttribute( "observations_", observations_ );
  adv.loadAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS
