//                                               -*- C++ -*-
/**
 * @brief The class that implements an event random vector, i.e. a
 *        composite random vector built upon a boolean function which
 *        is described by a threshold and a comparison operator. The
 *        antecedent of the event must be a "classical" composite random
 *        vector in the context of OpenTURNS.
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

#ifndef OPENTURNS_EVENTRANDOMVECTOR_HXX
#define OPENTURNS_EVENTRANDOMVECTOR_HXX

#include "openturns/CompositeRandomVector.hxx"


BEGIN_NAMESPACE_OPENTURNS




/**
 * @class EventRandomVector
 *
 *
 */
class OT_API EventRandomVector
  : public CompositeRandomVector
{
  CLASSNAME;

public:


  /** Default constructor */
  EventRandomVector();

  /** Constructor from RandomVector */
  EventRandomVector(const RandomVectorImplementation & antecedent,
                                  const ComparisonOperator & op,
                                  const NumericalScalar threshold);

  /** Virtual constructor */
  virtual EventRandomVector * clone() const;

  /** String converter */
  String __repr__() const;

  /** Dimension accessor */
  virtual UnsignedInteger getDimension() const;

  /** Operator accessor */
  ComparisonOperator getOperator() const;

  /** Threshold accessor */
  NumericalScalar getThreshold() const;

  /* Here is the interface that all derived class may implement */

  /** Realization accessor */
  virtual NumericalPoint getRealization() const;

  /** Numerical sample accessor */
  virtual NumericalSample getSample(const UnsignedInteger size) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:


private:

  friend class Factory<EventRandomVector>;

  /** The comparison operator of the eventRandomVectorImplementation */
  ComparisonOperator operator_;

  /** The threshold of the eventRandomVectorImplementation */
  NumericalScalar threshold_;

}; /* class EventRandomVector */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EVENTRANDOMVECTOR_HXX */
