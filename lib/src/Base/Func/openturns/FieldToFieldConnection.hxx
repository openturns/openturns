//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between field functions or
 *        point to field and field to point functions
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_FIELDTOFIELDCONNECTION_HXX
#define OPENTURNS_FIELDTOFIELDCONNECTION_HXX

#include "openturns/FieldFunction.hxx"
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/FieldToPointFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FieldToFieldConnection
 *
 * The class that implements the composition between field functions or
 * point to field and field to point functions
 */
class OT_API FieldToFieldConnection
  : public FieldFunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  FieldToFieldConnection();

  /** Parameter constructor */
  FieldToFieldConnection(const FieldFunction & leftFieldFunction,
                         const FieldFunction & rightFieldFunction);


  /** parameter constructor */
  FieldToFieldConnection(const PointToFieldFunction & pointToFieldFunction,
                         const FieldToPointFunction & fieldToPointFunction);


  /** Virtual constructor */
  FieldToFieldConnection * clone() const override;

  /** Comparison operator */
  Bool operator ==(const FieldToFieldConnection & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  using FieldFunctionImplementation::operator();
  Sample operator() (const Sample & inF) const override;

  ProcessSample operator() (const ProcessSample & inS) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Get the i-th marginal function */
  Implementation getMarginal(const UnsignedInteger i) const override;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Composed implementation accessor */
  FieldFunction getLeftFieldFunction() const;
  FieldFunction getRightFieldFunction() const;
  PointToFieldFunction getPointToFieldFunction() const;
  FieldToPointFunction getFieldToPointFunction() const;

protected:


private:

  /** Flag to tell if it is a composition of functions or a
   * composition of point to field and field to point functions */
  Bool isFieldFunctionComposition_;

  /** The field function f in h = f o g */
  FieldFunction leftFieldFunction_;

  /** The field function g in h = f o g */
  FieldFunction rightFieldFunction_;

  /** The field to point function */
  FieldToPointFunction fieldToPointFunction_;

  /** The point to field function */
  PointToFieldFunction pointToFieldFunction_;

}; /* class FieldToFieldConnection */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDTOFIELDCONNECTION_HXX */
