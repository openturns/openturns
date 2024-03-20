//                                               -*- C++ -*-
/**
 *  @brief
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
#ifndef OPENTURNS_SAMPLINGSTRATEGY_HXX
#define OPENTURNS_SAMPLINGSTRATEGY_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/SamplingStrategyImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SamplingStrategy
 */

class OT_API SamplingStrategy
  : public TypedInterfaceObject<SamplingStrategyImplementation>
{

  CLASSNAME

public:

  /** Default constructor */
  SamplingStrategy();

  /** Constructor with parameters */
  SamplingStrategy(const SamplingStrategyImplementation & implementation);

  /** Constructor with parameters */
  SamplingStrategy(Implementation & p_implementation);

  /** Constructor with parameters */
  explicit SamplingStrategy(const UnsignedInteger dimension);


  /** Generate a set of directions */
  Sample generate() const;

  /** Dimension accessor */
  void setDimension(const UnsignedInteger dimension);
  UnsignedInteger getDimension() const;

  /** String converter */
  String __repr__() const override;

} ; /* class SamplingStrategy */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SAMPLINGSTRATEGY_HXX */
