//                                               -*- C++ -*-
/**
 *  @brief OptGSS solver
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OptGSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

// OptPDS and OptGSS cannot both be included as OptDirect is redefined
#include <OptGSS.h>


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptGSS)

static const Factory<OptGSS> Factory_OptGSS;

/* Default constructor */
OptGSS::OptGSS()
  : OPTppImplementation()
{
}

OptGSS::OptGSS(const OptimizationProblem & problem)
  : OPTppImplementation(problem)
{
  checkProblem(problem);
}

/* Virtual constructor */
OptGSS * OptGSS::clone() const
{
  return new OptGSS(*this);
}

/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void OptGSS::checkProblem(const OptimizationProblem & problem) const
{
  if (problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << getClassName() << " does not support multi-objective optimization";
  if (problem.hasResidualFunction())
    throw InvalidArgumentException(HERE) << getClassName() << " does not support least-square problems";
  if (problem.hasBounds())
    throw InvalidArgumentException(HERE) << getClassName() << " does not support bound constraints";
  if (problem.hasInequalityConstraint())
    throw InvalidArgumentException(HERE) << getClassName() << " does not support inequality constraints";
  if (problem.hasEqualityConstraint())
    throw InvalidArgumentException(HERE) << getClassName() << " does not support equality constraints";
}


Pointer<OPTPP::OptimizeClass> OptGSS::instanciateSolver()
{
  const UnsignedInteger dimension = getProblem().getDimension();
  OPTPP::GenSetStd * genSet = new OPTPP::GenSetStd(dimension);
  OPTPP::OptGSS * solver = new OPTPP::OptGSS(buildNLP1(), genSet);
  // OptGSS has its own set of properties :[
  solver->setMaxIter(getMaximumIterationNumber());
  return solver;
}

END_NAMESPACE_OPENTURNS

