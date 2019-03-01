//                                               -*- C++ -*-
/**
 *  @brief The class builds generalized linear models
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_GENERALLINEARMODELALGORITHM_HXX
#define OPENTURNS_GENERALLINEARMODELALGORITHM_HXX

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/Basis.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/KrigingResult.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/GeneralLinearModelResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GeneralLinearModelAlgorithm
 *
 * The class building generalized linear model
 */

class OT_API GeneralLinearModelAlgorithm
  : public MetaModelAlgorithm
{
  CLASSNAME

public:

  typedef GeneralLinearModelResult::BasisCollection BasisCollection;
  typedef GeneralLinearModelResult::BasisPersistentCollection BasisPersistentCollection;
  typedef enum {NONE, JOINTLYROBUST, REFERENCE, FLAT} ScalePrior;
  typedef CovarianceModel::ScaleParametrization ScaleParametrization;

  /** Default constructor */
  GeneralLinearModelAlgorithm();

  /** Parameters constructor */
  GeneralLinearModelAlgorithm (const Sample & inputSample,
                               const Sample & outputSample,
                               const CovarianceModel & covarianceModel,
                               const Bool normalize = ResourceMap::GetAsBool("GeneralLinearModelAlgorithm-NormalizeData"),
                               const Bool keepCholeskyFactor = ResourceMap::GetAsBool("GeneralLinearModelAlgorithm-KeepCovariance"));

  GeneralLinearModelAlgorithm (const Sample & inputSample,
                               const Sample & outputSample,
                               const CovarianceModel & covarianceModel,
                               const Basis & basis,
                               const Bool normalize = ResourceMap::GetAsBool("GeneralLinearModelAlgorithm-NormalizeData"),
                               const Bool keepCholeskyFactor = ResourceMap::GetAsBool("GeneralLinearModelAlgorithm-KeepCovariance"));

  /** Parameters constructor */
  GeneralLinearModelAlgorithm (const Sample & inputSample,
                               const Sample & outputSample,
                               const CovarianceModel & covarianceModel,
                               const BasisCollection & basisCollection,
                               const Bool normalize = ResourceMap::GetAsBool("GeneralLinearModelAlgorithm-NormalizeData"),
                               const Bool keepCholeskyFactor = ResourceMap::GetAsBool("GeneralLinearModelAlgorithm-KeepCovariance"));

  /** Virtual constructor */
  GeneralLinearModelAlgorithm * clone() const;

  /** String converter */
  String __repr__() const;

  /** Perform regression */
  void run();

  /** input transformation accessor */
  void setInputTransformation(const Function & inputTransformation);
  Function getInputTransformation() const;

  /** Sample accessors */
  Sample getInputSample() const;
  Sample getOutputSample() const;

  /** result accessor */
  GeneralLinearModelResult getResult();

  /** Objective function (reduced log-Likelihood) accessor */
  Function getObjectiveFunction();

  /** Optimization solver accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);

  /** Optimization flag accessor */
  Bool getOptimizeParameters() const;
  void setOptimizeParameters(const Bool optimizeParameters);

  /** Accessor to optimization bounds */
  void setOptimizationBounds(const Interval & optimizationBounds);
  Interval getOptimizationBounds() const;

  /** Observation noise accessor */
  void setNoise(const Point & noise);
  Point getNoise() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  // Maximize the reduced log-likelihood
  Scalar maximizeReducedLogLikelihood();

  // Compute the output log-likelihood function
  Point computeReducedLogLikelihood(const Point & parameters) const;
  Scalar computeLapackLogDeterminantCholesky() const;
  Scalar computeHMatLogDeterminantCholesky() const;

  // Compute the design matrix on the normalized input sample
  void computeF();

  // Normalize the input sample
  void normalizeInputSample();

  /** Method accessor (lapack/hmat) */
  void initializeMethod();
  void setMethod(const UnsignedInteger method);
  UnsignedInteger getMethod() const;

  /** reset method - If one change method */
  void reset();

  // Initialize default optimization solver
  void initializeDefaultOptimizationAlgorithm();

  // when the likelihood is integrated: log(\det{FtR^{-1}F})
  Scalar correctIntegratedLikelihoodLogDeterminant() const;

  // If a prior is used, compute its value as a penalization term
  Scalar computeLogIntegratedLikelihoodPenalization() const;

  friend class KrigingAlgorithm;
  Point getRho() const;

  // Scale prior accessor
  ScalePrior getScalePrior() const;
  void setScalePrior(const ScalePrior likelihoodPrior);

private:

  // Helper class to compute the reduced log-likelihood function of the model
  class ReducedLogLikelihoodEvaluation: public EvaluationImplementation
  {
  public:
    // Constructor from a GLM algorithm
    ReducedLogLikelihoodEvaluation(GeneralLinearModelAlgorithm & algorithm)
      : EvaluationImplementation()
      , algorithm_(algorithm)
    {
      // Nothing to do
    }

    ReducedLogLikelihoodEvaluation * clone() const
    {
      return new ReducedLogLikelihoodEvaluation(*this);
    }

    // It is a simple call to the computeReducedLogLikelihood() of the algo
    Point operator() (const Point & point) const
    {
      const Point value(algorithm_.computeReducedLogLikelihood(point));
      return value;
    }

    UnsignedInteger getInputDimension() const
    {
      return algorithm_.getReducedCovarianceModel().getParameter().getDimension();
    }

    UnsignedInteger getOutputDimension() const
    {
      return 1;
    }

    Description getInputDescription() const
    {
      return algorithm_.getReducedCovarianceModel().getParameterDescription();
    }

    Description getOutputDescription() const
    {
      return Description(1, "ReducedLogLikelihood");
    }

    Description getDescription() const
    {
      Description description(getInputDescription());
      description.add(getOutputDescription());
      return description;
    }

    String __repr__() const
    {
      OSS oss;
      // Don't print algorithm_ here as it will result in an infinite loop!
      oss << "ReducedLogLikelihoodEvaluation";
      return oss;
    }

    String __str__(const String & offset) const
    {
      // Don't print algorithm_ here as it will result in an infinite loop!
      return OSS() << offset << __repr__();
    }

  private:
    GeneralLinearModelAlgorithm & algorithm_;
  }; // ReducedLogLikelihoodEvaluation

  /** set sample  method */
  void setData(const Sample & inputSample,
               const Sample & outputSample);

  /** Covariance model accessor */
  void setCovarianceModel(const CovarianceModel & covarianceModel);
  CovarianceModel getCovarianceModel() const;
  CovarianceModel getReducedCovarianceModel() const;

  /** Set basis collection method */
  void setBasisCollection(const BasisCollection & basisCollection);

  /** check that sample is centered to precison eps */
  void checkYCentered(const Sample & Y);

  // The input data
  Sample inputSample_;

  // Standardized version of the input data
  Sample normalizedInputSample_;

  // Standardization function
  Function inputTransformation_;
  mutable Bool normalize_;

  // The associated output data
  Sample outputSample_;

  // The covariance model parametric familly
  CovarianceModel covarianceModel_;
  mutable CovarianceModel reducedCovarianceModel_;

  // The optimization algorithm used for the meta-parameters estimation
  mutable OptimizationAlgorithm solver_;

  // Bounds used for parameter optimization
  Interval optimizationBounds_;

  // The coefficients of the current output conditional expectation part
  mutable Point beta_;
  // Temporarly used to compute gamma
  mutable Point rho_;
  // The current output Gram matrix
  mutable Matrix F_;

  /** Result */
  GeneralLinearModelResult result_;

  /** BasisCollection */
  BasisPersistentCollection basisCollection_;

  /** Cholesky factor ==>  TriangularMatrix */
  mutable TriangularMatrix covarianceCholeskyFactor_;

  /** Cholesky factor when using hmat-oss */
  mutable HMatrix covarianceCholeskyFactorHMatrix_;

  /** Boolean argument for keep covariance */
  Bool keepCholeskyFactor_;

  /** Method : 0 (lapack), 1 (hmat) */
  UnsignedInteger method_;

  /** Bool to tell if optimization has run */
  mutable Bool hasRun_;

  /** Flag to tell if the parameters of the covariance model
      have to be optimized */
  Bool optimizeParameters_;

  /** Observation noise */
  Point noise_;

  /** Flag to tell if the amplitude parameters are estimated using an analytical derivation */
  Bool analyticalAmplitude_;

  /** Cache of the last computed reduced log-likelihood */
  mutable Scalar lastReducedLogLikelihood_;

  // scale prior
  ScalePrior scalePrior_;

}; // class GeneralLinearModelAlgorithm


END_NAMESPACE_OPENTURNS

#endif
