//                                               -*- C++ -*-
/**
 *  @brief NLopt solver
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
#ifndef OPENTURNS_NLOPT_HXX
#define OPENTURNS_NLOPT_HXX

#include <map>
#include "openturns/OTprivate.hxx"
#include "openturns/OptimizationSolverImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NLopt
 * NLopt solver
 */
class OT_API NLopt :
  public OptimizationSolverImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  explicit NLopt(const UnsignedInteger algoType = 0);

  /** Constructor with parameters */
  explicit NLopt(const OptimizationProblem & problem,
                 const UnsignedInteger algoType = 0);

  /** Virtual constructor */
  virtual NLopt * clone() const;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run();

  /** NLopt algorithm names accessor */
  static Description GetAlgorithmNames();
  static String GetAlgorithmName(const UnsignedInteger code);

  /** NLopt algorithm codes */
  static Indices GetAlgorithmCodes();
  static UnsignedInteger GetAlgorithmCode(const String & name);

  /** Initialize the random generator seed */
  static void SetSeed(const UnsignedInteger seed);

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  virtual void checkProblem(const OptimizationProblem & problem) const;

  UnsignedInteger algoType_;

  /** Map of NLopt algorithms names */
  static std::map<String, UnsignedInteger> AlgorithmNames_;
  static Bool MustInitialize_;

  /** Method to initialize alrogithm names map */
  static void InitializeAlgorithmNames();

private:

  /** Compute the objective function of the optimization problem */
  static double ComputeObjective(const std::vector<double> & x, std::vector<double> & grad, void * f_data);

  /** Compute the inequality constraint of the optimization problem */
  static double ComputeInequalityConstraint(const std::vector<double> & x, std::vector<double> & grad, void * f_data);

  /** Compute the equality constraint of the optimization problem */
  static double ComputeEqualityConstraint(const std::vector<double> & x, std::vector<double> & grad, void * f_data);

  /// temporary, used to track input/outputs
  NumericalSample evaluationInputHistory_;
  NumericalSample evaluationOutputHistory_;

};


class OT_API BOBYQA : public NLopt
{
  CLASSNAME;
public:
  BOBYQA();
  virtual BOBYQA * clone() const;
  explicit BOBYQA(const OptimizationProblem & problem);
};

class OT_API CCSAQ : public NLopt
{
  CLASSNAME;
public:
  CCSAQ();
  virtual CCSAQ * clone() const;
  explicit CCSAQ(const OptimizationProblem & problem);
};

class OT_API COBYLANLOPT : public NLopt
{
  CLASSNAME;
public:
  COBYLANLOPT();
  virtual COBYLANLOPT * clone() const;
  explicit COBYLANLOPT(const OptimizationProblem & problem);
};

class OT_API LBFGS : public NLopt
{
  CLASSNAME;
public:
  LBFGS();
  virtual LBFGS * clone() const;
  explicit LBFGS(const OptimizationProblem & problem);
};

class OT_API MMA : public NLopt
{
  CLASSNAME;
public:
  MMA();
  virtual MMA * clone() const;
  explicit MMA(const OptimizationProblem & problem);
};

class OT_API NelderMead : public NLopt
{
  CLASSNAME;
public:
  NelderMead();
  virtual NelderMead * clone() const;
  explicit NelderMead(const OptimizationProblem & problem);
};

class OT_API SLSQP : public NLopt
{
  CLASSNAME;
public:
  SLSQP();
  virtual SLSQP * clone() const;
  explicit SLSQP(const OptimizationProblem & problem);
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NLOPT_HXX */
