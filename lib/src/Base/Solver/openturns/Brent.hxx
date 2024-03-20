//                                               -*- C++ -*-
/**
 *  @brief Implementation class of the scalar nonlinear solver based on
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
#ifndef OPENTURNS_BRENT_HXX
#define OPENTURNS_BRENT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/SolverImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class Brent
 *
 * This class is an interface for the 1D nonlinear solverImplementations
 */
class OT_API Brent : public SolverImplementation
{
  CLASSNAME
public:

  /* Parameter constructor */
  explicit Brent(const Scalar absoluteError = ResourceMap::GetAsScalar("Solver-DefaultAbsoluteError"),
                 const Scalar relativeError = ResourceMap::GetAsScalar("Solver-DefaultRelativeError"),
                 const Scalar residualError = ResourceMap::GetAsScalar("Solver-DefaultResidualError"),
                 const UnsignedInteger maximumFunctionEvaluation = ResourceMap::GetAsUnsignedInteger("Solver-DefaultMaximumFunctionEvaluation"));


  /* Copy constructor */

  /* Virtual constructor */
  Brent * clone() const override;

  /* String converter */
  String __repr__() const override;

  /* Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] given function(infPoint) and function(supPoint) with the Brent method */
  using SolverImplementation::solve;
  Scalar solve(const UniVariateFunction & function,
               const Scalar value,
               const Scalar infPoint,
               const Scalar supPoint,
               const Scalar infValue,
               const Scalar supValue) const override;

private:

}; /* Class Brent */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BRENT_HXX */
