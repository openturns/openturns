//                                               -*- C++ -*-
/**
 *  @brief Function mapping a point to a field.
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_VERTEXVALUEPOINTTOFIELDFUNCTION_HXX
#define OPENTURNS_VERTEXVALUEPOINTTOFIELDFUNCTION_HXX

#include "openturns/PointToFieldFunctionImplementation.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API VertexValuePointToFieldFunction : public PointToFieldFunctionImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  VertexValuePointToFieldFunction();

  /** Constructor with parameters */
  VertexValuePointToFieldFunction(const Function & function,
                                  const Mesh & mesh);

  /** Virtual constructor */
  VertexValuePointToFieldFunction * clone() const override;

  /** Operator () */
  using PointToFieldFunctionImplementation::operator();
  Sample operator() (const Point & inP) const override;

  /** Get the function corresponding to indices components */
  using PointToFieldFunctionImplementation::getMarginal;
  PointToFieldFunction getMarginal(const Indices & indices) const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Function accessor */
  Function getFunction() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  Function function_;
};

}
#endif
