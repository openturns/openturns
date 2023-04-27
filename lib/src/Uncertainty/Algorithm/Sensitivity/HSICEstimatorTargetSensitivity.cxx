//                                               -*- C++ -*-
/**
 * @brief HSICEstimatorTargetSensitivity implements the target sensitivity HSIC indices
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
#include "openturns/HSICEstimatorTargetSensitivity.hxx"

BEGIN_NAMESPACE_OPENTURNS
CLASSNAMEINIT(HSICEstimatorTargetSensitivity)

/* Default */
HSICEstimatorTargetSensitivity::HSICEstimatorTargetSensitivity()
  : HSICEstimatorImplementation()
  , unfilteredSample_()

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
  unfilteredSample_ = outputSample_;
  /* apply filter */
  outputSample_ = filterFunction_(unfilteredSample_);
  computeCovarianceMatrices();
  computeWeights();
}

/* Virtual constructor */
HSICEstimatorTargetSensitivity* HSICEstimatorTargetSensitivity::clone() const
{
  return new HSICEstimatorTargetSensitivity(*this);
}

/* Get the filter function */
Function HSICEstimatorTargetSensitivity::getFilterFunction() const
{
  return filterFunction_;
}

/* Get the filter function */
void HSICEstimatorTargetSensitivity::setFilterFunction(const Function & filterFunction)
{
  filterFunction_ =  filterFunction;
  /* apply filter */
  outputSample_ = filterFunction_(unfilteredSample_);
  resetIndices();
  outputCovarianceMatrix_ = covarianceModelCollection_[inputDimension_].discretize(outputSample_);
}

/** Compute the weights from the weight function */
void HSICEstimatorTargetSensitivity::computeWeights()
{
  weights_ = Point(n_, 1.0);
}


/* Method save() stores the object through the StorageManager */
void HSICEstimatorTargetSensitivity::save(Advocate & adv) const
{
  HSICEstimatorImplementation::save(adv);
  adv.saveAttribute( "filterFunction_", filterFunction_ );
  adv.saveAttribute( "unfilteredSample_", unfilteredSample_);
}

/* Method load() reloads the object from the StorageManager */
void HSICEstimatorTargetSensitivity::load(Advocate & adv)
{
  HSICEstimatorImplementation::load(adv);
  adv.loadAttribute( "filterFunction_", filterFunction_ );
  adv.loadAttribute( "unfilteredSample_", unfilteredSample_);
}

END_NAMESPACE_OPENTURNS
