//                                               -*- C++ -*-
/**
 *  @brief P1 Lagrange piecewise linear function.
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
#ifndef OPENTURNS_P1LAGRANGEEVALUATIONIMPLEMENTATION_HXX
#define OPENTURNS_P1LAGRANGEEVALUATIONIMPLEMENTATION_HXX

#include "NumericalMathEvaluationImplementation.hxx"
#include "PersistentCollection.hxx"
#include "Field.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class P1LagrangeEvaluationImplementation
 * Provided a field, compute the P1 piecewise-linear Lagrange interpolation.
 */
class OT_API P1LagrangeEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
  friend class NumericalMathFunctionImplementation;

public:

  /** Default constructor */
  P1LagrangeEvaluationImplementation();

  /** Default constructor */
  P1LagrangeEvaluationImplementation(const Field & field);

  /** Virtual constructor */
  virtual P1LagrangeEvaluationImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const P1LagrangeEvaluationImplementation & other) const;

  /** String converter */
  String __repr__() const;
  String __str__( const String & offset = "" ) const;

  /** Field accessor */
  void setField(const Field & field);
  Field getField() const;

  /** Vertices accessor */
  void setVertices(const NumericalSample & vertices);
  NumericalSample getVertices() const;

  /** Values accessor */
  void setValues(const NumericalSample & values);
  NumericalSample getValues() const;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  virtual NumericalPoint operator()(const NumericalPoint & inP) const;
  virtual NumericalSample operator()(const NumericalSample & inS) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:
  /* Field defining the P1 Lagrange interpolation */
  Field field_;

  /* Collection of indices storing for each vertex the simplices to which it belongs */
  PersistentCollection< Indices > verticesToSimplices_;

private:


}; /* class P1LagrangeEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_P1LAGRANGEEVALUATIONIMPLEMENTATION_HXX */
