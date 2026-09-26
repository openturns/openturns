//                                               -*- C++ -*-
/**
 *  @brief The class fits sparse gaussian process models
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
#ifndef OPENTURNS_SPARSEGAUSSIANPROCESSFITTER_HXX
#define OPENTURNS_SPARSEGAUSSIANPROCESSFITTER_HXX

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Interval.hxx"
#include "openturns/GradientImplementation.hxx"
#include "openturns/SparseGaussianProcessFitterResult.hxx"
#include "openturns/HMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SparseGaussianProcessFitter
 *
 * The class fitting sparse gaussian processes
 */

class OT_API SparseGaussianProcessFitter
  : public MetaModelAlgorithm
{
  CLASSNAME

public:

  typedef SparseGaussianProcessFitterResult::LinearAlgebra LinearAlgebra;

  /** Default constructor */
  SparseGaussianProcessFitter();

  /** Parameters constructor */
  SparseGaussianProcessFitter(const Sample & inputSample,
                              const Sample & outputSample,
                              const CovarianceModel & covarianceModel,
                              const Sample & inducingPoints);

  /** Parameters constructor with an initial number of inducing points */
  SparseGaussianProcessFitter(const Sample & inputSample,
                              const Sample & outputSample,
                              const CovarianceModel & covarianceModel,
                              const UnsignedInteger numberOfInducingPoints);

  /** Virtual constructor */
  SparseGaussianProcessFitter * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Perform regression */
  void run() override;

  /** result accessor */
  SparseGaussianProcessFitterResult getResult();

  /** Objective function (ELBO) accessor */
  Function getObjectiveFunction();

  /** Optimization solver accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);

  /** Optimization flag accessors */
  Bool getOptimizeParameters() const;
  void setOptimizeParameters(const Bool optimizeParameters);
  Bool getOptimizeInducingPoints() const;
  void setOptimizeInducingPoints(const Bool optimizeInducingPoints);
  Bool getOptimizeNoiseStdDev() const;
  void setOptimizeNoiseStdDev(const Bool optimizeNoiseStdDev);

  /** Noise standard deviation accessor */
  Scalar getNoiseStdDev() const;
  void setNoiseStdDev(const Scalar noiseStdDev);

  /** Inducing points accessor */
  Sample getInducingPoints() const;
  void setInducingPoints(const Sample & inducingPoints);

  /** Covariance model accessors */
  CovarianceModel getCovarianceModel() const;
  CovarianceModel getReducedCovarianceModel() const;

  /** Linear algebra method accessors */
  LinearAlgebra getMethod() const;
  void setMethod(const LinearAlgebra method);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  // Maximize the ELBO
  Scalar maximizeELBO();

  // Set the linear algebra method from the ResourceMap
  void initializeMethod();

  // Compute the ELBO function value
  Point computeELBO(const Point & parameters);

  // Compute the gradient of the ELBO wrt the optimization parameters
  Point computeELBOGradient(const Point & parameters);

  // Compute the collapsed ELBO for the given inducing points and noise standard deviation
  Scalar computeELBOValue(const Sample & inducingPoints,
                          const Scalar noiseStdDev);

  // Initialize default optimization solver
  void initializeDefaultOptimizationAlgorithm();

  /** reset method */
  void reset();

private:

  // Unpack the optimization parameter vector into covariance parameters, noise
  // standard deviation and inducing points.
  struct UnpackedParameters
  {
    Point covarianceParameters;
    Scalar noiseStdDev;
    Sample inducingPoints;
  };
  UnpackedParameters unpackParameters(const Point & parameters) const;

  // Helper class to compute the ELBO of the model.
  // Owns a clone of the algorithm so that the returned Function can outlive the
  // original algorithm without creating a dangling reference.
  class ELBOEvaluation: public EvaluationImplementation
  {
  public:
    // Constructor from a SparseGaussianProcessFitter algorithm
    ELBOEvaluation(SparseGaussianProcessFitter & algorithm)
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
    mutable Pointer<SparseGaussianProcessFitter> algorithm_;
  }; // ELBOEvaluation

  // Helper class to compute the gradient of the ELBO of the model.
  // Owns a clone, same as the evaluation class above.
  class ELBOGradient: public GradientImplementation
  {
  public:
    // Constructor from a SparseGaussianProcessFitter algorithm
    ELBOGradient(SparseGaussianProcessFitter & algorithm)
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
    mutable Pointer<SparseGaussianProcessFitter> algorithm_;
  }; // ELBOGradient

  // Build the vector of optimization parameters
  Point buildOptimizationParameters() const;

  // Build the bounds of the optimization parameters
  void buildOptimizationBounds();

  // Build the optimization parameter description
  Description buildOptimizationParameterDescription() const;

  // Size of the optimization parameter vector
  UnsignedInteger getOptimizationParameterSize() const;

  // Covariance model accessor
  void setCovarianceModel(const CovarianceModel & covarianceModel);

  // The covariance model parametric family
  CovarianceModel covarianceModel_;
  CovarianceModel reducedCovarianceModel_;

  // The inducing points
  Sample inducingPoints_;

  // The noise standard deviation
  Scalar noiseStdDev_;

  // The optimization algorithm used for the meta-parameters estimation
  OptimizationAlgorithm solver_;

  // Bounds used for parameter optimization
  Interval optimizationBounds_;

  // Flags controlling which parameters are optimized
  Bool optimizeParameters_ = true;
  Bool optimizeInducingPoints_ = false;
  Bool optimizeNoiseStdDev_ = true;

  // Boolean argument to tell if optimization has run
  Bool hasRun_ = false;

  // Cache of the last computed ELBO
  Scalar lastELBO_ = SpecFunc::LowestScalar;

  // By-products of the last ELBO evaluation
  TriangularMatrix whiteningFactor_;
  HMatrix whiteningFactorHMatrix_;
  Point posteriorMean_;
  CovarianceMatrix posteriorCovariance_;

  // The linear algebra method
  LinearAlgebra method_ = SparseGaussianProcessFitterResult::LAPACK;

  /** Result */
  SparseGaussianProcessFitterResult result_;

}; // class SparseGaussianProcessFitter


END_NAMESPACE_OPENTURNS

#endif
