//                                               -*- C++ -*-
/**
 *  @brief The class builds gaussian processes
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
#ifndef OPENTURNS_GAUSSIANPROCESSFITTER_HXX
#define OPENTURNS_GAUSSIANPROCESSFITTER_HXX

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/Basis.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/KrigingResult.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/GaussianProcessFitterResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussianProcessFitter
 *
 * The class fitting gaussian processes
 */

class OT_API GaussianProcessFitter
  : public MetaModelAlgorithm
{
  CLASSNAME

public:

  typedef GaussianProcessFitterResult::LinearAlgebra LinearAlgebra;

  /** Default constructor */
  GaussianProcessFitter();

  /** Parameters constructor */
  GaussianProcessFitter (const Sample & inputSample,
                         const Sample & outputSample,
                         const CovarianceModel & covarianceModel,
                         const Basis & basis = Basis(0));

  /** Virtual constructor */
  GaussianProcessFitter * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Perform regression */
  void run() override;

  /** result accessor */
  GaussianProcessFitterResult getResult();

  /** Objective function (reduced log-Likelihood) accessor */
  Function getReducedLogLikelihoodFunction();

  /** Optimization solver accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);

  /** Optimization flag accessor */
  Bool getOptimizeParameters() const;
  void setOptimizeParameters(const Bool optimizeParameters);

  /** Accessor to optimization bounds */
  void setOptimizationBounds(const Interval & optimizationBounds);
  Interval getOptimizationBounds() const;

  /** Accessor to keep cholesky (for Regression usage)*/
  Bool getKeepCholeskyFactor() const;
  void setKeepCholeskyFactor(const Bool keepCholeskyFactor);

  // method accessors
  void setMethod(const LinearAlgebra method);
  LinearAlgebra getMethod() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  // Maximize the reduced log-likelihood
  Scalar maximizeReducedLogLikelihood();

  // Compute the output log-likelihood function
  Point computeReducedLogLikelihood(const Point & parameters);
  Scalar computeLapackLogDeterminantCholesky();
  Scalar computeHMatLogDeterminantCholesky();

  // Compute the design matrix on the input sample
  void computeF();

  /** Method accessor (lapack/hmat) */
  void initializeMethod();

  /** reset method - If one change method */
  void reset();

  // Initialize default optimization solver
  void initializeDefaultOptimizationAlgorithm();

private:

  // Helper class to compute the reduced log-likelihood function of the model
  class ReducedLogLikelihoodEvaluation: public EvaluationImplementation
  {
  public:
    // Constructor from a GLM algorithm
    ReducedLogLikelihoodEvaluation(GaussianProcessFitter & algorithm)
      : EvaluationImplementation()
      , algorithm_(algorithm)
    {
      // Nothing to do
    }

    ReducedLogLikelihoodEvaluation * clone() const override
    {
      return new ReducedLogLikelihoodEvaluation(*this);
    }

    // It is a simple call to the computeReducedLogLikelihood() of the algo
    Point operator() (const Point & point) const override
    {
      const Point value(algorithm_.computeReducedLogLikelihood(point));
      return value;
    }

    UnsignedInteger getInputDimension() const override
    {
      return algorithm_.getReducedCovarianceModel().getParameter().getDimension();
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const override
    {
      return algorithm_.getReducedCovarianceModel().getParameterDescription();
    }

    Description getOutputDescription() const override
    {
      return Description(1, "ReducedLogLikelihood");
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
      oss << "ReducedLogLikelihoodEvaluation";
      return oss;
    }

    String __str__(const String & offset = "") const override
    {
      // Don't print algorithm_ here as it will result in an infinite loop!
      return OSS() << offset << __repr__();
    }

  private:
    GaussianProcessFitter & algorithm_;
  }; // ReducedLogLikelihoodEvaluation

  /** Covariance model accessor */
  void setCovarianceModel(const CovarianceModel & covarianceModel);
  CovarianceModel getCovarianceModel() const;
  CovarianceModel getReducedCovarianceModel() const;

  /** Set basis collection method */
  void setBasis(const Basis & basis);

  // The covariance model parametric family
  CovarianceModel covarianceModel_;
  CovarianceModel reducedCovarianceModel_;

  // The optimization algorithm used for the meta-parameters estimation
  OptimizationAlgorithm solver_;

  // Bounds used for parameter optimization
  Interval optimizationBounds_;

  // The coefficients of the current output conditional expectation part
  Point beta_;
  // Temporarily used to compute gamma
  Point rho_;
  // The current output Gram matrix
  Matrix F_;

  /** Result */
  GaussianProcessFitterResult result_;

  /** BasisCollection */
  Basis basis_;

  /** Cholesky factor ==>  TriangularMatrix */
  TriangularMatrix covarianceCholeskyFactor_;

  /** Cholesky factor when using hmat-oss */
  HMatrix covarianceCholeskyFactorHMatrix_;

  /** Boolean argument for keep covariance */
  Bool keepCholeskyFactor_ = ResourceMap::GetAsBool("GaussianProcessFitter-KeepCovariance");

  /** Linear algebra */
  LinearAlgebra method_ = GaussianProcessFitterResult::LAPACK;

  /** Bool to tell if optimization has run */
  Bool hasRun_ = false;

  /** Flag to tell if the parameters of the covariance model
      have to be optimized */
  Bool optimizeParameters_ = ResourceMap::GetAsBool("GaussianProcessFitter-OptimizeParameters");

  /** Flag to tell if the amplitude parameters are estimated using an analytical derivation */
  Bool analyticalAmplitude_  = false;

  /** Cache of the last computed reduced log-likelihood */
  Scalar lastReducedLogLikelihood_ = SpecFunc::LowestScalar;
}; // class GaussianProcessFitter


END_NAMESPACE_OPENTURNS

#endif
