//                                               -*- C++ -*-
/**
 *  @brief The class that implements the point to point connection
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
#ifndef OPENTURNS_POINTTOPOINTCONNECTION_HXX
#define OPENTURNS_POINTTOPOINTCONNECTION_HXX

#include "openturns/FunctionImplementation.hxx"
#include "openturns/Function.hxx"
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/FieldToPointFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class PointToPointConnection
 */

class OT_API PointToPointConnection
  : public FunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PointToPointConnection();

  /** Composition constructor */
  PointToPointConnection(const Function & leftFunction,
                         const Function & rightFunction);

  /** Composition constructor */
  PointToPointConnection(const FieldToPointFunction & fieldToPointFunction,
                         const PointToFieldFunction & pointToFieldFunction);

  /** Virtual constructor */
  PointToPointConnection * clone() const override;

  /** Comparison operator */
  Bool operator ==(const PointToPointConnection & other) const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

}; /* class PointToPointConnection */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINTTOPOINTCONNECTION_HXX */
