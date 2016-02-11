//                                               -*- C++ -*-
/**
 *  @brief Find the roots in a given direction according to the Proban
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
#ifndef OPENTURNS_MEDIUMSAFE_HXX
#define OPENTURNS_MEDIUMSAFE_HXX

#include "RootStrategyImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class MediumSafe
 */

class OT_API MediumSafe :
  public RootStrategyImplementation
{

  CLASSNAME;
public:


  /** Default constructor */
  MediumSafe();

  /** Constructor with parameters */
  explicit MediumSafe(const Solver & solver);

  /** Constructor with parameters */
  MediumSafe(const Solver & solver,
             const NumericalScalar maximumDistance,
             const NumericalScalar stepSize);


  /** Virtual constructor */
  virtual MediumSafe * clone() const;

  /** Solve gives all the roots found applying the root strategy */
  NumericalScalarCollection solve(const NumericalMathFunction & function,
                                  const NumericalScalar value);

  /** String converter */
  String __repr__() const;

} ; /* class MediumSafe */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MEDIUMSAFE_HXX */
