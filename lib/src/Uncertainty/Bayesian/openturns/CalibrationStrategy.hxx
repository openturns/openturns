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
#ifndef OPENTURNS_CALIBRATIONSTRATEGY_HXX
#define OPENTURNS_CALIBRATIONSTRATEGY_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/CalibrationStrategyImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CalibrationStrategy
 *
 * @brief The class describes the concept of a calibration strategy.
 *
 *
 * The implementation defines how the
 * @see CalibrationStrategyImplementation
 */
class OT_API CalibrationStrategy
  : public TypedInterfaceObject<CalibrationStrategyImplementation>
{
  CLASSNAME
public:

  /** Default constructor */
  CalibrationStrategy();

  /** Constructor with parameters*/
  CalibrationStrategy(const Interval & range);

  /** Constructor with parameters*/
  CalibrationStrategy(const Interval & range,
                      const Scalar expansionFactor,
                      const Scalar shrinkFactor);

  /** Constructor with parameters*/
  CalibrationStrategy(const Interval & range,
                      const Scalar expansionFactor,
                      const Scalar shrinkFactor,
                      const UnsignedInteger calibrationStep);

  /** Constructor from implementation */
  CalibrationStrategy(const CalibrationStrategyImplementation & op);

  /** Range accessor */
  void setRange(const Interval & range);
  Interval getRange() const;

  /** Expansion factor accessor */
  void setExpansionFactor(const Scalar expansionFactor);
  Scalar getExpansionFactor() const;

  /** Shrink factor accessor */
  void setShrinkFactor(const Scalar shrinkFactor);
  Scalar getShrinkFactor() const;

  /** Calibration period */
  void setCalibrationStep(const UnsignedInteger calibrationStep);
  UnsignedInteger getCalibrationStep() const;

  /**
   * Compute the new update factor
   * @param rhoEstimate The old rho estimate
   * @return The new rho estimate
   */
  Scalar computeUpdateFactor(const Scalar rho) const;


}; /* class CalibrationStrategy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CALIBRATIONSTRATEGY_HXX */
