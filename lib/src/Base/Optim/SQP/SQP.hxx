//                                               -*- C++ -*-
/**
 *  @brief SQP is an actual implementation for
 *         OptimizationSolver using the SQP algorithm.
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
#ifndef OPENTURNS_SQP_HXX
#define OPENTURNS_SQP_HXX

#include "OTprivate.hxx"
#include "SQPSpecificParameters.hxx"
#include "OptimizationSolverImplementation.hxx"
#include "NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SQP
 * SQP is an actual implementation for
 * OptimizationSolver
 */

class OT_API SQP :
  public OptimizationSolverImplementation
{
  CLASSNAME;

public:


  /** Default constructor */
  SQP();

  /** Constructor with parameters */
  explicit SQP(const OptimizationProblem & problem);

  /** Constructor with parameters */
  SQP(const OptimizationProblem & problem,
      const NumericalScalar tau,
      const NumericalScalar omega,
      const NumericalScalar smooth);

  SQP(const SQPSpecificParameters & specificParameters,
      const OptimizationProblem & problem);

  /** Virtual constructor */
  virtual SQP * clone() const;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run();

  /** Tau accessor */
  NumericalScalar getTau() const;
  void setTau(const NumericalScalar tau);

  /** Omega accessor */
  NumericalScalar getOmega() const;
  void setOmega(const NumericalScalar tau);

  /** Smooth accessor */
  NumericalScalar getSmooth() const;
  void setSmooth(const NumericalScalar tau);

  /** Specific parameters accessor */
  SQPSpecificParameters getSpecificParameters() const;

  /** Specific parameters accessor */
  void setSpecificParameters(const SQPSpecificParameters & specificParameters);

  /** Level function accessor */
  NumericalMathFunction getLevelFunction() const;

  /** Level function accessor */
  void setLevelFunction(const NumericalMathFunction & levelFunction);

  /** Level value accessor */
  NumericalScalar getLevelValue() const;

  /** Level value accessor */
  void setLevelValue(const NumericalScalar levelValue);

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  void checkProblem(const OptimizationProblem & problem) const;

private:

  /** Perform a line search in the given direction */
  NumericalScalar computeLineSearch();

  /** Initialize internal state */
  void initialize();

  /** Multiplicative decrease of linear step */
  NumericalScalar tau_;

  /** Armijo factor */
  NumericalScalar omega_;

  /** Growing factor in penalization term */
  NumericalScalar smooth_;

  /** SQP current penalization factor */
  NumericalScalar currentSigma_;

  /** SQP current point */
  NumericalPoint currentPoint_;

  /** SQP current direction */
  NumericalPoint currentDirection_;

  /** SQP current level value */
  NumericalScalar currentLevelValue_;

  /** SQP current gradient as a column vector */
  NumericalPoint currentGradient_;

  /** SQP current hessian as a symmetric tensor */
  SymmetricMatrix currentHessian_;

  /** SQP current system matrix as a symmetric matrix */
  SymmetricMatrix currentSystemMatrix_;

  /** SQP current system second member as a column vector */
  NumericalPoint currentSecondMember_;

  /** SQP current Lagrange multiplier */
  NumericalScalar currentLambda_;

}; /* class SQP */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SQP_HXX */
