//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionMoments implements Taylor expansion for moments estimation
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_TAYLOREXPANSIONMOMENTS_HXX
#define OPENTURNS_TAYLOREXPANSIONMOMENTS_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/SymmetricTensor.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class TaylorExpansionMoments
 * TaylorExpansionMoments estimates the mean and covariance of a random vector
 Y=G(X) by computing the Taylor approximation of G
*/

class OT_API TaylorExpansionMoments
  : public PersistentObject
{

  CLASSNAME
public:


  /** Default constructor */
  TaylorExpansionMoments();

  /** Constructor with parameters */
  explicit TaylorExpansionMoments(const RandomVector & limitStateVariable);


  /** Virtual constructor */
  virtual TaylorExpansionMoments * clone() const;

  /** String converter */
  String __repr__() const;

  /** limitStateVariable accessor */
  RandomVector getLimitStateVariable() const;

  /** meanFirstOrder accessor */
  Point getMeanFirstOrder() const;

  /** meanSecondOrder accessor */
  Point getMeanSecondOrder() const;

  /** covariance accessor */
  CovarianceMatrix getCovariance() const;

  /** Value at mean accessor */
  Point getValueAtMean() const;

  /** Gradient at mean accessor */
  Matrix getGradientAtMean() const;

  /** Hessian at mean accessor */
  SymmetricTensor getHessianAtMean() const;

  /** importance factors accessor */
  PointWithDescription getImportanceFactors() const;

  /** ImportanceFactors graph */
  Graph drawImportanceFactors() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** the function that computes the first order evaluation of the mean vector  */
  void computeMeanFirstOrder() const;

  /** the function that computes the second order evaluation of the mean vector */
  void computeMeanSecondOrder() const;

  /** the function that computes the matrix covariance */
  void computeCovariance() const;

  /** the function that computes the importance factors only in the scalar case */
  void computeImportanceFactors () const;

  RandomVector limitStateVariable_;
  mutable Point meanInputVector_;
  mutable Point valueAtMean_;
  mutable Matrix gradientAtMean_;
  mutable SymmetricTensor hessianAtMean_;
  mutable Bool isAlreadyComputedValue_;
  mutable Bool isAlreadyComputedGradient_;
  mutable Bool isAlreadyComputedHessian_;
  mutable Bool isAlreadyComputedMeanFirstOrder_;
  mutable Bool isAlreadyComputedMeanSecondOrder_;
  mutable Bool isAlreadyComputedCovariance_;
  mutable Bool isAlreadyComputedImportanceFactors_;
  mutable CovarianceMatrix inputCovariance_;
  mutable Point meanFirstOrder_;
  mutable Point meanSecondOrder_;
  mutable CovarianceMatrix covariance_;
  mutable PointWithDescription importanceFactors_;

} ; /* class TaylorExpansionMoments */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TAYLOREXPANSIONMOMENTS_HXX */
