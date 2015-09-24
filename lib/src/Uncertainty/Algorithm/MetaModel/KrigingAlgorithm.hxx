//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_KRIGINGALGORITHM_HXX
#define OPENTURNS_KRIGINGALGORITHM_HXX

#include "MetaModelAlgorithm.hxx"
#include "Basis.hxx"
#include "CovarianceModel.hxx"
#include "OptimizationSolver.hxx"
#include "KrigingResult.hxx"
#include "HMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KrigingAlgorithm
 *
 * The class building kriging process
 */

class OT_API KrigingAlgorithm
  : public MetaModelAlgorithm
{
  CLASSNAME;

public:

  typedef KrigingResult::BasisCollection BasisCollection;
  typedef KrigingResult::BasisPersistentCollection BasisPersistentCollection;

  /** Default constructor */
  KrigingAlgorithm();

  /** Constructor */
  KrigingAlgorithm (const NumericalSample & inputSample,
                    const NumericalSample & outputSample,
                    const Basis & basis,
                    const CovarianceModel & covarianceModel,
                    const Bool normalize = true,
                    const Bool keepCovariance = true);

  /** Constructor */
  KrigingAlgorithm (const NumericalSample & inputSample,
                    const NumericalMathFunction & inputTransformation,
                    const NumericalSample & outputSample,
                    const Basis & basis,
                    const CovarianceModel & covarianceModel,
                    const Bool keepCovariance = true);

  /** Constructor */
  KrigingAlgorithm (const NumericalSample & inputSample,
                    const NumericalSample & outputSample,
                    const BasisCollection & multivariateBasis,
                    const CovarianceModel & covarianceModel,
                    const Bool normalize = true,
                    const Bool keepCovariance = true);

  /** Constructor */
  KrigingAlgorithm (const NumericalSample & inputSample,
                    const NumericalMathFunction & inputTransformation,
                    const NumericalSample & outputSample,
                    const BasisCollection & multivariateBasis,
                    const CovarianceModel & covarianceModel,
                    const Bool keepCovariance = true);

  /** Virtual constructor */
  virtual KrigingAlgorithm * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Perform regression */
  void run();

  /** input transformation accessor */
  void setInputTransformation(const NumericalMathFunction & inputTransformation);
  NumericalMathFunction getInputTransformation() const;

  /** Sample accessors */
  virtual NumericalSample getInputSample() const;
  virtual NumericalSample getOutputSample() const;

  /** result accessor */
  virtual KrigingResult getResult();

  /** Marginal log-Likelihood function accessor */
  NumericalMathFunction getLogLikelihoodFunction();

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  /** Optimization solver accessor */
  OptimizationSolver getOptimizationSolver() const;
  void setOptimizationSolver(const OptimizationSolver & solver);

protected:
  // Optimize the marginal log-likelihood associated to outputIndex_
  NumericalPoint optimizeLogLikelihood();
  // Compute the marginal output log-likelihood function
  NumericalScalar computeLogLikelihood(const NumericalPoint & theta) const;
  NumericalScalar computeLapackLogLikelihood(const NumericalPoint & theta) const;
  NumericalScalar computeHMatLogLikelihood(const NumericalPoint & theta) const;
  // Compute the design matrix on the normalized input sample
  void computeF();
  // Normalize the input sample
  void normalizeInputSample();

  friend class Factory<KrigingAlgorithm>;

private:

  /** set sample & covariance method */
  void setDataAndCovariance(const NumericalSample & inputSample,
                            const NumericalSample & outputSample,
                            const CovarianceModel & covarianceModel);

  /** Set basis collection method */
  void setBasisCollection(const BasisCollection & basisCollection);

  /** The method helps to compute the gamma point */
  void computeGamma();

  // The input data
  NumericalSample inputSample_;
  // Standardized version of the input data
  NumericalSample normalizedInputSample_;
  // Standardization funtion
  NumericalMathFunction inputTransformation_;
  mutable Bool normalize_;
  // The associated output data
  NumericalSample outputSample_;
  // The covariance model parametric family
  CovarianceModel covarianceModel_;
  // The member of the covariance model fitted to the data
  CovarianceModel conditionalCovarianceModel_;
  // The optimization algorithm used for the meta-parameters estimation
  mutable OptimizationSolver  solver_;
  Bool optimizerProvided_;

  // The coefficients of the current output conditional expectation part
  mutable NumericalPoint beta_;
  // The coefficients of the current output deterministic trend
  mutable NumericalPoint gamma_;
  // Temporarly used to compute gamma
  mutable NumericalPoint rho_;
  // The current output Gram matrix
  mutable Matrix F_;

  /** Result */
  KrigingResult result_;

  /** Bool for keeping or not covariance factor */
  Bool keepCovariance_;

  /** BasisCollection */
  BasisPersistentCollection basis_;

  /** Cholesky factor ==>  TriangularMatrix */
  // Argument is mutable as beta/gamma
  mutable TriangularMatrix covarianceCholeskyFactor_;

  /** Cholesky factor when using hmat-oss */
  mutable HMatrix covarianceHMatrix_;

  /** sigma2 factor */
  mutable NumericalScalar sigma2_;

}; // class KrigingAlgorithm


END_NAMESPACE_OPENTURNS

#endif
