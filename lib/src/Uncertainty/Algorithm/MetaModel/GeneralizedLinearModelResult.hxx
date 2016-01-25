//                                               -*- C++ -*-
/**
 *  @brief The result of a linear model estimation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_GENERALIZEDLINEARMODELRESULT_HXX
#define OPENTURNS_GENERALIZEDLINEARMODELRESULT_HXX

#include "MetaModelResult.hxx"
#include "CovarianceModel.hxx"
#include "NumericalSample.hxx"
#include "Collection.hxx"
#include "PersistentCollection.hxx"
#include "NumericalMathFunction.hxx"
#include "Process.hxx"
#include "HMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GeneralizedLinearModelResult
 *
 * The result of a generalized linear model evaluation
 */

class OT_API GeneralizedLinearModelResult
  : public MetaModelResult
{
  CLASSNAME;

public:

  // friend class Factory<GeneralizedLinearModelResult>;
  typedef Collection<NumericalPoint> NumericalPointCollection;
  typedef PersistentCollection<NumericalPoint> NumericalPointPersistentCollection;
  typedef Collection<Basis> BasisCollection;
  typedef PersistentCollection<Basis> BasisPersistentCollection;

  /** Default constructor */
  GeneralizedLinearModelResult();

  /** Parameter constructor without any cholesky factor*/
  GeneralizedLinearModelResult(const NumericalSample & inputData,
                               const NumericalSample & outputData,
                               const NumericalMathFunction & metaModel,
                               const NumericalPoint & residuals,
                               const NumericalPoint & relativeErrors,
                               const BasisCollection & basis,
                               const NumericalPointCollection & trendCoefficients,
                               const CovarianceModel & covarianceModel);

  /** Parameter constructor with Cholesky factor (Lapack)*/
  GeneralizedLinearModelResult(const NumericalSample & inputData,
                               const NumericalSample & outputData,
                               const NumericalMathFunction & metaModel,
                               const NumericalPoint & residuals,
                               const NumericalPoint & relativeErrors,
                               const BasisCollection & basis,
                               const NumericalPointCollection & trendCoefficients,
                               const CovarianceModel & covarianceModel,
                               const TriangularMatrix & covarianceCholeskyFactor,
                               const HMatrix & covarianceHMatrix);

  /** Virtual constructor */
  GeneralizedLinearModelResult * clone() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Trend basis accessor */
  BasisCollection getBasisCollection() const;

  /** Trend coefficients accessor */
  NumericalPointCollection getTrendCoefficients() const;

  /** Conditional covariance models accessor */
  CovarianceModel getCovarianceModel() const;

  /** Transformation accessor */
  NumericalMathFunction getTransformation() const;
  void setTransformation(const NumericalMathFunction & transformation);

  /** process accessor */
  Process getNoise() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


 protected:

  // KrigingAlgorithm::run could ask for the Cholesky factor
  friend class KrigingAlgorithm;
  /** Method that returns the covariance factor - lapack */
  TriangularMatrix getCholeskyFactor() const;

  /** Method that returns the covariance factor - hmat */
  HMatrix getHMatCholeskyFactor() const;

  // Return input sample transformed
  NumericalSample getInputTransformedSample() const;

 private:

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
  NumericalPointPersistentCollection beta_;

  /** The covariance model */
  CovarianceModel covarianceModel_;

  /** Boolean for cholesky. */
  Bool hasCholeskyFactor_;

  /** Cholesky factor  */
  mutable TriangularMatrix covarianceCholeskyFactor_;

  /** Cholesky factor when using hmat-oss/hmat */
  mutable HMatrix covarianceHMatrix_;

} ; /* class GeneralizedLinearModelResult */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KRIGINGRESULT_HXX */
