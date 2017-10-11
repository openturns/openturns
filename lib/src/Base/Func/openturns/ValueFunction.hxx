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
#ifndef OPENTURNS_VALUEFUNCTION_HXX
#define OPENTURNS_VALUEFUNCTION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/FieldFunctionImplementation.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ValueFunction
 *
 * The class that simulates a dynamical function based on a
 * numerical math function that acts only on the spatial part
 * of a time series.
 */
class OT_API ValueFunction
  : public FieldFunctionImplementation
{
  CLASSNAME
public:

  typedef EvaluationImplementation::Implementation EvaluationPointer;

  /** Default constructor */
  explicit ValueFunction(const UnsignedInteger meshDimension = 1);

  /** Parameter constructor */
  explicit ValueFunction(const Function & function,
                         const UnsignedInteger meshDimension = 1);

#ifndef SWIG
  /** Parameter constructor */
  explicit ValueFunction(const EvaluationPointer & p_evaluation,
                         const UnsignedInteger meshDimension = 1);
#endif

  /** Parameter constructor */
  explicit ValueFunction(const EvaluationImplementation & evaluation,
                         const UnsignedInteger meshDimension = 1);

  /** Virtual constructor */
  virtual ValueFunction * clone() const;

  /** Comparison operator */
  Bool operator ==(const ValueFunction & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  using FieldFunctionImplementation::operator();
  Field operator() (const Field & inFld) const;

  /** Get the i-th marginal function */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

  /** Function accessor */
  Function getFunction() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** The underlying function */
  Function function_;

}; /* class ValueFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_VALUEFUNCTION_HXX */
