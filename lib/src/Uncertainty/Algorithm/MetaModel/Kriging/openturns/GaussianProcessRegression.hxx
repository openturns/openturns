//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_GAUSSIANPROCESSREGRESSION_HXX
#define OPENTURNS_GAUSSIANPROCESSREGRESSION_HXX

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Function.hxx"
#include "openturns/GaussianProcessRegressionResult.hxx"
#include "openturns/GaussianProcessFitterResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussianProcessRegression
 *
 * The class building a gaussian process regression, relying on a GaussianProcessFitterResult
 * for the evaluation of the coefficients of the parameters
 */

class OT_API GaussianProcessRegression
  : public MetaModelAlgorithm
{
  CLASSNAME

public:

  typedef GaussianProcessFitterResult::LinearAlgebra LinearAlgebra;

  /** Default constructor */
  GaussianProcessRegression();

  /** Constructor */
  GaussianProcessRegression(const GaussianProcessFitterResult & result);

  /** Constructor */
  GaussianProcessRegression (const Sample & inputSample,
                             const Sample & outputSample,
                             const CovarianceModel & covarianceModel,
                             const Function & trendFunction);

  /** Virtual constructor */
  GaussianProcessRegression * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Perform regression */
  void run() override;

  /** result accessor */
  GaussianProcessRegressionResult getResult() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:

  /** The method helps to compute the gamma point */
  Point computeGamma() const;

private:

  // The covariance model
  CovarianceModel covarianceModel_;

  // Basis argument
  Basis basis_;

  // The coefficients of the trend
  Point beta_;

  /** Results */
  GaussianProcessFitterResult gaussianProcessFitterResult_;
  GaussianProcessRegressionResult result_;


}; // class GaussianProcessRegression


END_NAMESPACE_OPENTURNS

#endif
