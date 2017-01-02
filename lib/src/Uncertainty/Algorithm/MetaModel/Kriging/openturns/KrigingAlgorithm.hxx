//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/Basis.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/KrigingResult.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/GeneralizedLinearModelAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KrigingAlgorithm
 *
 * The class building kriging process, relying on generalized linear model class (GeneralizedLinearModelAlgorithm)
 * for the evaluation of the coefficients of the parameters.
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
                    const Bool keepCholeskyFactor = true);

  /** Constructor */
  KrigingAlgorithm (const NumericalSample & inputSample,
                    const NumericalMathFunction & inputTransformation,
                    const NumericalSample & outputSample,
                    const Basis & basis,
                    const CovarianceModel & covarianceModel,
                    const Bool keepCholeskyFactor = true);

  /** Constructor */
  KrigingAlgorithm (const NumericalSample & inputSample,
                    const NumericalSample & outputSample,
                    const BasisCollection & multivariateBasis,
                    const CovarianceModel & covarianceModel,
                    const Bool normalize = true,
                    const Bool keepCholeskyFactor = true);

  /** Constructor */
  KrigingAlgorithm (const NumericalSample & inputSample,
                    const NumericalMathFunction & inputTransformation,
                    const NumericalSample & outputSample,
                    const BasisCollection & multivariateBasis,
                    const CovarianceModel & covarianceModel,
                    const Bool keepCholeskyFactor = true);

  /** Virtual constructor */
  KrigingAlgorithm * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Perform regression */
  void run();

  /** Sample accessors */
  NumericalSample getInputSample() const;
  NumericalSample getOutputSample() const;

  /** result accessor */
  KrigingResult getResult();

  /** Optimization solver accessor */
  OptimizationSolver getOptimizationSolver() const;
  void setOptimizationSolver(const OptimizationSolver & solver);

  /** Log-Likelihood function accessor */
  NumericalMathFunction getLogLikelihoodFunction();

  /** Optimization flag accessor */
  Bool getOptimizeParameters() const;
  void setOptimizeParameters(const Bool optimizeParameters);
  
  /** Observation noise accessor */
  void setNoise(const NumericalPoint & noise);
  NumericalPoint getNoise() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

  /** The method helps to compute the gamma point */
  void computeGamma();

  friend class Factory<KrigingAlgorithm>;

private:

  // The input data
  NumericalSample inputSample_;
  // The associated output data
  NumericalSample outputSample_;
  Bool normalize_;
  // The covariance model parametric family
  CovarianceModel covarianceModel_;
  // Underlying algo used for the evaluation of parameters
  GeneralizedLinearModelAlgorithm glmAlgo_;
  // The coefficients of the current output deterministic trend
  mutable NumericalPoint gamma_;
  // Temporarly used to compute gamma
  mutable NumericalPoint rho_;

  /** Result */
  KrigingResult result_;

  /** Bool for keeping or not covariance factor */
  Bool keepCholeskyFactor_;

  /** Cholesky factor ==>  TriangularMatrix */
  mutable TriangularMatrix covarianceCholeskyFactor_;

  /** Cholesky factor when using hmat-oss */
  mutable HMatrix covarianceCholeskyFactorHMatrix_;

}; // class KrigingAlgorithm


END_NAMESPACE_OPENTURNS

#endif
