
//                                               -*- C++ -*-
/**
 * @brief HSICEstimator is a base class that  implements the HSIC sensivity indices.
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
#ifndef OPENTURNS_HSICESTIMATOR_HXX
#define OPENTURNS_HSICESTIMATOR_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/HSICEstimatorImplementation.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HSICEstimator
 *
 */
class OT_API HSICEstimator
  : public TypedInterfaceObject<HSICEstimatorImplementation>
{
  CLASSNAME

public:

  typedef Collection<CovarianceModel> CovarianceModelCollection;
  typedef Pointer<HSICEstimatorImplementation> Implementation;

public:

  /** Default ctor */
  HSICEstimator();

  /** Constructor */
  HSICEstimator(const CovarianceModelCollection & covarianceList, const Sample & X, const Sample & Y, const HSICStat & estimatorType);

  /** Constructor */
  HSICEstimator(const CovarianceModelCollection & covarianceList, const Sample & X, const Sample & Y, const HSICStat & estimatorType, const Function & weightFunction);

  /** Constructor from implementation */
  HSICEstimator(const HSICEstimatorImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  HSICEstimator(HSICEstimatorImplementation * p_implementation);
#endif


protected:

  /** Compute the weight matrix from the weight function */
  SquareMatrix computeWeightMatrix(const Sample & Y) const;

  /** Compute a HSIC index (one marginal) by using the underlying estimator (biased or not) */
  Scalar computeHSICIndex( const Sample & inSample, const Sample & outSample, const CovarianceModel & inCovariance, const CovarianceModel & outCovariance, const SquareMatrix & weightMatrix) const;

  /** Compute p-value with permutation */
  void computePValuesPermutation();

  /** Compute HSIC and R2-HSIC indices */
  void computeIndices();

public:
  /** Get the HSIC indices.
   *  This is not const as it triggers a computation of the indices
   *  if they are not computed yet.
   * */
  Point getHSICIndices();

  /** Get the R2-HSIC indices.
   *  This is not const as it triggers a computation of the indices
   *  if they are not computed yet.
   * */
  Point getR2HSICIndices();

  /** Get the p-values by permutation.
   *  This is not const as it triggers a computation of the values
   *  if they are not computed yet.
   * */
  Point getPValuesPermutation();


public:
  /** Graphic methods */
  Graph drawHSICIndices();

  /** Graphic methods */
  Graph drawR2HSICIndices();

  /** Graphic methods */
  Graph drawPValuesPermutation();

public:
  /** Set the number of permutation used */
  void setPermutationSize(const UnsignedInteger B);

  /** Get the number of permutation used */
  UnsignedInteger getPermutationSize() const;

  /** Get the covariance models */
  HSICEstimatorImplementation::CovarianceModelCollection getCovarianceList() const;

  /** Set the covariance models */
  void setCovarianceList(const CovarianceModelCollection & coll);

  /** Get the input sample */
  Sample getInputSample() const;

  /** Set the input sample */
  void setInputSample(const Sample & inputSample);

  /** Get the output sample */
  Sample getOutputSample() const;

  /** Sset the output sample */
  void setOutputSample(const Sample & outputSample);

  /** Get the dimension of the indices: the number of marginals */
  UnsignedInteger getDimension() const;

  /** Get the size of the study sample */
  UnsignedInteger getSize() const;

  /** Get the underlying estimator: biased or unbiased*/
  HSICStat getEstimator() const;

protected:
  /** Reset indices to void */
  void resetIndices();

};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HSICESTIMATOR_HXX */
