//                                               -*- C++ -*-
/**
 *  @brief The result of a kriging estimation
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_KRIGINGRESULT_HXX
#define OPENTURNS_KRIGINGRESULT_HXX

#include "openturns/MetaModelResult.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Basis.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"
#include "openturns/Normal.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KrigingResult
 *
 * The result of a chaos expansion
 */

class OT_API KrigingResult
  : public MetaModelResult
{
  CLASSNAME

public:

  typedef Collection<CovarianceMatrix> CovarianceMatrixCollection;

  /** Default constructor */
  KrigingResult();

  /** Parameter constructor without any cholesky factor*/
  KrigingResult(const Sample & inputSample,
                const Sample & outputSample,
                const Function & metaModel,
                const Point & residuals,
                const Point & relativeErrors,
                const Basis & basis,
                const Point & trendCoefficients,
                const CovarianceModel & covarianceModel,
                const Sample & covarianceCoefficients);

  /** Parameter constructor with Cholesky factor (Lapack)*/
  KrigingResult(const Sample & inputSample,
                const Sample & outputSample,
                const Function & metaModel,
                const Point & residuals,
                const Point & relativeErrors,
                const Basis & basis,
                const Point & trendCoefficients,
                const CovarianceModel & covarianceModel,
                const Sample & covarianceCoefficients,
                const TriangularMatrix & covarianceCholeskyFactor,
                const HMatrix & covarianceHMatrix);

  /** Virtual constructor */
  KrigingResult * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Trend basis accessor */
  virtual Basis getBasis() const;

  /** Trend coefficients accessor */
  virtual Point getTrendCoefficients() const;

  /** Conditional covariance models accessor */
  virtual CovarianceModel getCovarianceModel() const;

  /** Process coefficients accessor */
  virtual Sample getCovarianceCoefficients() const;

  /** Compute mean of new points conditionally to observations */
  virtual Sample getConditionalMean(const Sample & xi) const;

  /** Compute mean of new points conditionally to observations */
  virtual Point getConditionalMean(const Point & xi) const;

  /** Compute covariance matrix conditionally to observations*/
  virtual CovarianceMatrix getConditionalCovariance(const Sample & xi) const ;

  /** Compute covariance matrix conditionally to observations*/
  virtual CovarianceMatrix getConditionalCovariance(const Point & xi) const;

  /** Compute covariance matrices conditionally to observations (1 cov / point)*/
  virtual CovarianceMatrixCollection getConditionalMarginalCovariance(const Sample & xi) const;

  /** Compute covariance matrix conditionally to observations (1 cov of size outdimension)*/
  virtual CovarianceMatrix getConditionalMarginalCovariance(const Point & xi) const;

  /** Compute marginal variance conditionally to observations (1 cov of size outdimension)*/
  virtual Scalar getConditionalMarginalVariance(const Point & point,
      const UnsignedInteger marginalIndex = 0) const;

  /** Compute marginal variance conditionally to observations (1 cov / point)*/
  virtual Sample getConditionalMarginalVariance(const Sample & xi,
      const UnsignedInteger marginalIndex = 0) const;

  virtual Point getConditionalMarginalVariance(const Point & point,
      const Indices & indices) const;

  virtual Sample getConditionalMarginalVariance(const Sample & xi,
      const Indices & indices) const;

  /** Compute joint normal distribution conditionally to observations*/
  virtual Normal operator()(const Sample & xi) const;

  /** Compute joint normal distribution conditionally to observations*/
  virtual Normal operator()(const Point & xi) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:

  /** Compute cross matrix method ==> not necessary square matrix  */
  void computeF() const;
  void computePhi() const;

private:

  /** The trend basis */
  Basis basis_;

  /** The trend coefficients */
  Point trendCoefficients_;

  /** The covariance model */
  CovarianceModel covarianceModel_;

  /** The covariance coefficients */
  Sample covarianceCoefficients_;

  /** Cholesky factor  */
  mutable TriangularMatrix covarianceCholeskyFactor_;

  /** Cholesky factor when using hmat-oss */
  mutable HMatrix covarianceHMatrix_;

  /** Matrix F : the regression matrix */
  mutable Matrix F_;

  /** Matrix phi = L^{-1}F ==> phiT is the transposed matrix */
  mutable Matrix phiT_;

  /** Matrix F^{t}R^{-1}F writes phi_ = L^{-1}F ==> QR decomposition */
  // G_ is the triangular matrix ==> Gt the transposed
  mutable Matrix Gt_;

} ; /* class KrigingResult */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KRIGINGRESULT_HXX */
