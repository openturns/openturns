//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_KRIGINGALGORITHM_HXX
#define OPENTURNS_KRIGINGALGORITHM_HXX

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/Basis.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/KrigingResult.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/GeneralLinearModelAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KrigingAlgorithm
 *
 * The class building kriging process, relying on generalized linear model class (GeneralLinearModelAlgorithm)
 * for the evaluation of the coefficients of the parameters.
 */

class OT_API KrigingAlgorithm
  : public MetaModelAlgorithm
{
  CLASSNAME

public:

  typedef KrigingResult::BasisCollection BasisCollection;
  typedef KrigingResult::BasisPersistentCollection BasisPersistentCollection;

  /** Default constructor */
  KrigingAlgorithm();

  /** Constructor */
  KrigingAlgorithm (const Sample & inputSample,
                    const Sample & outputSample,
                    const CovarianceModel & covarianceModel,
                    const Basis & basis);

  /** Constructor */
  KrigingAlgorithm (const Sample & inputSample,
                    const Sample & outputSample,
                    const CovarianceModel & covarianceModel,
                    const BasisCollection & basisCollection);

  /** Virtual constructor */
  KrigingAlgorithm * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Perform regression */
  void run() override;

  /** result accessor */
  KrigingResult getResult();

  /** Optimization solver accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);

  /** Accessor to optimization bounds */
  void setOptimizationBounds(const Interval & optimizationBounds);
  Interval getOptimizationBounds() const;

  /** Log-Likelihood function accessor */
  Function getReducedLogLikelihoodFunction();

  /** Optimization flag accessor */
  Bool getOptimizeParameters() const;
  void setOptimizeParameters(const Bool optimizeParameters);

  /** Observation noise accessor */
  void setNoise(const Point & noise);
  Point getNoise() const;

  /** Linear algebra method */
  void setMethod(const String & method);
  String getMethod() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:

  /** The method helps to compute the gamma point */
  void computeGamma();

private:

  // The input data
  Sample inputSample_;
  // The associated output data
  Sample outputSample_;
  // The covariance model parametric family
  CovarianceModel covarianceModel_;
  // Underlying algo used for the evaluation of parameters
  GeneralLinearModelAlgorithm glmAlgo_;
  // The coefficients of the current output deterministic trend
  mutable Point gamma_;
  // Temporarly used to compute gamma
  mutable Point rho_;

  /** Result */
  KrigingResult result_;

  /** Cholesky factor ==>  TriangularMatrix */
  mutable TriangularMatrix covarianceCholeskyFactor_;

  /** Cholesky factor when using hmat-oss */
  mutable HMatrix covarianceCholeskyFactorHMatrix_;

}; // class KrigingAlgorithm


END_NAMESPACE_OPENTURNS

#endif
