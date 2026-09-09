//                                               -*- C++ -*-
/**
 *  @brief EventSimulationImplementation is a generic view of simulation methods for computing
 * probabilities and related quantities by sampling and estimation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/EventSimulationImplementation.hxx"
#include "openturns/ThresholdEvent.hxx"
#include "openturns/Log.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Point.hxx"
#include "openturns/Less.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/IntersectionEvent.hxx"
#include "openturns/UnionEvent.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class EventSimulationImplementation
 */

CLASSNAMEINIT(EventSimulationImplementation)

static const Factory<EventSimulationImplementation> Factory_EventSimulationImplementation;

/** For save/load mechanism */
EventSimulationImplementation::EventSimulationImplementation(const HistoryStrategy & convergenceStrategy)
  : SimulationAlgorithm()
  , event_(ThresholdEvent(CompositeRandomVector(IdentityFunction(1), RandomVector(Uniform())), Less(), 0.0))
  , result_()
{
  convergenceStrategy_ = convergenceStrategy;
}

/* Constructor with parameters */
EventSimulationImplementation::EventSimulationImplementation(const RandomVector & event,
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
EventSimulationImplementation * EventSimulationImplementation::clone() const
{
  return new EventSimulationImplementation(*this);
}

/*  Event accessor */
RandomVector EventSimulationImplementation::getEvent() const
{
  return event_;
}

/*  Event accessor */
void EventSimulationImplementation::setEvent(const RandomVector & event)
{
  if (!event.isEvent())
    throw InvalidArgumentException(HERE) << "Not an event";
  event_ = event;
}

/* Result accessor */
void EventSimulationImplementation::setResult(const ProbabilitySimulationResult & result)
{
  result_ = result;
}


/* Result accessor */
ProbabilitySimulationResult EventSimulationImplementation::getResult() const
{
  return result_;
}

/* String converter */
String EventSimulationImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << EventSimulationImplementation::GetClassName()
      << " event=" << event_
      << " maximumOuterSampling=" << getMaximumOuterSampling()
      << " maximumCoefficientOfVariation=" << getMaximumCoefficientOfVariation()
      << " maximumStandardDeviation=" << getMaximumStandardDeviation()
      << " blockSize=" << getBlockSize();
  return oss;
}

/* Performs the actual computation. */
void EventSimulationImplementation::run()
{
  convergenceStrategy_.setDimension(3);
  UnsignedInteger outerSampling = 0;
  Scalar coefficientOfVariation = -1.0;
  Scalar standardDeviation = -1.0;
  Scalar probabilityEstimate = 0.0;
  Scalar varianceEstimate = 0.0;
  const UnsignedInteger blockSize = getBlockSize();
  result_.setEvent(event_);
  result_.setBlockSize(blockSize_);
  result_.setProbabilityEstimate(probabilityEstimate);
  result_.setVarianceEstimate(varianceEstimate);
  result_.setOuterSampling(outerSampling);

  Bool stop = false;
  std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();

  while ((outerSampling < getMaximumOuterSampling()) && ((coefficientOfVariation == -1.0) || (coefficientOfVariation > getMaximumCoefficientOfVariation())) && ((standardDeviation == -1.0) || (standardDeviation > getMaximumStandardDeviation())) && !stop)
  {
    const Sample blockSample(computeBlockSample());
    LOGDEBUG(OSS() << "EventSimulationImplementation::run: blockSample=\n" << blockSample);
    ++outerSampling;
    const Scalar meanBlock = blockSample.computeMean()[0];
    const Scalar varianceBlock = blockSample.computeCovariance()(0, 0);
    const Scalar size = outerSampling;
    varianceEstimate = (varianceBlock + (size - 1.0) * varianceEstimate) / size + (1.0 - 1.0 / size) * (probabilityEstimate - meanBlock) * (probabilityEstimate - meanBlock) / size;
    probabilityEstimate = (meanBlock + (size - 1.0) * probabilityEstimate) / size;
    const Scalar reducedVarianceEstimate = varianceEstimate / (size * blockSize);
    result_.setProbabilityEstimate(probabilityEstimate);
    result_.setVarianceEstimate(reducedVarianceEstimate);
    result_.setOuterSampling(outerSampling);
    LOGDEBUG(result_.__repr__());
    coefficientOfVariation = result_.getCoefficientOfVariation();
    standardDeviation = result_.getStandardDeviation();
    Point convergencePoint = {probabilityEstimate, -1.0, outerSampling * 1.0};
    if (standardDeviation >= 0.0)
      convergencePoint[1] = reducedVarianceEstimate;
    convergenceStrategy_.store(convergencePoint);

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0).count();
    result_.setTimeDuration(timeDuration);
    if ((getMaximumTimeDuration() > 0.0) && (timeDuration > getMaximumTimeDuration()))
    {
      LOGINFO(OSS() << "Maximum time exceeded");
      stop = true;
    }

    if (progressCallback_.first)
    {
      progressCallback_.first((100.0 * outerSampling) / getMaximumOuterSampling(), progressCallback_.second);
    }
    if (!stop && stopCallback_.first)
    {
      stop = stopCallback_.first(stopCallback_.second);
      if (stop)
        LOGINFO(OSS() << "Stopped due to user");
    }
  }
}

/* Accessor to input sample */
Sample EventSimulationImplementation::getInputSample() const
{
  throw NotYetImplementedException(HERE) << "In EventSimulationImplementation::getInputSample()";
}


/* Compute the block sample and the points that realized the event */
Sample EventSimulationImplementation::computeBlockSample()
{
  throw NotYetImplementedException(HERE) << "In EventSimulationImplementation::computeBlockSample()";
}

/** Setter keep sample */
void EventSimulationImplementation::setKeepSample(const Bool) 
{
  throw NotYetImplementedException(HERE) << "In EventSimulationImplementation::setKeepSample(Bool)";
} 
  
/* Draw the probability convergence at the given level */
Graph EventSimulationImplementation::drawProbabilityConvergence(const Scalar level) const
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
    const Scalar outerIndex = convergenceSample(i, 2);
    dataEstimate(i, 0) = outerIndex + 1;
    dataEstimate(i, 1) = probabilityEstimate;
    if (varianceEstimate >= 0.0)
    {
      const Interval confidenceInterval(ProbabilitySimulationResult(event_, probabilityEstimate, varianceEstimate, outerIndex + 1, blockSize_).getProbabilityDistribution().computeBilateralConfidenceInterval(level));
      dataLowerBound.add(Point({outerIndex + 1, confidenceInterval.getLowerBound()[0]}));
      dataUpperBound.add(Point({outerIndex + 1, confidenceInterval.getUpperBound()[0]}));
    }
  }
  Curve estimateCurve(dataEstimate, "probability estimate");
  estimateCurve.setLineWidth(2);
  OSS oss;
  oss << getClassName() << " convergence graph at level " << level;
  Graph convergenceGraph(oss, "outer iteration", "estimate");
  convergenceGraph.setLegendPosition("topright");
  convergenceGraph.add(estimateCurve);
  const Curve lowerBoundCurve(dataLowerBound, "bounds");
  Curve upperBoundCurve(dataUpperBound);
  convergenceGraph.add(lowerBoundCurve);
  upperBoundCurve.setColor(convergenceGraph.getDrawable(1).getColor());
  convergenceGraph.add(upperBoundCurve);
  return convergenceGraph;
}

/* Method save() stores the object through the StorageManager */
void EventSimulationImplementation::save(Advocate & adv) const
{

  SimulationAlgorithm::save(adv);
  adv.saveAttribute("event_", event_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void EventSimulationImplementation::load(Advocate & adv)
{
  SimulationAlgorithm::load(adv);
  adv.loadAttribute("event_", event_);
  adv.loadAttribute("result_", result_);
}


END_NAMESPACE_OPENTURNS
