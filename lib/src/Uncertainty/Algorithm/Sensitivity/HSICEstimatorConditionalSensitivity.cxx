//                                               -*- C++ -*-
/**
 * @brief HSICEstimatorConditionalSensitivity implements the conditional sensivity HSIC indices
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
#include "openturns/HSICEstimatorConditionalSensitivity.hxx"

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
  const CovarianceModelCollection & covarianceList
  , const Sample & X, const Sample & Y
  , const HSICStat & estimatorType
  , const Function & weightFunction)
  : HSICEstimatorImplementation(covarianceList, X, Y, estimatorType)
{
  if (!estimatorType_.isCompatibleWithConditionalAnalysis()) throw InvalidArgumentException(HERE) << "You must use an appropriate HSICStat object for ConditionalSensitivity";
  weightFunction_ = weightFunction;
}

/* Virtual constructor */
HSICEstimatorConditionalSensitivity* HSICEstimatorConditionalSensitivity::clone() const
{
  return new HSICEstimatorConditionalSensitivity(*this);
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
}

/* Method save() stores the object through the StorageManager */
void HSICEstimatorConditionalSensitivity::save(Advocate & adv) const
{
  HSICEstimatorImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void HSICEstimatorConditionalSensitivity::load(Advocate & adv)
{
  HSICEstimatorImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
