//                                               -*- C++ -*-
/**
 *  @brief An implementation class for constant random vectors
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CONSTANTRANDOMVECTOR_HXX
#define OPENTURNS_CONSTANTRANDOMVECTOR_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/RandomVectorImplementation.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS





/**
 * @class ConstantRandomVector
 *
 * An implementation class for constant random vectors
 */
class OT_API ConstantRandomVector
  : public RandomVectorImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  ConstantRandomVector();

  /** Parameter constructor */
  explicit ConstantRandomVector(const PointWithDescription & point);

  /** Parameter constructor */
  explicit ConstantRandomVector(const Point & point);

  /** Virtual constructor */
  virtual ConstantRandomVector * clone() const;

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameters);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;

  /** String converter */
  String __repr__() const;




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
  RandomVector getMarginal(const UnsignedInteger i) const;

  /** Get the marginal random vector corresponding to indices components */
  RandomVector getMarginal(const Indices & indices) const;

  /** Distribution accessor */
  Distribution getDistribution() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** The point where the vector realizes itself */
  Point point_;

}; /* class ConstantRandomVector */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONSTANTRANDOMVECTOR_HXX */
