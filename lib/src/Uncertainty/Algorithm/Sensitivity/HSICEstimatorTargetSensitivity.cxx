//                                               -*- C++ -*-
/**
 * @brief HSICEstimatorTargetSensitivity implements the target sensitivity HSIC indices
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
#include "openturns/HSICEstimatorTargetSensitivity.hxx"

BEGIN_NAMESPACE_OPENTURNS
CLASSNAMEINIT(HSICEstimatorTargetSensitivity)

/* Default */
HSICEstimatorTargetSensitivity::HSICEstimatorTargetSensitivity()
  : HSICEstimatorImplementation()
  , isAlreadyComputedPValuesAsymptotic_(false)
{
  // Nothing to do
}

/* Constructor */
HSICEstimatorTargetSensitivity::HSICEstimatorTargetSensitivity(
  const CovarianceModelCollection & covarianceList
  , const Sample & X
  , const Sample & Y
  , const HSICStat & estimatorType
  , const Function & filterFunction )
  : HSICEstimatorImplementation(covarianceList, X, Y, estimatorType)
  , isAlreadyComputedPValuesAsymptotic_(false)
{
  filterFunction_ =  filterFunction;
  /* apply filter */
  outputSample_ = filterFunction_(outputSample_);
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

/* Compute the asymptotic p-values */
void HSICEstimatorTargetSensitivity::computePValuesAsymptotic() const
{
  SquareMatrix W(computeWeightMatrix(outputSample_));

  PValuesAsymptotic_ = Point(inputDimension_);

  SquareMatrix H(n_, Collection<Scalar>(n_ * n_, -1.0 / n_));
  for(UnsignedInteger j = 0; j < n_; ++j)
  {
    H(j, j) += 1.0;
  }

  const CovarianceMatrix Ky(covarianceList_[inputDimension_].discretize(outputSample_));
  const Scalar traceKy = Ky.computeTrace();
  const Scalar sumKy = Ky.computeSumElements();

  const Scalar Ey = (sumKy - traceKy) / n_ / (n_ - 1 );
  const Matrix By = H * Ky * H;

  for(UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    const Sample Xi(inputSample_.getMarginal(dim));
    const Scalar HSICobs = computeHSICIndex(Xi, outputSample_, covarianceList_[dim], covarianceList_[inputDimension_], W);

    const CovarianceMatrix Kx(covarianceList_[dim].discretize(Xi));
    const Scalar traceKx = Kx.computeTrace();
    const Scalar sumKx = Kx.computeSumElements();
    const Scalar Ex = (sumKx - traceKx) / n_ / (n_ - 1);

    const Matrix Bx = H * Kx * H;

    /* Hadamard product then square all elements */
    SquareMatrix B(Bx.computeHadamardProduct(By).getImplementation());
    B.squareElements();

    const Point nullDiag(n_);
    B.setDiagonal(nullDiag, 0);

    const Scalar mHSIC = (1 + Ex * Ey - Ex - Ey) / n_;
    const Scalar factor = 2.0 * (n_ - 4) * (n_ - 5) / n_ / (n_ - 1) / (n_ - 2) / (n_ - 3) / n_ / (n_ - 1);
    const Scalar varHSIC = B.computeSumElements() * factor;

    const Scalar alpha = mHSIC * mHSIC / varHSIC;
    const Scalar beta = n_ * varHSIC / mHSIC;

    const Gamma distribution(alpha, 1.0 / beta);
    const Scalar p = estimatorType_.computePValue(distribution, n_, HSICobs, mHSIC);
    PValuesAsymptotic_[dim] = p;
  }
  isAlreadyComputedPValuesAsymptotic_ = true ;
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
    isAlreadyComputedIndices_ = true ;
  }

  /* Compute the p-values by permutation */
  if(!(isAlreadyComputedPValuesPermutation_))
  {
    computePValuesPermutation();
    isAlreadyComputedPValuesPermutation_ = true ;
  }

  /* Compute the p-values asymptotically */
  if(!(isAlreadyComputedPValuesAsymptotic_))
  {
    computePValuesAsymptotic();
    isAlreadyComputedPValuesAsymptotic_ = true ;
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
