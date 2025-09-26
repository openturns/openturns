//                                               -*- C++ -*-
/**
 *  @brief The PosteriorDistribution distribution
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/GaussLegendre.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(PosteriorDistribution)

static const Factory<PosteriorDistribution> Factory_PosteriorDistribution;


/* Default constructor */
PosteriorDistribution::PosteriorDistribution()
  : DistributionImplementation()
{
  setName("PosteriorDistribution");
  // First, set the observations
  observations_ = Sample(1, Point({0.5}));
  // Then, set the compound distribution. It also set the dimension.
  setCompoundDistribution(compoundDistribution_);
}

PosteriorDistribution::PosteriorDistribution(const Distribution & conditionedDistribution,
    const Distribution & conditioningDistribution,
    const Sample & observations)
  : DistributionImplementation()
  , observations_(observations)
{
  setName("PosteriorDistribution");
  setCompoundDistribution(CompoundDistribution(conditionedDistribution, conditioningDistribution));
}

PosteriorDistribution::PosteriorDistribution(const Distribution & conditionedDistribution,
    const Distribution & conditioningDistribution,
    const Function & linkFunction,
    const Sample & observations)
  : DistributionImplementation()
  , compoundDistribution_()
  , observations_(observations)
{
  setName("PosteriorDistribution");
  setCompoundDistribution(CompoundDistribution(conditionedDistribution, conditioningDistribution, linkFunction));
}

/* Parameters constructor */
PosteriorDistribution::PosteriorDistribution(const CompoundDistribution & compoundDistribution,
    const Sample & observations)
  : DistributionImplementation()
  , compoundDistribution_()
  , observations_(observations)
{
  setName("PosteriorDistribution");
  if (observations.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a posterior distribution with no observation.";
  setCompoundDistribution(compoundDistribution);
}

/* Comparison operator */
Bool PosteriorDistribution::operator ==(const PosteriorDistribution & other) const
{
  if (this == &other) return true;
  return (compoundDistribution_ == other.compoundDistribution_) && (observations_ == other.observations_);
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
      << " compound distribution=" << compoundDistribution_
      << " observations=" << observations_;
  return oss;
}

String PosteriorDistribution::__str__(const String & offset) const
{
  OSS oss;
  oss << getClassName() << "(compound distribution = " << compoundDistribution_.__str__() << ", observations =\n" << offset << observations_.__str__(offset) << ")";
  return oss;
}

/* Virtual constructor */
PosteriorDistribution * PosteriorDistribution::clone() const
{
  return new PosteriorDistribution(*this);
}

/* Compute the normalized likelihood of the observations */
Point PosteriorDistribution::computeNormalizedLikelihood(const Point & y) const
{
  return Point(1, std::exp(computeLogNormalizedLikelihood(y)));
}

/* Compute the log-normalized likelihood of the observations */
Scalar PosteriorDistribution::computeLogNormalizedLikelihood(const Point & y) const
{
  Distribution conditionedDistribution(compoundDistribution_.getConditionedDistribution());
  conditionedDistribution.setParameter(compoundDistribution_.getLinkFunction()(y));
  return -logNormalizationFactor_ + conditionedDistribution.computeLogPDF(observations_).computeMean()[0] * observations_.getSize();
}

class PosteriorDistributionNormalizedLikelihoodEvaluation : public EvaluationImplementation
{
public:
  PosteriorDistributionNormalizedLikelihoodEvaluation(const PosteriorDistribution & distribution) : EvaluationImplementation(), distribution_(distribution) {}
  PosteriorDistributionNormalizedLikelihoodEvaluation * clone() const override
  {
    return new PosteriorDistributionNormalizedLikelihoodEvaluation(*this);
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
    return distribution_.computeNormalizedLikelihood(inP);
  }

private:
  const PosteriorDistribution & distribution_;
};

/* Get the log PDF of the distribution */
Scalar PosteriorDistribution::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  return compoundDistribution_.getConditioningDistribution().computeLogPDF(point) + computeLogNormalizedLikelihood(point);
}

/* Get the PDF of the distribution */
Scalar PosteriorDistribution::computePDF(const Point & point) const
{
  return std::exp(computeLogPDF(point));
}

/* Get the CDF of the distribution */
Scalar PosteriorDistribution::computeCDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  const Function normalizedLikelihood(PosteriorDistributionNormalizedLikelihoodEvaluation(*this));
  const Scalar cdf = compoundDistribution_.computeExpectation(normalizedLikelihood, point)[0];
  return cdf;
}

/* Sampling */
Point PosteriorDistribution::getRealization() const
{
  // If the distribution is continuous, use the ratio of uniforms method
  if (isContinuous() && sampler_.isInitialized())
    return sampler_.getRealization();

  return DistributionImplementation::getRealization();
}

Sample PosteriorDistribution::getSample(const UnsignedInteger size) const
{
  // If the distribution is continuous, use the ratio of uniforms method
  if (isContinuous() && sampler_.isInitialized())
    return sampler_.getSample(size);

  return DistributionImplementation::getSample(size);
}

/* Check if the distribution is constinuous */
Bool PosteriorDistribution::isContinuous() const
{
  return compoundDistribution_.getConditioningDistribution().isContinuous();
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

/* Compound distribution accessor */
void PosteriorDistribution::setCompoundDistribution(const CompoundDistribution & compoundDistribution)
{
  if (observations_.getDimension() != compoundDistribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the conditioned distribution defining the compound distribution must have the same dimension as the observations.";
  compoundDistribution_ = compoundDistribution;
  setDimension(compoundDistribution.getConditioningDistribution().getDimension());
  setDescription(compoundDistribution.getConditioningDistribution().getDescription());
  // This must be done before to call computeCDF() for the normalization factor
  computeRange();
  // Fix the log-normalization at zero to compute its actual value using computeCDF()
  Scalar logScaling = 0.0;
  Bool done = false;
  UnsignedInteger maxIter = 10;
  UnsignedInteger iter = 0;
  while (!done)
  {
    ++iter;
    logNormalizationFactor_ = logScaling;
    logNormalizationFactor_ = std::log(computeCDF(range_.getUpperBound())) + logScaling;
    done = SpecFunc::IsNormal(logNormalizationFactor_) || (iter == maxIter);
    if (logNormalizationFactor_ < 0.0)
      logScaling -= std::pow(2.0, iter);
    else
      logScaling += std::pow(2.0, iter);
  }
  if (!SpecFunc::IsNormal(logNormalizationFactor_))
    throw InvalidArgumentException(HERE) << "Error: unable to compute the log-normalization factor despite a rescaling of " << logScaling;

  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isParallel_ = compoundDistribution_.getLinkFunction().getEvaluation().getImplementation()->isParallel()
                && compoundDistribution_.getConditioningDistribution().getImplementation()->isParallel()
                && compoundDistribution_.getConditionedDistribution().getImplementation()->isParallel();
  if (compoundDistribution_.getConditioningDistribution().isContinuous())
  {
    // initialize ratio of uniforms method, see https://en.wikipedia.org/wiki/Ratio_of_uniforms
    // r_ is a free parameter, could be optimized to maximize the acceptance ratio
    sampler_ = RatioOfUniforms();
    sampler_.setOptimizationAlgorithm(OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("PosteriorDistribution-OptimizationAlgorithm")));
    sampler_.setCandidateNumber(ResourceMap::GetAsUnsignedInteger("PosteriorDistribution-RatioUniformCandidateNumber"));
    sampler_.setLogUnscaledPDFAndRange(getLogPDF(), getRange(), true);
  } // isContinuous()
}

CompoundDistribution PosteriorDistribution::getCompoundDistribution() const
{
  return compoundDistribution_;
}

/* ConditionedDistribution distribution accessor */
void PosteriorDistribution::setConditionedDistribution(const Distribution & conditionedDistribution)
{
  compoundDistribution_.setConditionedDistribution(conditionedDistribution);
  setCompoundDistribution(compoundDistribution_);
}

Distribution PosteriorDistribution::getConditionedDistribution() const
{
  return compoundDistribution_.getConditionedDistribution();
}

/* ConditioningDistribution distribution accessor */
void PosteriorDistribution::setConditioningDistribution(const Distribution & conditioningDistribution)
{
  compoundDistribution_.setConditioningDistribution(conditioningDistribution);
  setCompoundDistribution(compoundDistribution_);
}

Distribution PosteriorDistribution::getConditioningDistribution() const
{
  return compoundDistribution_.getConditioningDistribution();
}

/* linkFunction accessor */
void PosteriorDistribution::setLinkFunction(const Function & linkFunction)
{
  compoundDistribution_.setLinkFunction(linkFunction);
  setCompoundDistribution(compoundDistribution_);
}

Function PosteriorDistribution::getLinkFunction() const
{
  return compoundDistribution_.getLinkFunction();
}


/* Observations accessor */
void PosteriorDistribution::setObservations(const Sample & observations)
{
  if (observations.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot use a posterior distribution with no observation.";
  if (observations.getDimension() != compoundDistribution_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the conditioned distribution defining the compound distribution must have the same dimension as the observations.";
  observations_ = observations;
  setCompoundDistribution(compoundDistribution_);
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
  setRange(compoundDistribution_.conditioningDistribution_.getRange());
}

/* Compute the mean of the distribution */
void PosteriorDistribution::computeMean() const
{
  const Description inputDescription(Description::BuildDefault(getDimension(), "x"));
  const SymbolicFunction meanFunction(inputDescription, inputDescription);
  const Function normalizedLikelihood(PosteriorDistributionNormalizedLikelihoodEvaluation(*this));
  mean_ = compoundDistribution_.computeExpectation(normalizedLikelihood * meanFunction, getRange().getUpperBound());
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
  const Point mean(getMean());
  // Here we build a symbolic function to compute in one pass both the marginal
  // variances and the centered third order moments
  const UnsignedInteger dimension(getDimension());
  const Description inputDescription(Description::BuildDefault(getDimension(), "x"));
  Description formulas(2 * dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    formulas[2 * i    ] = OSS() << "(" << inputDescription[i] << "-(" << mean[i] << "))^2";
    formulas[2 * i + 1] = OSS() << "(" << inputDescription[i] << "-(" << mean[i] << "))^3";
  }
  const SymbolicFunction skewnessFunction(inputDescription, formulas);
  const Function normalizedLikelihood(PosteriorDistributionNormalizedLikelihoodEvaluation(*this));
  const Point varThird(compoundDistribution_.computeExpectation(normalizedLikelihood * skewnessFunction, getRange().getUpperBound()));
  Point skewness(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    skewness[i] = varThird[2 * i + 1] / std::pow(varThird[2 * i], 1.5);
  return skewness;
}

/* Get the kurtosis of the distribution */
Point PosteriorDistribution::getKurtosis() const
{
  const Point mean(getMean());
  // Here we build a symbolic function to compute in one pass both the marginal
  // variances and the centered third order moments
  const UnsignedInteger dimension(getDimension());
  const Description inputDescription(Description::BuildDefault(getDimension(), "x"));
  Description formulas(2 * dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    formulas[2 * i    ] = OSS() << "(" << inputDescription[i] << "-(" << mean[i] << "))^2";
    formulas[2 * i + 1] = OSS() << "(" << inputDescription[i] << "-(" << mean[i] << "))^4";
  }
  const SymbolicFunction kurtosisFunction(inputDescription, formulas);
  const Function normalizedLikelihood(PosteriorDistributionNormalizedLikelihoodEvaluation(*this));
  const Point varFourth(compoundDistribution_.computeExpectation(normalizedLikelihood * kurtosisFunction, getRange().getUpperBound()));
  Point kurtosis(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    kurtosis[i] = varFourth[2 * i + 1] / std::pow(varFourth[2 * i], 2.0);
  return kurtosis;
}

/* Compute the covariance of the distribution */
void PosteriorDistribution::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  // To insure that the mean has been computed
  getMean();
  const Description inputDescription(Description::BuildDefault(dimension, "x"));
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
  const Function normalizedLikelihood(PosteriorDistributionNormalizedLikelihoodEvaluation(*this));
  const Point result(compoundDistribution_.computeExpectation(normalizedLikelihood * covarianceFunction, getRange().getUpperBound()));
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
  DistributionImplementation::save(adv);
  adv.saveAttribute( "compoundDistribution_", compoundDistribution_ );
  adv.saveAttribute( "observations_", observations_ );
  adv.saveAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void PosteriorDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "compoundDistribution_", compoundDistribution_ );
  adv.loadAttribute( "observations_", observations_ );
  setCompoundDistribution(compoundDistribution_);
}


END_NAMESPACE_OPENTURNS
