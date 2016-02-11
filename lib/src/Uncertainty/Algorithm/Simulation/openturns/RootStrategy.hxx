//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_ROOTSTRATEGY_HXX
#define OPENTURNS_ROOTSTRATEGY_HXX

#include "openturns/RootStrategyImplementation.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class RootStrategy
 */

class OT_API RootStrategy
  : public TypedInterfaceObject<RootStrategyImplementation>
{
  CLASSNAME;

public:
  typedef Pointer<RootStrategyImplementation>     Implementation;
  typedef Collection<NumericalScalar> NumericalScalarCollection;

  /** Constructor with parameters */
  RootStrategy(const RootStrategyImplementation & implementation);

  /** Constructor with parameters */
  RootStrategy(Implementation & p_implementation);

  /** Default Constructor: choose the SafeAndSlow strategy with its default solver */
  RootStrategy();


  /** Solve gives all the roots found applying the root strategy */
  NumericalScalarCollection solve(const NumericalMathFunction & function,
                                  const NumericalScalar value);

  /** Solver accessor */
  void setSolver(const Solver & solver);
  Solver getSolver() const;

  /** Maximum distance accessor */
  void setMaximumDistance(const NumericalScalar maximumDistance);
  NumericalScalar getMaximumDistance() const;

  /** Step size accessor */
  void setStepSize(const NumericalScalar stepSize);
  NumericalScalar getStepSize() const;

  /** Value of the performance function at the origin accessor */
  void setOriginValue(const NumericalScalar originValue);
  NumericalScalar getOriginValue() const;

  /** String converter */
  String __repr__() const;

} ; /* class RootStrategy */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ROOTSTRATEGY_HXX */
