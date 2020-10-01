//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all CalibrationAlgorithm
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
#ifndef OPENTURNS_CALIBRATIONALGORITHM_HXX
#define OPENTURNS_CALIBRATIONALGORITHM_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/CalibrationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CalibrationAlgorithm
 *
 * @brief The class describes the concept of a calibration algorithm.
 *
 *
 * The implementation defines how the
 * @see CalibrationAlgorithmImplementation
 */
class OT_API CalibrationAlgorithm
  : public TypedInterfaceObject<CalibrationAlgorithmImplementation>
{
  CLASSNAME
public:

  /** Default constructor */
  CalibrationAlgorithm();

  /** Constructor from implementation */
  CalibrationAlgorithm(const CalibrationAlgorithmImplementation & op);

  /** Performs the actual computation. Must be overloaded by the actual calibration algorithm */
  virtual void run();

  /** Output observations accessor */
  Sample getOutputObservations() const;

  /** Parameter prior accessor */
  Distribution getParameterPrior() const;

  /** Result accessor */
  void setResult(const CalibrationResult & result);
  CalibrationResult getResult() const;

}; /* class CalibrationAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CALIBRATIONALGORITHM_HXX */
