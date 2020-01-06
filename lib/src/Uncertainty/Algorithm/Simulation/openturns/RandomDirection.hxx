//                                               -*- C++ -*-
/**
 *  @brief
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
#ifndef OPENTURNS_RANDOMDIRECTION_HXX
#define OPENTURNS_RANDOMDIRECTION_HXX

#include "openturns/SamplingStrategyImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class RandomDirection
 */

class OT_API RandomDirection :
  public SamplingStrategyImplementation
{

  CLASSNAME

public:


  /** Default constructor */
  RandomDirection();

  /** Constructor with parameters */
  explicit RandomDirection(const UnsignedInteger dimension);


  /** Virtual constructor */
  virtual RandomDirection * clone() const;

  /** Generate a set of directions */
  Sample generate() const;

  /** String converter */
  String __repr__() const;

} ; /* class RandomDirection */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMDIRECTION_HXX */
