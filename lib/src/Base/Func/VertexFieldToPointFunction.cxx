//                                               -*- C++ -*-
/**
 *  @brief Vertex function
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/VertexFieldToPointFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(VertexFieldToPointFunction)

/* Default constructor */
VertexFieldToPointFunction::VertexFieldToPointFunction()
  : FieldToPointFunctionImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
VertexFieldToPointFunction::VertexFieldToPointFunction(const Mesh & inputMesh,
    const UnsignedInteger dimension,
    const Indices & indices)
  : FieldToPointFunctionImplementation(inputMesh, dimension, dimension)
  , indices_(indices)
{
  if (indices.getSize() != 1)
    throw InvalidArgumentException(HERE) << "VertexFieldToPointFunction only supports indices of size 1 for now";
  if (!indices.check(getInputMesh().getVerticesNumber()))
    throw InvalidArgumentException(HERE) << "VertexFieldToPointFunction indices (" << indices << ") must be in [0; " << getInputMesh().getVerticesNumber() << "[";
}

/* Virtual constructor */
VertexFieldToPointFunction * VertexFieldToPointFunction::clone() const
{
  return new VertexFieldToPointFunction(*this);
}

/* Comparison operator */
Bool VertexFieldToPointFunction::operator ==(const VertexFieldToPointFunction & other) const
{
  if (this == &other) return true;
  return indices_ == other.indices_;
}

/* Operator () */
Point VertexFieldToPointFunction::operator() (const Sample & inF) const
{
  if (inF.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a FieldToPointConnection with an argument of invalid dimension";
  if (inF.getSize() != getInputMesh().getVerticesNumber()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a FieldToPointConnection with an argument of invalid size";
  callsNumber_.increment();

  const Point outValue(inF[indices_[0]]);
  return outValue;
}

/* String converter */
String VertexFieldToPointFunction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << VertexFieldToPointFunction::GetClassName()
      << " indices=" << indices_;
  return oss;
}

String VertexFieldToPointFunction::__str__(const String & ) const
{
  OSS oss(false);
  oss << "VertexFieldToPointFunction(indices=" << indices_ << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void VertexFieldToPointFunction::save(Advocate & adv) const
{
  FieldToPointFunctionImplementation::save(adv);
  adv.saveAttribute( "indices_", indices_ );
}

/* Method load() reloads the object from the StorageManager */
void VertexFieldToPointFunction::load(Advocate & adv)
{
  FieldToPointFunctionImplementation::load(adv);
  adv.loadAttribute( "indices_", indices_ );
}

END_NAMESPACE_OPENTURNS
