//                                               -*- C++ -*-
/**
 *  @brief Cobyla is an actual implementation for OptimizationAlgorithmImplementation using the cobyla library
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Cobyla.hxx"
#include "algocobyla.h"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Cobyla)

static const Factory<Cobyla> Factory_Cobyla;

/* Default constructor */
Cobyla::Cobyla()
  : OptimizationAlgorithmImplementation()
  , rhoBeg_(ResourceMap::GetAsScalar("Cobyla-DefaultRhoBeg"))
{
  // Nothing to do
}

Cobyla::Cobyla(const OptimizationProblem & problem)
  : OptimizationAlgorithmImplementation(problem)
  , rhoBeg_(ResourceMap::GetAsScalar("Cobyla-DefaultRhoBeg"))
{
  checkProblem(problem);
}

Cobyla::Cobyla(const OptimizationProblem & problem,
               const Scalar rhoBeg)
  : OptimizationAlgorithmImplementation(problem)
  , rhoBeg_(rhoBeg)
{
  checkProblem(problem);
}


/* Virtual constructor */
Cobyla * Cobyla::clone() const
{
  return new Cobyla(*this);
}

/** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void Cobyla::checkProblem(const OptimizationProblem & problem) const
{
  if (problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support multi-objective optimization";
  if (!problem.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support non continuous problems";

}

/* Performs the actual computation by calling the Cobyla algorithm
 */
void Cobyla::run()
{
  result_ = OptimizationResult(getProblem());

  const UnsignedInteger dimension = getProblem().getDimension();
  int n(dimension);
  int m(getProblem().getInequalityConstraint().getOutputDimension() + 2 * getProblem().getEqualityConstraint().getOutputDimension());

  Point x(getStartingPoint());
  if (x.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Invalid starting point dimension (" << x.getDimension() << "), expected " << dimension;
  if (dimension == 0)
    throw InvalidArgumentException(HERE) << "Dimension of the problem is zero";

  if (getProblem().hasBounds())
  {
    Interval bounds(getProblem().getBounds());
    if (!bounds.contains(x))
    {
      LOGWARN(OSS() << "Starting point is not inside bounds x=" << x.__str__() << " bounds=" << bounds);
    }
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      if (bounds.getFiniteLowerBound()[i]) ++ m;
      if (bounds.getFiniteUpperBound()[i]) ++ m;
    }
  }

  Scalar rhoEnd = getMaximumAbsoluteError();
  int maxFun = getMaximumCallsNumber();
  cobyla_message message = (Log::HasDebug() ? COBYLA_MSG_INFO : COBYLA_MSG_NONE);

  // initialize history
  evaluationInputHistory_ = Sample(0, dimension);
  evaluationOutputHistory_ = Sample(0, 1);
  equalityConstraintHistory_ = Sample(0, getProblem().getEqualityConstraint().getOutputDimension());
  inequalityConstraintHistory_ = Sample(0, getProblem().getInequalityConstraint().getOutputDimension());
  result_ = OptimizationResult(getProblem());

  t0_ = std::chrono::steady_clock::now();

  /*
   * cobyla : minimize a function subject to constraints
   *
   * n         : number of variables (>=0)
   * m         : number of constraints (>=0)
   * x         : on input, initial estimate ; on output, the solution
   * rhobeg    : a reasonable initial change to the variables
   * rhoend    : the required accuracy for the variables
   * message   : see the cobyla_message enum
   * maxfun    : on input, the maximum number of function evaluations
   *             on output, the number of function evaluations done
   * calcfc    : the function to minimize (see cobyla_function)
   * state     : used by function (see cobyla_function)
   *
   * The cobyla function returns a code defined in the cobyla_rc enum.
   *
   * extern int cobyla(int n, int m, double *x, double rhobeg, double rhoend,
   *  int message, int *maxfun, cobyla_function *calcfc, void *state);
   */
  const int returnCode = ot_cobyla(n, m, &(*x.begin()), rhoBeg_, rhoEnd, message, &maxFun, Cobyla::ComputeObjectiveAndConstraint, (void*) this);

  // The solver enforces the shifted constraints h +/- maximumConstraintError >= 0,
  // so a successful run may converge exactly onto the relaxed boundary
  // |h| == maximumConstraintError (e.g. nearest-point problems). The replay below
  // judges the raw residuals strictly and may retain nothing because of rounding:
  // recover the solver's own final center to report it in that case.
  // The final center is a simplex vertex, hence an evaluated point which is
  // found back in the history with exact floating point equality.
  Point solverSolution;
  Point solverSolutionValue;
  Point solverSolutionEquality;
  Point solverSolutionInequality;
  if (returnCode == COBYLA_NORMAL)
  {
    const Point center(x);
    const UnsignedInteger historySize = evaluationInputHistory_.getSize();
    for (UnsignedInteger k = 0; k < historySize; ++k)
    {
      if (evaluationInputHistory_[k] == center)
      {
        solverSolution = center;
        solverSolutionValue = evaluationOutputHistory_[k];
        if (getProblem().hasEqualityConstraint()) solverSolutionEquality = equalityConstraintHistory_[k];
        if (getProblem().hasInequalityConstraint()) solverSolutionInequality = inequalityConstraintHistory_[k];
        break;
      }
    }
  }

  // The stop callback flags INTERRUPTION on result_ while the solver runs;
  // the rebuild below would wipe it: latch it first. The COBYLA_USERABORT
  // return code cannot be used instead: evaluation failures share it.
  const Bool userInterrupted = (result_.getStatus() == OptimizationResult::INTERRUPTION);
  result_ = OptimizationResult(getProblem());
  result_.setStatusMessage(cobyla_rc_string[returnCode - COBYLA_MINRC]);
  if (userInterrupted)
    result_.setStatus(OptimizationResult::INTERRUPTION);
  else if (returnCode == COBYLA_MAXFUN)
    result_.setStatus(OptimizationResult::MAXIMUMCALLS);
  else if ((returnCode != COBYLA_NORMAL) && (returnCode != COBYLA_USERABORT))
    result_.setStatus(OptimizationResult::FAILURE);

  try
  {
    setResultFromEvaluationHistory(evaluationInputHistory_, evaluationOutputHistory_, inequalityConstraintHistory_, equalityConstraintHistory_);
  }
  catch (const InvalidArgumentException &)
  {
    // Only rescue the "no feasible point" outcome: replayed history with no
    // retained optimum. Inconsistent histories (nothing replayed at all) or
    // an unsuccessful run still throw.
    const Bool replayExhausted = (result_.getInputSample().getSize() > 0) && !result_.getOptimalPoint().getDimension();
    if (!solverSolution.getDimension() || !replayExhausted) throw;
    // Raw violation of the solver's solution, same definition as the replay
    Scalar solutionConstraintError = 0.0;
    if (getProblem().hasBounds())
    {
      const Interval bounds(getProblem().getBounds());
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        if (bounds.getFiniteLowerBound()[j]) solutionConstraintError = std::max(solutionConstraintError, bounds.getLowerBound()[j] - solverSolution[j]);
        if (bounds.getFiniteUpperBound()[j]) solutionConstraintError = std::max(solutionConstraintError, solverSolution[j] - bounds.getUpperBound()[j]);
      }
    }
    if (getProblem().hasEqualityConstraint()) solutionConstraintError = std::max(solutionConstraintError, solverSolutionEquality.normInf());
    if (getProblem().hasInequalityConstraint())
    {
      Point violatedConstraints(solverSolutionInequality);
      for (UnsignedInteger k = 0; k < violatedConstraints.getDimension(); ++k) violatedConstraints[k] = std::min(violatedConstraints[k], 0.0);
      solutionConstraintError = std::max(solutionConstraintError, violatedConstraints.normInf());
    }
    // Rescue only a rounding-scale excess over the tolerance, not genuine
    // infeasibility: evaluation rounding may push the raw residual slightly
    // above maximumConstraintError, never orders of magnitude above it
    const Scalar tolerance = getMaximumConstraintError() * (1.0 + std::sqrt(SpecFunc::Precision)) + SpecFunc::Precision;
    if (!(solutionConstraintError <= tolerance)) throw;
    // Report the solver's own solution: it satisfied the shifted constraints
    // enforced during the run.
    // The replay stamped a FAILURE status before throwing: restore SUCCESS,
    // the solver itself converged normally.
    result_.setStatus(OptimizationResult::SUCCESS);
    result_.setStatusMessage(cobyla_rc_string[returnCode - COBYLA_MINRC]);
    result_.setOptimalPoint(solverSolution);
    result_.setOptimalValue(solverSolutionValue);
    result_.setAbsoluteError(0.0);
    result_.setRelativeError(0.0);
    result_.setResidualError(0.0);
    result_.setConstraintError(solutionConstraintError);
  }

  // check for timeout
  std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0_).count();
  result_.setTimeDuration(timeDuration);
  if ((getMaximumTimeDuration() > 0.0) && (timeDuration > getMaximumTimeDuration()))
  {
    result_.setStatus(OptimizationResult::TIMEOUT);
    result_.setStatusMessage(OSS() << "Cobyla optimization timeout after " << timeDuration << "s");
  }

  // a user-requested stop is reported as INTERRUPTION without throwing,
  // like Dlib/CMinpack: checkStatus only guards actual failures and timeouts
  if ((result_.getStatus() != OptimizationResult::SUCCESS) && (result_.getStatus() != OptimizationResult::INTERRUPTION))
  {
    if (getCheckStatus())
      throw InternalException(HERE) << "Solving problem by cobyla method failed (" << result_.getStatusMessage() << ")";
    else
      LOGWARN(OSS() << "The Cobyla algorithm failed. The error message is " << result_.getStatusMessage());
  }
}

/* RhoBeg accessor */
Scalar Cobyla::getRhoBeg() const
{
  return rhoBeg_;
}

void Cobyla::setRhoBeg(const Scalar rhoBeg)
{
  rhoBeg_ = rhoBeg;
}

/* String converter */
String Cobyla::__repr__() const
{
  OSS oss;
  oss << "class=" << Cobyla::GetClassName()
      << " " << OptimizationAlgorithmImplementation::__repr__()
      << " rhoBeg=" << rhoBeg_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void Cobyla::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("rhoBeg_", rhoBeg_);
}

/* Method load() reloads the object from the StorageManager */
void Cobyla::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  adv.loadAttribute("rhoBeg_", rhoBeg_);
}

/*
 * Wrapper of the Function operator() compatible with
 * cobyla signature
 */
int Cobyla::ComputeObjectiveAndConstraint(int n,
    int,
    double *x,
    double *f,
    double *con,
    void *state)
{
  Cobyla *algorithm = static_cast<Cobyla *>(state);
  const OptimizationProblem problem(algorithm->getProblem());

  /* Convert the input vector to Point */
  Point inP(n);
  std::copy(x, x + n, inP.begin());

  const UnsignedInteger nbIneqConst = problem.getInequalityConstraint().getOutputDimension();
  const UnsignedInteger nbEqConst = problem.getEqualityConstraint().getOutputDimension();
  Point constraintValue(nbIneqConst + 2 * nbEqConst, -1.0);

  Point outP;
  try
  {
    for (UnsignedInteger i = 0; i < inP.getDimension(); ++ i)
      if (!std::isfinite(inP[i]))
        throw InvalidArgumentException(HERE) << "Cobyla got a nan/inf input value";

    outP = problem.getObjective().operator()(inP);

    if (std::isnan(outP[0]))
      throw InvalidArgumentException(HERE) << "Cobyla got a nan output value";

    *f = problem.isMinimization() ? outP[0] : -outP[0];
  }
  catch (const std::exception & exc)
  {
    LOGINFO(OSS() << "Cobyla went to an abnormal point x=" << inP.__str__() << " y=" << outP.__str__() << " msg=" << exc.what());

    // penalize it
    *f = problem.isMinimization() ? SpecFunc::Infinity : -SpecFunc::Infinity;
    std::copy(constraintValue.begin(), constraintValue.end(), con);

    // exit gracefully
    return 1;
  }
  UnsignedInteger shift = 0;

  /* Compute the inequality constraints */
  if (problem.hasInequalityConstraint())
  {
    const Point constraintInequalityValue(problem.getInequalityConstraint().operator()(inP));
    algorithm->inequalityConstraintHistory_.add(constraintInequalityValue);
    for(UnsignedInteger index = 0; index < nbIneqConst; ++index) constraintValue[index + shift] = constraintInequalityValue[index];
    shift += nbIneqConst;
  }

  /* Compute the equality constraints */
  if (problem.hasEqualityConstraint())
  {
    const Point constraintEqualityValue = problem.getEqualityConstraint().operator()(inP);
    algorithm->equalityConstraintHistory_.add(constraintEqualityValue);
    for(UnsignedInteger index = 0; index < nbEqConst; ++index) constraintValue[index + shift] = constraintEqualityValue[index] + algorithm->getMaximumConstraintError();
    shift += nbEqConst;
    for(UnsignedInteger index = 0; index < nbEqConst; ++index) constraintValue[index + shift] = -constraintEqualityValue[index] + algorithm->getMaximumConstraintError();
  }

  /* Compute the bound constraints */
  if (problem.hasBounds())
  {
    const Interval bounds(problem.getBounds());
    for (UnsignedInteger index = 0; index < bounds.getDimension(); ++index)
    {
      if (bounds.getFiniteLowerBound()[index])
        constraintValue.add(inP[index] - bounds.getLowerBound()[index]);
      if (bounds.getFiniteUpperBound()[index])
        constraintValue.add(bounds.getUpperBound()[index] - inP[index]);
    }
  }

  /* Convert the constraint vector in double format */
  std::copy(constraintValue.begin(), constraintValue.end(), con);

  // track input/outputs
  algorithm->evaluationInputHistory_.add(inP);
  algorithm->evaluationOutputHistory_.add(outP);

  // update result
  algorithm->result_.setCallsNumber(algorithm->evaluationInputHistory_.getSize());
  algorithm->result_.store(inP, outP, 0.0, 0.0, 0.0, constraintValue.normInf(), algorithm->getMaximumConstraintError());

  int returnValue = 0;

  std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - algorithm->t0_).count();
  if ((algorithm->getMaximumTimeDuration() > 0.0) && (timeDuration > algorithm->getMaximumTimeDuration()))
    returnValue = 1;

  if (algorithm->progressCallback_.first)
  {
    algorithm->progressCallback_.first((100.0 * algorithm->evaluationInputHistory_.getSize()) / algorithm->getMaximumCallsNumber(), algorithm->progressCallback_.second);
  }
  if (algorithm->stopCallback_.first && algorithm->stopCallback_.first(algorithm->stopCallback_.second))
  {
    // This value is passed to algocobyla. Any non-zero value should work but 1
    // is the most standard value.
    returnValue = 1;
    LOGINFO(OSS() << "Cobyla was stopped by user");
    algorithm->result_.setStatus(OptimizationResult::INTERRUPTION);
  }
  return returnValue;
}

END_NAMESPACE_OPENTURNS
