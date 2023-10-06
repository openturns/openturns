//                                               -*- C++ -*-
/**
 * @brief The class that implements an event random vector, i.e. a
 *        composite random vector built upon a boolean function which
 *        is described by a threshold and a comparison operator. The
 *        antecedent of the event must be a "classical" composite random
 *        vector in the context of OpenTURNS.
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_THRESHOLDEVENTIMPLEMENTATION_HXX
#define OPENTURNS_THRESHOLDEVENTIMPLEMENTATION_HXX

#include "openturns/CompositeRandomVector.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ThresholdEventImplementation
 *
 *
 */
class OT_API ThresholdEventImplementation
  : public CompositeRandomVector
{
  CLASSNAME

public:


  /** Default constructor */
  ThresholdEventImplementation();

  /** Constructor from comparison operator */
  ThresholdEventImplementation(const RandomVector & antecedent,
                               const ComparisonOperator & op,
                               const Scalar threshold);

  /** Constructor from Interval */
  ThresholdEventImplementation(const RandomVector & antecedent,
                               const Interval & interval);

  /** Virtual constructor */
  ThresholdEventImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Dimension accessor */
  UnsignedInteger getDimension() const override;

  /** Operator accessor */
  ComparisonOperator getOperator() const override;

  /** Threshold accessor */
  Scalar getThreshold() const override;

  /* Here is the interface that all derived class may implement */

  /** Domain accessor */
  Domain getDomain() const override;

  /** Realization accessor */
  Point getRealization() const override;

  /** Fixed value accessor */
  Point getFrozenRealization(const Point & fixedValue) const override;

  /** Numerical sample accessor */
  Sample getSample(const UnsignedInteger size) const override;

  /** Parameters value accessor */
  Point getParameter() const override;
  void setParameter(const Point & parameters) override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Whether it is an event */
  Bool isEvent() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The comparison operator of the eventRandomVectorImplementation */
  ComparisonOperator operator_;

  /** The threshold of the eventRandomVectorImplementation */
  Scalar threshold_;

}; /* class ThresholdEventImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_THRESHOLDEVENTIMPLEMENTATION_HXX */
