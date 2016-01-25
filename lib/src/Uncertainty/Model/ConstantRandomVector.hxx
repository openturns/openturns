//                                               -*- C++ -*-
/**
 *  @brief An implementation class for constant random vectors
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
#ifndef OPENTURNS_CONSTANTRANDOMVECTOR_HXX
#define OPENTURNS_CONSTANTRANDOMVECTOR_HXX

#include "OTprivate.hxx"
#include "RandomVectorImplementation.hxx"
#include "NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS





/**
 * @class ConstantRandomVector
 *
 * An implementation class for constant random vectors
 */
class OT_API ConstantRandomVector
  : public RandomVectorImplementation
{
  CLASSNAME;

public:

  /** Default constructor */
  ConstantRandomVector(const NumericalPointWithDescription & point);

  /** Parameter constructor */
  ConstantRandomVector(const NumericalPoint & point);


  /** Virtual constructor */
  virtual ConstantRandomVector * clone() const;

  /** String converter */
  String __repr__() const;




  /* Here is the interface that all derived class must implement */

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Realization accessor */
  NumericalPoint getRealization() const;

  /** Numerical sample accessor */
  NumericalSample getSample(const UnsignedInteger size) const;

  /** Mean accessor */
  NumericalPoint getMean() const;

  /** Covariance accessor */
  CovarianceMatrix getCovariance() const;

  /** Get the random vector corresponding to the i-th marginal component */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the marginal random vector corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

  /** Distribution accessor */
  Distribution getDistribution() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  friend class Factory<ConstantRandomVector>;

  /** Default constructor */
  ConstantRandomVector() : RandomVectorImplementation() {};

private:

  /** The point where the vector realizes itself */
  NumericalPoint point_;

}; /* class ConstantRandomVector */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONSTANTRANDOMVECTOR_HXX */
