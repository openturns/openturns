//                                               -*- C++ -*-
/**
 *  @brief The result of a gaussian process regression estimation
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
#ifndef OPENTURNS_GAUSSIANPROCESSREGRESSIONRESULT_HXX
#define OPENTURNS_GAUSSIANPROCESSREGRESSIONRESULT_HXX

#include "openturns/GaussianProcessFitterResult.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Basis.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussianProcessRegressionResult
 *
 * The result of a chaos expansion
 */

class OT_API GaussianProcessRegressionResult
  : public MetaModelResult
{
  CLASSNAME

public:

  typedef GaussianProcessFitterResult::LinearAlgebra LinearAlgebra;

  /** Default constructor */
  GaussianProcessRegressionResult();

  /** Parameter constructor */
  GaussianProcessRegressionResult(const GaussianProcessFitterResult & result,
                                  const Sample & covarianceCoefficients);

  /** Virtual constructor */
  GaussianProcessRegressionResult * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Trend basis accessor */
  Basis getBasis() const;

  /** Trend coefficients accessor */
  Point getTrendCoefficients() const;

  /** Conditional covariance models accessor */
  CovarianceModel getCovarianceModel() const;

  /** optimal log-likelihood value */
  Scalar getOptimalLogLikelihood() const;

  /** linear algebra method */
  LinearAlgebra getLinearAlgebraMethod() const;

  /** Regression matrix accessor */
  Matrix getRegressionMatrix() const;

  /** process accessor */
  Process getNoise() const;

  /** Process coefficients accessor */
  Sample getCovarianceCoefficients() const;

  /** Fitter result accessor */
  GaussianProcessFitterResult getGaussianProcessFitterResult() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  // GaussianProcessFitter::run could set the Cholesky factor
  friend class GaussianProcessFitter;

  // GaussianProcessFitter::run could set the Cholesky factor
  friend class GaussianProcessConditionalCovariance;

  // GaussianProcessRegressionResult could use Cholesky setters
  friend class GaussianProcessRegression;

  /** Method that returns the covariance factor - lapack */
  TriangularMatrix getCholeskyFactor() const;

  /** Method that returns the covariance factor - hmat */
  HMatrix getHMatCholeskyFactor() const;

private:

  /** GaussianProcessFitterResult */
  GaussianProcessFitterResult gpfResult_;

  /** The covariance coefficients */
  Sample covarianceCoefficients_;


} ; /* class GaussianProcessRegressionResult */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GaussianProcessRegressionResult_HXX */
