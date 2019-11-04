//                                               -*- C++ -*-
/**
 *  @brief Find the roots in a given direction according to the Proban
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_RISKYANDFAST_HXX
#define OPENTURNS_RISKYANDFAST_HXX

#include "openturns/RootStrategyImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class RiskyAndFast
 */

class OT_API RiskyAndFast :
  public RootStrategyImplementation
{

  CLASSNAME
public:


  /** Default constructor */
  RiskyAndFast();

  /** Constructor with parameters */
  explicit RiskyAndFast(const Solver & solver);

  /** Constructor with parameters */
  RiskyAndFast(const Solver & solver,
               const Scalar maximumDistance);


  /** Virtual constructor */
  virtual RiskyAndFast * clone() const;

  /** Solve gives all the roots found applying the root strategy */
  ScalarCollection solve(const Function & function,
                         const Scalar value);

  /** String converter */
  String __repr__() const;

} ; /* class RiskyAndFast */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RISKYANDFAST_HXX */
