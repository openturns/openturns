//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the point to point connection
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
#include "openturns/PointToPointConnection.hxx"
#include "openturns/PointToPointEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PointToPointConnection)

static const Factory<PointToPointConnection> Factory_PointToPointConnection;

/* Default constructor */
PointToPointConnection::PointToPointConnection()
  : FunctionImplementation(new PointToPointEvaluation())
{
  // Nothing to do
}

/* Parameters constructor */
PointToPointConnection::PointToPointConnection(const Function & leftFunction,
    const Function & rightFunction)
  : FunctionImplementation(new PointToPointEvaluation(leftFunction, rightFunction))
{
  // Nothing to do
}

/* Parameters constructor */
PointToPointConnection::PointToPointConnection(const FieldToPointFunction & fieldToPointFunction,
    const PointToFieldFunction & pointToFieldFunction)
  : FunctionImplementation(new PointToPointEvaluation(fieldToPointFunction, pointToFieldFunction))
{
  // Nothing to do
}

/* Virtual constructor */
PointToPointConnection * PointToPointConnection::clone() const
{
  return new PointToPointConnection(*this);
}

/* Comparison operator */
Bool PointToPointConnection::operator ==(const PointToPointConnection & ) const
{
  return true;
}

/* String converter */
String PointToPointConnection::__repr__() const
{
  OSS oss;
  oss << "class=" << PointToPointConnection::GetClassName()
      << " name=" << getName()
      << " evaluation=" << getEvaluation().getImplementation()->__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void PointToPointConnection::save(Advocate & adv) const
{
  FunctionImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void PointToPointConnection::load(Advocate & adv)
{
  FunctionImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
