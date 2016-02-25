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
#include "openturns/NLopt.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#ifdef OPENTURNS_HAVE_NLOPT
# include "nlopt.hpp"
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NLopt);

static const Factory<NLopt> RegisteredFactory;

/* Default constructor */
NLopt::NLopt(const UnsignedInteger algoType)
  : OptimizationSolverImplementation()
  , algoType_(algoType)
{
  // Nothing to do
}

NLopt::NLopt(const OptimizationProblem & problem,
             const UnsignedInteger algoType)
  : OptimizationSolverImplementation(problem)
  , algoType_(algoType)
{
  checkProblem(problem);
}

/* Virtual constructor */
NLopt * NLopt::clone() const
{
  return new NLopt(*this);
}

/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void NLopt::checkProblem(const OptimizationProblem & problem) const
{
#ifdef OPENTURNS_HAVE_NLOPT
  if (problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support multi-objective optimization";
  if (problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support level-function optimization";

  const UnsignedInteger dimension = getProblem().getDimension();
  const nlopt::algorithm algo = static_cast<nlopt::algorithm>(algoType_);
  nlopt::opt opt(algo, dimension);

  if (problem.hasInequalityConstraint())
  {
    try {
      opt.add_inequality_constraint(NLopt::ComputeInequalityConstraint, 0);
    } catch (std::invalid_argument) {
      throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support inequality constraints";
    }
  }

  if (problem.hasEqualityConstraint())
  {
    try {
      opt.add_equality_constraint(NLopt::ComputeEqualityConstraint, 0);
    } catch (std::invalid_argument) {
      throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support equality constraints";
    }
  }
#else
  throw NotYetImplementedException(HERE) << "No NLopt support";
#endif
}

// Struct to store class ptr and marginal index as well
struct MarginalData {
  MarginalData(NLopt * p_algo, const UnsignedInteger marginalIndex)
  : p_algo_(p_algo), marginalIndex_(marginalIndex) {}
  NLopt * p_algo_;
  int marginalIndex_;
};

/* Performs the actual computation by calling the NLopt library
 */
void NLopt::run()
{
#ifdef OPENTURNS_HAVE_NLOPT
  const UnsignedInteger dimension = getProblem().getDimension();
  const nlopt::algorithm algo = static_cast<nlopt::algorithm>(algoType_);

  nlopt::opt opt(algo, dimension);

  if (getProblem().isMinimization()) {
    opt.set_min_objective(NLopt::ComputeObjective, this);
  }
  else {
    opt.set_max_objective(NLopt::ComputeObjective, this);
  }

  opt.set_xtol_abs(getMaximumAbsoluteError());
  opt.set_xtol_rel(getMaximumRelativeError());
  opt.set_ftol_abs(getMaximumResidualError());
  opt.set_maxeval(getMaximumEvaluationNumber());

  if (getProblem().hasBounds())
  {
    Interval::BoolCollection finiteLowerBound(getProblem().getBounds().getFiniteLowerBound());
    Interval::BoolCollection finiteUpperBound(getProblem().getBounds().getFiniteUpperBound());
    NumericalPoint lowerBound(getProblem().getBounds().getLowerBound());
    NumericalPoint upperBound(getProblem().getBounds().getUpperBound());
    std::vector<double> lb(dimension, 0.0);
    std::vector<double> ub(dimension, 0.0);
    std::copy(lowerBound.begin(), lowerBound.end(), lb.begin());
    std::copy(upperBound.begin(), upperBound.end(), ub.begin());
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      if (!finiteLowerBound[i]) lb[i] = -HUGE_VAL;
      if (!finiteUpperBound[i]) ub[i] = HUGE_VAL;
    }
    opt.set_lower_bounds(lb);
    opt.set_upper_bounds(ub);
  }

  Collection<Pointer<MarginalData> > inequalityData;
  if (getProblem().hasInequalityConstraint())
  {
    const UnsignedInteger inequalityDimension = getProblem().getInequalityConstraint().getOutputDimension();
    inequalityData.resize(inequalityDimension);
    for (UnsignedInteger i = 0; i < inequalityDimension; ++ i)
    {
      inequalityData[i] = Pointer<MarginalData>(new MarginalData(this, i));
      opt.add_inequality_constraint(NLopt::ComputeInequalityConstraint, inequalityData[i].get(), 1e-8);
    }
  }

  Collection<Pointer<MarginalData> > equalityData;
  if (getProblem().hasEqualityConstraint())
  {
    const UnsignedInteger equalityDimension = getProblem().getEqualityConstraint().getOutputDimension();
    equalityData.resize(equalityDimension);
    for (UnsignedInteger i = 0; i < equalityDimension; ++ i)
    {
      equalityData[i] = Pointer<MarginalData>(new MarginalData(this, i));
      opt.add_equality_constraint(NLopt::ComputeEqualityConstraint, equalityData[i].get(), 1e-8);
    }
  }

  std::vector<double> x(dimension, 0.0);
  NumericalPoint startingPoint(getStartingPoint());
  if (startingPoint.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Invalid starting point dimension, expected " << dimension;
  std::copy(startingPoint.begin(), startingPoint.end(), x.begin());
  double optimalValue = 0.0;
  nlopt::result err = opt.optimize(x, optimalValue);
  if (err < 0) throw InternalException(HERE) << "NLopt returned error code " << err;

  NumericalPoint optimizer(dimension);
  std::copy(x.begin(), x.end(), optimizer.begin());
  setResult(OptimizationResult(optimizer, NumericalPoint(1, optimalValue), 0, 0., 0., 0., 0.));
#else
  throw NotYetImplementedException(HERE) << "No NLopt support";
#endif
}

/* String converter */
String NLopt::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " " << OptimizationSolverImplementation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void NLopt::save(Advocate & adv) const
{
  OptimizationSolverImplementation::save(adv);
  adv.saveAttribute("algoType_", algoType_);
}

/* Method load() reloads the object from the StorageManager */
void NLopt::load(Advocate & adv)
{
  OptimizationSolverImplementation::load(adv);
  adv.loadAttribute("algoType_", algoType_);
}

double NLopt::ComputeObjective(const std::vector<double> & x, std::vector<double> & grad, void * f_data)
{
  NLopt *algorithm = static_cast<NLopt *>(f_data);
  const UnsignedInteger dimension = algorithm->getProblem().getDimension();
  NumericalPoint inP(dimension);
  std::copy(x.begin(), x.end(), inP.begin());

  // evaluation
  NumericalPoint outP(algorithm->getProblem().getObjective()(inP));

  // gradient
  if (!grad.empty())
  {
    Matrix gradient(algorithm->getProblem().getObjective().gradient(inP));
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      grad[i] = gradient(i, 0);
    }
  }
  return outP[0];
}

double NLopt::ComputeInequalityConstraint(const std::vector< double >& x, std::vector< double >& grad, void* f_data)
{
  MarginalData * mData = static_cast<MarginalData *>(f_data);
  NLopt *algorithm = mData->p_algo_;
  const UnsignedInteger marginalIndex = mData->marginalIndex_;
  const UnsignedInteger dimension = algorithm->getProblem().getDimension();
  NumericalPoint inP(dimension);
  std::copy(x.begin(), x.end(), inP.begin());

  // evaluation
  NumericalPoint outP(algorithm->getProblem().getInequalityConstraint()(inP));

  // gradient
  if (!grad.empty())
  {
    Matrix gradient(algorithm->getProblem().getInequalityConstraint().gradient(inP));
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      // nlopt solves h(x)<=0
      grad[i] = -gradient(i, marginalIndex);
    }
  }
  // nlopt solves h(x)<=0
  return -outP[marginalIndex];
}


double NLopt::ComputeEqualityConstraint(const std::vector< double >& x, std::vector< double >& grad, void* f_data)
{
  MarginalData * mData = static_cast<MarginalData *>(f_data);
  NLopt *algorithm = mData->p_algo_;
  const UnsignedInteger marginalIndex = mData->marginalIndex_;
  const UnsignedInteger dimension = algorithm->getProblem().getDimension();
  NumericalPoint inP(dimension);
  std::copy(x.begin(), x.end(), inP.begin());

  // evaluation
  NumericalPoint outP(algorithm->getProblem().getEqualityConstraint()(inP));

  // gradient
  if (!grad.empty())
  {
    Matrix gradient(algorithm->getProblem().getEqualityConstraint().gradient(inP));
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      grad[i] = gradient(i, marginalIndex);
    }
  }
  return outP[marginalIndex];
}


CLASSNAMEINIT(SLSQP);
static const Factory<SLSQP> RegisteredFactory2;
SLSQP::SLSQP() : NLopt(
#ifdef OPENTURNS_HAVE_NLOPT
nlopt::LD_SLSQP
#endif
) {}
SLSQP::SLSQP(const OptimizationProblem & problem) : NLopt(problem
#ifdef OPENTURNS_HAVE_NLOPT
, nlopt::LD_SLSQP
#endif
) {}
SLSQP * SLSQP::clone() const { return new SLSQP(*this); }


CLASSNAMEINIT(LBFGS);
static const Factory<LBFGS> RegisteredFactory3;
LBFGS::LBFGS() : NLopt(
#ifdef OPENTURNS_HAVE_NLOPT
nlopt::LD_LBFGS
#endif
) {}
LBFGS::LBFGS(const OptimizationProblem & problem) : NLopt(problem
#ifdef OPENTURNS_HAVE_NLOPT
, nlopt::LD_LBFGS
#endif
) {}
LBFGS * LBFGS::clone() const { return new LBFGS(*this); }


CLASSNAMEINIT(NelderMead);
static const Factory<NelderMead> RegisteredFactory4;
NelderMead::NelderMead() : NLopt(
#ifdef OPENTURNS_HAVE_NLOPT
nlopt::LN_NELDERMEAD
#endif
) {}
NelderMead::NelderMead(const OptimizationProblem & problem) : NLopt(problem
#ifdef OPENTURNS_HAVE_NLOPT
, nlopt::LN_NELDERMEAD
#endif
) {}
NelderMead * NelderMead::clone() const { return new NelderMead(*this); }


END_NAMESPACE_OPENTURNS
