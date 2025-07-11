//                                               -*- C++ -*-
/**
 *  @brief Simulation algorithm to estimate Sobol indices
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
#include <chrono>

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SobolSimulationAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Normal.hxx"
#include "openturns/SobolIndicesExperiment.hxx"
#include "openturns/SobolIndicesAlgorithm.hxx"
#include "openturns/SaltelliSensitivityAlgorithm.hxx"
#include "openturns/JointDistribution.hxx"
#include "openturns/Dirac.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class SobolSimulationAlgorithm
 */

CLASSNAMEINIT(SobolSimulationAlgorithm)

static const Factory<SobolSimulationAlgorithm> Factory_SobolSimulationAlgorithm;

/** For save/load mechanism */
SobolSimulationAlgorithm::SobolSimulationAlgorithm()
  : SimulationAlgorithm()
  , experimentSize_(ResourceMap::GetAsUnsignedInteger("SobolSimulationAlgorithm-DefaultExperimentSize"))
  , indexQuantileLevel_(ResourceMap::GetAsScalar("SobolSimulationAlgorithm-DefaultIndexQuantileLevel"))
  , indexQuantileEpsilon_(ResourceMap::GetAsScalar("SobolSimulationAlgorithm-DefaultIndexQuantileEpsilon"))
{
}

/* Constructor with parameters */
SobolSimulationAlgorithm::SobolSimulationAlgorithm(const Distribution & distribution,
    const Function & model,
    const SobolIndicesAlgorithm & estimator)
  : SimulationAlgorithm()
  , distribution_(distribution)
  , model_(model)
  , estimator_(estimator)
  , experimentSize_(ResourceMap::GetAsUnsignedInteger("SobolSimulationAlgorithm-DefaultExperimentSize"))
  , indexQuantileLevel_(ResourceMap::GetAsScalar("SobolSimulationAlgorithm-DefaultIndexQuantileLevel"))
  , indexQuantileEpsilon_(ResourceMap::GetAsScalar("SobolSimulationAlgorithm-DefaultIndexQuantileEpsilon"))
{
}

/* Virtual constructor */
SobolSimulationAlgorithm * SobolSimulationAlgorithm::clone() const
{
  return new SobolSimulationAlgorithm(*this);
}

/*  RandomVector accessor */
Distribution SobolSimulationAlgorithm::getDistribution() const
{
  return distribution_;
}

/* Result accessor */
void SobolSimulationAlgorithm::setResult(const SobolSimulationResult & result)
{
  result_ = result;
}

/* Result accessor */
SobolSimulationResult SobolSimulationAlgorithm::getResult() const
{
  return result_;
}

/* String converter */
String SobolSimulationAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << SobolSimulationAlgorithm::GetClassName()
      << " distribution=" << distribution_
      << " model=" << model_
      << " estimator=" << estimator_
      << " maximumOuterSampling=" << getMaximumOuterSampling()
      << " experimentSize=" << getExperimentSize()
      << " blockSize=" << blockSize_;
  return oss;
}

/* Performs the actual computation. */
void SobolSimulationAlgorithm::run()
{
  const UnsignedInteger dimension = model_.getInputDimension();

  // First, reset the convergence history
  convergenceStrategy_.setDimension(4 * dimension);

  UnsignedInteger outerSampling = 0;
  const UnsignedInteger experimentSize = getExperimentSize();
  const UnsignedInteger blockSize = getBlockSize();
  Point meanFO;
  Point meanTO;
  Point varianceFO;
  Point varianceTO;

  // Initialize the result.
  result_ = SobolSimulationResult();
  result_.setOuterSampling(outerSampling);
  result_.setBlockSize(blockSize);
  result_.setExperimentSize(experimentSize);

  Bool stop = false;
  std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();

  // We loop if there remains some outer sampling and the coefficient of variation is greater than the limit or has not been computed yet.
  while ((outerSampling < getMaximumOuterSampling()) && !stop)
  {
    // Perform a block of simulation
    const SobolIndicesExperiment experiment(distribution_, experimentSize, false);
    const Sample inputSample(experiment.generate());
    Sample outputSample(0, model_.getOutputDimension());

    // Evaluate the output per blocks
    const UnsignedInteger totalExperimentSize = inputSample.getSize();
    const UnsignedInteger blockNumber = std::ceil(totalExperimentSize * 1.0 / blockSize);
    UnsignedInteger startIndex = 0;
    for (UnsignedInteger blockIndex = 0; (blockIndex < blockNumber) && !stop; ++ blockIndex)
    {
      const UnsignedInteger endIndex = std::min(startIndex + blockSize, totalExperimentSize);
      const Sample inputBlock(inputSample, startIndex, endIndex);
      startIndex = endIndex;
      const Sample outputBlock(model_(inputBlock));
      outputSample.add(outputBlock);

      std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
      const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0).count();
      result_.setTimeDuration(timeDuration);
      if ((getMaximumTimeDuration() > 0.0) && (timeDuration > getMaximumTimeDuration()))
      {
        LOGINFO(OSS() << "Maximum time exceeded");
        if (outerSampling == 0)
          throw InternalException (HERE) << "Stopped without enough samples";
        stop = true;
      }

      // callbacks
      if (progressCallback_.first)
      {
        // progress in outerSampling loop + blockIndex loop
        progressCallback_.first((100.0 * (1.0 * outerSampling + blockIndex * 1.0 / blockNumber)) / getMaximumOuterSampling(), progressCallback_.second);
      }
      if (!stop && stopCallback_.first)
      {
        stop = stopCallback_.first(stopCallback_.second);
        if (stop)
        {
          if (outerSampling == 0)
            throw InternalException (HERE) << "Stopped without enough samples";
          LOGINFO(OSS() << "Stopped due to user");
          break;
        }
      }
    }

    // Abort if we stopped before completion
    if (outputSample.getSize() < totalExperimentSize)
      break;

    LOGDEBUG(OSS() << "SobolSimulationAlgorithm::run: blockSample=\n" << outputSample);
    ++ outerSampling;

    SobolIndicesAlgorithm estimator(estimator_);
    estimator.setDesign(inputSample, outputSample, experimentSize);
    const Point meanFOBlock(estimator.getAggregatedFirstOrderIndices());
    const Point meanTOBlock(estimator.getAggregatedTotalOrderIndices());
    const Point stddevFOBlock(estimator.getFirstOrderIndicesDistribution().getStandardDeviation());
    const Point stddevTOBlock(estimator.getTotalOrderIndicesDistribution().getStandardDeviation());
    Point varianceFOBlock(dimension);
    Point varianceTOBlock(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      varianceFOBlock[j] = stddevFOBlock[j] * stddevFOBlock[j];
      varianceTOBlock[j] = stddevTOBlock[j] * stddevTOBlock[j];
    }
    const UnsignedInteger size = outerSampling;
    if (outerSampling > 1)
    {
      for (UnsignedInteger j = 0; j < dimension; ++ j)
      {
        varianceFO[j] = (varianceFOBlock[j] + (size - 1.0) * varianceFO[j]) / size + (1.0 - 1.0 / size) * (meanFO[j] - meanFOBlock[j]) * (meanFO[j] - meanFOBlock[j]) / size;
        varianceTO[j] = (varianceTOBlock[j] + (size - 1.0) * varianceTO[j]) / size + (1.0 - 1.0 / size) * (meanTO[j] - meanTOBlock[j]) * (meanTO[j] - meanTOBlock[j]) / size;
      }
      meanFO = (meanFOBlock + (size - 1.0) * meanFO) / size;
      meanTO = (meanTOBlock + (size - 1.0) * meanTO) / size;
    }
    else
    {
      meanFO = meanFOBlock;
      meanTO = meanTOBlock;
      varianceFO = varianceFOBlock;
      varianceTO = varianceTOBlock;
    }

    // Update result
    Point stddevFO(dimension);
    Point stddevTO(dimension);
    const Point reducedVarianceFO(varianceFO / size);
    const Point reducedVarianceTO(varianceTO / size);
    JointDistribution::DistributionCollection marginalsFO(dimension);
    JointDistribution::DistributionCollection marginalsTO(dimension);
    Bool allNormalFO = true;
    Bool allNormalTO = true;
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      stddevFO[j] = sqrt(reducedVarianceFO[j]);
      if (stddevFO[j] > 0.0)
        marginalsFO[j] = Normal(meanFO[j], stddevFO[j]);
      else
      {
        allNormalFO = false;
        marginalsFO[j] = Dirac(meanFO[j]);
      }

      stddevTO[j] = sqrt(reducedVarianceTO[j]);
      if (stddevTO[j] > 0.0)
        marginalsTO[j] = Normal(meanTO[j], stddevTO[j]);
      else
      {
        allNormalTO = false;
        marginalsTO[j] = Dirac(meanTO[j]);
      }
    }

    Distribution firstOrderIndicesDistribution;
    if (allNormalFO)
      firstOrderIndicesDistribution = Normal(meanFO, stddevFO, CorrelationMatrix(dimension));
    else
      firstOrderIndicesDistribution = JointDistribution(marginalsFO);
    firstOrderIndicesDistribution.setDescription(distribution_.getDescription());
    result_.setFirstOrderIndicesDistribution(firstOrderIndicesDistribution);

    Distribution totalOrderIndicesDistribution;
    if (allNormalTO)
      totalOrderIndicesDistribution = Normal(meanTO, stddevTO, CorrelationMatrix(dimension));
    else
      totalOrderIndicesDistribution = JointDistribution(marginalsTO);
    totalOrderIndicesDistribution.setDescription(distribution_.getDescription());
    result_.setTotalOrderIndicesDistribution(totalOrderIndicesDistribution);

    result_.setOuterSampling(outerSampling);
    LOGINFO(OSS() << "SobolSimulationAlgorithm::run: FO=" << firstOrderIndicesDistribution);

    // Display the result at each outer sample
    LOGDEBUG(result_.__repr__());

    Bool converged = true;
    for (UnsignedInteger j = 0; (j < dimension) && converged; ++ j)
    {
      const Distribution distFO(firstOrderIndicesDistribution.getMarginal(j));
      const Distribution distTO(totalOrderIndicesDistribution.getMarginal(j));

      // or, check if F and T confidence lengths are small enough
      const Scalar foConfidenceLength = distFO.computeScalarQuantile(indexQuantileLevel_ * 0.5, true) - distFO.computeScalarQuantile(indexQuantileLevel_ * 0.5);
      const Scalar toConfidenceLength = distTO.computeScalarQuantile(indexQuantileLevel_ * 0.5, true) - distTO.computeScalarQuantile(indexQuantileLevel_ * 0.5);
      const Bool tight = (foConfidenceLength <= indexQuantileEpsilon_) && (toConfidenceLength <= indexQuantileEpsilon_);

      // all indices must 'tight'
      if (!tight)
        converged = false;
    }
    if (!stop && converged)
    {
      stop = true;
      LOGINFO(OSS() << "Stopped -- All indices converged");
    }

    // Update the history
    Point convergencePoint(meanFO);
    convergencePoint.add(meanTO);
    convergencePoint.add(reducedVarianceFO);
    convergencePoint.add(reducedVarianceTO);
    convergenceStrategy_.store(convergencePoint);

    // callbacks
    if (progressCallback_.first)
    {
      progressCallback_.first((100.0 * outerSampling) / getMaximumOuterSampling(), progressCallback_.second);
    }
    if (!stop && stopCallback_.first)
    {
      stop = stopCallback_.first(stopCallback_.second);
      if (stop)
      {
        LOGINFO(OSS() << "Stopped due to user");
      }
    }
  }
}

/** Criterion type on standard deviation accessor */
void SobolSimulationAlgorithm::setIndexQuantileLevel(const Scalar indexQuantileLevel)
{
  indexQuantileLevel_ = indexQuantileLevel;
}

Scalar SobolSimulationAlgorithm::getIndexQuantileLevel() const
{
  return indexQuantileLevel_;
}

/** Maximum of standard deviation on each component */
void SobolSimulationAlgorithm::setIndexQuantileEpsilon(const Scalar indexQuantileEpsilon)
{
  indexQuantileEpsilon_ = indexQuantileEpsilon;
}

Scalar SobolSimulationAlgorithm::getIndexQuantileEpsilon() const
{
  return indexQuantileEpsilon_;
}

/** Estimator accessor */
void SobolSimulationAlgorithm::setEstimator(const SobolIndicesAlgorithm & estimator)
{
  estimator_ = estimator;
}

SobolIndicesAlgorithm SobolSimulationAlgorithm::getEstimator() const
{
  return estimator_;
}

/* Size of experiment accessor */
void SobolSimulationAlgorithm::setExperimentSize(const UnsignedInteger experimentSize)
{
  experimentSize_ = experimentSize;
}

UnsignedInteger SobolSimulationAlgorithm::getExperimentSize() const
{
  return experimentSize_;
}

/* Draw the probability convergence at the given level */
Graph SobolSimulationAlgorithm::drawIndexConvergence(const UnsignedInteger marginalIndex,
    const Scalar level, const String & label) const
{
  // Check if the given level is in ]0, 1[
  if ((level <= 0.0) || (level >= 1.0))
    throw InvalidArgumentException(HERE) << "Confidence level must be in ]0, 1[";

  const Sample convergenceSample(convergenceStrategy_.getSample());
  const UnsignedInteger dimension = convergenceSample.getDimension() / 4;
  if (!(marginalIndex < 2 * dimension))
    throw InvalidDimensionException (HERE) << "Marginal index must be <" << 2 * dimension;
  const UnsignedInteger size = convergenceSample.getSize();
  Sample dataEstimate(size, 2);
  Sample dataLowerBound(0, 2);
  Sample dataUpperBound(0, 2);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    const Scalar expectationEstimate = convergenceSample(i, marginalIndex);
    const Scalar varianceEstimate = convergenceSample(i, 2 * dimension + marginalIndex);
    dataEstimate(i, 0) = i + 1;
    dataEstimate(i, 1) = expectationEstimate;
    // The bounds are drawn only if there is a useable variance estimate
    if (varianceEstimate >= 0.0)
    {
      // The probability estimate is asymptotically normal
      const Scalar xq = DistFunc::qNormal(0.5 + 0.5 * level);
      const Scalar confidenceLength = 2.0 * xq * sqrt(varianceEstimate);

      Point pt(2);
      pt[0] = i + 1;
      pt[1] = expectationEstimate - 0.5 * confidenceLength;
      dataLowerBound.add(pt);
      pt[1] = expectationEstimate + 0.5 * confidenceLength;
      dataUpperBound.add(pt);
    }
  }
  Curve estimateCurve(dataEstimate, "index estimate");
  estimateCurve.setLineWidth(2);
  OSS oss;
  oss << label << " order index convergence graph at level " << level;
  Graph convergenceGraph(oss, "outer iteration", "estimate", true, "topright");
  convergenceGraph.add(estimateCurve);
  const Curve lowerBoundCurve(dataLowerBound, "bounds");
  Curve upperBoundCurve(dataUpperBound);
  convergenceGraph.add(lowerBoundCurve);
  upperBoundCurve.setColor(convergenceGraph.getDrawable(1).getColor());
  convergenceGraph.add(upperBoundCurve);
  return convergenceGraph;
}

Graph SobolSimulationAlgorithm::drawFirstOrderIndexConvergence(const UnsignedInteger marginalIndex,
    const Scalar level) const
{
  return drawIndexConvergence(marginalIndex, level, "First");
}

Graph SobolSimulationAlgorithm::drawTotalOrderIndexConvergence(const UnsignedInteger marginalIndex,
    const Scalar level) const
{
  return drawIndexConvergence(marginalIndex + model_.getInputDimension(), level, "Total");
}

/* Method save() stores the object through the StorageManager */
void SobolSimulationAlgorithm::save(Advocate & adv) const
{
  SimulationAlgorithm::save(adv);
  adv.saveAttribute("distribution_", distribution_);
  adv.saveAttribute("model_", model_);
  adv.saveAttribute("estimator_", estimator_);
  adv.saveAttribute("indexQuantileEpsilon_", indexQuantileEpsilon_);
  adv.saveAttribute("indexQuantileLevel_", indexQuantileLevel_);
  adv.saveAttribute("smallIndexThreshold_", smallIndexThreshold_);
  adv.saveAttribute("result_", result_);
  adv.saveAttribute("experimentSize_", experimentSize_);
}

/* Method load() reloads the object from the StorageManager */
void SobolSimulationAlgorithm::load(Advocate & adv)
{
  SimulationAlgorithm::load(adv);
  adv.loadAttribute("distribution_", distribution_);
  adv.loadAttribute("model_", model_);
  adv.loadAttribute("estimator_", estimator_);
  adv.loadAttribute("indexQuantileEpsilon_", indexQuantileEpsilon_);
  adv.loadAttribute("indexQuantileLevel_", indexQuantileLevel_);
  adv.loadAttribute("smallIndexThreshold_", smallIndexThreshold_);
  adv.loadAttribute("result_", result_);
  if (adv.hasAttribute("experimentSize_"))
    adv.loadAttribute("experimentSize_", experimentSize_);
  else if (adv.hasAttribute("batchSize_"))
  {
    // in OT<1.24 blockSize_ was used as the experiment size
    experimentSize_ = blockSize_;
    // and batchSize_ was the block size
    adv.loadAttribute("batchSize_", blockSize_);
  }
}


END_NAMESPACE_OPENTURNS
