//                                               -*- C++ -*-
/**
 *  @brief Cobyla is an actual implementation for OptimizationSolverImplementation using the cobyla library
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_COBYLA_HXX
#define OPENTURNS_COBYLA_HXX

#include "OTprivate.hxx"
#include "OptimizationSolverImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class cobyla
 * cobyla is an actual implementation for OptimizationSolverImplementation using the cobyla library
 */
class OT_API Cobyla :
  public OptimizationSolverImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  Cobyla();

  /** Constructor with parameters */
  explicit Cobyla(const OptimizationProblem & problem);

  /** Constructor with parameters */
  Cobyla(const OptimizationProblem & problem,
         const NumericalScalar rhoBeg);

  /** Virtual constructor */
  virtual Cobyla * clone() const;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run();

  /** RhoBeg accessor */
  NumericalScalar getRhoBeg() const;
  void setRhoBeg(const NumericalScalar rhoBeg);

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

  /**
   * Function that computes half of the squared norm of a Vector. It will
   * become the objective function of the minimisation problem
   */
  static int ComputeObjectiveAndConstraint(int n,
      int m,
      double *x,
      double *f,
      double *con,
      void *state);

  NumericalScalar rhoBeg_;

  /// temporary, used to track input/outputs
  NumericalSample evaluationInputHistory_;
  NumericalSample evaluationOutputHistory_;

}; /* class Cobyla */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COBYLA_HXX */
