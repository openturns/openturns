//                                               -*- C++ -*-
/**
 *  @brief The class that implements the field to point connection
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FIELDTOPOINTCONNECTION_HXX
#define OPENTURNS_FIELDTOPOINTCONNECTION_HXX

#include "openturns/FieldToPointFunctionImplementation.hxx"
#include "openturns/Function.hxx"
#include "openturns/FieldFunction.hxx"
#include "openturns/FieldToPointFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class FieldToPointConnection
 */

class OT_API FieldToPointConnection
  : public FieldToPointFunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  FieldToPointConnection();

  /** Composition constructor */
  FieldToPointConnection(const Function & function,
                         const FieldToPointFunction & fieldToPointFunction);

  /** Composition constructor */
  FieldToPointConnection(const FieldToPointFunction & fieldToPointFunction,
                         const FieldFunction & fieldFunction);

  /** Virtual constructor */
  virtual FieldToPointConnection * clone() const;

  /** Comparison operator */
  Bool operator ==(const FieldToPointConnection & other) const;

  /** Operator () */
  using FieldToPointFunctionImplementation::operator();
  Point operator() (const Sample & inF) const;

  Sample operator() (const ProcessSample & inS) const;

  /** String converter */
  virtual String __repr__() const;

  virtual String __str__(const String & offset = "") const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Get the i-th marginal function */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Composed implementation accessor */
  FieldFunction getFieldFunction() const;
  Function getFunction() const;
  FieldToPointFunction getFieldToPointFunction() const;

protected:

private:

  /** Flag to tell if it is a composition of a function with
   * a field to point function or a field to point function
   * and a field function */
  Bool startByFieldToPointFunction_;

  /** The function f in h = f o g */
  Function function_;

  /** The field function g in h = f o g */
  FieldFunction fieldFunction_;

  /** The field to point function */
  FieldToPointFunction fieldToPointFunction_;

}; /* class FieldToPointConnection */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDTOPOINTCONNECTION_HXX */
