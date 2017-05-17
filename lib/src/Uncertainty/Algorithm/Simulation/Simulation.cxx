//                                               -*- C++ -*-
/**
 *  @brief Simulation is a generic view of simulation methods for computing
 * probabilities and related quantities by sampling and estimation
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Simulation.hxx"
#include "openturns/Log.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class Simulation
 */

CLASSNAMEINIT(Simulation);

static const Factory<Simulation> Factory_Simulation;

/** For save/load mechanism */
Simulation::Simulation(const Bool verbose, const HistoryStrategy & convergenceStrategy)
  : PersistentObject()
  , convergenceStrategy_(convergenceStrategy)
  , blockSize_(ResourceMap::GetAsUnsignedInteger( "Simulation-DefaultBlockSize" ))
  , event_()
  , result_()
  , progressCallback_(std::make_pair<ProgressCallback, void *>(0, 0))
  , stopCallback_(std::make_pair<StopCallback, void *>(0, 0))
  , maximumOuterSampling_(ResourceMap::GetAsUnsignedInteger( "Simulation-DefaultMaximumOuterSampling" ))
  , maximumCoefficientOfVariation_(ResourceMap::GetAsScalar( "Simulation-DefaultMaximumCoefficientOfVariation" ))
  , maximumStandardDeviation_(ResourceMap::GetAsScalar( "Simulation-DefaultMaximumStandardDeviation" ))
  , verbose_(verbose)
{
  // Nothing to do
}

/* Constructor with parameters */
Simulation::Simulation(const Event & event,
                       const Bool verbose,
                       const HistoryStrategy & convergenceStrategy)
  : PersistentObject()
  , convergenceStrategy_(convergenceStrategy)
  , blockSize_(ResourceMap::GetAsUnsignedInteger( "Simulation-DefaultBlockSize" ))
  , event_(event)
  , result_()
  , progressCallback_(std::make_pair<ProgressCallback, void *>(0, 0))
  , stopCallback_(std::make_pair<StopCallback, void *>(0, 0))
  , maximumOuterSampling_(ResourceMap::GetAsUnsignedInteger( "Simulation-DefaultMaximumOuterSampling" ))
  , maximumCoefficientOfVariation_(ResourceMap::GetAsScalar( "Simulation-DefaultMaximumCoefficientOfVariation" ))
  , maximumStandardDeviation_(ResourceMap::GetAsScalar( "Simulation-DefaultMaximumStandardDeviation" ))
  , verbose_(verbose)
{
  // Nothing to do
}

/* Virtual constructor */
Simulation * Simulation::clone() const
{
  return new Simulation(*this);
}

/*  Event accessor */
Event Simulation::getEvent() const
{
  return event_;
}

/* Result accessor */
void Simulation::setResult(const SimulationResult & result)
{
  result_ = result;
}

/* Result accessor */
SimulationResult Simulation::getResult() const
{
  return result_;
}

/* Maximum sample size accessor */
void Simulation::setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling)
{
  maximumOuterSampling_ = maximumOuterSampling;
}

/* Maximum sample size accessor */
UnsignedInteger Simulation::getMaximumOuterSampling() const
{
  return maximumOuterSampling_;
}

/* Maximum coefficient of variation accessor */
void Simulation::setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation)
{
  // Check if the given coefficient of variation is >= 0
  //      if (!(maximumCoefficientOfVariation >= 0.0)) throw InvalidArgumentException(HERE) << "The maximum coefficient of variation must be >= 0.0";
  maximumCoefficientOfVariation_ = maximumCoefficientOfVariation;
}

/* Maximum coefficient of variation accessor */
Scalar Simulation::getMaximumCoefficientOfVariation() const
{
  return maximumCoefficientOfVariation_;
}

/* Maximum standard deviation accessor */
void Simulation::setMaximumStandardDeviation(const Scalar maximumStandardDeviation)
{
  maximumStandardDeviation_ = maximumStandardDeviation;
}

Scalar Simulation::getMaximumStandardDeviation() const
{
  return maximumStandardDeviation_;
}

/* Block size accessor */
void Simulation::setBlockSize(const UnsignedInteger blockSize)
{
  // Check if the given block size is >= 1
  if (blockSize < 1) throw InvalidArgumentException(HERE) << "The block size must be >= 1";
  blockSize_ = blockSize;
}

/* Block size accessor */
UnsignedInteger Simulation::getBlockSize() const
{
  return blockSize_;
}

/* Verbosity accessor */
void Simulation::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}

/* Verbosity accessor */
Bool Simulation::getVerbose() const
{
  return verbose_;
}

/* String converter */
String Simulation::__repr__() const
{
  OSS oss;
  oss << "class=" << Simulation::GetClassName()
      << " event=" << event_
      << " maximumOuterSampling=" << maximumOuterSampling_
      << " maximumCoefficientOfVariation=" << maximumCoefficientOfVariation_
      << " maximumStandardDeviation=" << maximumStandardDeviation_
      << " blockSize=" << blockSize_;
  return oss;
}

/* Performs the actual computation. */
void Simulation::run()
{
  /* We want to compute the probability of occurence of the given event
   *  We estimate this probability by computing the empirical mean of a
   * sample of size at most outerSampling * blockSize, this sample being
   * built by blocks of size blockSize. It allows to use efficiently the
   * distribution of the computation as well as it allows to deal with
   * a sample size > 2^32 by a combination of blockSize and outerSampling
   */
  // First, reset the convergence history
  convergenceStrategy_.clear();
  UnsignedInteger outerSampling = 0;
  Scalar coefficientOfVariation = -1.0;
  Scalar standardDeviation = -1.0;
  Scalar probabilityEstimate = 0.0;
  Scalar varianceEstimate = 0.0;
  const UnsignedInteger blockSize = getBlockSize();
  // Initialize the result. We use the accessors in order to preserve the exact nature of the result (SimulationResult or QuasiMonteCarloResult)
  // First, the invariant part
  // For the event, we have to access to the implementation as the interface does not provide the setEvent() method ON PURPOSE!
  result_.getImplementation()->setEvent(event_);
  result_.setBlockSize(blockSize_);
  // Second, the variant part
  result_.setProbabilityEstimate(probabilityEstimate);
  result_.setVarianceEstimate(varianceEstimate);
  result_.setOuterSampling(outerSampling);

  Bool stop = false;
  // We loop if there remains some outer sampling and the coefficient of variation is greater than the limit or has not been computed yet.
  while ((outerSampling < getMaximumOuterSampling()) && ((coefficientOfVariation == -1.0) || (coefficientOfVariation > getMaximumCoefficientOfVariation())) && ((standardDeviation == -1.0) || (standardDeviation > getMaximumStandardDeviation())) && !stop)
  {
    // Perform a block of simulation
    const Sample blockSample(computeBlockSample());
    LOGDEBUG(OSS() << "Simulation::run: blockSample=\n" << blockSample);
    ++outerSampling;
    // Then, actualize the estimates
    const Scalar meanBlock = blockSample.computeMean()[0];
    const Scalar varianceBlock = blockSample.computeCovariance()(0, 0);
    // Let Skp be the empirical variance of a sample of size k*p
    // Let Mkp be the empirical mean of a sample of size k*p
    // Let Sp be the empirical variance of a sample of size p
    // Let Mp be the empirical mean of a sample of size p
    // Then, the empirical variance of the concatenated sample of size (k+1)*p is
    // S(k+1)p = (Sp + k * Skp) / (k + 1) + k * (Mkp - Mp)^2 / (k + 1)^2
    // and the empirical mean of the concatenated sample of size (k+1)*p is
    // M(k+1)p = (Mp + k * Mkp) / (k + 1)
    // To avoid integer overflow and double loss of precision, the formulas have to be written the way they are
    const Scalar size = outerSampling;
    varianceEstimate = (varianceBlock + (size - 1.0) * varianceEstimate) / size + (1.0 - 1.0 / size) * (probabilityEstimate - meanBlock) * (probabilityEstimate - meanBlock) / size;
    probabilityEstimate = (meanBlock + (size - 1.0) * probabilityEstimate) / size;
    const Scalar reducedVarianceEstimate = varianceEstimate / (size * blockSize);
    // Update result
    result_.setProbabilityEstimate(probabilityEstimate);
    result_.setVarianceEstimate(reducedVarianceEstimate);
    result_.setOuterSampling(outerSampling);
    // Display the result at each outer sample
    if (verbose_) LOGINFO(result_.__repr__());
    // Get the coefficient of variation back
    // We use the result to compute these quantities in order to
    // delegate the treatment of the degenerate cases (i.e. the
    // variance estimate is 0)
    coefficientOfVariation = result_.getCoefficientOfVariation();
    standardDeviation = result_.getStandardDeviation();
    // Update the history
    Point convergencePoint(2);
    convergencePoint[0] = probabilityEstimate;
    // Get the variance estimate from the result in order to deal with simulation
    // methods that do not provide variance estimate (conventional value: -1.0)
    // It is checked using the value of the standard deviation
    if (standardDeviation >= 0.0)
      convergencePoint[1] = reducedVarianceEstimate;
    else
      convergencePoint[1] = -1.0;
    convergenceStrategy_.store(convergencePoint);

    // callbacks
    if (progressCallback_.first)
    {
      progressCallback_.first((100.0 * outerSampling) / getMaximumOuterSampling(), progressCallback_.second);
    }
    if (stopCallback_.first)
    {
      stop = stopCallback_.first(stopCallback_.second);
    }
  }
}

/* Compute the block sample and the points that realized the event */
Sample Simulation::computeBlockSample()
{
  throw NotYetImplementedException(HERE) << "In Simulation::computeBlockSample()";
}

/* Convergence strategy accessor */
void Simulation::setConvergenceStrategy(const HistoryStrategy & convergenceStrategy)
{
  convergenceStrategy_ = convergenceStrategy;
}

HistoryStrategy Simulation::getConvergenceStrategy() const
{
  return convergenceStrategy_;
}

/* Draw the probability convergence at the given level */
Graph Simulation::drawProbabilityConvergence(const Scalar level) const
{
  const Sample convergenceSample(convergenceStrategy_.getSample());
  const UnsignedInteger size = convergenceSample.getSize();
  Sample dataEstimate(size, 2);
  Sample dataLowerBound(0, 2);
  Sample dataUpperBound(0, 2);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    const Scalar probabilityEstimate = convergenceSample[i][0];
    const Scalar varianceEstimate = convergenceSample[i][1];
    dataEstimate[i][0] = i + 1;
    dataEstimate[i][1] = probabilityEstimate;
    // The bounds are drawn only if there is a useable variance estimate
    if (varianceEstimate >= 0.0)
    {
      const Scalar confidenceLength = SimulationResult(event_, probabilityEstimate, varianceEstimate, i + 1, blockSize_).getConfidenceLength(level);
      Point pt(2);
      pt[0] = i + 1;
      pt[1] = probabilityEstimate - 0.5 * confidenceLength;
      dataLowerBound.add(pt);
      pt[1] = probabilityEstimate + 0.5 * confidenceLength;
      dataUpperBound.add(pt);
    }
  }
  const Curve estimateCurve(dataEstimate, "red", "solid", 2, "probability estimate");
  OSS oss;
  oss << getClassName() << " convergence graph at level " << level;
  Graph convergenceGraph(oss, "outer iteration", "estimate", true, "topright");
  convergenceGraph.add(estimateCurve);
  const Curve lowerBoundCurve(dataLowerBound, "green", "solid", 1, "bounds");
  const Curve upperBoundCurve(dataUpperBound, "green", "solid", 1, "");
  convergenceGraph.add(lowerBoundCurve);
  convergenceGraph.add(upperBoundCurve);
  return convergenceGraph;
}

/* Method save() stores the object through the StorageManager */
void Simulation::save(Advocate & adv) const
{

  PersistentObject::save(adv);
  adv.saveAttribute("convergenceStrategy_", convergenceStrategy_);
  adv.saveAttribute("event_", event_);
  adv.saveAttribute("result_", result_);
  adv.saveAttribute("blockSize_", blockSize_);
  adv.saveAttribute("maximumOuterSampling_", maximumOuterSampling_);
  adv.saveAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.saveAttribute("maximumStandardDeviation_", maximumStandardDeviation_);
  adv.saveAttribute("verbose_", verbose_);

}

/* Method load() reloads the object from the StorageManager */
void Simulation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("convergenceStrategy_", convergenceStrategy_);
  adv.loadAttribute("event_", event_);
  adv.loadAttribute("result_", result_);
  adv.loadAttribute("blockSize_", blockSize_);
  adv.loadAttribute("maximumOuterSampling_", maximumOuterSampling_);
  adv.loadAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.loadAttribute("maximumStandardDeviation_", maximumStandardDeviation_);
  adv.loadAttribute("verbose_", verbose_);
}


void Simulation::setProgressCallback(ProgressCallback callBack, void * state)
{
  progressCallback_ = std::pair<ProgressCallback, void *>(callBack, state);
}


void Simulation::setStopCallback(StopCallback callBack, void * state)
{
  stopCallback_ = std::pair<StopCallback, void *>(callBack, state);
}


END_NAMESPACE_OPENTURNS
