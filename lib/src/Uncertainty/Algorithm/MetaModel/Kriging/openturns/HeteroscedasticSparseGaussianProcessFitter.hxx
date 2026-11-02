//                                               -*- C++ -*-
/**
 *  @brief The class fits heteroscedastic sparse gaussian process models
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
#ifndef OPENTURNS_HETEROSCEDASTICSPARSEGAUSSIANPROCESSFITTER_HXX
#define OPENTURNS_HETEROSCEDASTICSPARSEGAUSSIANPROCESSFITTER_HXX

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Interval.hxx"
#include "openturns/GradientImplementation.hxx"
#include "openturns/HeteroscedasticSparseGaussianProcessFitterResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HeteroscedasticSparseGaussianProcessFitter
 *
 * The class fitting heteroscedastic sparse gaussian processes with a latent
 * log-variance process
 */

class OT_API HeteroscedasticSparseGaussianProcessFitter
  : public MetaModelAlgorithm
{
  CLASSNAME

public:

  /** Default constructor */
  HeteroscedasticSparseGaussianProcessFitter();

  /** Parameters constructor */
  HeteroscedasticSparseGaussianProcessFitter(const Sample & inputSample,
      const Sample & outputSample,
      const CovarianceModel & covarianceModelF,
      const CovarianceModel & covarianceModelG,
      const Sample & inducingPointsF,
      const Sample & inducingPointsG);

  /** Virtual constructor */
  HeteroscedasticSparseGaussianProcessFitter * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Perform regression */
  void run() override;

  /** Result accessor */
  HeteroscedasticSparseGaussianProcessFitterResult getResult();

  /** Objective function accessor */
  Function getObjectiveFunction();

  /** Optimization solver accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);

  /** Optimization flag accessors */
  Bool getOptimizeParameters() const;
  void setOptimizeParameters(const Bool optimizeParameters);
  Bool getOptimizeVariational() const;
  void setOptimizeVariational(const Bool optimizeVariational);

  /** Prior mean of the log-variance process accessor */
  Scalar getMu0() const;
  void setMu0(const Scalar mu0);

  /** Inducing points accessors */
  Sample getInducingPointsF() const;
  void setInducingPointsF(const Sample & inducingPointsF);
  Sample getInducingPointsG() const;
  void setInducingPointsG(const Sample & inducingPointsG);

  /** Replicate structure accessors */
  Sample getUniqueInputSample() const;
  Indices getReplicateCounts() const;
  Sample getReplicateMeanOutput() const;

  /** Covariance models accessors */
  CovarianceModel getCovarianceModelF() const;
  CovarianceModel getCovarianceModelG() const;
  CovarianceModel getReducedCovarianceModelF() const;
  CovarianceModel getReducedCovarianceModelG() const;

  /** Variational parameters accessors */
  Point getVariationalMeanF() const;
  CovarianceMatrix getVariationalCovarianceF() const;
  Point getVariationalMeanG() const;
  CovarianceMatrix getVariationalCovarianceG() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  // Maximize the ELBO
  Scalar maximizeELBO();

  // Compute the ELBO function value
  Point computeELBO(const Point & parameters);

  // Compute the gradient of the ELBO wrt the optimization parameters
  Point computeELBOGradient(const Point & parameters);

  // Compute the uncollapsed ELBO and its by-products for the current members
  Scalar computeELBOValue();

  // Build the vector of optimization parameters
  Point buildOptimizationParameters() const;

  // Build the bounds of the optimization parameters
  void buildOptimizationBounds();

  // Build the optimization parameter description
  Description buildOptimizationParameterDescription() const;

  // Size of the optimization parameter vector
  UnsignedInteger getOptimizationParameterSize() const;

  // Covariance models accessors
  void setCovarianceModelF(const CovarianceModel & covarianceModelF);
  void setCovarianceModelG(const CovarianceModel & covarianceModelG);

  // Detect exact input replicates and build the per-site sufficient statistics
  void buildReplicateStructure();

  // The covariance models parametric families
  CovarianceModel covarianceModelF_;
  CovarianceModel covarianceModelG_;
  CovarianceModel reducedCovarianceModelF_;
  CovarianceModel reducedCovarianceModelG_;

  // The inducing points (fixed)
  Sample inducingPointsF_;
  Sample inducingPointsG_;

  // Replicate structure: unique input sites with sufficient statistics
  // (counts, sums and sums of squares of the observations per site)
  Sample uniqueInputSample_;
  Indices replicateCounts_;
  Point replicateSums_;
  Point replicateSumSquares_;

  // The prior mean of the log-variance process
  Scalar mu0_ = 0.0;

  // The variational parameters (whitened): means and covariances
  Point variationalMeanF_;
  CovarianceMatrix variationalCovarianceF_;
  Point variationalMeanG_;
  CovarianceMatrix variationalCovarianceG_;

  // The optimization algorithm used for the meta-parameters estimation
  OptimizationAlgorithm solver_;

  // Bounds used for parameter optimization
  Interval optimizationBounds_;

  // Flags controlling which parameters are optimized
  Bool optimizeParameters_ = true;
  Bool optimizeVariational_ = true;

  // Boolean argument to tell if optimization has run
  Bool hasRun_ = false;

  // Cache of the last computed ELBO
  Scalar lastELBO_ = SpecFunc::LowestScalar;

private:

  // Unpack the optimization parameter vector. The Cholesky factors carry
  // log-diagonal entries so that the covariances stay positive definite
  // for any unconstrained parameter vector.
  struct UnpackedParameters
  {
    Point covarianceParametersF;
    Point covarianceParametersG;
    Scalar mu0;
    Point variationalMeanF;
    Matrix variationalCholF;
    Point variationalMeanG;
    Matrix variationalCholG;
  };
  UnpackedParameters unpackParameters(const Point & parameters) const;

  // Helper class to compute the ELBO of the model.
  // Owns a clone of the algorithm so that the returned Function can outlive the
  // original algorithm without creating a dangling reference.
  class ELBOEvaluation: public EvaluationImplementation
  {
  public:
    // Constructor from a HeteroscedasticSparseGaussianProcessFitter algorithm
    ELBOEvaluation(HeteroscedasticSparseGaussianProcessFitter & algorithm)
      : EvaluationImplementation()
      , algorithm_(algorithm.clone())
    {
      // Nothing to do
    }

    ELBOEvaluation * clone() const override
    {
      return new ELBOEvaluation(*this);
    }

    // It is a simple call to the computeELBO() of the algo
    Point operator() (const Point & point) const override
    {
      const Point value(algorithm_->computeELBO(point));
      return value;
    }

    UnsignedInteger getInputDimension() const override
    {
      return algorithm_->getOptimizationParameterSize();
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const override
    {
      return algorithm_->buildOptimizationParameterDescription();
    }

    Description getOutputDescription() const override
    {
      return Description(1, "ELBO");
    }

    Description getDescription() const override
    {
      Description description(getInputDescription());
      description.add(getOutputDescription());
      return description;
    }

    String __repr__() const override
    {
      OSS oss;
      // Don't print algorithm_ here as it will result in an infinite loop!
      oss << "ELBOEvaluation";
      return oss;
    }

    String __str__(const String & offset = "") const override
    {
      // Don't print algorithm_ here as it will result in an infinite loop!
      return OSS() << offset << __repr__();
    }

  private:
    mutable Pointer<HeteroscedasticSparseGaussianProcessFitter> algorithm_;
  }; // ELBOEvaluation

  // Helper class to compute the gradient of the ELBO of the model.
  // Owns a clone, same as the evaluation class above.
  class ELBOGradient: public GradientImplementation
  {
  public:
    // Constructor from a HeteroscedasticSparseGaussianProcessFitter algorithm
    ELBOGradient(HeteroscedasticSparseGaussianProcessFitter & algorithm)
      : GradientImplementation()
      , algorithm_(algorithm.clone())
    {
      // Nothing to do
    }

    ELBOGradient * clone() const override
    {
      return new ELBOGradient(*this);
    }

    // It is a simple call to the computeELBOGradient() of the algo
    Matrix gradient(const Point & point) const override
    {
      const Point value(algorithm_->computeELBOGradient(point));
      const UnsignedInteger parameterSize = algorithm_->getOptimizationParameterSize();
      Matrix result(parameterSize, 1);
      for (UnsignedInteger i = 0; i < parameterSize; ++i)
        result(i, 0) = value[i];
      return result;
    }

    UnsignedInteger getInputDimension() const override
    {
      return algorithm_->getOptimizationParameterSize();
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    String __repr__() const override
    {
      OSS oss;
      // Don't print algorithm_ here as it will result in an infinite loop!
      oss << "ELBOGradient";
      return oss;
    }

    String __str__(const String & offset = "") const override
    {
      // Don't print algorithm_ here as it will result in an infinite loop!
      return OSS() << offset << __repr__();
    }

  private:
    mutable Pointer<HeteroscedasticSparseGaussianProcessFitter> algorithm_;
  }; // ELBOGradient

  // Initialize default optimization solver
  void initializeDefaultOptimizationAlgorithm();

  /** reset method */
  void reset();

  // By-products of the last ELBO evaluation
  TriangularMatrix whiteningFactorF_;
  TriangularMatrix whiteningFactorG_;

  /** Result */
  HeteroscedasticSparseGaussianProcessFitterResult result_;

}; // class HeteroscedasticSparseGaussianProcessFitter


END_NAMESPACE_OPENTURNS

#endif
