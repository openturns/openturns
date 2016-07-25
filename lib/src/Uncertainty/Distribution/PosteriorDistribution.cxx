//                                               -*- C++ -*-
/**
 *  @brief The PosteriorDistribution distribution
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
#include "openturns/PosteriorDistribution.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(PosteriorDistribution);

static const Factory<PosteriorDistribution> Factory_PosteriorDistribution;


/* Default constructor */
PosteriorDistribution::PosteriorDistribution()
  : ContinuousDistribution(),
    conditionalDistribution_(),
    observations_()
{
  setName("PosteriorDistribution");
  // First, set the observations
  observations_ = NumericalSample(1, conditionalDistribution_.getConditionedDistribution().getMean());
  // Then, set the conditional distribution. It also set the dimension.
  setConditionalDistribution(conditionalDistribution_);
  computeRange();
}

/* Parameters constructor */
PosteriorDistribution::PosteriorDistribution(const ConditionalDistribution & conditionalDistribution,
    const NumericalSample & observations)
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
  oss << offset << getClassName() << "(conditional distribution = " << conditionalDistribution_.__str__() << ", observations = \n" << observations_.__str__(offset) << ")";
  return oss;
}

/* Virtual constructor */
PosteriorDistribution * PosteriorDistribution::clone() const
{
  return new PosteriorDistribution(*this);
}

/* Compute the likelihood of the observations */
NumericalPoint PosteriorDistribution::computeLikelihood(const NumericalPoint & theta) const
{
  Distribution conditionedDistribution(conditionalDistribution_.getConditionedDistribution());
  conditionedDistribution.setParameter(theta);
  NumericalScalar likelihood(1.0);
  const UnsignedInteger size(observations_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar atomicValue(conditionedDistribution.computePDF(observations_[i]));
    likelihood *= atomicValue;
  }
  return NumericalPoint(1, likelihood);
}

/* Get the PDF of the distribution */
NumericalScalar PosteriorDistribution::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  const NumericalScalar value(std::log(conditionalDistribution_.getConditioningDistribution().computePDF(point)) - logNormalizationFactor_ + std::log(computeLikelihood(point)[0]));
  return std::exp(value);
}


/* Get the CDF of the distribution */
NumericalScalar PosteriorDistribution::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  Description inputDescription(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i) inputDescription[i] = String(OSS() << "x" << i);
  const NumericalMathFunction f(inputDescription, Description(1, "1"));
  const NumericalScalar cdf(conditionalDistribution_.computeExpectation(f, point)[0]);
  return cdf;
}

/* Parameters value and description accessor */
PosteriorDistribution::NumericalPointWithDescriptionCollection PosteriorDistribution::getParametersCollection() const
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::getParametersCollection() const";
}

void PosteriorDistribution::setParametersCollection(const NumericalPointCollection & parametersCollection)
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::setParametersCollection(const NumericalPointCollection & parametersCollection)";
}


/* Conditional distribution accessor */
void PosteriorDistribution::setConditionalDistribution(const ConditionalDistribution & conditionalDistribution)
{
  if (observations_.getDimension() != conditionalDistribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the conditioned distribution defining the conditional distribution must have the same dimension as the observations.";
  conditionalDistribution_ = conditionalDistribution;
  setDimension(conditionalDistribution.getConditioningDistribution().getDimension());
  logNormalizationFactor_ = 0.0;
  const UnsignedInteger size(observations_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
    logNormalizationFactor_ += std::log(conditionalDistribution_.computePDF(observations_[i]));
  if (logNormalizationFactor_ == -SpecFunc::MaxNumericalScalar) throw InvalidArgumentException(HERE) << "Error: the normalization factor is null with the given conditional distribution and observations.";
  computeRange();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
}

ConditionalDistribution PosteriorDistribution::getConditionalDistribution() const
{
  return conditionalDistribution_;
}


/* Observations accessor */
void PosteriorDistribution::setObservations(const NumericalSample & observations)
{
  if (observations.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot use a posterior distribution with no observation.";
  if (observations.getDimension() != conditionalDistribution_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the conditioned distribution defining the conditional distribution must have the same dimension as the observations.";
  observations_ = observations;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
}

NumericalSample PosteriorDistribution::getObservations() const
{
  return observations_;
}

/* Log normalization factor accessor */
NumericalScalar PosteriorDistribution::getLogNormalizationFactor() const
{
  return logNormalizationFactor_;
}

/* Compute the numerical range of the distribution given the parameters values */
void PosteriorDistribution::computeRange()
{
  setRange(conditionalDistribution_.conditioningDistribution_.getRange());
}

/* Compute the mean of the distribution */
void PosteriorDistribution::computeMean() const
{
  Description inputDescription(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i) inputDescription[i] = String(OSS() << "x" << i);
  const NumericalMathFunction meanFunction(inputDescription, inputDescription);
  const NumericalMathFunction likelihood(bindMethod<PosteriorDistribution, NumericalPoint, NumericalPoint>(PosteriorDistribution(*this), &PosteriorDistribution::computeLikelihood, getDimension(), 1));
  mean_ = conditionalDistribution_.computeExpectation(likelihood * meanFunction, getRange().getUpperBound()) / std::exp(logNormalizationFactor_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint PosteriorDistribution::getStandardDeviation() const
{
  // To insure that the covariance has been computed
  getCovariance();
  NumericalPoint sigma(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i) sigma[i] = std::sqrt(covariance_(i, i));
  return sigma;
}

/* Get the skewness of the distribution */
NumericalPoint PosteriorDistribution::getSkewness() const
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::getSkewness() const";
}

/* Get the kurtosis of the distribution */
NumericalPoint PosteriorDistribution::getKurtosis() const
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::getKurtosis() const";
}

/* Compute the covariance of the distribution */
void PosteriorDistribution::computeCovariance() const
{
  const UnsignedInteger dimension(getDimension());
  covariance_ = CovarianceMatrix(dimension);
  // To insure that the mean has been computed
  getMean();
  Description inputDescription(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) inputDescription[i] = String(OSS() << "x" << i);
  Description formulas((dimension * (dimension + 1)) / 2);
  UnsignedInteger index(0);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const NumericalScalar muI(mean_[i]);
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      const NumericalScalar muJ(mean_[j]);
      formulas[index] = String(OSS() << "(x" << i << "-" << muI << ")*(x" << j << "-" << muJ << ")");
      ++index;
    }
  }
  const NumericalMathFunction covarianceFunction(inputDescription, formulas);
  const NumericalMathFunction likelihood(bindMethod<PosteriorDistribution, NumericalPoint, NumericalPoint>(PosteriorDistribution(*this), &PosteriorDistribution::computeLikelihood, getDimension(), 1));
  const NumericalPoint result(conditionalDistribution_.computeExpectation(likelihood * covarianceFunction, getRange().getUpperBound()) / std::exp(logNormalizationFactor_));
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
