//                                               -*- C++ -*-
/**
 *  @brief Bonmin allows one to describe a MINLP optimization algorithm
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
#ifndef OPENTURNS_BONMIN_HXX
#define OPENTURNS_BONMIN_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/OptimizationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Bonmin
 * Bonmin allows one to solve a MINLP optimization problem, including:
 * + objective functions of continuous, discrete or binary variables
 * + multiple equality, inequality and bounds constraints
 */

class OT_API Bonmin
  : public OptimizationAlgorithmImplementation
{
  CLASSNAME

public:

  /** Constructors */
  explicit Bonmin(const String & algoName = "B-BB");

  explicit Bonmin( OptimizationProblem & problem,
                   const String & algoName = "B-BB");

  /** Virtual constructor */
  Bonmin * clone() const override;

  /** Bonmin static methods */
  static Description GetAlgorithmNames(); // List available bonmin algorithm names

  /** Accessors to Bonmin attributes */
  void setAlgorithmName(const String & algoName);
  String getAlgorithmName() const;

  /** Performing the actual computation. */
  void run() override;

  /** Description of object */
  String __str__(const String & offset = "") const override;
  String __repr__() const override;

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  void checkProblem(const OptimizationProblem & problem) const override;

private:
  String algoName_;

};

END_NAMESPACE_OPENTURNS

#endif
