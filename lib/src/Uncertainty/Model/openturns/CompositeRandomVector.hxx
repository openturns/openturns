//                                               -*- C++ -*-
/**
 *  @brief An implementation class for composite random vectors
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_COMPOSITERANDOMVECTOR_HXX
#define OPENTURNS_COMPOSITERANDOMVECTOR_HXX

#include "openturns/RandomVectorImplementation.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Function.hxx"
#include "openturns/CovarianceMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CompositeRandomVector
 *
 * An implementation class for composite random vectors
 */
class OT_API CompositeRandomVector
  : public RandomVectorImplementation
{
  CLASSNAME;

public:

  /** Some typedefs to ease reading */

  /** Default constructor */
  CompositeRandomVector();

  /** Standard constructor */
  CompositeRandomVector(const Function & function,
                        const Antecedent & p_antecedent);

  /** Standard constructor */
  CompositeRandomVector(const Function & function,
                        const RandomVector & antecedent);


  /** Virtual constructor */
  virtual CompositeRandomVector * clone() const;

  /** String converter */
  String __repr__() const;



  /** Is the underlying random vector composite ? */
  Bool isComposite() const;


  /* Here is the interface that all derived class must implement */

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Realization accessor */
  Point getRealization() const;

  /** Numerical sample accessor */
  Sample getSample(const UnsignedInteger size) const;

  /** Mean accessor */
  Point getMean() const;

  /** Covariance accessor */
  CovarianceMatrix getCovariance() const;

  /** Get the random vector corresponding to the i-th marginal component */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the marginal random vector corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

  /** Antecedent accessor */
  Antecedent getAntecedent() const;

  /** Function accessor */
  Function getFunction() const;

  /** Distribution accessor */
  Distribution getDistribution() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** The function the vector is the output */
  Function function_;

  /** The antecedent of the vector through the numerical math function */
  Antecedent p_antecedent_;

private:

}; /* class CompositeRandomVector */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSITERANDOMVECTOR_HXX */
