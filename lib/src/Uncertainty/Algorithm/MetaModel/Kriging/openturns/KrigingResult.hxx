//                                               -*- C++ -*-
/**
 *  @brief The result of a kriging estimation
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

  typedef Collection<Point> PointCollection;
  typedef PersistentCollection<Point> PointPersistentCollection;
  typedef Collection<Basis> BasisCollection;
  typedef PersistentCollection<Basis> BasisPersistentCollection;

  /** Default constructor */
  KrigingResult();

  /** Parameter constructor without any cholesky factor*/
  KrigingResult(const Sample & inputSample,
                const Sample & outputSample,
                const Function & metaModel,
                const Point & residuals,
                const Point & relativeErrors,
                const BasisCollection & basis,
                const PointCollection & trendCoefficients,
                const CovarianceModel & covarianceModel,
                const Sample & covarianceCoefficients);

  /** Parameter constructor with Cholesky factor (Lapack)*/
  KrigingResult(const Sample & inputSample,
                const Sample & outputSample,
                const Function & metaModel,
                const Point & residuals,
                const Point & relativeErrors,
                const BasisCollection & basis,
                const PointCollection & trendCoefficients,
                const CovarianceModel & covarianceModel,
                const Sample & covarianceCoefficients,
                const TriangularMatrix & covarianceCholeskyFactor,
                const HMatrix & covarianceHMatrix);

  /** Virtual constructor */
  virtual KrigingResult * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Design accessors */
  virtual Sample getInputSample() const;
  virtual Sample getOutputSample() const;

  /** Trend basis accessor */
  virtual BasisCollection getBasisCollection() const;

  /** Trend coefficients accessor */
  virtual PointCollection getTrendCoefficients() const;

  /** Conditional covariance models accessor */
  virtual CovarianceModel getCovarianceModel() const;

  /** Process coefficients accessor */
  virtual Sample getCovarianceCoefficients() const;

  /** Transformation accessor */
  virtual Function getTransformation() const;
  virtual void setTransformation(const Function & transformation);

  /** Compute mean of new points conditionnaly to observations */
  virtual Point getConditionalMean(const Sample & xi) const;

  /** Compute mean of new points conditionnaly to observations */
  virtual Point getConditionalMean(const Point & xi) const;

  /** Compute covariance matrix conditionnaly to observations*/
  virtual CovarianceMatrix getConditionalCovariance(const Sample & xi) const ;

  /** Compute covariance matrix conditionnaly to observations*/
  virtual CovarianceMatrix getConditionalCovariance(const Point & xi) const;

  /** Compute joint normal distribution conditionnaly to observations*/
  virtual Normal operator()(const Sample & xi) const;

  /** Compute joint normal distribution conditionnaly to observations*/
  virtual Normal operator()(const Point & xi) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

  /** Compute cross matrix method ==> not necessary square matrix  */
  Matrix getCrossMatrix(const Sample & x) const;
  void computeF() const;

private:

  // Structure for evaluation of crossCovariance
  friend struct KrigingResultCrossCovarianceFunctor;

  /** inputData should be keeped*/
  Sample inputSample_;

  /** input transformed data: store data*/
  Sample inputTransformedSample_;

  Sample outputSample_;

  /** inputTransformation ==> iso-probabilistic transformation */
  Function inputTransformation_;

  /** Boolean transformation */
  Bool hasTransformation_;

  /** The trend basis */
  BasisPersistentCollection basis_;

  /** The trend coefficients */
  PointPersistentCollection trendCoefficients_;

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
