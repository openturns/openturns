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
  // Nothing to do
}

/* Virtual constructor */
HSICEstimatorGlobalSensitivity* HSICEstimatorGlobalSensitivity::clone() const
{
  return new HSICEstimatorGlobalSensitivity(*this);
}

/* Compute the weight matrix */
SquareMatrix HSICEstimatorGlobalSensitivity::computeWeightMatrix(const Sample&) const
{
  IdentityMatrix mat(n_);
  return mat;
}

/* Get the asymptotic p-values */
Point HSICEstimatorGlobalSensitivity::getPValuesAsymptotic() const
{
  if(!(isAlreadyComputedPValuesAsymptotic_))
  {
    computePValuesAsymptotic();
    isAlreadyComputedPValuesAsymptotic_ = true ;
  }
  return PValuesAsymptotic_;
}

/* Reset all indices to void */
void HSICEstimatorGlobalSensitivity::resetIndices()
{
  HSICEstimatorImplementation::resetIndices();
  PValuesAsymptotic_ = Point();
  isAlreadyComputedPValuesAsymptotic_ = false;
}

/* Draw the asymptotic p-values */
Graph HSICEstimatorGlobalSensitivity::drawPValuesAsymptotic() const
{
  return drawValues(getPValuesAsymptotic(), "Asymptotic p-values");
}

/* Compute all indices at once */
void HSICEstimatorGlobalSensitivity::run() const
{
  /* Compute the HSIC and R2-HSIC indices */
  if(!(isAlreadyComputedIndices_))
  {
    computeIndices();
  }

  /* Compute the p-values by permutation */
  if(!(isAlreadyComputedPValuesPermutation_))
  {
    computePValuesPermutation();
  }

  /* Compute the p-values asymptotically */
  if(!(isAlreadyComputedPValuesAsymptotic_))
  {
    computePValuesAsymptotic();
  }

}

/* Method save() stores the object through the StorageManager */
void HSICEstimatorGlobalSensitivity::save(Advocate & adv) const
{
  HSICEstimatorImplementation::save(adv);
  adv.saveAttribute( "PValuesAsymptotic_", PValuesAsymptotic_ );
  adv.saveAttribute( "isAlreadyComputedPValuesAsymptotic_", isAlreadyComputedPValuesAsymptotic_ );
}

/* Method load() reloads the object from the StorageManager */
void HSICEstimatorGlobalSensitivity::load(Advocate & adv)
{
  HSICEstimatorImplementation::load(adv);
  adv.loadAttribute( "PValuesAsymptotic_", PValuesAsymptotic_ );
  adv.loadAttribute( "isAlreadyComputedPValuesAsymptotic_", isAlreadyComputedPValuesAsymptotic_ );
}

END_NAMESPACE_OPENTURNS
