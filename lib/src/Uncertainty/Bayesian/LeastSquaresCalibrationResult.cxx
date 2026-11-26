//                                               -*- C++ -*-
/**
 *  @brief CalibrationResult stores the result of a least squares calibration algorithm
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
#include "openturns/LeastSquaresCalibrationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LeastSquaresCalibrationResult)

static const Factory<LeastSquaresCalibrationResult> Factory_LeastSquaresCalibrationResult;

/* Default constructor */
LeastSquaresCalibrationResult::LeastSquaresCalibrationResult()
  : CalibrationResult()
{
  // Nothing to do
}

/* Parameter constructor */
LeastSquaresCalibrationResult::LeastSquaresCalibrationResult(const Distribution & parameterPrior,
                                     const Distribution & parameterPosterior,
                                     const Point & calibratedParameter,
                                     const Distribution & observationsError,
                                     const Sample & inputObservations,
                                     const Sample & outputObservations,
                                     const Function & residualFunction,
                                     const UnsignedInteger bootstrapSize)
  : CalibrationResult(parameterPrior, parameterPosterior, calibratedParameter, observationsError, inputObservations, outputObservations, residualFunction, bootstrapSize)
{
  // Nothing to do
}

/* Virtual constructor */
LeastSquaresCalibrationResult * LeastSquaresCalibrationResult::clone() const
{
  return new LeastSquaresCalibrationResult(*this);
}

/* Bayesian method accessor */
Bool LeastSquaresCalibrationResult::isBayesian() const
{
  return false;
}

/* Parameter distribution accessor */
Distribution LeastSquaresCalibrationResult::getParameterDistribution() const
{
  return parameterPosterior_;
}

/* Calibrated parameter accessor */
Point LeastSquaresCalibrationResult::getParameter() const
{
  return calibratedParameter_;
}

/* String converter */
String LeastSquaresCalibrationResult::__repr__() const
{
  return CalibrationResult::__repr__();
}

END_NAMESPACE_OPENTURNS
