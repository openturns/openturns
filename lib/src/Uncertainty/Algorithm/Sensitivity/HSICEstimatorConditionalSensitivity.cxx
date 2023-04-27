//                                               -*- C++ -*-
/**
 * @brief HSICEstimatorConditionalSensitivity implements the conditional sensivity HSIC indices
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
#include "openturns/HSICEstimatorConditionalSensitivity.hxx"
#include "openturns/HSICVStat.hxx"

BEGIN_NAMESPACE_OPENTURNS
CLASSNAMEINIT(HSICEstimatorConditionalSensitivity)

/* Default */
HSICEstimatorConditionalSensitivity::HSICEstimatorConditionalSensitivity()
  : HSICEstimatorImplementation()
{
  // Nothing to do
}

/* Constructor */
HSICEstimatorConditionalSensitivity::HSICEstimatorConditionalSensitivity(
  const CovarianceModelCollection & covarianceModelCollection
  , const Sample & X
  , const Sample & Y
  , const Function & weightFunction)
  : HSICEstimatorImplementation(covarianceModelCollection, X, Y, HSICVStat())
{
  computeCovarianceMatrices();
  setWeightFunction(weightFunction);
}

/* Virtual constructor */
HSICEstimatorConditionalSensitivity* HSICEstimatorConditionalSensitivity::clone() const
{
  return new HSICEstimatorConditionalSensitivity(*this);
}

/* Compute the asymptotic p-values */
void HSICEstimatorConditionalSensitivity::computePValuesAsymptotic() const
{
  throw NotYetImplementedException(HERE) << "HSICEstimatorConditionalSensitivity cannot compute asymptotic p-values.";
}

/* Get the weight function */
Function HSICEstimatorConditionalSensitivity::getWeightFunction() const
{
  return weightFunction_;
}

/* Set the weight function: user should check properties! */
void HSICEstimatorConditionalSensitivity::setWeightFunction(const Function & weightFunction)
{
  weightFunction_ = weightFunction;
  resetIndices();
  computeWeights();
}

/** Compute the weights from the weight function */
void HSICEstimatorConditionalSensitivity::computeWeights()
{
  const Sample wY(weightFunction_(outputSample_));
  const Scalar meanWY(wY.computeMean()[0]);
  weights_ = wY.asPoint() / meanWY;
}

END_NAMESPACE_OPENTURNS
