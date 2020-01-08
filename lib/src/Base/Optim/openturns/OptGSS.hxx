//                                               -*- C++ -*-
/**
 *  @brief OPTpp solver implementation
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
#ifndef OPENTURNS_OPTGSS_HXX
#define OPENTURNS_OPTGSS_HXX

#include "openturns/OPTppImplementation.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OptGSS
 * OptGSS solver
 */
class OptGSS : public OPTppImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  OptGSS();

  /** Constructor with parameters */
  explicit OptGSS(const OptimizationProblem & problem);

  /** Virtual constructor */
  virtual OptGSS * clone() const;


protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  virtual void checkProblem(const OptimizationProblem & problem) const;

  /** Instanciate the solver */
  virtual Pointer<OPTPP::OptimizeClass> instanciateSolver();
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTGSS_HXX */
