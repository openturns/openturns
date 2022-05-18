//                                               -*- C++ -*-
/**
 * @brief HSICEstimatorTargetSensitivity implements the target sensitivity HSIC indices
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/HSICEstimatorTargetSensitivity.hxx"

BEGIN_NAMESPACE_OPENTURNS
CLASSNAMEINIT(HSICEstimatorTargetSensitivity)

/* Default */
HSICEstimatorTargetSensitivity::HSICEstimatorTargetSensitivity()
  : HSICEstimatorImplementation()
{
  // Nothing to do
}

/* Constructor */
HSICEstimatorTargetSensitivity::HSICEstimatorTargetSensitivity(
  const CovarianceModelCollection & covarianceModelCollection
  , const Sample & X
  , const Sample & Y
  , const HSICStat & estimatorType
  , const Function & filterFunction )
  : HSICEstimatorImplementation(covarianceModelCollection, X, Y, estimatorType)
{
  filterFunction_ =  filterFunction;
  /* apply filter */
  outputSample_ = filterFunction_(outputSample_);
  computeCovarianceMatrices();
}

/* Virtual constructor */
HSICEstimatorTargetSensitivity* HSICEstimatorTargetSensitivity::clone() const
{
  return new HSICEstimatorTargetSensitivity(*this);
}

/* Compute the weight matrix from a sample */
SquareMatrix HSICEstimatorTargetSensitivity::computeWeightMatrix(const Sample&) const
{
  /* Identity matrix */
  const IdentityMatrix mat(n_);
  return mat;
}

/* Get the asymptotic p-values */
Point HSICEstimatorTargetSensitivity::getPValuesAsymptotic() const
{
  if(!(isAlreadyComputedPValuesAsymptotic_))
  {
    computePValuesAsymptotic();
    isAlreadyComputedPValuesAsymptotic_ = true ;
  }
  return PValuesAsymptotic_;
}

/* Get the filter functon */
Function HSICEstimatorTargetSensitivity::getFilterFunction() const
{
  return filterFunction_;
}

/* Get the filter functon */
void HSICEstimatorTargetSensitivity::setFilterFunction(const Function & filterFunction)
{
  filterFunction_ =  filterFunction;
  /* apply filter */
  outputSample_ = filterFunction_(outputSample_);
  resetIndices();
  computeCovarianceMatrices();
}

/* Reset all indices to void */
void HSICEstimatorTargetSensitivity::resetIndices()
{
  HSICEstimatorImplementation::resetIndices();
  PValuesAsymptotic_ = Point();
  isAlreadyComputedPValuesAsymptotic_ = false;
}

/* Draw the asymptotic p-values */
Graph HSICEstimatorTargetSensitivity::drawPValuesAsymptotic() const
{
  return drawValues(getPValuesAsymptotic(), "Asymptotic p-values");
}

/* Compute all indices at once */
void HSICEstimatorTargetSensitivity::run() const
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
void HSICEstimatorTargetSensitivity::save(Advocate & adv) const
{
  HSICEstimatorImplementation::save(adv);
  adv.saveAttribute( "PValuesAsymptotic_", PValuesAsymptotic_ );
  adv.saveAttribute( "isAlreadyComputedPValuesAsymptotic_", isAlreadyComputedPValuesAsymptotic_ );
  adv.saveAttribute( "filterFunction_", filterFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void HSICEstimatorTargetSensitivity::load(Advocate & adv)
{
  HSICEstimatorImplementation::load(adv);
  adv.loadAttribute( "PValuesAsymptotic_", PValuesAsymptotic_ );
  adv.loadAttribute( "isAlreadyComputedPValuesAsymptotic_", isAlreadyComputedPValuesAsymptotic_ );
  adv.loadAttribute( "filterFunction_", filterFunction_ );
}

END_NAMESPACE_OPENTURNS
