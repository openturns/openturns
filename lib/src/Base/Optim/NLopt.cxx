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
# include <nlopt.hpp>
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NLopt);

static const Factory<NLopt> Factory_NLopt;

/* Map to associate algorithm names and codes */
std::map<String, UnsignedInteger> NLopt::AlgorithmNames_;

/* Static method to initialize the algorithm names/codes pairing in nlopt */
void NLopt::InitializeAlgorithmNames()
{
#ifdef OPENTURNS_HAVE_NLOPT
  if (!AlgorithmNames_.empty()) return;
  AlgorithmNames_["GN_DIRECT"] = nlopt::GN_DIRECT;
  AlgorithmNames_["GN_DIRECT_L"] = nlopt::GN_DIRECT_L;
  AlgorithmNames_["GN_DIRECT_L_RAND"] = nlopt::GN_DIRECT_L_RAND;
  AlgorithmNames_["GN_DIRECT_NOSCAL"] = nlopt::GN_DIRECT_NOSCAL;
  AlgorithmNames_["GN_DIRECT_L_NOSCAL"] = nlopt::GN_DIRECT_L_NOSCAL;
  AlgorithmNames_["GN_DIRECT_L_RAND_NOSCAL"] = nlopt::GN_DIRECT_L_RAND_NOSCAL;
  AlgorithmNames_["GN_ORIG_DIRECT"] = nlopt::GN_ORIG_DIRECT;
  AlgorithmNames_["GN_ORIG_DIRECT_L"] = nlopt::GN_ORIG_DIRECT_L;
  AlgorithmNames_["GD_STOGO"] = nlopt::GD_STOGO;
  AlgorithmNames_["GD_STOGO_RAND"] = nlopt::GD_STOGO_RAND;
  AlgorithmNames_["LD_LBFGS_NOCEDAL"] = nlopt::LD_LBFGS_NOCEDAL;
  AlgorithmNames_["LD_LBFGS"] = nlopt::LD_LBFGS;
  AlgorithmNames_["LN_PRAXIS"] = nlopt::LN_PRAXIS;
  AlgorithmNames_["LD_VAR1"] = nlopt::LD_VAR1;
  AlgorithmNames_["LD_VAR2"] = nlopt::LD_VAR2;
  AlgorithmNames_["LD_TNEWTON"] = nlopt::LD_TNEWTON;
  AlgorithmNames_["LD_TNEWTON_RESTART"] = nlopt::LD_TNEWTON_RESTART;
  AlgorithmNames_["LD_TNEWTON_PRECOND"] = nlopt::LD_TNEWTON_PRECOND;
  AlgorithmNames_["LD_TNEWTON_PRECOND_RESTART"] = nlopt::LD_TNEWTON_PRECOND_RESTART;
  AlgorithmNames_["GN_CRS2_LM"] = nlopt::GN_CRS2_LM;
  AlgorithmNames_["GN_MLSL"] = nlopt::GN_MLSL;
  AlgorithmNames_["GD_MLSL"] = nlopt::GD_MLSL;
  AlgorithmNames_["GN_MLSL_LDS"] = nlopt::GN_MLSL_LDS;
  AlgorithmNames_["GD_MLSL_LDS"] = nlopt::GD_MLSL_LDS;
  AlgorithmNames_["LD_MMA"] = nlopt::LD_MMA;
  AlgorithmNames_["LN_COBYLA"] = nlopt::LN_COBYLA;
  AlgorithmNames_["LN_NEWUOA"] = nlopt::LN_NEWUOA;
  AlgorithmNames_["LN_NEWUOA_BOUND"] = nlopt::LN_NEWUOA_BOUND;
  AlgorithmNames_["LN_NELDERMEAD"] = nlopt::LN_NELDERMEAD;
  AlgorithmNames_["LN_SBPLX"] = nlopt::LN_SBPLX;
  AlgorithmNames_["LN_AUGLAG"] = nlopt::LN_AUGLAG;
  AlgorithmNames_["LD_AUGLAG"] = nlopt::LD_AUGLAG;
  AlgorithmNames_["LN_AUGLAG_EQ"] = nlopt::LN_AUGLAG_EQ;
  AlgorithmNames_["LD_AUGLAG_EQ"] = nlopt::LD_AUGLAG_EQ;
  AlgorithmNames_["LN_BOBYQA"] = nlopt::LN_BOBYQA;
  AlgorithmNames_["GN_ISRES"] = nlopt::GN_ISRES;
  AlgorithmNames_["AUGLAG"] = nlopt::AUGLAG;
  AlgorithmNames_["AUGLAG_EQ"] = nlopt::AUGLAG_EQ;
  AlgorithmNames_["G_MLSL"] = nlopt::G_MLSL;
  AlgorithmNames_["G_MLSL_LDS"] = nlopt::G_MLSL_LDS;
  AlgorithmNames_["LD_SLSQP"] = nlopt::LD_SLSQP;
  AlgorithmNames_["LD_CCSAQ"] = nlopt::LD_CCSAQ;
  AlgorithmNames_["GN_ESCH"] = nlopt::GN_ESCH;
#else
  throw NotYetImplementedException(HERE) << "No NLopt support";
#endif
}

/* Static methods to access algorithm names and codes */
Description NLopt::GetAlgorithmNames()
{
  InitializeAlgorithmNames();
  Description names;
  std::map<String, UnsignedInteger>::const_iterator it;
  for (it = AlgorithmNames_.begin(); it != AlgorithmNames_.end(); ++it) names.add(it->first);
  return names;
}

/* Static methods to access algorithm names and codes */
UnsignedInteger NLopt::GetAlgorithmCode(const String & name)
{
  InitializeAlgorithmNames();
  const std::map<String, UnsignedInteger>::const_iterator it(AlgorithmNames_.find(name));
  if (it == AlgorithmNames_.end()) throw InvalidArgumentException(HERE) << "Error: the given NLopt algorithm name=" << name << " is unknown.";
  return it->second;
}


void NLopt::SetSeed(const UnsignedInteger seed)
{
#ifdef OPENTURNS_HAVE_NLOPT
  nlopt::srand(seed);
#else
  throw NotYetImplementedException(HERE) << "No NLopt support";
#endif
}


/* Default constructor */
NLopt::NLopt(const String & algoName)
  : OptimizationSolverImplementation()
  , algoName_(algoName)
{
}

NLopt::NLopt(const OptimizationProblem & problem,
             const String & algoName)
  : OptimizationSolverImplementation(problem)
  , algoName_(algoName)
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
    throw InvalidArgumentException(HERE) << "Error: " << getAlgorithmName() << " does not support multi-objective optimization";

  const UnsignedInteger dimension = problem.getDimension();
  const nlopt::algorithm algo = static_cast<nlopt::algorithm>(GetAlgorithmCode(getAlgorithmName()));
  nlopt::opt opt(algo, dimension);

  if (problem.hasInequalityConstraint())
  {
    try {
      opt.add_inequality_constraint(NLopt::ComputeInequalityConstraint, 0);
    } catch (std::invalid_argument) {
      throw InvalidArgumentException(HERE) << "Error: " << getAlgorithmName() << " does not support inequality constraints";
    }
  }

  if (problem.hasEqualityConstraint() || problem.hasLevelFunction())
  {
    try {
      opt.add_equality_constraint(NLopt::ComputeEqualityConstraint, 0);
    } catch (std::invalid_argument) {
      throw InvalidArgumentException(HERE) << "Error: " << getAlgorithmName() << " does not support equality constraints";
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
  const nlopt::algorithm algo = static_cast<nlopt::algorithm>(GetAlgorithmCode(algoName_));

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
      opt.add_inequality_constraint(NLopt::ComputeInequalityConstraint, inequalityData[i].get(), getMaximumConstraintError());
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
      opt.add_equality_constraint(NLopt::ComputeEqualityConstraint, equalityData[i].get(), getMaximumConstraintError());
    }
  }

  if (getProblem().hasLevelFunction())
  {
    opt.add_equality_constraint(NLopt::ComputeLevelFunction, this, getMaximumConstraintError());
    opt.set_min_objective(NLopt::ComputeObjectiveNearest, this);
  }

  if (initialStep_.getDimension() > 0)
  {
    if (initialStep_.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Invalid dx point dimension, expected " << dimension;
    std::vector<double> dx(dimension, 0.0);
    std::copy(initialStep_.begin(), initialStep_.end(), dx.begin());
    opt.set_default_initial_step(dx);
  }

  if (!p_localSolver_.isNull())
  {
    const nlopt::algorithm local_algo = static_cast<nlopt::algorithm>(GetAlgorithmCode(p_localSolver_->getAlgorithmName()));
    nlopt::opt local_opt(local_algo, dimension);
    local_opt.set_xtol_abs(p_localSolver_->getMaximumAbsoluteError());
    local_opt.set_xtol_rel(p_localSolver_->getMaximumRelativeError());
    local_opt.set_ftol_abs(p_localSolver_->getMaximumResidualError());
    local_opt.set_maxeval(p_localSolver_->getMaximumEvaluationNumber());
    if (p_localSolver_->getInitialStep().getDimension() > 0)
    {
      NumericalPoint localInitialStep(p_localSolver_->getInitialStep());
      if (localInitialStep.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Invalid local dx point dimension, expected " << dimension;
      std::vector<double> local_dx(dimension, 0.0);
      std::copy(localInitialStep.begin(), localInitialStep.end(), local_dx.begin());
      local_opt.set_default_initial_step(local_dx);
    }
    opt.set_local_optimizer(local_opt);
  }

  std::vector<double> x(dimension, 0.0);
  NumericalPoint startingPoint(getStartingPoint());
  if (startingPoint.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Invalid starting point dimension, expected " << dimension;
  std::copy(startingPoint.begin(), startingPoint.end(), x.begin());
  double optimalValue = 0.0;

  try
  {
    // The C++ interface of NLopt does not return a code for failures cases.
    // It is either positive (termination criterion) or an exception is thrown.
    opt.optimize(x, optimalValue);
  }
  catch (nlopt::roundoff_limited)
  {
    // Here we catch the roundoff_limited exception as the result
    // of the optimization may be useful even if not at the requested precision
    LOGWARN(OSS() << "NLopt raised a roundoff-limited exception");
  }
  catch (std::exception & exc)
  {
    throw InternalException(HERE) << "NLopt raised an exception: " << exc.what();
  }

  NumericalPoint optimizer(dimension);
  std::copy(x.begin(), x.end(), optimizer.begin());
  setResult(OptimizationResult(optimizer, NumericalPoint(1, optimalValue), 0, 0.0, 0.0, 0.0, 0.0, getProblem()));
#else
  throw NotYetImplementedException(HERE) << "No NLopt support";
#endif
}

/* String converter */
String NLopt::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " " << OptimizationSolverImplementation::__repr__()
      << " algorithm=" << algoName_;
  return oss;
}

/* String converter */
String NLopt::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << getClassName()
      << " algorithm=" << algoName_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void NLopt::save(Advocate & adv) const
{
  OptimizationSolverImplementation::save(adv);
  adv.saveAttribute("algoName_", algoName_);
  adv.saveAttribute("initialStep_", initialStep_);
  if (!p_localSolver_.isNull())
    adv.saveAttribute("localSolver_", *p_localSolver_);
}

/* Method load() reloads the object from the StorageManager */
void NLopt::load(Advocate & adv)
{
  OptimizationSolverImplementation::load(adv);
  adv.loadAttribute("algoName_", algoName_);
  adv.loadAttribute("initialStep_", initialStep_);
  if (adv.hasAttribute("localSolver_"))
  {
    NLopt localSolver;
    adv.loadAttribute("localSolver_", localSolver);
    p_localSolver_ = localSolver.clone();
  }
}

void NLopt::setAlgorithmName(const String algoName)
{
  algoName_ = algoName;
  checkProblem(getProblem());
}

String NLopt::getAlgorithmName() const
{
  return algoName_;
}

/* Initial derivative-free local-optimization algorithms step accessor */
void NLopt::setInitialStep(const NumericalPoint & initialStep)
{
  initialStep_ = initialStep;
}

NumericalPoint NLopt::getInitialStep() const
{
  return initialStep_;
}

/* Local optimizer */
void NLopt::setLocalSolver(const NLopt & localSolver)
{
  p_localSolver_ = localSolver.clone();
}

NLopt NLopt::getLocalSolver() const
{
  return *p_localSolver_;
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


double NLopt::ComputeLevelFunction(const std::vector< double >& x, std::vector< double >& grad, void* f_data)
{
  NLopt *algorithm = static_cast<NLopt *>(f_data);
  const UnsignedInteger dimension = algorithm->getProblem().getDimension();
  NumericalPoint inP(dimension);
  std::copy(x.begin(), x.end(), inP.begin());

  // evaluation
  NumericalPoint outP(algorithm->getProblem().getLevelFunction()(inP));

  // gradient
  if (!grad.empty())
  {
    Matrix gradient(algorithm->getProblem().getLevelFunction().gradient(inP));
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      grad[i] = gradient(i, 0);
    }
  }
  return outP[0] - algorithm->getProblem().getLevelValue();
}


double NLopt::ComputeObjectiveNearest(const std::vector<double> & x, std::vector<double> & grad, void * f_data)
{
  NLopt *algorithm = static_cast<NLopt *>(f_data);
  const UnsignedInteger dimension = algorithm->getProblem().getDimension();
  NumericalPoint inP(dimension);
  std::copy(x.begin(), x.end(), inP.begin());

  // evaluation
  NumericalScalar outP = 0.5 * inP.normSquare();

  // gradient
  if (!grad.empty())
  {
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      grad[i] = inP[i];
    }
  }
  return outP;
}


CLASSNAMEINIT(SLSQP);
static const Factory<SLSQP> Factory_SLSQP;
SLSQP::SLSQP() : NLopt("LD_SLSQP")
{
  Log::Warn(OSS() << "SLSQP class is deprecated.");
}
SLSQP::SLSQP(const OptimizationProblem & problem)
: NLopt(problem, "LD_SLSQP")
{
  Log::Warn(OSS() << "SLSQP class is deprecated.");
}
SLSQP * SLSQP::clone() const { return new SLSQP(*this); }

CLASSNAMEINIT(LBFGS);
static const Factory<LBFGS> Factory_LBFGS;
LBFGS::LBFGS() : NLopt("LD_LBFGS")
{
  Log::Warn(OSS() << "LBFGS class is deprecated.");
}
LBFGS::LBFGS(const OptimizationProblem & problem)
: NLopt(problem, "LD_LBFGS")
{
  Log::Warn(OSS() << "LBFGS class is deprecated.");
}
LBFGS * LBFGS::clone() const { return new LBFGS(*this); }

CLASSNAMEINIT(NelderMead);
static const Factory<NelderMead> Factory_NelderMead;
NelderMead::NelderMead() : NLopt("LN_NELDERMEAD")
{
  Log::Warn(OSS() << "NelderMead class is deprecated.");
}
NelderMead::NelderMead(const OptimizationProblem & problem)
: NLopt(problem, "LN_NELDERMEAD")
{
  Log::Warn(OSS() << "NelderMead class is deprecated.");
}
NelderMead * NelderMead::clone() const { return new NelderMead(*this); }

END_NAMESPACE_OPENTURNS
