//                                               -*- C++ -*-
/**
 *  @brief The class building sparse gaussian process regression
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
#ifndef OPENTURNS_SPARSEGAUSSIANPROCESSREGRESSION_HXX
#define OPENTURNS_SPARSEGAUSSIANPROCESSREGRESSION_HXX

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/SparseGaussianProcessFitter.hxx"
#include "openturns/SparseGaussianProcessFitterResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SparseGaussianProcessRegression
 *
 * The class building sparse gaussian process regression
 */
class OT_API SparseGaussianProcessRegression
  : public MetaModelAlgorithm
{
  CLASSNAME

public:

  typedef SparseGaussianProcessFitterResult::LinearAlgebra LinearAlgebra;

  /** Default constructor */
  SparseGaussianProcessRegression();

  /** Constructor from a sparse gaussian process fitter result */
  SparseGaussianProcessRegression(const SparseGaussianProcessFitterResult & result);

  /** Parameters constructor */
  SparseGaussianProcessRegression(const Sample & inputSample,
                                  const Sample & outputSample,
                                  const CovarianceModel & covarianceModel,
                                  const Sample & inducingPoints);

  /** Virtual constructor */
  SparseGaussianProcessRegression * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Perform regression */
  void run() override;

  /** result accessor */
  SparseGaussianProcessFitterResult getResult() const;

  /** Linear algebra method accessors */
  LinearAlgebra getMethod() const;
  void setMethod(const LinearAlgebra method);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  // Build the metamodel from the fitter result
  void buildMetaModel();

  /** Sparse gaussian process fitter result */
  SparseGaussianProcessFitterResult sparseGaussianProcessFitterResult_;

  /** The result */
  SparseGaussianProcessFitterResult result_;

  /** The covariance model */
  CovarianceModel covarianceModel_;

  /** The inducing points */
  Sample inducingPoints_;

  /** The linear algebra method */
  LinearAlgebra method_ = LinearAlgebra::LAPACK;

  /** Whether the fitting has run */
  Bool hasRun_ = false;

  /** Whether a fit has to be performed (parameters constructor) */
  Bool needsFit_ = false;

}; /* class SparseGaussianProcessRegression */


END_NAMESPACE_OPENTURNS

#endif
