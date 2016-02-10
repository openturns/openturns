//                                               -*- C++ -*-
/**
 *  @brief An implementation class for distribution-based random vectors
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
#ifndef OPENTURNS_CONDITIONALRANDOMVECTOR_HXX
#define OPENTURNS_CONDITIONALRANDOMVECTOR_HXX

#include "OTprivate.hxx"
#include "RandomVectorImplementation.hxx"
#include "RandomVector.hxx"
#include "Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS





/**
 * @class ConditionalRandomVector
 *
 * An implementation class for distribution-based random vectors
 */
class OT_API ConditionalRandomVector
  : public RandomVectorImplementation
{
  CLASSNAME;

public:


  /** Default constructor */
  ConditionalRandomVector(const Distribution & distribution,
                          const RandomVector & randomParameters);


  /** Virtual constructor */
  virtual ConditionalRandomVector * clone() const;

  /** String converter */
  String __repr__() const;


  /* Here is the interface that all derived class must implement */

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Realization accessor */
  NumericalPoint getRealization() const;
  NumericalPoint getRealization(NumericalPoint & parameters) const;

  /** Distribution accessor */
  Distribution getDistribution() const;

  /** Random parameters accessor */
  RandomVector getRandomParameters() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  friend class Factory<ConditionalRandomVector>;

  /** Default constructor */
  ConditionalRandomVector() : RandomVectorImplementation() {};

private:

  /** The distribution the vector follows */
  mutable Distribution distribution_;

  /** The random vector defining the distribution parameters */
  RandomVector randomParameters_;

}; /* class ConditionalRandomVector */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONDITIONALRANDOMVECTOR_HXX */
