//                                               -*- C++ -*-
/**
 *  @brief The result of a sparse gaussian process fitter
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SPARSEGAUSSIANPROCESSFITTERRESULT_HXX
#define OPENTURNS_SPARSEGAUSSIANPROCESSFITTERRESULT_HXX

#include "openturns/MetaModelResult.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/Sample.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/HMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SparseGaussianProcessFitterResult
 *
 * The result of a sparse gaussian process fitter
 */

class OT_API SparseGaussianProcessFitterResult
  : public MetaModelResult
{
  CLASSNAME

public:

  enum LinearAlgebra { LAPACK, HMAT };

  /** Default constructor */
  SparseGaussianProcessFitterResult();

  /** Parameter constructor after a sparse gaussian process fitting */
  SparseGaussianProcessFitterResult(const Sample & inputSample,
                                    const Sample & outputSample,
                                    const CovarianceModel & covarianceModel,
                                    const Sample & inducingPoints,
                                    const TriangularMatrix & whiteningFactor,
                                    const Point & posteriorMean,
                                    const CovarianceMatrix & posteriorCovariance,
                                     const Scalar noiseStdDev,
                                    const Scalar optimalELBO,
                                    const Function & metaModel,
                                    const LinearAlgebra linearAlgebraMethod = LAPACK);

  /** Virtual constructor */
  SparseGaussianProcessFitterResult * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Covariance model accessor */
  CovarianceModel getCovarianceModel() const;

  /** Inducing points accessor */
  Sample getInducingPoints() const;

  /** Whitening factor accessor */
  TriangularMatrix getWhiteningFactor() const;

  /** Whitening factor accessor, in HMAT form */
  HMatrix getWhiteningFactorHMatrix() const;

  /** Whitening factor setter */
  void setWhiteningFactorHMatrix(const HMatrix & whiteningFactorHMatrix);

  /** Linear algebra method accessor */
  LinearAlgebra getLinearAlgebraMethod() const;

  /** Posterior mean accessor */
  Point getPosteriorMean() const;

  /** Posterior covariance accessor */
  CovarianceMatrix getPosteriorCovariance() const;

  /** Noise variance accessor */
  Scalar getNoiseStdDev() const;

  /** optimal ELBO value */
  Scalar getOptimalELBO() const;

  /** Conditional variance accessor */
  Scalar getConditionalVariance(const Point & point) const;
  Point getConditionalVariance(const Sample & sample) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The covariance model */
  CovarianceModel covarianceModel_;

  /** The inducing points */
  Sample inducingPoints_;

  /** The Cholesky factor of the inducing points covariance matrix */
  TriangularMatrix whiteningFactor_;

  /** The Cholesky factor of the inducing points covariance matrix, in HMAT form */
  HMatrix whiteningFactorHMatrix_;

  /** The linear algebra method */
  LinearAlgebra linearAlgebraMethod_ = LAPACK;

  /** The mean of the whitened variational posterior */
  Point posteriorMean_;

  /** The covariance of the whitened variational posterior */
  CovarianceMatrix posteriorCovariance_;

  /** The noise standard deviation */
  Scalar noiseStdDev_ = 1.0;

  /** optimal ELBO value */
  Scalar optimalELBO_ = 0.0;

}; /* class SparseGaussianProcessFitterResult */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPARSEGAUSSIANPROCESSFITTERRESULT_HXX */
