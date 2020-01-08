//                                               -*- C++ -*-
/**
 *  @brief OPT++ solver implementation
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
#ifndef OPENTURNS_OPTPPIMPLEMENTATION_HXX
#define OPENTURNS_OPTPPIMPLEMENTATION_HXX

#include "openturns/OptimizationAlgorithmImplementation.hxx"

// necessary for NEWMAT includes
#define HAVE_NAMESPACES

#include <Opt.h>
#include <BoundConstraint.h>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OPTppImplementation
 * OPTpp solver implementation
 */
class OPTppImplementation : public OptimizationAlgorithmImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  OPTppImplementation();

  /** Constructor with parameters */
  explicit OPTppImplementation(const OptimizationProblem & problem);

  /** Virtual constructor */
  virtual OPTppImplementation * clone() const;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run();

  /** OPTppImplementation algorithm names accessor */
  static Description GetAlgorithmNames();

  /** Minimum step accessor */
  void setMinimumStep(const Scalar minimumStep);
  Scalar getMinimumStep() const;

  /** Maximum step accessor */
  void setMaximumStep(const Scalar maximumStep);
  Scalar getMaximumStep() const;

  /** Maximum line search tolerance accessor */
  void setLineSearchTolerance(const Scalar lineSearchTolerance);
  Scalar getLineSearchTolerance() const;

  /** Maximum line search iteration number accessor */
  void setMaximumLineSearchIterationNumber(const UnsignedInteger maximumLineSearchIterationNumber);
  UnsignedInteger getMaximumLineSearchIterationNumber() const;

  /** Trust region size accessor */
  void setTrustRegionSize(const Scalar trustRegionSize);
  Scalar getTrustRegionSize() const;

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

  /** Instanciate the solver */
  virtual Pointer<OPTPP::OptimizeClass> instanciateSolver();

  /** Map of OPTppImplementation algorithms names */
  static Description AlgorithmNames_;

  /** Method to initialize alrogithm names map */
  static void InitializeAlgorithmNames();

  /** Callback to initialize the problem */
  static void InitializeProblem(int ndim, NEWMAT::ColumnVector & x);

  /** Callback to compute the objective function of the optimization problem */
  static void ComputeObjective(int ndim, const NEWMAT::ColumnVector & x, double & fx, int & result, void* v);

  /** Callback to compute the objective function/gradient of the optimization problem */
  static void ComputeObjectiveGradient(int mode, int ndim, const NEWMAT::ColumnVector & x, double & fx, NEWMAT::ColumnVector & g, int & result, void* v);

  /** Callback to compute the objective function/gradient/hessian of the optimization problem */
  static void ComputeObjectiveGradientHessian(int mode, int ndim, const NEWMAT::ColumnVector & x, double & fx, NEWMAT::ColumnVector & g, NEWMAT::SymmetricMatrix & H, int & result, void* v);

  /** Callback to compute the inequality constraint of the optimization problem */
  static void ComputeInequalityConstraint(int mode, int ndim, const NEWMAT::ColumnVector & x, NEWMAT::ColumnVector & fx, NEWMAT::Matrix & g, OPTPP::OptppArray<NEWMAT::SymmetricMatrix> & H, int & result);

  /** Callback to compute the inequality constraint of the optimization problem */
  static void ComputeEqualityConstraint(int mode, int ndim, const NEWMAT::ColumnVector & x, NEWMAT::ColumnVector & fx, NEWMAT::Matrix & g, OPTPP::OptppArray<NEWMAT::SymmetricMatrix> & H, int & result);

  // Build the problem using optpp objects
  void addConstraints(OPTPP::NLP0 * nlp);
  OPTPP::BoundConstraint * buildBoundConstraints();
  OPTPP::NLP0* buildNLP0();
  OPTPP::NLP1* buildNLP1();
  OPTPP::NLP2* buildNLP2();

private:
  Scalar minimumStep_;
  Scalar maximumStep_;
  Scalar lineSearchTolerance_;
  Scalar maximumLineSearchIterationNumber_;
  Scalar trustRegionSize_;

  // temporary, used to track input/outputs
  Sample evaluationInputHistory_;
  Sample evaluationOutputHistory_;
  Sample equalityConstraintHistory_;
  Sample inequalityConstraintHistory_;

  // internal solver
  OPTPP::OptimizeClass * p_opt_;
  Pointer<OPTPP::NLPBase> p_nlp_;

  // there is no overload of the initialization callback available with void ptr
  // although there is one available for the evaluation
  // so we have to use a static variable to retrieve the current instance
  static OPTppImplementation * P_current_opt_;
};

#define DECLARE_OPTPP_SOLVER_IMPLEMENTATION(CName) \
class OT_API CName : public OPTppImplementation \
{ \
  CLASSNAME \
public: \
  CName (); \
  explicit CName (const OptimizationProblem & problem); \
  CName * clone() const; \
protected: \
  virtual void checkProblem(const OptimizationProblem & problem) const; \
  virtual Pointer<OPTPP::OptimizeClass> instanciateSolver(); \
};


DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptBaNewton)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptBaQNewton)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptBCEllipsoid)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptBCFDNewton)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptBCNewton)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptBCQNewton)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptCG)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptConstrFDNewton)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptConstrNewton)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptConstrQNewton)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptDHNIPS)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptFDNewton)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptFDNIPS)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptLBFGS)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptNewton)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptNIPS)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptPDS)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptQNewton)
DECLARE_OPTPP_SOLVER_IMPLEMENTATION(OptQNIPS)

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTPPIMPLEMENTATION_HXX */
