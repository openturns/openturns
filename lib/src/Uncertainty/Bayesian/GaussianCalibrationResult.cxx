//                                               -*- C++ -*-
/**
 *  @brief CalibrationResult stores the result of a Gaussian calibration algorithm
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
#include "openturns/GaussianCalibrationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianCalibrationResult)

static const Factory<GaussianCalibrationResult> Factory_GaussianCalibrationResult;

/* Default constructor */
GaussianCalibrationResult::GaussianCalibrationResult()
  : CalibrationResult()
{
  // Nothing to do
}

/* Parameter constructor */
GaussianCalibrationResult::GaussianCalibrationResult(const Distribution & parameterPrior,
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
GaussianCalibrationResult * GaussianCalibrationResult::clone() const
{
  return new GaussianCalibrationResult(*this);
}

/* Bayesian method accessor */
Bool GaussianCalibrationResult::isBayesian() const
{
  return true;
}

/* Parameter prior accessors */
Distribution GaussianCalibrationResult::getParameterPrior() const
{
  return parameterPrior_;
}

/* Parameter posterior accessors */
Distribution GaussianCalibrationResult::getParameterPosterior() const
{
  if (!(bootstrapSize_ == 0)) throw InvalidArgumentException(HERE) << "Error: not available, use getMAPDistribution() instead";
  return parameterPosterior_;
}

/* Bootstrap MAP distribution accessor */
Distribution GaussianCalibrationResult::getMAPDistribution() const
{
  if (!(bootstrapSize_ > 0)) throw InvalidArgumentException(HERE) << "Error: not available, use getParameterPosterior() instead";
  return parameterPosterior_;
}

/* Parameter MAP accessor */
Point GaussianCalibrationResult::getParameterMAP() const
{
  return calibratedParameter_;
}

/* String converter */
String GaussianCalibrationResult::__repr__() const
{
  return CalibrationResult::__repr__();
}

END_NAMESPACE_OPENTURNS
