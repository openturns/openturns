//                                               -*- C++ -*-
/**
 *  @brief NLopt solver
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
#ifndef OPENTURNS_NLOPT_HXX
#define OPENTURNS_NLOPT_HXX

#include <map>
#include "openturns/OTprivate.hxx"
#include "openturns/OptimizationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NLopt
 * NLopt solver
 */
class OT_API NLopt :
  public OptimizationAlgorithmImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  explicit NLopt(const String & algoName = "LD_SLSQP");

  /** Constructor with parameters */
  explicit NLopt(const OptimizationProblem & problem,
                 const String & algoName = "LD_SLSQP");

  /** Virtual constructor */
  virtual NLopt * clone() const;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run();

  /** NLopt algorithm names accessor */
  static Description GetAlgorithmNames();

  void setAlgorithmName(const String algoName);
  String getAlgorithmName() const;

  /** Initial derivative-free local-optimization algorithms step accessor */
  void setInitialStep(const Point & initialStep);
  Point getInitialStep() const;

  /** Local optimizer */
  void setLocalSolver(const NLopt & localSolver);
  NLopt getLocalSolver() const;

  /** Initialize the random generator seed */
  static void SetSeed(const UnsignedInteger seed);

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  static Bool IsAvailable();

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  virtual void checkProblem(const OptimizationProblem & problem) const;

  String algoName_;

  /** Initial derivative-free local-optimization algorithms step */
  Point initialStep_;

  /** Auxiliary solver */
  Pointer<NLopt> p_localSolver_;

  /** Map of NLopt algorithms names */
  static std::map<String, UnsignedInteger> AlgorithmNames_;

  /** Method to initialize alrogithm names map */
  static void InitializeAlgorithmNames();

private:
  /** NLopt algorithm code */
  static UnsignedInteger GetAlgorithmCode(const String & name);

  /** Compute the objective function of the optimization problem */
  static double ComputeObjective(const std::vector<double> & x, std::vector<double> & grad, void * f_data);

  /** Compute the inequality constraint of the optimization problem */
  static void ComputeInequalityConstraint(unsigned m, double * result, unsigned n, const double * x, double * grad, void * f_data);

  /** Compute the equality constraint of the optimization problem */
  static void ComputeEqualityConstraint(unsigned m, double * result, unsigned n, const double * x, double * grad, void * f_data);

  /// temporary, used to track input/outputs
  Sample evaluationInputHistory_;
  Sample evaluationOutputHistory_;
  Sample equalityConstraintHistory_;
  Sample inequalityConstraintHistory_;

  // internal solver
  void * p_opt_;
};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NLOPT_HXX */
