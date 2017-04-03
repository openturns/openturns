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
#ifndef OPENTURNS_TRENDTRANSOFORM_HXX
#define OPENTURNS_TRENDTRANSOFORM_HXX

#include "openturns/VertexValueFunction.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TrendTransform
 *
 * The class that simulates a dynamical function based on a
 * numerical math function that acts only on the temporal part
 * of a time series.
 */

class InverseTrendTransform;

class OT_API TrendTransform
  : public VertexValueFunction
{
  CLASSNAME;
public:

  typedef EvaluationImplementation::Implementation EvaluationPointer;

  /** Default constructor */
  TrendTransform();

  /** Parameter constructor */
  explicit TrendTransform(const Function & function);

  /** Parameter constructor */
  TrendTransform(const EvaluationPointer & p_evaluation);

  /** Parameter constructor */
  TrendTransform(const EvaluationImplementation & evaluation);

  /** Virtual constructor */
  virtual TrendTransform * clone() const;

  /** Comparison operator */
  Bool operator ==(const TrendTransform & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Operator () */
  using VertexValueFunction::operator();
  Field operator() (const Field & inFld) const;

  /** Inverse accessor */
  InverseTrendTransform getInverse() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

}; /* class TrendTransform */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRENDTRANSOFORM_HXX */
