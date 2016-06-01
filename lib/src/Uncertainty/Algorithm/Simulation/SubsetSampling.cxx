//                                               -*- C++ -*-
/**
 *  @brief Subset simulation method
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

static const Factory<SubsetSampling> RegisteredFactory;


/* Default constructor */
SubsetSampling::SubsetSampling()
: Simulation()
, proposalRange_(0.)
, targetProbability_(0.)
, iSubset_(false)
, betaMin_(0.)
, keepEventSample_(false)
, numberOfSteps_(0)
{
}


/* Constructor with parameters */
SubsetSampling::SubsetSampling(const Event & event,
                               const NumericalScalar proposalRange,
                               const NumericalScalar targetProbability)
: Simulation(event)
, proposalRange_(proposalRange)
, targetProbability_(targetProbability)
, iSubset_(false)
, betaMin_(ResourceMap::GetAsNumericalScalar("SubsetSampling-DefaultBetaMin"))
, keepEventSample_(false)
, numberOfSteps_(0)
{
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
  convergenceStrategy_.clear();
  numberOfSteps_ = 0;
  thresholdPerStep_.clear();
  gammaPerStep_.clear();
  coefficientOfVariationPerStep_.clear();
  probabilityEstimatePerStep_.clear();
  eventInputSample_.clear();
  eventOutputSample_.clear();

  dimension_ = getEvent().getAntecedent()->getDimension();

  if (getMaximumCoefficientOfVariation() != ResourceMap::GetAsNumericalScalar("Simulation-DefaultMaximumCoefficientOfVariation"))
    Log::Warn(OSS() << "The maximum coefficient of variation was set. It won't be used as termination criteria.");

  if (targetProbability_ * getMaximumOuterSampling() < 1)
    throw InvalidArgumentException(HERE) << "maximumOuterSampling (" << getMaximumOuterSampling() << ") should be >= " << ceil(1.0 / targetProbability_);

  if (getMaximumOuterSampling() * getBlockSize() <= 100)
    Log::Warn(OSS() << "The number of samples per step is very low : " << getMaximumOuterSampling()*getBlockSize() << ".");

  // perform isoprobabilistic transformation (the study is done in the standard space):
  standardEvent_ = StandardEvent(getEvent());

  NumericalScalar currentCoVsquare = 0.0;
  NumericalScalar varianceEstimate = 0.0;
  NumericalScalar coefficientOfVariationSquare = 0.0;

  // allocate input/output samples
  const UnsignedInteger maximumOuterSampling = getMaximumOuterSampling();
  const UnsignedInteger blockSize = getBlockSize();
  const UnsignedInteger N = maximumOuterSampling * blockSize;
  currentPointSample_ = NumericalSample(N, dimension_);
  currentLevelSample_ = NumericalSample(N, getEvent().getFunction().getOutputDimension());
  
  // Step 1: sampling
  for (UnsignedInteger i = 0; i < maximumOuterSampling; ++ i)
  {
    NumericalSample inputSample;
    if (!iSubset_) {
      // crude MC
      inputSample = standardEvent_.getAntecedent()->getDistribution().getSample(blockSize);
    }
    else {
      // conditional sampling
      TruncatedDistribution truncatedChiSquare(Chi(dimension_), betaMin_, TruncatedDistribution::LOWER);
      Normal normal(dimension_);
      inputSample = NumericalSample(0, dimension_);
      for (UnsignedInteger j = 0; j < blockSize; ++ j)
      {
        NumericalPoint direction(normal.getRealization());
        NumericalScalar norm = direction.norm();
        NumericalScalar radius = truncatedChiSquare.getRealization()[0];
        if (norm != 0.0)
        {
          radius *= 1.0 / norm;
        }
        inputSample.add(direction * radius);
      }
    }
    NumericalSample blockSample(standardEvent_.getFunction()(inputSample));
    for (UnsignedInteger j = 0 ; j < blockSize; ++ j)
    {
      currentPointSample_[i * blockSize + j] = inputSample[j];
      currentLevelSample_[i * blockSize + j] = blockSample[j];
    }
  }
  ++ numberOfSteps_;

  // computation of the first intermediate threshold with the sample create with a normal distribution */
  NumericalScalar currentThreshold = computeThreshold();

  // as long as the conditional failure domain do not overlap the global one
  Bool stop = !getEvent().getOperator()(getEvent().getThreshold(), currentThreshold) || (currentThreshold == getEvent().getThreshold());
  if (stop)
  {
    currentThreshold = getEvent().getThreshold();
  }  
  thresholdPerStep_.add(currentThreshold);

  // compute monte carlo probability estimate
  NumericalScalar probabilityEstimate = computeProbability(1.0, currentThreshold);

  if (iSubset_)
  {
    NumericalScalar correction = ChiSquare(standardEvent_.getImplementation()->getAntecedent()->getDistribution().getDimension()).computeComplementaryCDF(betaMin_ * betaMin_);
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

  gammaPerStep_.add(0.);
  probabilityEstimatePerStep_.add(probabilityEstimate);
  coefficientOfVariationPerStep_.add(coefficientOfVariationSquare);

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
    thresholdPerStep_.add(currentThreshold);

    // compute probability estimate on the current sample and group seeds at the beginning of the work sample
    NumericalScalar currentProbabilityEstimate = computeProbability(probabilityEstimate, currentThreshold);

    // update coefficient of variation 
    NumericalScalar gamma = computeVarianceGamma(currentProbabilityEstimate, currentThreshold);
    currentCoVsquare = (1.0 - currentProbabilityEstimate) / (currentProbabilityEstimate * currentLevelSample_.getSize() * 1.0);
    coefficientOfVariationSquare += (1.0 + gamma) * currentCoVsquare;

    // update probability estimate
    probabilityEstimate *= currentProbabilityEstimate;

    gammaPerStep_.add(gamma);
    probabilityEstimatePerStep_.add(probabilityEstimate);
    coefficientOfVariationPerStep_.add(sqrt(coefficientOfVariationSquare));

    // stop if the number of subset steps is too high, else results are not numerically defined anymore
    if (std::abs(pow(probabilityEstimate, 2.)) < SpecFunc::MinNumericalScalar)
      throw NotDefinedException(HERE) << "Probability estimate too low: " << probabilityEstimate;

    // compute variance estimate
    varianceEstimate = coefficientOfVariationSquare * pow(probabilityEstimate, 2.);

    ++ numberOfSteps_;
  }

  setResult(SubsetSamplingResult(getEvent(), probabilityEstimate, varianceEstimate, numberOfSteps_ * getMaximumOuterSampling(), getBlockSize(), sqrt(coefficientOfVariationSquare)));

  // keep the event sample if requested
  if (keepEventSample_)
  {
    eventInputSample_ = NumericalSample(0, dimension_);  
    eventOutputSample_ = NumericalSample (0, getEvent().getFunction().getOutputDimension());
    for (UnsignedInteger i = 0; i < currentPointSample_.getSize(); ++ i)
    {
      if (getEvent().getOperator()(currentLevelSample_[i][0], getEvent().getThreshold()))
      {
        eventInputSample_.add(standardEvent_.getAntecedent()->getDistribution().getInverseIsoProbabilisticTransformation()(currentPointSample_[i]));
        eventOutputSample_.add(currentLevelSample_[i]);
      }
    }
  }

  // free work samples
  currentLevelSample_.clear();
  currentPointSample_.clear();
}


/* Compute the block sample */
NumericalSample SubsetSampling::computeBlockSample()
{
  return NumericalSample();
}


/* Compute the new threshold corresponding to the target failure probability */
NumericalScalar SubsetSampling::computeThreshold()
{
  // compute the quantile according to the event operator
  NumericalScalar ratio = getEvent().getOperator()(1.0, 2.0) ?  targetProbability_ : 1.0 - targetProbability_;

  NumericalScalar currentThreshold = currentLevelSample_.computeQuantile(ratio)[0];

  return currentThreshold;
}


NumericalScalar SubsetSampling::computeProbability(NumericalScalar probabilityEstimateFactor, NumericalScalar threshold)
{
  const UnsignedInteger maximumOuterSampling = getMaximumOuterSampling();
  const UnsignedInteger blockSize = getBlockSize();
  NumericalScalar probabilityEstimate = 0.0;
  NumericalScalar varianceEstimate = 0.0;

  for (UnsignedInteger i = 0; i < maximumOuterSampling; ++ i)
  {
    const NumericalScalar size = i + 1.0;
    NumericalScalar meanBlock = 0.0;
    NumericalScalar varianceBlock = 0.0;
    for (UnsignedInteger j = 0 ; j < blockSize; ++ j)
    {
      if (getEvent().getOperator()(currentLevelSample_[i * blockSize + j][0], threshold))
      {
        // update local mean and variance
        meanBlock += 1.0 / blockSize;
        varianceBlock += 1.0 * 1.0 / blockSize;
      }
    }
    varianceBlock -= pow(meanBlock, 2.0);   

    // update global mean and variance
    varianceEstimate = (varianceBlock + (size - 1.0) * varianceEstimate) / size + (1.0 - 1.0 / size) * (probabilityEstimate - meanBlock) * (probabilityEstimate - meanBlock) / size;
    probabilityEstimate = (meanBlock + (size - 1.0) * probabilityEstimate) / size;

    // store convergence at each block
    NumericalPoint convergencePoint(2);
    convergencePoint[0] = probabilityEstimate * probabilityEstimateFactor;
    convergencePoint[1] = varianceEstimate * probabilityEstimateFactor * probabilityEstimateFactor / size;
    convergenceStrategy_.store(convergencePoint);
  }

  // cannot determine next subset domain if no variance
  const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("SpecFunc-Precision");
  if (std::abs(varianceEstimate) < epsilon)
    throw NotDefinedException(HERE) << "Null output variance";

  return probabilityEstimate;
}


/* Sort new seeds */
void SubsetSampling::initializeSeed(NumericalScalar threshold)
{
  UnsignedInteger seedIndex = 0;
  const UnsignedInteger maximumOuterSampling = getMaximumOuterSampling();
  const UnsignedInteger blockSize = getBlockSize();
  for (UnsignedInteger i = 0; i < maximumOuterSampling; ++ i)
  {
    for (UnsignedInteger j = 0 ; j < blockSize; ++ j)
    {
      if (getEvent().getOperator()(currentLevelSample_[i * blockSize + j][0], threshold))
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
NumericalScalar SubsetSampling::computeVarianceGamma(NumericalScalar currentFailureProbability, NumericalScalar threshold)
{
  const UnsignedInteger N = currentPointSample_.getSize();
  const UnsignedInteger Nc = std::max<UnsignedInteger>(1, targetProbability_ * N);
  Matrix IndicatorMatrice(Nc, N / Nc);
  NumericalPoint correlationSequence(N / Nc - 1);
  NumericalScalar currentFailureProbability2 = pow(currentFailureProbability, 2.);
  for (UnsignedInteger i = 0; i < N / Nc; ++ i)
  {
    for (UnsignedInteger j = 0; j < Nc; ++ j)
    {
      IndicatorMatrice(j, i) = getEvent().getOperator()(currentLevelSample_[ i*Nc+j ][0], threshold);
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
  const NumericalScalar R0 = currentFailureProbability * (1.0 - currentFailureProbability);
  NumericalPoint rho = ((1.0 / R0) * correlationSequence);
  NumericalScalar gamma = 0.0;
  for (UnsignedInteger k = 0; k < N / Nc - 1; ++ k)
  {
    gamma += 2.0 * (1.0 - (k + 1) * 1.0 * Nc / N) * rho[k];
  }
  return gamma;
}


/* Iterate one step of the algorithm */
void SubsetSampling::generatePoints(NumericalScalar threshold)
{
  UnsignedInteger maximumOuterSampling = getMaximumOuterSampling();
  UnsignedInteger blockSize = getBlockSize();
  Distribution randomWalk(ComposedDistribution(ComposedDistribution::DistributionCollection(dimension_, Uniform(-0.5*proposalRange_, 0.5*proposalRange_))));
  UnsignedInteger N = currentPointSample_.getSize(); // total sample size
  UnsignedInteger Nc = targetProbability_ * N; //number of seeds (also = maximumOuterSampling*blockSize)

  for (UnsignedInteger i = 0; i < maximumOuterSampling; ++ i)
  {
    NumericalSample inputSample(blockSize, dimension_);
    for (UnsignedInteger j = 0; j < blockSize; ++ j)
    {
      // assign the new point to the seed, seed points being regrouped at the beginning of the sample
      if (i*blockSize+j >= Nc)
      {
        currentPointSample_[i * blockSize + j] = currentPointSample_[ i*blockSize+j-Nc ];
        currentLevelSample_[i * blockSize + j] = currentLevelSample_[ i*blockSize+j-Nc ];     
      }

      // generate a new point
      NumericalPoint oldPoint(currentPointSample_[i * blockSize + j]);
      NumericalPoint newPoint(oldPoint + randomWalk.getRealization());

      // 1. accept / reject new components
      NumericalPoint uniform(RandomGenerator::Generate(dimension_));
      for (UnsignedInteger k = 0; k < dimension_; ++ k)
      {
        // compute ratio
        NumericalScalar ratio = exp(0.5 * (oldPoint[k] * oldPoint[k] - newPoint[k] * newPoint[k]));

        // accept new point with probability ratio
        if (ratio < uniform[k])
        {
          newPoint[k] = oldPoint[k];
        }
      }

      inputSample[j] = newPoint;
    }

    NumericalSample blockSample(standardEvent_.getFunction()(inputSample));

    for (UnsignedInteger j = 0; j < getBlockSize(); ++ j)
    {
      // 2. accept the new point if in the failure domain
      if (getEvent().getOperator()(blockSample[j][0], threshold))
      {
        currentPointSample_[i * blockSize + j] = inputSample[j];
        currentLevelSample_[i * blockSize + j] = blockSample[j];
      }
    }
  }
}


/* Markov parameter accessor */
void SubsetSampling::setProposalRange(NumericalScalar proposalRange)
{
  proposalRange_ = proposalRange;
}


NumericalScalar SubsetSampling::getProposalRange() const
{
  return proposalRange_;
}


/* Ratio accessor */
void SubsetSampling::setTargetProbability(NumericalScalar targetProbability)
{
  if ((targetProbability <= 0.) || (targetProbability >= 1.)) throw InvalidArgumentException(HERE) << "Probability should be in (0, 1)";
  targetProbability_ = targetProbability;
}


NumericalScalar SubsetSampling::getTargetProbability() const
{
  return targetProbability_;
}


UnsignedInteger SubsetSampling::getNumberOfSteps()
{
  return numberOfSteps_;
}


NumericalPoint SubsetSampling::getGammaPerStep() const
{
  return gammaPerStep_;
}


NumericalPoint SubsetSampling::getCoefficientOfVariationPerStep() const
{
  return coefficientOfVariationPerStep_;
}


NumericalPoint SubsetSampling::getProbabilityEstimatePerStep() const
{
  return probabilityEstimatePerStep_;
}


NumericalPoint SubsetSampling::getThresholdPerStep() const
{
  return thresholdPerStep_;
}


void SubsetSampling::setKeepEventSample(bool keepEventSample)
{
  keepEventSample_ = keepEventSample;
}


NumericalSample SubsetSampling::getEventInputSample() const
{
  return eventInputSample_;
}


NumericalSample SubsetSampling::getEventOutputSample() const
{
  return eventOutputSample_;
}


void SubsetSampling::setISubset(Bool iSubset)
{
  iSubset_ = iSubset;
}

void SubsetSampling::setBetaMin(NumericalScalar betaMin)
{
  if (betaMin <= 0.) throw InvalidArgumentException(HERE) << "Beta min should be positive";
  betaMin_ = betaMin;
}


/* String converter */
String SubsetSampling::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " derived from " << Simulation::__repr__()
      << " proposalRange=" << proposalRange_
      << " targetProbability=" << targetProbability_
      << " keepEventSample_=" << keepEventSample_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SubsetSampling::save(Advocate & adv) const
{
  Simulation::save(adv);
  adv.saveAttribute("proposalRange_", proposalRange_);
  adv.saveAttribute("targetProbability_", targetProbability_);
  adv.saveAttribute("iSubset_", iSubset_);
  adv.saveAttribute("betaMin_", betaMin_);
  adv.saveAttribute("keepEventSample_", keepEventSample_);  

  adv.saveAttribute("numberOfSteps_", numberOfSteps_);
  adv.saveAttribute("thresholdPerStep_", thresholdPerStep_);
  adv.saveAttribute("gammaPerStep_", gammaPerStep_);
  adv.saveAttribute("coefficientOfVariationPerStep_", coefficientOfVariationPerStep_);
  adv.saveAttribute("probabilityEstimatePerStep_", probabilityEstimatePerStep_);
}


/* Method load() reloads the object from the StorageManager */
void SubsetSampling::load(Advocate & adv)
{
  Simulation::load(adv);
  adv.loadAttribute("proposalRange_", proposalRange_);
  adv.loadAttribute("targetProbability_", targetProbability_);
  adv.loadAttribute("keepEventSample_", keepEventSample_);
  adv.loadAttribute("iSubset_", iSubset_);
  adv.loadAttribute("betaMin_", betaMin_);

  adv.loadAttribute("numberOfSteps_", numberOfSteps_);
  adv.loadAttribute("thresholdPerStep_", thresholdPerStep_);
  adv.loadAttribute("gammaPerStep_", gammaPerStep_);
  adv.loadAttribute("coefficientOfVariationPerStep_", coefficientOfVariationPerStep_);
  adv.loadAttribute("probabilityEstimatePerStep_", probabilityEstimatePerStep_);
}


END_NAMESPACE_OPENTURNS

