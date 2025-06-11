//                                               -*- C++ -*-
/**
 *  @brief Analytical implements an algorithm to find the design point
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

#include "openturns/Analytical.hxx"
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
    event_(event)
{
  if (physicalStartingPoint.getSize())
  {
    LOGWARN("FORM/SORM physicalStartingPoint argument is deprecated");
    nearestPointAlgorithm_.setStartingPoint(physicalStartingPoint);
  }

  const UnsignedInteger dimension = event.getImplementation()->getFunction().getInputDimension();

  try
  {
    if (nearestPointAlgorithm_.getStartingPoint().getDimension() != dimension)
      throw InvalidArgumentException(HERE) << "Starting point dimension (" << nearestPointAlgorithm_.getStartingPoint().getDimension() << ") does not match event dimension (" << dimension << ").";
  }
  catch (const NotDefinedException &) // MultiStart algorithm
  {
    if (nearestPointAlgorithm_.getStartingSample().getDimension() != dimension)
      throw InvalidArgumentException(HERE) << "Starting sample dimension (" << nearestPointAlgorithm_.getStartingSample()[0].getDimension() << ") does not match event dimension (" << dimension << ").";
  }

  if (!event_.getImplementation()->getAntecedent().getDistribution().isContinuous())
    throw InvalidArgumentException(HERE) << "FORM/SORM only allows for continuous distributions";
}


/* Virtual constructor */
Analytical * Analytical::clone() const
{
  return new Analytical(*this);
}

/* Physical starting point accessor */
Point Analytical::getPhysicalStartingPoint() const
{
  LOGWARN("Analytical.getPhysicalStartingPoint is deprecated");
  return nearestPointAlgorithm_.getStartingPoint();
}

/* Physical starting point accessor */
void Analytical::setPhysicalStartingPoint(const Point & physicalStartingPoint)
{
  LOGWARN("Analytical.setPhysicalStartingPoint is deprecated");
  nearestPointAlgorithm_.setStartingPoint(physicalStartingPoint);
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
      << " physicalstartingPoint=" << nearestPointAlgorithm_.getStartingPoint();
  return oss;
}

/* Performs the actual computation. */
void Analytical::run()
{
  /* Build a standard event from the event */
  StandardEvent standardEvent(event_);

  /* set the level function of the algorithm */
  OptimizationAlgorithm nearestPointAlgorithm(nearestPointAlgorithm_);
  nearestPointAlgorithm.setProblem(NearestPointProblem(standardEvent.getImplementation()->getFunction(), standardEvent.getThreshold()));

  try
  {
    /* set the starting point of the algorithm in the standard space */
    nearestPointAlgorithm.setStartingPoint(event_.getImplementation()->getAntecedent().getDistribution().getIsoProbabilisticTransformation().operator()(nearestPointAlgorithm.getStartingPoint()));
  }
  catch (const NotDefinedException &) // MultiStart algorithm
  {
    /* set the starting sample of the algorithm in the standard space */
    nearestPointAlgorithm.setStartingSample(event_.getImplementation()->getAntecedent().getDistribution().getIsoProbabilisticTransformation().operator()(nearestPointAlgorithm.getStartingSample()));
  }

  /* solve the nearest point problem */
  nearestPointAlgorithm.run();

  /* set standard space design point in Result */
  Point standardSpaceDesignPoint(nearestPointAlgorithm.getResult().getOptimalPoint());
  standardSpaceDesignPoint.setName("Standard Space Design Point");

  result_ = AnalyticalResult(standardSpaceDesignPoint, event_, true);

  /* store the optimization result into the analytical result */
  result_.setOptimizationResult(nearestPointAlgorithm.getResult());

  /* set isStandardPointOriginInFailureSpace in Result */
  const Point origin(standardSpaceDesignPoint.getDimension(), 0.0);
  const Point value(standardEvent.getImplementation()->getFunction().operator()(origin));

  result_.setIsStandardPointOriginInFailureSpace(event_.getOperator().compare(value[0], event_.getThreshold()));

  /* check if result is valid */
  const Point physicalSpaceDesignPoint(event_.getImplementation()->getAntecedent().getDistribution().getInverseIsoProbabilisticTransformation().operator()(standardSpaceDesignPoint));

  const Point valuePhysicalSpaceDesignPoint(event_.getImplementation()->getFunction().operator()(physicalSpaceDesignPoint));

  const Scalar residual = result_.getOptimizationResult().getConstraintError();

  const Scalar limitStateTolerance = nearestPointAlgorithm.getMaximumConstraintError();

  if (!(residual <= 1.1 * limitStateTolerance)) // 1.1 is added to prevent numerical approximation made in Cobyla for constraint satisfaction tolerance.
    throw Exception(HERE) << "Obtained design point is not on the limit state: its image by the limit state function is " << valuePhysicalSpaceDesignPoint[0] << ", which is incompatible with the threshold: " << event_.getThreshold() << " considering the limit state tolerance of the optimization algorithm: " << limitStateTolerance;

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
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void Analytical::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("nearestPointAlgorithm_", nearestPointAlgorithm_);
  adv.loadAttribute("event_", event_);
  if (adv.hasAttribute("physicalStartingPoint_"))
  {
    // OT < 1.25
    Point physicalStartingPoint;
    adv.loadAttribute("physicalStartingPoint_", physicalStartingPoint);
    nearestPointAlgorithm_.setStartingPoint(physicalStartingPoint);
  }
  adv.loadAttribute("result_", result_);
}

END_NAMESPACE_OPENTURNS

