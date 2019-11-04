//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all CalibrationStrategy
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/CalibrationStrategy.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(CalibrationStrategy)

/* Default constructor */
CalibrationStrategy::CalibrationStrategy()
  : TypedInterfaceObject<CalibrationStrategyImplementation>(new CalibrationStrategyImplementation)
{
  // Nothing to do
}


/* Constructor with parameters */
CalibrationStrategy::CalibrationStrategy(const Interval & range)
  : TypedInterfaceObject<CalibrationStrategyImplementation>(new CalibrationStrategyImplementation(range))
{
  // Nothing to do
}


/* Constructor with parameters */
CalibrationStrategy::CalibrationStrategy(const Interval & range,
    const Scalar expansionFactor,
    const Scalar shrinkFactor)
  : TypedInterfaceObject<CalibrationStrategyImplementation>(new CalibrationStrategyImplementation(range,
      expansionFactor,
      shrinkFactor))
{
  // Nothing to do
}

/* Constructor with parameters */
CalibrationStrategy::CalibrationStrategy(const Interval & range,
    const Scalar expansionFactor,
    const Scalar shrinkFactor,
    const UnsignedInteger calibrationStep)
  : TypedInterfaceObject<CalibrationStrategyImplementation>(new CalibrationStrategyImplementation(range,
      expansionFactor,
      shrinkFactor,
      calibrationStep))
{
  // Nothing to do
}


/* Constructor from implementation */
CalibrationStrategy::CalibrationStrategy(const CalibrationStrategyImplementation & op)
  : TypedInterfaceObject<CalibrationStrategyImplementation>(op.clone())
{
  // Nothing to do
}


void CalibrationStrategy::setRange(const Interval & range)
{
  copyOnWrite();
  getImplementation()->setRange(range);
}


Interval CalibrationStrategy::getRange() const
{
  return getImplementation()->getRange();
}


void CalibrationStrategy::setExpansionFactor(const Scalar expansionFactor)
{
  copyOnWrite();
  getImplementation()->setExpansionFactor(expansionFactor);
}


Scalar CalibrationStrategy::getExpansionFactor() const
{
  return getImplementation()->getExpansionFactor();
}


void CalibrationStrategy::setShrinkFactor(const Scalar shrinkFactor)
{
  copyOnWrite();
  getImplementation()->setShrinkFactor(shrinkFactor);
}

Scalar CalibrationStrategy::getShrinkFactor() const
{
  return getImplementation()->getShrinkFactor();
}


void CalibrationStrategy::setCalibrationStep(const UnsignedInteger calibrationStep)
{
  copyOnWrite();
  getImplementation()->setCalibrationStep(calibrationStep);
}


UnsignedInteger CalibrationStrategy::getCalibrationStep() const
{
  return getImplementation()->getCalibrationStep();
}


/* Return true if comparison succeeds */
Scalar CalibrationStrategy::computeUpdateFactor(const Scalar rho) const
{
  return getImplementation()->computeUpdateFactor(rho);
}



END_NAMESPACE_OPENTURNS
