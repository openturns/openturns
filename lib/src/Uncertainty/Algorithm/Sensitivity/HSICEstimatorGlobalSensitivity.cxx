//                                               -*- C++ -*-
/**
 * @brief HSICEstimatorGlobalSensitivity implements the global sensivity HSIC indices
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/HSICEstimatorGlobalSensitivity.hxx"

BEGIN_NAMESPACE_OPENTURNS
CLASSNAMEINIT(HSICEstimatorGlobalSensitivity)

/* Default */
HSICEstimatorGlobalSensitivity::HSICEstimatorGlobalSensitivity()
  : HSICEstimatorImplementation()
{
  // Nothing to do
}

/* Constructor */
HSICEstimatorGlobalSensitivity::HSICEstimatorGlobalSensitivity(
  const CovarianceModelCollection & covarianceModelCollection
  , const Sample & X
  , const Sample & Y,
  const HSICStat & estimatorType)
  : HSICEstimatorImplementation(covarianceModelCollection, X, Y, estimatorType)
{
  computeCovarianceMatrices();
  computeWeights();
}

/* Virtual constructor */
HSICEstimatorGlobalSensitivity* HSICEstimatorGlobalSensitivity::clone() const
{
  return new HSICEstimatorGlobalSensitivity(*this);
}

/** Compute the weights from the weight function */
void HSICEstimatorGlobalSensitivity::computeWeights()
{
  weights_ = Point(n_, 1.0);
}

END_NAMESPACE_OPENTURNS
