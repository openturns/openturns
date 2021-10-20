//                                               -*- C++ -*-
/**
 * @brief HSICEstimatorGlobalSensitivity implements the global sensivity HSIC indices
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
#include "openturns/HSICEstimatorGlobalSensitivity.hxx"

BEGIN_NAMESPACE_OPENTURNS
CLASSNAMEINIT(HSICEstimatorGlobalSensitivity)

/** Default */
HSICEstimatorGlobalSensitivity::HSICEstimatorGlobalSensitivity(): HSICEstimatorImplementation()
{
  // Nothing to do
}

/** Constructor */
HSICEstimatorGlobalSensitivity::HSICEstimatorGlobalSensitivity(const CovarianceModelCollection & covarianceList, const Sample & X, const Sample & Y,
    const HSICStat & estimatorType): HSICEstimatorImplementation(covarianceList, X, Y, estimatorType)
{
  // Nothing to do
}

/** Virtual constructor */
HSICEstimatorGlobalSensitivity* HSICEstimatorGlobalSensitivity::clone() const
{
  return new HSICEstimatorGlobalSensitivity(*this);
}

/** Compute the weight matrix from the weight function */
SquareMatrix HSICEstimatorGlobalSensitivity::computeWeightMatrix(const Sample&) const
{
  IdentityMatrix mat(n_);
  return mat;
}

/** Compute the asymptotic p-values */
void HSICEstimatorGlobalSensitivity::computePValuesAsymptotic()
{
  HSICEstimatorImplementation::CovarianceModelCollection coll = covarianceList_;
  SquareMatrix W(computeWeightMatrix(outputSample_));

  PValuesAsymptotic_ = Point(inputDimension_);

  Matrix H(n_, n_);
  for(UnsignedInteger j = 0; j < n_; ++j)
  {
    for(UnsignedInteger i = 0; i < n_; ++i)
    {
      H(i, j) = -1.0 / n_;
    }
  }
  for(UnsignedInteger j = 0; j < n_; ++j)
  {
    H(j, j) += 1.0;
  }

  CovarianceMatrix Ky(coll[inputDimension_].discretize(outputSample_));
  Scalar traceKy = Ky.computeTrace();
  Scalar sumKy = Ky.computeSumElements();

  Scalar Ey = (sumKy - traceKy) / n_ / (n_ - 1 );
  Matrix By = H * Ky * H;

  for(UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    Sample Xi(inputSample_.getMarginal(dim));
    Scalar HSICobs = computeHSICIndex(Xi, outputSample_, coll[dim], coll[inputDimension_], W);

    CovarianceMatrix Kx(coll[dim].discretize(Xi));
    Scalar traceKx = Kx.computeTrace();
    Scalar sumKx = Kx.computeSumElements() ;
    Scalar Ex = (sumKx - traceKx) / n_ / (n_ - 1);

    Matrix Bx = H * Kx * H;

    /* Hadamard product then square all elements */
    Matrix B(n_, n_);
    B = Bx.computeHadamardProduct(By);
    B.squareElements();

    Point nullDiag(n_);
    B.setDiagonal(nullDiag, 0);

    Scalar mHSIC = (1 + Ex * Ey - Ex - Ey) / n_;
    Scalar factor = 2.0 * (n_ - 4) * (n_ - 5) / n_ / (n_ - 1) / (n_ - 2) / (n_ - 3) / n_ / (n_ - 1);
    Scalar varHSIC = B.computeSumElements() * factor;

    Scalar alpha = mHSIC * mHSIC / varHSIC;
    Scalar beta = n_ * varHSIC / mHSIC;

    Gamma distribution(alpha, 1.0 / beta);
    Scalar p = estimatorType_.computePValue(distribution, n_, HSICobs, mHSIC);
    PValuesAsymptotic_[dim] = p;
  }

}

/** Get the asymptotic p-values */
Point HSICEstimatorGlobalSensitivity::getPValuesAsymptotic()
{
  if( PValuesAsymptotic_.getDimension() == 0)
  {
    computePValuesAsymptotic();
  }
  return PValuesAsymptotic_;
}

/** Draw the asymptotic p-values */
Graph HSICEstimatorGlobalSensitivity::drawPValuesAsymptotic()
{
  String title = "Asymptotic p-values";
  Graph gph = drawValues(getPValuesAsymptotic(), title);
  return gph;
}

END_NAMESPACE_OPENTURNS
