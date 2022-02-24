
//                                               -*- C++ -*-
/**
 * @brief HSICEstimatorImplementation implements the HSIC sensivity index
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_HSICESTIMATORIMPLEMENTATION_HXX
#define OPENTURNS_HSICESTIMATORIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Graph.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/Function.hxx"
#include "openturns/HSICStat.hxx"
#include "openturns/RandomGenerator.hxx"
BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HSICEstimatorImplementation
 *
 */
class OT_API HSICEstimatorImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  typedef Collection <CovarianceModel>  CovarianceModelCollection;

  /** Default constructor */
  HSICEstimatorImplementation();

  /** Constructor */
  HSICEstimatorImplementation(const CovarianceModelCollection & covarianceList, const Sample & X, const Sample & Y, const HSICStat & estimatorType);

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  HSICEstimatorImplementation * clone() const override;

  /** Set the number of permutation used */
  void setPermutationSize(const UnsignedInteger B);

  /** Get the number of permutation used */
  UnsignedInteger getPermutationSize() const;

  /** Get the covariance models */
  PersistentCollection <CovarianceModel> getCovarianceList() const;

  /** Set the covariance models */
  void setCovarianceList(const CovarianceModelCollection & coll);

  /** Get the input sample */
  Sample getInputSample() const;

  /** Set the input sample */
  void setInputSample(const Sample & inputSample);

  /** Get the output sample */
  Sample getOutputSample() const;

  /** Set the output sample */
  void setOutputSample(const Sample & outputSample);

  /** Get the dimension of the indices: the number of marginals */
  UnsignedInteger getDimension() const;

  /** Get the size of the study sample */
  UnsignedInteger getSize() const;

  /** Get the underlying estimator: biased or unbiased*/
  HSICStat getEstimator() const;

  /** Get the HSIC indices */
  Point getHSICIndices() const;

  /** Get the R2-HSIC indices */
  Point getR2HSICIndices() const;

  /** Get the p-values by permutation */
  Point getPValuesPermutation() const;

  /** Compute all indices at once */
  virtual void run() const;

  /** Draw the HSIC indices */
  Graph drawHSICIndices() const;

  /** Draw the R2-HSIC indices */
  Graph drawR2HSICIndices() const;

  /** Draw the p-values by permutation */
  Graph drawPValuesPermutation() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Reset indices to void */
  virtual void resetIndices();

  /** Compute p-value with permutation */
  virtual void computePValuesPermutation() const;

  /** Compute the weight matrix from the weight function */
  virtual SquareMatrix computeWeightMatrix(const Sample & Y) const;

  /** Compute a HSIC index (one marginal) by using the underlying estimator (biased or not) */
  virtual Scalar computeHSICIndex( const Sample & inSample, const Sample & outSample, const CovarianceModel & inCovariance, const CovarianceModel & outCovariance, const SquareMatrix & weightMatrix) const;

  /** Compute HSIC and R2-HSIC indices */
  virtual void computeIndices() const;

  /** Draw values stored in a point */
  Graph drawValues(const Point &values, const String &title) const;

  Sample shuffledCopy(const Sample & inSample) const;

  /** Data */
  PersistentCollection <CovarianceModel> covarianceList_ ;
  Sample inputSample_ ;
  Sample outputSample_ ;
  HSICStat estimatorType_;
  Function weightFunction_ ;
  UnsignedInteger n_ ;
  UnsignedInteger inputDimension_ ;
  mutable Point HSIC_XY_ ;
  mutable Point HSIC_XX_ ;
  mutable Point HSIC_YY_ ;
  mutable Point R2HSICIndices_;
  mutable Point PValuesPermutation_ ;
  UnsignedInteger permutationSize_ ;
  mutable Bool isAlreadyComputedIndices_ ;
  mutable Bool isAlreadyComputedPValuesPermutation_ ;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HSICESTIMATORIMPLEMENTATION_HXX */
