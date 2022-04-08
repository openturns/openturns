//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an combinatorialGenerator plane
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_COMBINATORIALGENERATOR_HXX
#define OPENTURNS_COMBINATORIALGENERATOR_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/CombinatorialGeneratorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CombinatorialGenerator
 *
 * The class describes the probabilistic concept of combinatorialGenerator plan
 */
class OT_API CombinatorialGenerator
  : public TypedInterfaceObject<CombinatorialGeneratorImplementation>
{
  CLASSNAME
public:

  /** Default constructor */
  CombinatorialGenerator();

  /** Constructor from implementation */
  CombinatorialGenerator(const CombinatorialGeneratorImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  CombinatorialGenerator(const Implementation & p_implementation);
#endif

  /** String converter */
  String __repr__() const override;

  /** Sample generation */
  virtual IndicesCollection generate() const;

}; /* class CombinatorialGenerator */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMBINATORIALGENERATOR_HXX */
