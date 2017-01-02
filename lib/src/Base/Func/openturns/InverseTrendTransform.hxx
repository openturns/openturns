//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math function implementations
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
#ifndef OPENTURNS_INVERSETRENDTRANSOFORM_HXX
#define OPENTURNS_INVERSETRENDTRANSOFORM_HXX


#include "openturns/TemporalFunction.hxx"
#include "openturns/NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseTrendTransform
 *
 * The class that simulates a dynamical function based on a
 * numerical math function that acts only on the mesh part
 * of a field.
 */

class TrendTransform;

class OT_API InverseTrendTransform
  : public TemporalFunction
{
  CLASSNAME;
public:

  typedef NumericalMathEvaluationImplementation::Implementation EvaluationImplementation;

  /** Default constructor */
  InverseTrendTransform();

  /** Parameter constructor */
  InverseTrendTransform(const NumericalMathFunction & function);

  /** Parameter constructor */
  InverseTrendTransform(const EvaluationImplementation & p_evaluation);

  /** Parameter constructor */
  InverseTrendTransform(const NumericalMathEvaluationImplementation & evaluation);

  /** Virtual constructor */
  virtual InverseTrendTransform * clone() const;

  /** Inverse accessor */
  TrendTransform getInverse() const;

  /** Comparison operator */
  Bool operator ==(const InverseTrendTransform & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Operator () */
  using TemporalFunction::operator();
  Field operator() (const Field & inTS) const;


  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

}; /* class InverseTrendTransform */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSETRENDTRANSOFORM_HXX */
