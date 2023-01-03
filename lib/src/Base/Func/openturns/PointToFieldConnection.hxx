//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between evaluation and
 *        point to field function or between point to field function and
 *        field function
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

#ifndef OPENTURNS_POINTTOFIELDCONNECTION_HXX
#define OPENTURNS_POINTTOFIELDCONNECTION_HXX

#include "openturns/PointToFieldFunctionImplementation.hxx"
#include "openturns/Function.hxx"
#include "openturns/FieldFunction.hxx"
#include "openturns/PointToFieldFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PointToFieldConnection
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API PointToFieldConnection
  : public PointToFieldFunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PointToFieldConnection();

  /** Parameter constructor */
  PointToFieldConnection(const FieldFunction & fieldFunction,
                         const PointToFieldFunction & pointToFieldFunction);


  /** parameter constructor */
  PointToFieldConnection(const PointToFieldFunction & pointToFieldFunction,
                         const Function & function);


  /** Virtual constructor */
  PointToFieldConnection * clone() const override;

  /** Comparison operator */
  Bool operator ==(const PointToFieldConnection & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  using PointToFieldFunctionImplementation::operator();
  Sample operator() (const Point & inP) const override;

  ProcessSample operator() (const Sample & inS) const override;

  /** Get the i-th marginal function */
  PointToFieldFunction getMarginal(const UnsignedInteger i) const override;

  /** Get the function corresponding to indices components */
  PointToFieldFunction getMarginal(const Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Composed implementation accessor */
  Function getFunction() const;
  FieldFunction getFieldFunction() const;
  PointToFieldFunction getPointToFieldFunction() const;

protected:


private:

  /** Flag to tell if it is a composition between a field function and a point
      to field function (true) or a point to field function and a field function */
  Bool startByPointToFieldFunction_;

  /** The function f in h = f o g */
  Function function_;

  /** The function g in h = f o g */
  FieldFunction fieldFunction_;

  /** The point to field function */
  PointToFieldFunction pointToFieldFunction_;

}; /* class PointToFieldConnection */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINTTOFIELDCONNECTION_HXX */
