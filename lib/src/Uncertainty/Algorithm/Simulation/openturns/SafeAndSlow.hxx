//                                               -*- C++ -*-
/**
 *  @brief Find the roots in a given direction according to the Proban
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SAFEANDSLOW_HXX
#define OPENTURNS_SAFEANDSLOW_HXX

#include "openturns/RootStrategyImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SafeAndSlow
 */

class OT_API SafeAndSlow :
  public RootStrategyImplementation
{

  CLASSNAME
public:


  /** Default constructor */
  SafeAndSlow();

  /** Constructor with parameters */
  explicit SafeAndSlow(const Solver & solver);

  /** Constructor with parameters */
  SafeAndSlow(const Solver & solver,
              const Scalar maximumDistance,
              const Scalar stepSize);


  /** Virtual constructor */
  SafeAndSlow * clone() const override;

  /** Solve gives all the roots found applying the root strategy */
  ScalarCollection solve(const Function & function,
                         const Scalar value) override;

  /** String converter */
  String __repr__() const override;

private:

} ; /* class SafeAndSlow */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SAFEANDSLOW_HXX */
