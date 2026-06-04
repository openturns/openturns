//                                               -*- C++ -*-
/**
 *  @brief Vertex function
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_VERTEXFIELDTOPOINTFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_VERTEXFIELDTOPOINTFUNCTIONIMPLEMENTATION_HXX

#include "openturns/FieldToPointFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API VertexFieldToPointFunction
  : public FieldToPointFunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  VertexFieldToPointFunction();

  /** Default constructor */
  VertexFieldToPointFunction(const Mesh & inputMesh,
                             const UnsignedInteger dimension,
                             const Indices & indices);

  /** Virtual constructor */
  VertexFieldToPointFunction * clone() const override;

  /** Comparison operator */
  using FieldToPointFunctionImplementation::operator ==;
  Bool operator ==(const VertexFieldToPointFunction & other) const;

  /** Operator () */
  using FieldToPointFunctionImplementation::operator();
  Point operator() (const Sample & inF) const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Indices indices_;
}; /* class VertexFieldToPointFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_VERTEXFIELDTOPOINTFUNCTIONIMPLEMENTATION_HXX */
