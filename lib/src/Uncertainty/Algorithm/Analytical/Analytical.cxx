//                                               -*- C++ -*-
/**
 *  @brief Analytical implements an algorithm to find the design point
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
#include <cmath>

#include "openturns/Analytical.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NearestPointProblem.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Analytical)

static const Factory<Analytical> Factory_Analytical;

/*
 * @brief  Standard constructor: the class is defined by an optimisation algorithm, a failure event and a physical starting point
 */
Analytical::Analytical(const OptimizationAlgorithm & nearestPointAlgorithm,
                       const RandomVector & event,
                       const Point & physicalStartingPoint)
  : PersistentObject(),
    nearestPointAlgorithm_(nearestPointAlgorithm),
    event_(event),
    physicalStartingPoint_(physicalStartingPoint),
    limitStateTolerance_(ResourceMap::GetAsScalar("Analytical-defaultLimitStateTolerance"))
{
  const UnsignedInteger dimension = event.getImplementation()->getFunction().getInputDimension();
  if (physicalStartingPoint.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Starting point dimension (" << physicalStartingPoint.getDimension() << ") does not match event dimension (" << dimension << ").";
  if (!event_.getImplementation()->getAntecedent().getDistribution().isContinuous())
    throw InvalidArgumentException(HERE) << "FORM/SORM only allows for continuous distributions";
  result_ = AnalyticalResult(event_.getImplementation()->getAntecedent().getDistribution().getIsoProbabilisticTransformation().operator()(physicalStartingPoint_), event, true);
}


Analytical::Analytical(const OptimizationAlgorithm & nearestPointAlgorithm,
                       const RandomVector & event,
                       const Point & physicalStartingPoint,
                       const Scalar & limitStateTolerance)
  : PersistentObject(),
    nearestPointAlgorithm_(nearestPointAlgorithm),
    event_(event),
    physicalStartingPoint_(physicalStartingPoint),
    limitStateTolerance_(limitStateTolerance)
{
  const UnsignedInteger dimension = event.getImplementation()->getFunction().getInputDimension();
  if (physicalStartingPoint.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Starting point dimension (" << physicalStartingPoint.getDimension() << ") does not match event dimension (" << dimension << ").";
  if (!event_.getImplementation()->getAntecedent().getDistribution().isContinuous())
    throw InvalidArgumentException(HERE) << "FORM/SORM only allows for continuous distributions";
  result_ = AnalyticalResult(event_.getImplementation()->getAntecedent().getDistribution().getIsoProbabilisticTransformation().operator()(physicalStartingPoint_), event, true);
}


/* Virtual constructor */
Analytical * Analytical::clone() const
{
  return new Analytical(*this);
}


/* limitStateTolerance accessor */
Scalar Analytical::getLimitStateTolerance() const
{
  return limitStateTolerance_;
}

/* limitStateTolerance accessor */
void Analytical::setLimitStateTolerance(const Scalar & limitStateTolerance) 
{
  limitStateTolerance_ = limitStateTolerance;
}

/* Physical starting point accessor */
Point Analytical::getPhysicalStartingPoint() const
{
  return physicalStartingPoint_;
}

/* Physical starting point accessor */
void Analytical::setPhysicalStartingPoint(const Point & physicalStartingPoint)
{
  physicalStartingPoint_ = physicalStartingPoint;
}

/* Event accessor */
RandomVector Analytical::getEvent() const
{
  return event_;
}

/* Event accessor */
void Analytical::setEvent(const RandomVector & event)
{
  event_ = event;
}

/* OptimizationAlgorithm accessor */
OptimizationAlgorithm Analytical::getNearestPointAlgorithm() const
{
  return nearestPointAlgorithm_;
}

/* OptimizationAlgorithm accessor */
void Analytical::setNearestPointAlgorithm(const OptimizationAlgorithm & nearestPointAlgorithm)
{
  nearestPointAlgorithm_ = nearestPointAlgorithm;
}

/* String converter */
String Analytical::__repr__() const
{
  OSS oss;
  oss << "class=" << Analytical::GetClassName()
      << " nearestPointAlgorithm=" << nearestPointAlgorithm_.__repr__()
      << " event=" << event_
      << " physicalstartingPoint=" << physicalStartingPoint_;
  return oss;
}

/* Performs the actual computation. */
void Analytical::run()
{
  /* Build a standard event from the event */
  StandardEvent standardEvent(event_);

  /* set the level function of the algorithm */
  nearestPointAlgorithm_.setProblem(NearestPointProblem(standardEvent.getImplementation()->getFunction(), standardEvent.getThreshold()));

  /* set the starting point of the algorithm in the standard space  */
  nearestPointAlgorithm_.setStartingPoint(event_.getImplementation()->getAntecedent().getDistribution().getIsoProbabilisticTransformation().operator()(physicalStartingPoint_));

  /* solve the nearest point problem */
  nearestPointAlgorithm_.run();

  /* store the optimization result into the analytical result */
  result_.setOptimizationResult(nearestPointAlgorithm_.getResult());
  /* set standard space design point in Result */
  Point standardSpaceDesignPoint(nearestPointAlgorithm_.getResult().getOptimalPoint());
  standardSpaceDesignPoint.setName("Standard Space Design Point");
  result_.setStandardSpaceDesignPoint(standardSpaceDesignPoint);

  /* set isStandardPointOriginInFailureSpace in Result */
  Point origin(standardSpaceDesignPoint.getDimension(), 0.0);
  Point value(standardEvent.getImplementation()->getFunction().operator()(origin));

  result_.setIsStandardPointOriginInFailureSpace(event_.getOperator().compare(value[0], event_.getThreshold()));
  
  /* check is result is valid */
  Point physicalSpaceDesignPoint(event_.getImplementation()->getAntecedent().getDistribution().getInverseIsoProbabilisticTransformation().operator()(standardSpaceDesignPoint));
  
  Point valuePhysicalSpaceDesignPoint(event_.getImplementation()->getFunction().operator()(physicalSpaceDesignPoint));
  
  Scalar residual = (valuePhysicalSpaceDesignPoint[0]-event_.getThreshold())*(valuePhysicalSpaceDesignPoint[0]-event_.getThreshold());  
  
  if (residual > limitStateTolerance_*limitStateTolerance_)
  throw Exception(HERE) << "Obtained design point is not on the limit state : its image by the limit state function is " << valuePhysicalSpaceDesignPoint[0] << " that is uncompatible with the threshold : " << event_.getThreshold() << " considering the limit state tolerance "<< limitStateTolerance_;
  
  
} /* Analytical::run() */

/* Result accessor */
AnalyticalResult Analytical::getAnalyticalResult() const
{
  return result_;
}


/* Method save() stores the object through the StorageManager */
void Analytical::save(Advocate & adv) const
{

  PersistentObject::save(adv);
  adv.saveAttribute("nearestPointAlgorithm_", nearestPointAlgorithm_);
  adv.saveAttribute("event_", event_);
  adv.saveAttribute("physicalStartingPoint_", physicalStartingPoint_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void Analytical::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("nearestPointAlgorithm_", nearestPointAlgorithm_);
  adv.loadAttribute("event_", event_);
  adv.loadAttribute("physicalStartingPoint_", physicalStartingPoint_);
  adv.loadAttribute("result_", result_);
}

END_NAMESPACE_OPENTURNS

