//                                               -*- C++ -*-
/**
 *  @brief LeastSquaresCalibrationResult stores the result of a least squares calibration algorithm
 *
 *  Copyright 2005-2026 Airbus-EDF-ONERA-Phimeca
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
#ifndef OPENTURNS_LEASTSQUARESCALIBRATIONRESULT_HXX
#define OPENTURNS_LEASTSQUARESCALIBRATIONRESULT_HXX

#include "openturns/CalibrationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LeastSquaresCalibrationResult
 * LeastSquaresCalibrationResult stores the result of a least squares calibration
 */
class OT_API LeastSquaresCalibrationResult
  : public CalibrationResult
{
  CLASSNAME

public:

  /** Default constructor */
  LeastSquaresCalibrationResult();

  /** Parameter constructor */
  LeastSquaresCalibrationResult(const Distribution & parameterPrior,
                    const Distribution & parameterPosterior,
                    const Point & calibratedParameter,
                    const Distribution & observationsError,
                    const Sample & inputObservations,
                    const Sample & outputObservations,
                    const Function & residualFunction,
                    const UnsignedInteger bootstrapSize);

  /** Virtual constructor */
  LeastSquaresCalibrationResult * clone() const override;

  /** Bayesian method accessor */
  Bool isBayesian() const override;

  /** Parameter distribution accessor */
  Distribution getParameterDistribution() const;

  /** Calibrated parameter accessor */
  Point getParameter() const;

  /** String converter */
  String __repr__() const override;

}; // class LeastSquaresCalibrationResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEASTSQUARESCALIBRATIONRESULT_HXX */
