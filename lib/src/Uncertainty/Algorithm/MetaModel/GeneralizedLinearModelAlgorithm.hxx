//                                               -*- C++ -*-
/**
 *  @brief The class builds generalized linear models
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_GENERALIZEDLINEARMODELALGORITHM_HXX
#define OPENTURNS_GENERALIZEDLINEARMODELALGORITHM_HXX

#include "MetaModelAlgorithm.hxx"
#include "Basis.hxx"
#include "CovarianceModel.hxx"
#include "KrigingResult.hxx"
#include "HMatrix.hxx"
#include "OptimizationSolver.hxx"
#include "GeneralizedLinearModelResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GeneralizedLinearModelAlgorithm
 *
 * The class building generalized linear model
 */

class OT_API GeneralizedLinearModelAlgorithm
  : public MetaModelAlgorithm
{
  CLASSNAME;

public:

  typedef GeneralizedLinearModelResult::BasisCollection BasisCollection;
  typedef GeneralizedLinearModelResult::BasisPersistentCollection BasisPersistentCollection;

  /** Default constructor */
  GeneralizedLinearModelAlgorithm();

  /** Parameters constructor */
  GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
                                   const NumericalSample & outputSample,
                                   const CovarianceModel & covarianceModel,
                                   const Bool normalize = ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-NormalizeData"),
                                   const Bool keepCovariance = ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-KeepCovariance"));

  GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
                                   const NumericalSample & outputSample,
                                   const CovarianceModel & covarianceModel,
                                   const Basis & basis,
                                   const Bool normalize = ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-NormalizeData"),
                                   const Bool keepCovariance = ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-KeepCovariance"));

  /** Parameters constructor */
  GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
                                   const NumericalMathFunction & inputTransformation,
                                   const NumericalSample & outputSample,
                                   const CovarianceModel & covarianceModel,
                                   const Basis & basis,
                                   const Bool keepCovariance = ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-KeepCovariance"));

  /** Parameters constructor */
  GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
                                   const NumericalSample & outputSample,
                                   const CovarianceModel & covarianceModel,
                                   const BasisCollection & multivariateBasis,
                                   const Bool normalize = ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-NormalizeData"),
                                   const Bool keepCovariance = ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-KeepCovariance"));

  /** Parameters constructor */
  GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
                                   const NumericalMathFunction & inputTransformation,
                                   const NumericalSample & outputSample,
                                   const CovarianceModel & covarianceModel,
                                   const BasisCollection & multivariateBasis,
                                   const Bool keepCovariance = ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-KeepCovariance"));

  /** Virtual constructor */
  GeneralizedLinearModelAlgorithm * clone() const;

  /** String converter */
  String __repr__() const;

  /** Perform regression */
  void run();

  /** input transformation accessor */
  void setInputTransformation(const NumericalMathFunction & inputTransformation);
  NumericalMathFunction getInputTransformation() const;

  /** Sample accessors */
  NumericalSample getInputSample() const;
  NumericalSample getOutputSample() const;

  /** result accessor */
  GeneralizedLinearModelResult getResult();

  /** Objective function (log-Likelihood) accessor */
  NumericalMathFunction getObjectiveFunction();

  /** Optimization solver accessor */
  OptimizationSolver getOptimizationSolver() const;
  void setOptimizationSolver(const OptimizationSolver & solver);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:
  // Optimize the log-likelihood
  NumericalPoint optimizeLogLikelihood();
  // Compute the output log-likelihood function
  NumericalScalar computeLogLikelihood(const NumericalPoint & theta) const;
  NumericalScalar computeLapackLogLikelihood(const NumericalPoint & theta) const;
  NumericalScalar computeHMatLogLikelihood(const NumericalPoint & theta) const;
  // Compute the design matrix on the normalized input sample
  void computeF();
  // Normalize the input sample
  void normalizeInputSample();

  /** Method accessor (lapack/hmat) */
  void initializeMethod();
  void setMethod(const UnsignedInteger method);

  // Initialize default optimization solver
  void initializeDefaultOptimizationSolver();
  friend class Factory<GeneralizedLinearModelAlgorithm>;


  friend class KrigingAlgorithm;
  NumericalPoint getRho() const;

private:

  /** set sample  method */
  void setData(const NumericalSample & inputSample,
               const NumericalSample & outputSample);

  /** set covariance method */
  void setCovariance(const CovarianceModel & covarianceModel);

  /** Set basis collection method */
  void setBasis(const BasisCollection & basisCollection);

  /** check that sample is centered to precison eps */
  void checkYCentered(const NumericalSample & Y);

  // The input data
  NumericalSample inputSample_;
  // Standardized version of the input data
  NumericalSample normalizedInputSample_;
  // Standardization funtion
  NumericalMathFunction inputTransformation_;
  mutable Bool normalize_;
  // The associated output data
  NumericalSample outputSample_;
  // The covariance model parametric familly
  CovarianceModel covarianceModel_;
  // The member of the covariance model fitted to the data
  CovarianceModel conditionalCovarianceModel_;

  // The optimization algorithm used for the meta-parameters estimation
  mutable OptimizationSolver  solver_;

  // The coefficients of the current output conditional expectation part
  mutable NumericalPoint beta_;
  // Temporarly used to compute gamma
  mutable NumericalPoint rho_;
  // The current output Gram matrix
  mutable Matrix F_;

  /** Result */
  GeneralizedLinearModelResult result_;

  /** BasisCollection */
  BasisPersistentCollection basis_;

  /** Cholesky factor ==>  TriangularMatrix */
  mutable TriangularMatrix covarianceCholeskyFactor_;

  /** Cholesky factor when using hmat-oss */
  mutable HMatrix covarianceHMatrix_;

  /** Boolean argument for keep covariance */
  mutable Bool keepCovariance_;

  /** Method : 0 (lapack), 1 (hmat) */
  UnsignedInteger method_;

  /** Bool to tell if optimization has run */
  Bool hasRun_;
}; // class GeneralizedLinearModelAlgorithm


END_NAMESPACE_OPENTURNS

#endif
