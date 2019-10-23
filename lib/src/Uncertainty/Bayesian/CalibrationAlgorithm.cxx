//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all CalibrationAlgorithm
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/CalibrationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(CalibrationAlgorithm)

/* Default constructor */
CalibrationAlgorithm::CalibrationAlgorithm()
  : TypedInterfaceObject<CalibrationAlgorithmImplementation>(new CalibrationAlgorithmImplementation)
{
  // Nothing to do
}


/* Constructor with parameters */
CalibrationAlgorithm::CalibrationAlgorithm(const Sample & outputObservations,
    const Distribution & parameterPrior)
  : TypedInterfaceObject<CalibrationAlgorithmImplementation>(new CalibrationAlgorithmImplementation(outputObservations, parameterPrior))
{
  // Nothing to do
}

/* Constructor from implementation */
CalibrationAlgorithm::CalibrationAlgorithm(const CalibrationAlgorithmImplementation & op)
  : TypedInterfaceObject<CalibrationAlgorithmImplementation>(op.clone())
{
  // Nothing to do
}

/* Performs the actual computation. Must be overloaded by the actual calibration algorithm */
void CalibrationAlgorithm::run()
{
  getImplementation()->run();
}

/* Output observations accessor */
Sample CalibrationAlgorithm::getOutputObservations() const
{
  return getImplementation()->getOutputObservations();
}

/* Parameter prior accessor */
Distribution CalibrationAlgorithm::getParameterPrior() const
{
  return getImplementation()->getParameterPrior();
}

/* Result accessor */
void CalibrationAlgorithm::setResult(const CalibrationResult & result)
{
  getImplementation()->setResult(result);
}

CalibrationResult CalibrationAlgorithm::getResult() const
{
  return getImplementation()->getResult();
}


END_NAMESPACE_OPENTURNS
