//                                               -*- C++ -*-
/**
 *  @brief SQP is an actual implementation for
 *         OptimizationAlgorithm using the SQP algorithm.
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SQP_HXX
#define OPENTURNS_SQP_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/OptimizationAlgorithmImplementation.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SQP
 * SQP is an actual implementation for
 * OptimizationAlgorithm
 */

class OT_API SQP :
  public OptimizationAlgorithmImplementation
{
  CLASSNAME

public:


  /** Default constructor */
  SQP();

  /** Constructor with parameters */
  explicit SQP(const OptimizationProblem & problem);

  /** Constructor with parameters */
  SQP(const OptimizationProblem & problem,
      const Scalar tau,
      const Scalar omega,
      const Scalar smooth);

  /** Virtual constructor */
  SQP * clone() const override;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run() override;

  /** Tau accessor */
  Scalar getTau() const;
  void setTau(const Scalar tau);

  /** Omega accessor */
  Scalar getOmega() const;
  void setOmega(const Scalar tau);

  /** Smooth accessor */
  Scalar getSmooth() const;
  void setSmooth(const Scalar tau);

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  void checkProblem(const OptimizationProblem & problem) const override;

private:

  /** Perform a line search in the given direction */
  Scalar computeLineSearch();

  /** Initialize internal state */
  void initialize();

  /** Multiplicative decrease of linear step */
  Scalar tau_;

  /** Armijo factor */
  Scalar omega_;

  /** Growing factor in penalization term */
  Scalar smooth_;

  /** SQP current penalization factor */
  Scalar currentSigma_;

  /** SQP current point */
  Point currentPoint_;

  /** SQP current direction */
  Point currentDirection_;

  /** SQP current level value */
  Scalar currentLevelValue_;

  /** SQP current gradient as a column vector */
  Point currentGradient_;

  /** SQP current hessian as a symmetric tensor */
  SymmetricMatrix currentHessian_;

  /** SQP current system matrix as a symmetric matrix */
  SymmetricMatrix currentSystemMatrix_;

  /** SQP current system second member as a column vector */
  Point currentSecondMember_;

  /** SQP current Lagrange multiplier */
  Scalar currentLambda_;

}; /* class SQP */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SQP_HXX */
