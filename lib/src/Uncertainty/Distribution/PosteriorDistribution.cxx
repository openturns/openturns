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
#include "openturns/RandomGenerator.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(PosteriorDistribution)

static const Factory<PosteriorDistribution> Factory_PosteriorDistribution;


/* Default constructor */
PosteriorDistribution::PosteriorDistribution()
  : DistributionImplementation()
  , deconditionedDistribution_()
  , observations_()
{
  setName("PosteriorDistribution");
  // First, set the observations
  observations_ = Sample(1, Point(1, 0.5));
  // Then, set the deconditioned distribution. It also set the dimension.
  setDeconditionedDistribution(deconditionedDistribution_);
}

  PosteriorDistribution::PosteriorDistribution(const Distribution & conditionedDistribution,
					       const Distribution & conditioningDistribution,
					       const Sample & observations)
  : DistributionImplementation()
  , deconditionedDistribution_()
  , observations_(observations)
{
  setName("PosteriorDistribution");
  setDeconditionedDistribution(DeconditionedDistribution(conditionedDistribution, conditioningDistribution));
}

  PosteriorDistribution::PosteriorDistribution(const Distribution & conditionedDistribution,
					       const Distribution & conditioningDistribution,
					       const Function & linkFunction,
					       const Sample & observations)
  : DistributionImplementation()
  , deconditionedDistribution_()
  , observations_(observations)
{
  setName("PosteriorDistribution");
  setDeconditionedDistribution(DeconditionedDistribution(conditionedDistribution, conditioningDistribution, linkFunction));
}

/* Parameters constructor */
PosteriorDistribution::PosteriorDistribution(const DeconditionedDistribution & deconditionedDistribution,
    const Sample & observations)
  : DistributionImplementation()
  , deconditionedDistribution_()
  , observations_(observations)
{
  setName("PosteriorDistribution");
  if (observations.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a posterior distribution with no observation.";
  setDeconditionedDistribution(deconditionedDistribution);
}

/* Comparison operator */
Bool PosteriorDistribution::operator ==(const PosteriorDistribution & other) const
{
  if (this == &other) return true;
  return (deconditionedDistribution_ == other.deconditionedDistribution_) && (observations_ == other.observations_);
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
      << " deconditioned distribution=" << deconditionedDistribution_
      << " observations=" << observations_;
  return oss;
}

String PosteriorDistribution::__str__(const String & offset) const
{
  OSS oss;
  oss << getClassName() << "(deconditioned distribution = " << deconditionedDistribution_.__str__() << ", observations =\n" << offset << observations_.__str__(offset) << ")";
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
  Distribution conditionedDistribution(deconditionedDistribution_.getConditionedDistribution());
  conditionedDistribution.setParameter(deconditionedDistribution_.getLinkFunction()(y));
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
  return deconditionedDistribution_.getConditioningDistribution().computeLogPDF(point) + computeLogNormalizedLikelihood(point);
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
  const Scalar cdf = deconditionedDistribution_.computeExpectation(normalizedLikelihood, point)[0];
  return cdf;
}

/* Sampling */
Point PosteriorDistribution::getRealization() const
{
  if (isContinuous())
  {
    const UnsignedInteger dimension = getDimension();
    if (!infV_.getSize())
      throw InvalidArgumentException(HERE) << "Sampling was not initialized";

    // Now, the sampling using rejection
    Bool accepted = false;
    Point result(dimension);
    while (!accepted)
    {
      const Scalar u = supU_ * RandomGenerator::Generate();
      const Scalar ur = std::pow(u, r_);
      for (UnsignedInteger i = 0; i < dimension; ++ i)
        result[i] = (infV_[i] + (supV_[i] - infV_[i]) * RandomGenerator::Generate()) / ur;
      accepted = (1.0 + r_ * dimension) * std::log(u) <= computeLogPDF(result);
    }
    return result;
  }

  return DistributionImplementation::getRealization();
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

class PosteriorDistributionUBoundEvaluation : public EvaluationImplementation
{
public:
  PosteriorDistributionUBoundEvaluation(const Distribution & distribution, const Scalar r)
    : EvaluationImplementation()
    , distribution_(distribution)
    , r_(r)
  {
    // Nothing to do
  }

  PosteriorDistributionUBoundEvaluation * clone() const override
  {
    return new PosteriorDistributionUBoundEvaluation(*this);
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
    const UnsignedInteger dimension = distribution_.getDimension();
    Scalar result = distribution_.computeLogPDF(inP) / (1.0 + r_ * dimension);
    result = std::max(-SpecFunc::LogMaxScalar, result);
    return {result};
  }

private:
  Distribution distribution_;
  Scalar r_ = 0.0;

};

class PosteriorDistributionVBoundEvaluation : public EvaluationImplementation
{
public:
  PosteriorDistributionVBoundEvaluation(const Distribution & distribution, const Scalar r)
    : EvaluationImplementation()
    , distribution_(distribution)
    , r_(r)
  {
    // Nothing to do
  }

  PosteriorDistributionVBoundEvaluation * clone() const override
  {
    return new PosteriorDistributionVBoundEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return distribution_.getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return distribution_.getDimension();
  }

  Point operator()(const Point & inP) const override
  {
    const UnsignedInteger dimension = distribution_.getDimension();
    const Scalar value = distribution_.computeLogPDF(inP) * r_ / (1.0 + r_ * dimension);
    Point result(dimension, value);
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      result[i] += std::log(std::abs(inP[i]));
      result[i] = std::max(-SpecFunc::LogMaxScalar, result[i]);
    }
    return result;
  }

private:
  Distribution distribution_;
  Scalar r_ = 0.0;

};

/* Deconditioned distribution accessor */
void PosteriorDistribution::setDeconditionedDistribution(const DeconditionedDistribution & deconditionedDistribution)
{
  if (observations_.getDimension() != deconditionedDistribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the conditioned distribution defining the deconditioned distribution must have the same dimension as the observations.";
  deconditionedDistribution_ = deconditionedDistribution;
  setDimension(deconditionedDistribution.getConditioningDistribution().getDimension());
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
  const Bool flag1 = deconditionedDistribution_.getLinkFunction().getEvaluation().getImplementation()->isParallel();
  const Bool flag2 = deconditionedDistribution_.getConditioningDistribution().getImplementation()->isParallel();
  const Bool flag3 = deconditionedDistribution_.getConditionedDistribution().getImplementation()->isParallel();
  isParallel_ = flag1 && flag2 && flag3;
  if (deconditionedDistribution_.getConditioningDistribution().isContinuous())
  {
    const Interval bounds(getRange());
    const Point lb(bounds.getLowerBound());
    const Point ub(bounds.getUpperBound());

    // find a feasible starting point
    SobolSequence sequence(dimension);
    Point start;
    const UnsignedInteger candidateNumber = ResourceMap::GetAsUnsignedInteger("PointConditionalDistribution-RatioUniformCandidateNumber");
    for (UnsignedInteger k = 0; k < candidateNumber; ++ k)
    {
      Point candidate(sequence.generate());
      for (UnsignedInteger j = 0; j < dimension; ++ j)
        candidate[j] = lb[j] + candidate[j] * (ub[j] - lb[j]);
      if (computePDF(candidate) > 0.0)
      if (SpecFunc::IsNormal(computeLogPDF(candidate)))
      {
        start = candidate;
        break;
      }
    }
    if (!start.getDimension())
      throw InternalException(HERE) << "Could not find a feasible starting point to initialize ration of uniforms U sup";

    // First, the upper bound on U
    const Function objectiveU(new PosteriorDistributionUBoundEvaluation(*this, r_));
    OptimizationProblem problemU(objectiveU);
    problemU.setMinimization(false);
    problemU.setBounds(bounds);
    OptimizationAlgorithm algo(OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("PointConditionalDistribution-OptimizationAlgorithm")));
    algo.setProblem(problemU);
    algo.setStartingPoint(start);
    algo.run();
    supU_ = std::exp(algo.getResult().getOptimalValue()[0]);
    LOGDEBUG(OSS() << "supU_=" << supU_ << " u*=" << algo.getResult().getOptimalPoint());

    // Second, the lower and upper bounds on V
    const Function objectiveV(new PosteriorDistributionVBoundEvaluation(*this, r_));
    infV_.resize(dimension);
    supV_.resize(dimension);
    const Point zero(dimension, 0.0);
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      const Function objectiveVI(objectiveV.getMarginal(i));
      OptimizationProblem problemVI(objectiveVI);
      problemVI.setMinimization(false);
      if (ub[i] > 0.0)
      {
        // find a feasible starting point in [0, ub]
        start.clear();
        for (UnsignedInteger k = 0; k < candidateNumber; ++ k)
        {
          Point candidate(sequence.generate());
          for (UnsignedInteger j = 0; j < dimension; ++ j)
            candidate[j] = candidate[j] * ub[j];
	  if (SpecFunc::IsNormal(computeLogPDF(candidate)))
          {
            start = candidate;
            break;
          }
        }
        if (!start.getDimension())
          throw InternalException(HERE) << "Could not find a feasible starting point to initialize ration of uniforms V sup";
        problemVI.setBounds(Interval(zero, ub));
        algo.setProblem(problemVI);
        algo.setStartingPoint(start);
        algo.run();
        supV_[i] = std::exp(algo.getResult().getOptimalValue()[0]);
        LOGDEBUG(OSS() << "supV_[" << i << "]=" << supV_[i] << " v*=" << algo.getResult().getOptimalPoint());
      }
      if (lb[i] < 0.0)
      {
        // find a feasible starting point in [lb, 0]
        start.clear();
        for (UnsignedInteger k = 0; k < candidateNumber; ++ k)
        {
          Point candidate(sequence.generate());
          for (UnsignedInteger j = 0; j < dimension; ++ j)
            candidate[j] = candidate[j] * lb[j];
	  if (SpecFunc::IsNormal(computeLogPDF(candidate)))
          {
            start = candidate;
            break;
          }
        }
        if (!start.getDimension())
          throw InternalException(HERE) << "Could not find a feasible starting point to initialize ration of uniforms V inf";
        problemVI.setBounds(Interval(lb, zero));
        algo.setProblem(problemVI);
        algo.setStartingPoint(start);
        algo.run();
        infV_[i] = -std::exp(algo.getResult().getOptimalValue()[0]);
        LOGDEBUG(OSS() << "infV_[" << i << "]=" << infV_[i] << " v*=" << algo.getResult().getOptimalPoint());
      }
    }
  }
}

DeconditionedDistribution PosteriorDistribution::getDeconditionedDistribution() const
{
  return deconditionedDistribution_;
}

/* ConditionedDistribution distribution accessor */
void PosteriorDistribution::setConditionedDistribution(const Distribution & conditionedDistribution)
{
  deconditionedDistribution_.setConditionedDistribution(conditionedDistribution);
  setDeconditionedDistribution(deconditionedDistribution_);
}

Distribution PosteriorDistribution::getConditionedDistribution() const
{
  return deconditionedDistribution_.getConditionedDistribution();
}

/* ConditioningDistribution distribution accessor */
void PosteriorDistribution::setConditioningDistribution(const Distribution & conditioningDistribution)
{
  deconditionedDistribution_.setConditioningDistribution(conditioningDistribution);
  setDeconditionedDistribution(deconditionedDistribution_);
}

Distribution PosteriorDistribution::getConditioningDistribution() const
{
  return deconditionedDistribution_.getConditioningDistribution();
}

/* linkFunction accessor */
void PosteriorDistribution::setLinkFunction(const Function & linkFunction)
{
  deconditionedDistribution_.setLinkFunction(linkFunction);
  setDeconditionedDistribution(deconditionedDistribution_);
}

Function PosteriorDistribution::getLinkFunction() const
{
  return deconditionedDistribution_.getLinkFunction();
}


/* Observations accessor */
void PosteriorDistribution::setObservations(const Sample & observations)
{
  if (observations.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot use a posterior distribution with no observation.";
  if (observations.getDimension() != deconditionedDistribution_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the conditioned distribution defining the deconditioned distribution must have the same dimension as the observations.";
  observations_ = observations;
  setDeconditionedDistribution(deconditionedDistribution_);
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
  setRange(deconditionedDistribution_.conditioningDistribution_.getRange());
}

/* Compute the mean of the distribution */
void PosteriorDistribution::computeMean() const
{
  const Description inputDescription(Description::BuildDefault(getDimension(), "x"));
  const SymbolicFunction meanFunction(inputDescription, inputDescription);
  const Function normalizedLikelihood(PosteriorDistributionNormalizedLikelihoodEvaluation(*this));
  mean_ = deconditionedDistribution_.computeExpectation(normalizedLikelihood * meanFunction, getRange().getUpperBound());
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
  const Point varThird(deconditionedDistribution_.computeExpectation(normalizedLikelihood * skewnessFunction, getRange().getUpperBound()));
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
  const Point varFourth(deconditionedDistribution_.computeExpectation(normalizedLikelihood * kurtosisFunction, getRange().getUpperBound()));
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
  const Point result(deconditionedDistribution_.computeExpectation(normalizedLikelihood * covarianceFunction, getRange().getUpperBound()));
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
  adv.saveAttribute( "deconditionedDistribution_", deconditionedDistribution_ );
  adv.saveAttribute( "observations_", observations_ );
  adv.saveAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void PosteriorDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "deconditionedDistribution_", deconditionedDistribution_ );
  adv.loadAttribute( "observations_", observations_ );
  adv.loadAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS
