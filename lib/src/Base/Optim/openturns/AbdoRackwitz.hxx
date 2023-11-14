//                                               -*- C++ -*-
/**
 *  @brief AbdoRackwitz is an actual implementation for
 *         OptimizationAlgorithm using the AbdoRackwitz algorithm.
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
#ifndef OPENTURNS_ABDORACKWITZ_HXX
#define OPENTURNS_ABDORACKWITZ_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/OptimizationAlgorithmImplementation.hxx"
#include "openturns/OptimizationProblem.hxx"
#include "openturns/Point.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class AbdoRackwitz
 * AbdoRackwitz is an actual implementation for
 * OptimizationAlgorithm
 */
class OT_API AbdoRackwitz :
  public OptimizationAlgorithmImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  AbdoRackwitz();

  /** Constructor with parameters */
  explicit AbdoRackwitz(const OptimizationProblem & problem);

  /** Constructor with parameters */
  AbdoRackwitz(const OptimizationProblem & problem,
               const Scalar tau,
               const Scalar omega,
               const Scalar smooth);

  /** Virtual constructor */
  AbdoRackwitz * clone() const override;

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

  /** Abdo Rackwitz current penalization factor */
  Scalar currentSigma_;

  /** Abdo Rackwitz current point */
  Point currentPoint_;

  /** Abdo Rackwitz current direction */
  Point currentDirection_;

  /** Abdo Rackwitz current level value */
  Scalar currentLevelValue_;

  /** Abdo Rackwitz current gradient as a column vector */
  Point currentGradient_;

  /** Abdo Rackwitz current Lagrange multiplier */
  Scalar currentLambda_;

}; /* class AbdoRackwitz */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ABDORACKWITZ_HXX */
