//                                               -*- C++ -*-
/**
 *  @brief The result of a heteroscedastic sparse gaussian process fitter
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
#ifndef OPENTURNS_HETEROSCEDASTICSPARSEGAUSSIANPROCESSFITTERRESULT_HXX
#define OPENTURNS_HETEROSCEDASTICSPARSEGAUSSIANPROCESSFITTERRESULT_HXX

#include "openturns/MetaModelResult.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/Sample.hxx"
#include "openturns/TriangularMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HeteroscedasticSparseGaussianProcessFitterResult
 *
 * The result of a heteroscedastic sparse gaussian process fitter
 */

class OT_API HeteroscedasticSparseGaussianProcessFitterResult
  : public MetaModelResult
{
  CLASSNAME

public:

  /** Default constructor */
  HeteroscedasticSparseGaussianProcessFitterResult();

  /** Parameter constructor after a heteroscedastic sparse gaussian process fitting */
  HeteroscedasticSparseGaussianProcessFitterResult(const Sample & inputSample,
      const Sample & outputSample,
      const CovarianceModel & covarianceModelF,
      const CovarianceModel & covarianceModelG,
      const Sample & inducingPointsF,
      const Sample & inducingPointsG,
      const TriangularMatrix & whiteningFactorF,
      const TriangularMatrix & whiteningFactorG,
      const Point & posteriorMeanF,
      const CovarianceMatrix & posteriorCovarianceF,
      const Point & posteriorMeanG,
      const CovarianceMatrix & posteriorCovarianceG,
      const Scalar mu0,
      const Scalar optimalELBO,
      const Function & metaModel);

  /** Virtual constructor */
  HeteroscedasticSparseGaussianProcessFitterResult * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Covariance models accessors */
  CovarianceModel getCovarianceModelF() const;
  CovarianceModel getCovarianceModelG() const;

  /** Inducing points accessors */
  Sample getInducingPointsF() const;
  Sample getInducingPointsG() const;

  /** Whitening factors accessors */
  TriangularMatrix getWhiteningFactorF() const;
  TriangularMatrix getWhiteningFactorG() const;

  /** Posterior moments accessors (whitened variational posteriors) */
  Point getPosteriorMeanF() const;
  CovarianceMatrix getPosteriorCovarianceF() const;
  Point getPosteriorMeanG() const;
  CovarianceMatrix getPosteriorCovarianceG() const;

  /** Prior mean of the log-variance process accessor */
  Scalar getMu0() const;

  /** optimal ELBO value */
  Scalar getOptimalELBO() const;

  /** Conditional variance accessor (latent mean process) */
  Scalar getConditionalVariance(const Point & point) const;
  Point getConditionalVariance(const Sample & sample) const;

  /** Predictive variance accessor (latent process plus lognormal noise moment) */
  Scalar getPredictiveVariance(const Point & point) const;
  Point getPredictiveVariance(const Sample & sample) const;

  /** Sequential design criteria accessors */
  UnsignedInteger computeALM(const Sample & candidateSample) const;
  Scalar computeIMSPE(const Sample & referenceSample) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The covariance models of the mean and log-variance processes */
  CovarianceModel covarianceModelF_;
  CovarianceModel covarianceModelG_;

  /** The inducing points of the mean and log-variance processes */
  Sample inducingPointsF_;
  Sample inducingPointsG_;

  /** The Cholesky factors of the inducing points covariance matrices */
  TriangularMatrix whiteningFactorF_;
  TriangularMatrix whiteningFactorG_;

  /** The means and covariances of the whitened variational posteriors */
  Point posteriorMeanF_;
  CovarianceMatrix posteriorCovarianceF_;
  Point posteriorMeanG_;
  CovarianceMatrix posteriorCovarianceG_;

  /** The prior mean of the log-variance process */
  Scalar mu0_ = 0.0;

  /** optimal ELBO value */
  Scalar optimalELBO_ = 0.0;

  // Whitened cross-covariance of a process at a point: L^{-1} k(Z, point)
  Point computeWhitenedVector(const CovarianceModel & covarianceModel,
                              const Sample & inducingPoints,
                              const TriangularMatrix & whiteningFactor,
                              const Point & point) const;

}; /* class HeteroscedasticSparseGaussianProcessFitterResult */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HETEROSCEDASTICSPARSEGAUSSIANPROCESSFITTERRESULT_HXX */
