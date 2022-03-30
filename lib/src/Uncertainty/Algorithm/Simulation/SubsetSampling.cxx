//                                               -*- C++ -*-
/**
 *  @brief Subset simulation method
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SubsetSampling.hxx"
#include "openturns/SubsetSamplingResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TruncatedDistribution.hxx"
#include "openturns/Chi.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/Normal.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/RandomGenerator.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SubsetSampling)

static const Factory<SubsetSampling> Factory_SubsetSampling;


/* Default constructor */
SubsetSampling::SubsetSampling()
  : EventSimulation()
  , proposalRange_(0.0)
  , conditionalProbability_(0.0)
  , iSubset_(false)
  , betaMin_(0.0)
  , keepEventSample_(false)
  , minimumProbability_(std::sqrt(SpecFunc::MinScalar))
  , numberOfSteps_(0)
  , seedNumber_(0)
{
}


/* Constructor with parameters */
SubsetSampling::SubsetSampling(const RandomVector & event,
                               const Scalar proposalRange,
                               const Scalar conditionalProbability)
  : EventSimulation(event)
  , proposalRange_(proposalRange)
  , conditionalProbability_(conditionalProbability)
  , iSubset_(false)
  , betaMin_(ResourceMap::GetAsScalar("SubsetSampling-DefaultBetaMin"))
  , keepEventSample_(false)
  , minimumProbability_(std::sqrt(SpecFunc::MinScalar))
  , numberOfSteps_(0)
  , seedNumber_(0)
{
  if (!event.isEvent() || !event.isComposite()) throw InvalidArgumentException(HERE) << "SubsetSampling requires a composite event";
  setMaximumOuterSampling(ResourceMap::GetAsUnsignedInteger("SubsetSampling-DefaultMaximumOuterSampling"));// overide simulation default outersampling
  UnsignedInteger outputDimension = event.getFunction().getOutputDimension();
  if (outputDimension > 1)
    throw InvalidArgumentException(HERE) << "Output dimension for SubsetSampling cannot be greater than 1, here output dimension=" << outputDimension;
}


/* Virtual constructor */
SubsetSampling * SubsetSampling::clone() const
{
  return new SubsetSampling(*this);
}


/* Performs the actual computation. */
void SubsetSampling::run()
{
  // First, initialize some parameters
  convergenceStrategy_.setDimension(2);
  numberOfSteps_ = 0;
  thresholdPerStep_.clear();
  gammaPerStep_.clear();
  coefficientOfVariationPerStep_.clear();
  probabilityEstimatePerStep_.clear();
  eventInputSample_.clear();
  eventOutputSample_.clear();
  dimension_ = getEvent().getAntecedent().getDimension();

  const UnsignedInteger maximumOuterSampling = getMaximumOuterSampling();
  const UnsignedInteger blockSize = getBlockSize();
  const UnsignedInteger N = maximumOuterSampling * blockSize;
  const Scalar epsilon = ResourceMap::GetAsScalar("SpecFunc-Precision");

  if (getMaximumCoefficientOfVariation() != ResourceMap::GetAsScalar("SimulationAlgorithm-DefaultMaximumCoefficientOfVariation"))
    Log::Warn(OSS() << "The maximum coefficient of variation was set. It won't be used as termination criteria.");

  seedNumber_ = static_cast<UnsignedInteger>(conditionalProbability_ * N);
  if (seedNumber_ < 1)
    throw InvalidArgumentException(HERE) << "The number of samples per step (" << N << ") should be >= " << ceil(1.0 / conditionalProbability_);

  // perform isoprobabilistic transformation (the study is done in the standard space):
  standardEvent_ = StandardEvent(getEvent());

  Scalar currentCoVsquare = 0.0;
  Scalar varianceEstimate = 0.0;
  Scalar previousVariance = 0.0;
  Scalar coefficientOfVariationSquare = 0.0;

  // allocate input/output samples
  currentPointSample_ = Sample(N, dimension_);
  currentLevelSample_ = Sample(N, getEvent().getFunction().getOutputDimension());

  // Step 1: sampling
  for (UnsignedInteger i = 0; i < maximumOuterSampling; ++ i)
  {
    Sample inputSample;
    if (!iSubset_)
    {
      // crude MC
      inputSample = standardEvent_.getAntecedent().getDistribution().getSample(blockSize);
    }
    else
    {
      // conditional sampling
      TruncatedDistribution truncatedChiSquare(Chi(dimension_), betaMin_, TruncatedDistribution::LOWER);
      Normal normal(dimension_);
      inputSample = Sample(0, dimension_);
      for (UnsignedInteger j = 0; j < blockSize; ++ j)
      {
        Point direction(normal.getRealization());
        Scalar norm = direction.norm();
        Scalar radius = truncatedChiSquare.getRealization()[0];
        if (norm != 0.0)
        {
          radius *= 1.0 / norm;
        }
        inputSample.add(direction * radius);
      }
    }
    Sample blockSample(standardEvent_.getFunction()(inputSample));
    for (UnsignedInteger j = 0 ; j < blockSize; ++ j)
    {
      currentPointSample_[i * blockSize + j] = inputSample[j];
      currentLevelSample_[i * blockSize + j] = blockSample[j];
    }

    if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
      throw InternalException(HERE) << "User stopped simulation";
  }
  ++ numberOfSteps_;

  // computation of the first intermediate threshold with the sample create with a normal distribution */
  Scalar currentThreshold = computeThreshold();

  // as long as the conditional failure domain do not overlap the global one
  Bool stop = !getEvent().getOperator()(getEvent().getThreshold(), currentThreshold) || (currentThreshold == getEvent().getThreshold());
  if (stop)
  {
    currentThreshold = getEvent().getThreshold();
  }
  thresholdPerStep_.add(currentThreshold);

  // compute monte carlo probability estimate
  Scalar probabilityEstimate = computeProbabilityVariance(1.0, currentThreshold, varianceEstimate);

  // cannot determine next subset domain if no variance
  stop = stop || (std::abs(varianceEstimate) < epsilon);

  if (iSubset_)
  {
    Scalar correction = ChiSquare(standardEvent_.getImplementation()->getAntecedent().getDistribution().getDimension()).computeComplementaryCDF(betaMin_ * betaMin_);
    probabilityEstimate *= correction;
  }

  // if there is no subset step ...
  if (stop)
  {
    if (probabilityEstimate > 0.0)
    {
      // ... compute coefficient of variation
      coefficientOfVariationSquare = (1.0 - probabilityEstimate) / (probabilityEstimate * currentLevelSample_.getSize() * 1.0);
      // ... compute variance estimate
      varianceEstimate = coefficientOfVariationSquare * probabilityEstimate * probabilityEstimate;
    }
  }

  gammaPerStep_.add(0.0);
  probabilityEstimatePerStep_.add(probabilityEstimate);
  coefficientOfVariationPerStep_.add(coefficientOfVariationSquare);

  Log::Info(OSS() << "Subset step #" << numberOfSteps_ << " probability=" << probabilityEstimate << " variance=" << varianceEstimate);
  // as long as the conditional failure domain do not overlap the global one
  while (!stop)
  {
    // prepare new seeds
    initializeSeed(currentThreshold);

    // create new points using MCMC
    generatePoints(currentThreshold);

    // compute new threshold
    currentThreshold = computeThreshold();

    // update stopping criteria
    stop = !getEvent().getOperator()(getEvent().getThreshold(), currentThreshold) || (currentThreshold == getEvent().getThreshold());

    // make sure the last failure domain does not overlap the real failure domain
    if (stop)
    {
      currentThreshold = getEvent().getThreshold();
    }

    // compute probability estimate on the current sample and group seeds at the beginning of the work sample
    previousVariance = varianceEstimate;
    Scalar currentProbabilityEstimate = computeProbabilityVariance(probabilityEstimate, currentThreshold, varianceEstimate);

    // new points are all in the failure domain because the new threshold is too close to the global threshold, the last step was the previous step
    if (std::abs(varianceEstimate) < epsilon)
    {
      varianceEstimate = previousVariance;
      break;
    }

    thresholdPerStep_.add(currentThreshold);

    // update coefficient of variation
    Scalar gamma = computeVarianceGamma(currentProbabilityEstimate, currentThreshold);
    currentCoVsquare = (1.0 - currentProbabilityEstimate) / (currentProbabilityEstimate * currentLevelSample_.getSize() * 1.0);
    coefficientOfVariationSquare += (1.0 + gamma) * currentCoVsquare;

    // update probability estimate
    probabilityEstimate *= currentProbabilityEstimate;

    gammaPerStep_.add(gamma);
    probabilityEstimatePerStep_.add(probabilityEstimate);
    coefficientOfVariationPerStep_.add(sqrt(coefficientOfVariationSquare));

    // stop if the number of subset steps is too high, else results are not numerically defined anymore
    if (probabilityEstimate < minimumProbability_)
      throw NotDefinedException(HERE) << "Probability estimate too small: " << probabilityEstimate;

    // compute variance estimate
    varianceEstimate = coefficientOfVariationSquare * pow(probabilityEstimate, 2.0);

    ++ numberOfSteps_;

    Log::Info(OSS() << "Subset step #" << numberOfSteps_ << " probability=" << probabilityEstimate << " variance=" << varianceEstimate);
  }

  setResult(SubsetSamplingResult(getEvent(), probabilityEstimate, varianceEstimate, numberOfSteps_ * getMaximumOuterSampling(), getBlockSize(), sqrt(coefficientOfVariationSquare)));

  // keep the event sample if requested
  if (keepEventSample_)
  {
    eventInputSample_ = Sample(0, dimension_);
    eventOutputSample_ = Sample (0, getEvent().getFunction().getOutputDimension());
    for (UnsignedInteger i = 0; i < currentPointSample_.getSize(); ++ i)
    {
      if (getEvent().getOperator()(currentLevelSample_(i, 0), getEvent().getThreshold()))
      {
        eventInputSample_.add(getEvent().getAntecedent().getDistribution().getInverseIsoProbabilisticTransformation()(currentPointSample_[i]));
        eventOutputSample_.add(currentLevelSample_[i]);
      }
    }
  }

  // free work samples
  currentLevelSample_.clear();
  currentPointSample_.clear();
}


/* Compute the block sample */
Sample SubsetSampling::computeBlockSample()
{
  return Sample();
}


/* Compute the new threshold corresponding to the conditional failure probability */
Scalar SubsetSampling::computeThreshold()
{
  // compute the quantile according to the event operator
  const Scalar ratio = getEvent().getOperator()(1.0, 2.0) ?  conditionalProbability_ : 1.0 - conditionalProbability_;

  const Scalar currentThreshold = currentLevelSample_.computeQuantile(ratio)[0];

  return currentThreshold;
}


Scalar SubsetSampling::computeProbabilityVariance(Scalar probabilityEstimateFactor, Scalar threshold, Scalar & varianceEstimate)
{
  const UnsignedInteger maximumOuterSampling = getMaximumOuterSampling();
  const UnsignedInteger blockSize = getBlockSize();
  Scalar probabilityEstimate = 0.0;
  varianceEstimate = 0.0;

  for (UnsignedInteger i = 0; i < maximumOuterSampling; ++ i)
  {
    const Scalar size = i + 1.0;
    Scalar meanBlock = 0.0;
    Scalar varianceBlock = 0.0;
    for (UnsignedInteger j = 0 ; j < blockSize; ++ j)
    {
      if (getEvent().getOperator()(currentLevelSample_(i * blockSize + j, 0), threshold))
      {
        // update local mean and variance
        meanBlock += 1.0 / blockSize;
        varianceBlock += 1.0 * 1.0 / blockSize;
      }
    }
    varianceBlock -= pow(meanBlock, 2.0);

    // update global mean and variance
    varianceEstimate = (varianceBlock + (size - 1.0) * varianceEstimate) / size + (1.0 - 1.0 / size) * (probabilityEstimate - meanBlock) * (probabilityEstimate - meanBlock) / size;
    probabilityEstimate = std::min(1.0, (meanBlock + (size - 1.0) * probabilityEstimate) / size);

    // store convergence at each block
    const Point convPt = {probabilityEstimate * probabilityEstimateFactor,
                          varianceEstimate * probabilityEstimateFactor * probabilityEstimateFactor / size
                         };
    convergenceStrategy_.store(convPt);
  }
  return probabilityEstimate;
}


/* Sort new seeds */
void SubsetSampling::initializeSeed(Scalar threshold)
{
  UnsignedInteger seedIndex = 0;
  const UnsignedInteger maximumOuterSampling = getMaximumOuterSampling();
  const UnsignedInteger blockSize = getBlockSize();
  for (UnsignedInteger i = 0; i < maximumOuterSampling; ++ i)
  {
    for (UnsignedInteger j = 0 ; j < blockSize; ++ j)
    {
      if (getEvent().getOperator()(currentLevelSample_(i * blockSize + j, 0), threshold))
      {
        // initialize seeds : they're grouped at the beginning of the sample
        currentPointSample_[ seedIndex ] = currentPointSample_[i * blockSize + j];
        currentLevelSample_[ seedIndex ] = currentLevelSample_[i * blockSize + j];
        ++ seedIndex;
      }
    }
  }
}


/* Compute the correlation on markov chains at the current state of the algorithm */
Scalar SubsetSampling::computeVarianceGamma(Scalar currentFailureProbability, Scalar threshold)
{
  const UnsignedInteger N = currentPointSample_.getSize();
  const UnsignedInteger Nc = seedNumber_;
  Matrix IndicatorMatrice(Nc, N / Nc);
  Point correlationSequence(N / Nc - 1);
  Scalar currentFailureProbability2 = pow(currentFailureProbability, 2.);
  for (UnsignedInteger i = 0; i < N / Nc; ++ i)
  {
    for (UnsignedInteger j = 0; j < Nc; ++ j)
    {
      IndicatorMatrice(j, i) = getEvent().getOperator()(currentLevelSample_(i * Nc + j, 0), threshold);
    }
  }
  for (UnsignedInteger k = 0; k < N / Nc - 1; ++ k)
  {
    for (UnsignedInteger j = 0; j < Nc; ++ j)
    {
      for (UnsignedInteger l = 0; l < N / Nc - k - 1; ++ l)
      {
        correlationSequence[k] += 1.0 * IndicatorMatrice(j, l) * IndicatorMatrice(j, l + (k + 1));
      }
    }
    correlationSequence[k] /= 1.0 * N - 1.0 * (k + 1) * Nc;
    correlationSequence[k] -= currentFailureProbability2;
  }
  const Scalar R0 = currentFailureProbability * (1.0 - currentFailureProbability);
  const Point rho = ((1.0 / R0) * correlationSequence);
  Scalar gamma = 0.0;
  for (UnsignedInteger k = 0; k < N / Nc - 1; ++ k)
  {
    gamma += 2.0 * (1.0 - (k + 1) * 1.0 * Nc / N) * rho[k];
  }
  return gamma;
}


/* Iterate one step of the algorithm */
void SubsetSampling::generatePoints(Scalar threshold)
{
  const UnsignedInteger maximumOuterSampling = getMaximumOuterSampling();
  const UnsignedInteger blockSize = getBlockSize();
  const Distribution randomWalk(ComposedDistribution(ComposedDistribution::DistributionCollection(dimension_, Uniform(-0.5 * proposalRange_, 0.5 * proposalRange_))));
  const UnsignedInteger Nc = seedNumber_;

  for (UnsignedInteger i = 0; i < maximumOuterSampling; ++ i)
  {
    Sample inputSample(blockSize, dimension_);
    for (UnsignedInteger j = 0; j < blockSize; ++ j)
    {
      // assign the new point to the seed, seed points being regrouped at the beginning of the sample
      if (i * blockSize + j >= Nc)
      {
        currentPointSample_[i * blockSize + j] = currentPointSample_[ i * blockSize + j - Nc ];
        currentLevelSample_[i * blockSize + j] = currentLevelSample_[ i * blockSize + j - Nc ];
      }

      // generate a new point
      Point oldPoint(currentPointSample_[i * blockSize + j]);
      Point newPoint(oldPoint + randomWalk.getRealization());

      // 1. accept / reject new components
      Point uniform(RandomGenerator::Generate(dimension_));
      for (UnsignedInteger k = 0; k < dimension_; ++ k)
      {
        // compute ratio
        const Scalar ratio = exp(0.5 * (oldPoint[k] * oldPoint[k] - newPoint[k] * newPoint[k]));

        // accept new point with probability ratio
        if (ratio < uniform[k])
        {
          newPoint[k] = oldPoint[k];
        }
      }

      inputSample[j] = newPoint;
    }

    Sample blockSample(standardEvent_.getFunction()(inputSample));

    for (UnsignedInteger j = 0; j < getBlockSize(); ++ j)
    {
      // 2. accept the new point if in the failure domain
      if (getEvent().getOperator()(blockSample(j, 0), threshold))
      {
        currentPointSample_[i * blockSize + j] = inputSample[j];
        currentLevelSample_[i * blockSize + j] = blockSample[j];
      }
    }

    if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
      throw InternalException(HERE) << "User stopped simulation";
  }
}


/* Markov parameter accessor */
void SubsetSampling::setProposalRange(Scalar proposalRange)
{
  proposalRange_ = proposalRange;
}


Scalar SubsetSampling::getProposalRange() const
{
  return proposalRange_;
}


/* Ratio accessor */
void SubsetSampling::setConditionalProbability(Scalar conditionalProbability)
{
  if (!(conditionalProbability > 0.0) || !(conditionalProbability < 1.0)) throw InvalidArgumentException(HERE) << "Probability should be in (0, 1)";
  conditionalProbability_ = conditionalProbability;
}


Scalar SubsetSampling::getConditionalProbability() const
{
  return conditionalProbability_;
}


void SubsetSampling::setMinimumProbability(const Scalar minimumProbability)
{
  if (!(minimumProbability > 0.0) || !(minimumProbability < 1.0))
    throw InvalidArgumentException(HERE) << "Minimum probability should be in (0, 1)";
  minimumProbability_ = minimumProbability;
}


Scalar SubsetSampling::getMinimumProbability() const
{
  return minimumProbability_;
}


UnsignedInteger SubsetSampling::getStepsNumber()
{
  return numberOfSteps_;
}


Point SubsetSampling::getGammaPerStep() const
{
  return gammaPerStep_;
}


Point SubsetSampling::getCoefficientOfVariationPerStep() const
{
  return coefficientOfVariationPerStep_;
}


Point SubsetSampling::getProbabilityEstimatePerStep() const
{
  return probabilityEstimatePerStep_;
}


Point SubsetSampling::getThresholdPerStep() const
{
  return thresholdPerStep_;
}


void SubsetSampling::setKeepEventSample(bool keepEventSample)
{
  keepEventSample_ = keepEventSample;
}


Sample SubsetSampling::getEventInputSample() const
{
  return eventInputSample_;
}


Sample SubsetSampling::getEventOutputSample() const
{
  return eventOutputSample_;
}


void SubsetSampling::setISubset(Bool iSubset)
{
  iSubset_ = iSubset;
}

void SubsetSampling::setBetaMin(Scalar betaMin)
{
  if (!(betaMin > 0.0)) throw InvalidArgumentException(HERE) << "Beta min should be positive";
  betaMin_ = betaMin;
}


/* String converter */
String SubsetSampling::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " derived from " << EventSimulation::__repr__()
      << " proposalRange=" << proposalRange_
      << " conditionalProbability=" << conditionalProbability_
      << " keepEventSample_=" << keepEventSample_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SubsetSampling::save(Advocate & adv) const
{
  EventSimulation::save(adv);
  adv.saveAttribute("proposalRange_", proposalRange_);
  adv.saveAttribute("conditionalProbability_", conditionalProbability_);
  adv.saveAttribute("iSubset_", iSubset_);
  adv.saveAttribute("betaMin_", betaMin_);
  adv.saveAttribute("keepEventSample_", keepEventSample_);
  adv.saveAttribute("minimumProbability_", minimumProbability_);

  adv.saveAttribute("numberOfSteps_", numberOfSteps_);
  adv.saveAttribute("thresholdPerStep_", thresholdPerStep_);
  adv.saveAttribute("gammaPerStep_", gammaPerStep_);
  adv.saveAttribute("coefficientOfVariationPerStep_", coefficientOfVariationPerStep_);
  adv.saveAttribute("probabilityEstimatePerStep_", probabilityEstimatePerStep_);
}


/* Method load() reloads the object from the StorageManager */
void SubsetSampling::load(Advocate & adv)
{
  EventSimulation::load(adv);
  adv.loadAttribute("proposalRange_", proposalRange_);
  adv.loadAttribute("conditionalProbability_", conditionalProbability_);
  adv.loadAttribute("iSubset_", iSubset_);
  adv.loadAttribute("betaMin_", betaMin_);
  adv.loadAttribute("keepEventSample_", keepEventSample_);
  adv.loadAttribute("minimumProbability_", minimumProbability_);

  adv.loadAttribute("numberOfSteps_", numberOfSteps_);
  adv.loadAttribute("thresholdPerStep_", thresholdPerStep_);
  adv.loadAttribute("gammaPerStep_", gammaPerStep_);
  adv.loadAttribute("coefficientOfVariationPerStep_", coefficientOfVariationPerStep_);
  adv.loadAttribute("probabilityEstimatePerStep_", probabilityEstimatePerStep_);
}


END_NAMESPACE_OPENTURNS

