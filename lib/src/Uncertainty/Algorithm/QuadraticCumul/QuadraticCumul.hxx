//                                               -*- C++ -*-
/**
 *  @brief
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
#ifndef OPENTURNS_QUADRATICCUMUL_HXX
#define OPENTURNS_QUADRATICCUMUL_HXX

#include "PersistentObject.hxx"
#include "NumericalPoint.hxx"
#include "NumericalPointWithDescription.hxx"
#include "RandomVector.hxx"
#include "SymmetricTensor.hxx"
#include "CovarianceMatrix.hxx"
#include "Matrix.hxx"
#include "Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class QuadraticCumul
 * QuadraticCumul implements the mean and covariance of a random vector
 Y=G(X) by implementing the Taylor approximation of G
*/

class OT_API QuadraticCumul
  : public PersistentObject
{

  CLASSNAME;
public:


  /** Default constructor */
  QuadraticCumul() {};

  /** Constructor with parameters */
  explicit QuadraticCumul(const RandomVector & limitStateVariable);


  /** Virtual constructor */
  virtual QuadraticCumul * clone() const;

  /** String converter */
  String __repr__() const;

  /** limitStateVariable accessor */
  RandomVector getLimitStateVariable() const;

  /** meanFirstOrder accessor */
  NumericalPoint getMeanFirstOrder() const;

  /** meanSecondOrder accessor */
  NumericalPoint getMeanSecondOrder() const;

  /** covariance accessor */
  CovarianceMatrix getCovariance() const;

  /** Value at mean accessor */
  NumericalPoint getValueAtMean() const;

  /** Gradient at mean accessor */
  Matrix getGradientAtMean() const;

  /** Hessian at mean accessor */
  SymmetricTensor getHessianAtMean() const;

  /** importance factors accessor */
  NumericalPointWithDescription getImportanceFactors() const;

  /** ImportanceFactors graph */
  Graph drawImportanceFactors() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  friend class Factory<QuadraticCumul>;

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
  mutable NumericalPoint meanInputVector_;
  mutable NumericalPoint valueAtMean_;
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
  mutable NumericalPoint meanFirstOrder_;
  mutable NumericalPoint meanSecondOrder_;
  mutable CovarianceMatrix covariance_;
  mutable NumericalPointWithDescription importanceFactors_;

} ; /* class QuadraticCumul */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUADRATICCUMUL_HXX */
