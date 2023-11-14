//                                               -*- C++ -*-
/**
 *  @brief EventSimulation is a generic view of simulation methods for computing
 * probabilities and related quantities by sampling and estimation
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EventSimulation.hxx"
#include "openturns/ThresholdEvent.hxx"
#include "openturns/Log.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Less.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/IntersectionEvent.hxx"
#include "openturns/UnionEvent.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class EventSimulation
 */

CLASSNAMEINIT(EventSimulation)

static const Factory<EventSimulation> Factory_EventSimulation;

/** For save/load mechanism */
EventSimulation::EventSimulation(const HistoryStrategy & convergenceStrategy)
  : SimulationAlgorithm()
  , event_(ThresholdEvent(CompositeRandomVector(IdentityFunction(1), RandomVector(Uniform())), Less(), 0.0))
  , result_()
{
  convergenceStrategy_ = convergenceStrategy;
}

/* Constructor with parameters */
EventSimulation::EventSimulation(const RandomVector & event,
                                 const HistoryStrategy & convergenceStrategy)
  : SimulationAlgorithm()
  , event_(event)
  , result_()
{
  convergenceStrategy_ = convergenceStrategy;
  if (!event.isEvent())
    throw InvalidArgumentException(HERE) << "Not an event";
}

/* Virtual constructor */
EventSimulation * EventSimulation::clone() const
{
  return new EventSimulation(*this);
}

/*  Event accessor */
RandomVector EventSimulation::getEvent() const
{
  return event_;
}

/* Result accessor */
void EventSimulation::setResult(const ProbabilitySimulationResult & result)
{
  result_ = result;
}

/* Result accessor */
ProbabilitySimulationResult EventSimulation::getResult() const
{
  return result_;
}

/* String converter */
String EventSimulation::__repr__() const
{
  OSS oss;
  oss << "class=" << EventSimulation::GetClassName()
      << " event=" << event_
      << " maximumOuterSampling=" << getMaximumOuterSampling()
      << " maximumCoefficientOfVariation=" << getMaximumCoefficientOfVariation()
      << " maximumStandardDeviation=" << getMaximumStandardDeviation()
      << " blockSize=" << getBlockSize();
  return oss;
}

/* Performs the actual computation. */
void EventSimulation::run()
{
  /* We want to compute the probability of occurrence of the given event
   *  We estimate this probability by computing the empirical mean of a
   * sample of size at most outerSampling * blockSize, this sample being
   * built by blocks of size blockSize. It allows one to use efficiently the
   * distribution of the computation as well as it allows one to deal with
   * a sample size > 2^32 by a combination of blockSize and outerSampling
   */
  // First, reset the convergence history
  convergenceStrategy_.setDimension(2);
  UnsignedInteger outerSampling = 0;
  Scalar coefficientOfVariation = -1.0;
  Scalar standardDeviation = -1.0;
  Scalar probabilityEstimate = 0.0;
  Scalar varianceEstimate = 0.0;
  const UnsignedInteger blockSize = getBlockSize();
  // Initialize the result. We use the accessors in order to preserve the exact nature of the result (SimulationResult or QuasiMonteCarloResult)
  // First, the invariant part
  // For the event, we have to access to the implementation as the interface does not provide the setEvent() method ON PURPOSE!
  result_.setEvent(event_);
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
    LOGDEBUG(OSS() << "EventSimulation::run: blockSample=\n" << blockSample);
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
    LOGDEBUG(result_.__repr__());
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
      if (stop)
      {
        LOGINFO(OSS() << "Stopped due to user");
      }
    } // stopCallback
  } // while
}

/* Compute the block sample and the points that realized the event */
Sample EventSimulation::computeBlockSample()
{
  throw NotYetImplementedException(HERE) << "In EventSimulation::computeBlockSample()";
}

/* Draw the probability convergence at the given level */
Graph EventSimulation::drawProbabilityConvergence(const Scalar level) const
{
  const Sample convergenceSample(convergenceStrategy_.getSample());
  const UnsignedInteger size = convergenceSample.getSize();
  Sample dataEstimate(size, 2);
  Sample dataLowerBound(0, 2);
  Sample dataUpperBound(0, 2);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    const Scalar probabilityEstimate = convergenceSample(i, 0);
    const Scalar varianceEstimate = convergenceSample(i, 1);
    dataEstimate(i, 0) = i + 1;
    dataEstimate(i, 1) = probabilityEstimate;
    // The bounds are drawn only if there is a useable variance estimate
    if (varianceEstimate >= 0.0)
    {
      const Scalar confidenceLength = ProbabilitySimulationResult(event_, probabilityEstimate, varianceEstimate, i + 1, blockSize_).getConfidenceLength(level);
      Point pt(2);
      pt[0] = i + 1;
      pt[1] = probabilityEstimate - 0.5 * confidenceLength;
      dataLowerBound.add(pt);
      pt[1] = probabilityEstimate + 0.5 * confidenceLength;
      dataUpperBound.add(pt);
    }
  }
  Curve estimateCurve(dataEstimate, "probability estimate");
  estimateCurve.setLineWidth(2);
  OSS oss;
  oss << getClassName() << " convergence graph at level " << level;
  Graph convergenceGraph(oss, "outer iteration", "estimate", true, "topright");
  convergenceGraph.add(estimateCurve);
  const Curve lowerBoundCurve(dataLowerBound, "bounds");
  Curve upperBoundCurve(dataUpperBound);
  convergenceGraph.add(lowerBoundCurve);
  upperBoundCurve.setColor(convergenceGraph.getDrawable(1).getColor());
  convergenceGraph.add(upperBoundCurve);
  return convergenceGraph;
}

/* Method save() stores the object through the StorageManager */
void EventSimulation::save(Advocate & adv) const
{

  SimulationAlgorithm::save(adv);
  adv.saveAttribute("event_", event_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void EventSimulation::load(Advocate & adv)
{
  SimulationAlgorithm::load(adv);
  adv.loadAttribute("event_", event_);
  adv.loadAttribute("result_", result_);
}


END_NAMESPACE_OPENTURNS
