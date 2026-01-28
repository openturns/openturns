//                                               -*- C++ -*-
/**
 *  @brief The result of a gaussian process fitter
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
#ifndef OPENTURNS_GAUSSIANPROCESSFITTERRESULT_HXX
#define OPENTURNS_GAUSSIANPROCESSFITTERRESULT_HXX

#include "openturns/MetaModelResult.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Basis.hxx"
#include "openturns/Function.hxx"
#include "openturns/Process.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/Basis.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussianProcessFitterResult
 *
 * The result of a gaussian process fitter
 */

class OT_API GaussianProcessFitterResult
  : public MetaModelResult
{
  CLASSNAME

public:

  enum LinearAlgebra { LAPACK, HMAT };

  typedef Collection<Point> PointCollection;
  typedef PersistentCollection<Point> PointPersistentCollection;

  /** Default constructor */
  GaussianProcessFitterResult();

  /** Parameter constructor after a gaussian process fitting  */
  GaussianProcessFitterResult(const Sample & inputData,
                              const Sample & outputData,
                              const Function & metaModel,
                              const Matrix & regressionMatrix,
                              const Basis & basis,
                              const Point & trendCoefficients,
                              const CovarianceModel & covarianceModel,
                              const Scalar optimalLogLikelihood,
                              const LinearAlgebra linearAlgebraMethod);

  /** Virtual constructor */
  GaussianProcessFitterResult * clone() const override;

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

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:

  // GaussianProcessFitter::run could set the Cholesky factor
  friend class GaussianProcessFitter;

  // GaussianProcessRegressionResult could use Cholesky setters
  friend class GaussianProcessRegressionResult;

  // GaussianProcessRegression could use Cholesky setters
  friend class GaussianProcessRegression;

  // GaussianProcessRegressionCrossValidation could use getRho()
  friend class GaussianProcessRegressionCrossValidation;

  /** Accessor to the Cholesky factor*/
  void setCholeskyFactor(const TriangularMatrix & covarianceCholeskyFactor,
                         const HMatrix & covarianceHMatrix);

  /** Method that returns the covariance factor - lapack */
  TriangularMatrix getCholeskyFactor() const;

  /** Method that returns the covariance factor - hmat */
  HMatrix getHMatCholeskyFactor() const;

  /** rho accessor */
  Point getRho() const;
  void setRho(const Point & rho);


private:

  /** Regression matrix (aka F matrix) */
  Matrix regressionMatrix_;

  /** The trend basis */
  Basis basis_;

  /** The trend coefficients */
  Point beta_;

  /** The covariance model */
  CovarianceModel covarianceModel_;

  /** The rho point */
  Point rho_;

  /** optimal log-likelihood value */
  Scalar optimalLogLikelihood_;

  /** Linear algebra method */
  LinearAlgebra linearAlgebraMethod_;

  /** Boolean for Cholesky. */
  Bool hasCholeskyFactor_;

  /** Cholesky factor  */
  TriangularMatrix covarianceCholeskyFactor_;

  /** Cholesky factor when using hmat-oss/hmat */
  HMatrix covarianceHMatrix_;

} ; /* class GaussianProcessFitterResult */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSIANPROCESSFITTERRESULT_HXX */
