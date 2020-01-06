//                                               -*- C++ -*-
/**
 *  @brief Default CalibrationStrategyImplementation
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/CalibrationStrategyImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(CalibrationStrategyImplementation)

static const Factory<CalibrationStrategyImplementation> Factory_CalibrationStrategyImplementation;

/* Default constructor */
CalibrationStrategyImplementation::CalibrationStrategyImplementation(const Interval & range,
    const Scalar expansionFactor,
    const Scalar shrinkFactor,
    const UnsignedInteger calibrationStep)
  : PersistentObject()
{
  setRange(range);
  setExpansionFactor(expansionFactor);
  setShrinkFactor(shrinkFactor);
  setCalibrationStep(calibrationStep);
}

/* String converter */
String CalibrationStrategyImplementation::__repr__() const
{
  return OSS() << "class=" << CalibrationStrategyImplementation::GetClassName()
         << " name=" << getName();
}

void CalibrationStrategyImplementation::setRange(const Interval& range)
{
  if ( range.getDimension() != 1 ) throw InvalidDimensionException(HERE) << "Range should be 1-d. Got " << range.getDimension();
  range_ = range;
}

Interval CalibrationStrategyImplementation::getRange() const
{
  return range_;
}


void CalibrationStrategyImplementation::setExpansionFactor(Scalar expansionFactor)
{
  if (!( expansionFactor > 1.0)) throw InvalidArgumentException(HERE) << "Expansion factor should be > 1. Got " << expansionFactor;
  expansionFactor_ = expansionFactor;
}

Scalar CalibrationStrategyImplementation::getExpansionFactor() const
{
  return expansionFactor_;
}


void CalibrationStrategyImplementation::setShrinkFactor(Scalar shrinkFactor)
{
  if ( (shrinkFactor <= 0.0) || (shrinkFactor >= 1.0) ) throw InvalidArgumentException(HERE) << "Shrink factor should be in (0, 1). Got " << shrinkFactor;
  shrinkFactor_ = shrinkFactor;
}

Scalar CalibrationStrategyImplementation::getShrinkFactor() const
{
  return shrinkFactor_;
}




CalibrationStrategyImplementation* CalibrationStrategyImplementation::clone() const
{
  return new CalibrationStrategyImplementation(*this);
}

/* Here is the interface that all derived class must implement */

Scalar CalibrationStrategyImplementation::computeUpdateFactor(const Scalar rho) const
{
  Scalar factor = 1.0;
  // if the acceptance rate it too low, make smaller steps
  if ( rho < range_.getLowerBound()[0] ) factor = shrinkFactor_;
  // if the acceptance rate is too high, make larger steps
  else if ( rho > range_.getUpperBound()[0] ) factor = expansionFactor_;
  return factor;
}

void CalibrationStrategyImplementation::setCalibrationStep(const UnsignedInteger calibrationStep)
{
  if (calibrationStep == 0) throw InvalidArgumentException(HERE) << "The calibration period must be positive.";
  calibrationStep_ = calibrationStep;
}


UnsignedInteger CalibrationStrategyImplementation::getCalibrationStep() const
{
  return calibrationStep_;
}

/* Method save() stores the object through the StorageManager */
void CalibrationStrategyImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("range_", range_);
  adv.saveAttribute("expansionFactor_", expansionFactor_);
  adv.saveAttribute("shrinkFactor_", shrinkFactor_);
  adv.saveAttribute("calibrationStep_", calibrationStep_);
}

/* Method load() reloads the object from the StorageManager */
void CalibrationStrategyImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("range_", range_);
  adv.loadAttribute("expansionFactor_", expansionFactor_);
  adv.loadAttribute("shrinkFactor_", shrinkFactor_);
  adv.loadAttribute("calibrationStep_", calibrationStep_);
}


END_NAMESPACE_OPENTURNS
