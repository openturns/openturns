//                                               -*- C++ -*-
/**
 *  @brief The result of a kriging estimation
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
#ifndef OPENTURNS_KRIGINGRESULT_HXX
#define OPENTURNS_KRIGINGRESULT_HXX

#include "openturns/MetaModelResult.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/Normal.hxx"
#include "openturns/HMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KrigingResult
 *
 * The result of a chaos expansion
 */

class OT_API KrigingResult
  : public MetaModelResult
{
  CLASSNAME;

public:

  // friend class Factory<KrigingResult>;
  typedef Collection<NumericalPoint> NumericalPointCollection;
  typedef PersistentCollection<NumericalPoint> NumericalPointPersistentCollection;
  typedef Collection<Basis> BasisCollection;
  typedef PersistentCollection<Basis> BasisPersistentCollection;

  /** Default constructor */
  KrigingResult();

  /** Parameter constructor without any cholesky factor*/
  KrigingResult(const NumericalSample & inputData,
                const NumericalSample & outputData,
                const NumericalMathFunction & metaModel,
                const NumericalPoint & residuals,
                const NumericalPoint & relativeErrors,
                const BasisCollection & basis,
                const NumericalPointCollection & trendCoefficients,
                const CovarianceModel & covarianceModel,
                const NumericalSample & covarianceCoefficients);

  /** Parameter constructor with Cholesky factor (Lapack)*/
  KrigingResult(const NumericalSample & inputData,
                const NumericalSample & outputData,
                const NumericalMathFunction & metaModel,
                const NumericalPoint & residuals,
                const NumericalPoint & relativeErrors,
                const BasisCollection & basis,
                const NumericalPointCollection & trendCoefficients,
                const CovarianceModel & covarianceModel,
                const NumericalSample & covarianceCoefficients,
                const TriangularMatrix & covarianceCholeskyFactor,
                const HMatrix & covarianceHMatrix);

  /** Virtual constructor */
  virtual KrigingResult * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Trend basis accessor */
  virtual BasisCollection getBasisCollection() const;

  /** Trend coefficients accessor */
  virtual NumericalPointCollection getTrendCoefficients() const;

  /** Conditional covariance models accessor */
  virtual CovarianceModel getCovarianceModel() const;

  /** Process coefficients accessor */
  virtual NumericalSample getCovarianceCoefficients() const;

  /** Transformation accessor */
  virtual NumericalMathFunction getTransformation() const;
  virtual void setTransformation(const NumericalMathFunction & transformation);

  /** Compute mean of new points conditionnaly to observations */
  virtual NumericalPoint getConditionalMean(const NumericalSample & xi) const;

  /** Compute mean of new points conditionnaly to observations */
  virtual NumericalPoint getConditionalMean(const NumericalPoint & xi) const;

  /** Compute covariance matrix conditionnaly to observations*/
  virtual CovarianceMatrix getConditionalCovariance(const NumericalSample & xi) const ;

  /** Compute covariance matrix conditionnaly to observations*/
  virtual CovarianceMatrix getConditionalCovariance(const NumericalPoint & xi) const;

  /** Compute joint normal distribution conditionnaly to observations*/
  virtual Normal operator()(const NumericalSample & xi) const;

  /** Compute joint normal distribution conditionnaly to observations*/
  virtual Normal operator()(const NumericalPoint & xi) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

  /** Compute cross matrix method ==> not necessary square matrix  */
  Matrix getCrossMatrix(const NumericalSample & x) const;
  void computeF() const;

private:

  // Structure for evaluation of crossCovariance
  friend struct KrigingResultCrossCovarianceFunctor;

  /** inputData should be keeped*/
  NumericalSample inputData_;

  /** input transformed data: store data*/
  NumericalSample inputTransformedData_;

  /** inputTransformation ==> iso-probabilistic transformation */
  NumericalMathFunction inputTransformation_;

  /** Boolean transformation */
  Bool hasTransformation_;

  /** The trend basis */
  BasisPersistentCollection basis_;

  /** The trend coefficients */
  NumericalPointPersistentCollection trendCoefficients_;

  /** The covariance model */
  CovarianceModel covarianceModel_;

  /** The covariance coefficients */
  NumericalSample covarianceCoefficients_;

  /** Boolean for cholesky. The factor is not mandatory (see KrigingAlgorithm) */
  Bool hasCholeskyFactor_;

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
