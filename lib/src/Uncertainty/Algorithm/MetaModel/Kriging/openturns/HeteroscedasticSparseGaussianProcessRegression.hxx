//                                               -*- C++ -*-
/**
 *  @brief The class building heteroscedastic sparse gaussian process regression
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
#ifndef OPENTURNS_HETEROSCEDASTICSPARSEGAUSSIANPROCESSREGRESSION_HXX
#define OPENTURNS_HETEROSCEDASTICSPARSEGAUSSIANPROCESSREGRESSION_HXX

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/HeteroscedasticSparseGaussianProcessFitter.hxx"
#include "openturns/HeteroscedasticSparseGaussianProcessFitterResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HeteroscedasticSparseGaussianProcessRegression
 *
 * The class building heteroscedastic sparse gaussian process regression
 */
class OT_API HeteroscedasticSparseGaussianProcessRegression
  : public MetaModelAlgorithm
{
  CLASSNAME

public:

  /** Default constructor */
  HeteroscedasticSparseGaussianProcessRegression();

  /** Constructor from a heteroscedastic sparse gaussian process fitter result */
  HeteroscedasticSparseGaussianProcessRegression(const HeteroscedasticSparseGaussianProcessFitterResult & result);

  /** Parameters constructor */
  HeteroscedasticSparseGaussianProcessRegression(const Sample & inputSample,
      const Sample & outputSample,
      const CovarianceModel & covarianceModelF,
      const CovarianceModel & covarianceModelG,
      const Sample & inducingPointsF,
      const Sample & inducingPointsG);

  /** Virtual constructor */
  HeteroscedasticSparseGaussianProcessRegression * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Perform regression */
  void run() override;

  /** result accessor */
  HeteroscedasticSparseGaussianProcessFitterResult getResult() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  // Build the metamodel from the fitter result
  void buildMetaModel();

  /** Heteroscedastic sparse gaussian process fitter result */
  HeteroscedasticSparseGaussianProcessFitterResult heteroscedasticSparseGaussianProcessFitterResult_;

  /** The result */
  HeteroscedasticSparseGaussianProcessFitterResult result_;

  /** The covariance models */
  CovarianceModel covarianceModelF_;
  CovarianceModel covarianceModelG_;

  /** The inducing points */
  Sample inducingPointsF_;
  Sample inducingPointsG_;

  /** Whether the fitting has run */
  Bool hasRun_ = false;

  /** Whether a fit has to be performed (parameters constructor) */
  Bool needsFit_ = false;

}; /* class HeteroscedasticSparseGaussianProcessRegression */


END_NAMESPACE_OPENTURNS

#endif
