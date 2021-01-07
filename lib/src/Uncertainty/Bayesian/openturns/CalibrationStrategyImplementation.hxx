//                                               -*- C++ -*-
/**
 *  @brief Default CalibrationStrategyImplementation
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CALIBRATIONSTRATEGYIMPLEMENTATION_HXX
#define OPENTURNS_CALIBRATIONSTRATEGYIMPLEMENTATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Interval.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class CalibrationStrategyImplementation
 *
 * @brief The class implements the concept of comparison operator defined in CalibrationStrategy.
 *
 * This class is abstract so it can not be instanciated. It must be derived.
 * @see CalibrationStrategy
 */
class OT_API CalibrationStrategyImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor with parameters */
  CalibrationStrategyImplementation(const Interval & range = Interval(ResourceMap::GetAsScalar("CalibrationStrategy-DefaultLowerBound"),
                                    ResourceMap::GetAsScalar("CalibrationStrategy-DefaultUpperBound")),
                                    const Scalar expansionFactor = ResourceMap::GetAsScalar("CalibrationStrategy-DefaultExpansionFactor"),
                                    const Scalar shrinkFactor = ResourceMap::GetAsScalar("CalibrationStrategy-DefaultShrinkFactor"),
                                    const UnsignedInteger calibrationStep = ResourceMap::GetAsUnsignedInteger("CalibrationStrategy-DefaultCalibrationStep"));

  /** String converter */
  String __repr__() const override;

  /** Range accessor */
  void setRange(const Interval & range);
  Interval getRange() const;

  /** Expansion factor accessor */
  void setExpansionFactor(Scalar expansionFactor);
  Scalar getExpansionFactor() const;

  /** Shrink factor accessor */
  void setShrinkFactor(Scalar shrinkFactor);
  Scalar getShrinkFactor() const;

  /** Calibration period */
  void setCalibrationStep(const UnsignedInteger calibrationStep);
  UnsignedInteger getCalibrationStep() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  CalibrationStrategyImplementation * clone() const override;

  /** @copydoc CalibrationStrategy::computeUpdateFactor() const */
  virtual Scalar computeUpdateFactor(const Scalar rho) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Interval range_;
  Scalar shrinkFactor_;
  Scalar expansionFactor_;

  /// period between recalibrations
  UnsignedInteger calibrationStep_;

}; /* class CalibrationStrategyImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CALIBRATIONSTRATEGYIMPLEMENTATION_HXX */
