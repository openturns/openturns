//                                               -*- C++ -*-
/**
 *  @brief Cobyla is an actual implementation for OptimizationAlgorithmImplementation using the cobyla library
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
  if (problem.hasResidualFunction())
    throw InvalidArgumentException(HERE) << getClassName() << " does not support least-square problems";
  if (!problem.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support non continuous problems";

}

/* Performs the actual computation by calling the Cobyla algorithm
 */
void Cobyla::run()
{
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
  int maxFun = getMaximumEvaluationNumber();
  cobyla_message message((getVerbose() ? COBYLA_MSG_INFO : COBYLA_MSG_NONE));

  // initialize history
  evaluationInputHistory_ = Sample(0, dimension);
  evaluationOutputHistory_ = Sample(0, 1);
  equalityConstraintHistory_ = Sample(0, getProblem().getEqualityConstraint().getOutputDimension());
  inequalityConstraintHistory_ = Sample(0, getProblem().getInequalityConstraint().getOutputDimension());
  result_ = OptimizationResult(getProblem());

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
  int returnCode = ot_cobyla(n, m, &(*x.begin()), rhoBeg_, rhoEnd, message, &maxFun, Cobyla::ComputeObjectiveAndConstraint, (void*) this);
  if ((returnCode != COBYLA_NORMAL) && (returnCode != COBYLA_USERABORT))
  {
    if (ignoreFailure_)
      LOGWARN(OSS() << "Warning! The Cobyla algorithm failed. The error message is " << cobyla_rc_string[returnCode - COBYLA_MINRC]);
    else
      throw InternalException(HERE) << "Solving problem by cobyla method failed (" << cobyla_rc_string[returnCode - COBYLA_MINRC] << ")";
  }

  setResultFromEvaluationHistory(evaluationInputHistory_, evaluationOutputHistory_, inequalityConstraintHistory_, equalityConstraintHistory_);
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
  static const Scalar cobylaMaxScalar(1.0e-6 * SpecFunc::MaxScalar);

  Point outP;
  try
  {
    for (UnsignedInteger i = 0; i < inP.getDimension(); ++ i)
      if (!SpecFunc::IsNormal(inP[i]))
        throw InvalidArgumentException(HERE) << "Cobyla got a nan input value";

    outP = problem.getObjective().operator()(inP);

    if (!SpecFunc::IsNormal(outP[0]))
      throw InvalidArgumentException(HERE) << "Cobyla got a nan output value";

    // cobyla freezes when dealing with SpecFunc::MaxScalar
    if (outP[0] > cobylaMaxScalar) outP[0] = cobylaMaxScalar;
    if (outP[0] < -cobylaMaxScalar) outP[0] = -cobylaMaxScalar;
    *f = problem.isMinimization() ? outP[0] : -outP[0];
  }
  catch (const std::exception & exc)
  {
    LOGWARN(OSS() << "Cobyla went to an abnormal point x=" << inP.__str__() << " y=" << outP.__str__() << " msg=" << exc.what());

    // penalize it
    *f = problem.isMinimization() ? cobylaMaxScalar : cobylaMaxScalar;
    std::copy(constraintValue.begin(), constraintValue.end(), con);

    // exit gracefully
    return 1;
  }

  UnsignedInteger shift = 0;

  /* Compute the inequality constraints at inP */
  if (problem.hasInequalityConstraint())
  {
    const Point constraintInequalityValue(problem.getInequalityConstraint().operator()(inP));
    algorithm->inequalityConstraintHistory_.add(constraintInequalityValue);
    for(UnsignedInteger index = 0; index < nbIneqConst; ++index) constraintValue[index + shift] = constraintInequalityValue[index];
    shift += nbIneqConst;
  }

  /* Compute the equality constraints at inP */
  if (problem.hasEqualityConstraint())
  {
    const Point constraintEqualityValue = problem.getEqualityConstraint().operator()(inP);
    algorithm->equalityConstraintHistory_.add(constraintEqualityValue);
    for(UnsignedInteger index = 0; index < nbEqConst; ++index) constraintValue[index + shift] = constraintEqualityValue[index] + algorithm->getMaximumConstraintError();
    shift += nbEqConst;
    for(UnsignedInteger index = 0; index < nbEqConst; ++index) constraintValue[index + shift] = -constraintEqualityValue[index] + algorithm->getMaximumConstraintError();
  }

  /* Compute the bound constraints at inP */
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
  algorithm->result_.setEvaluationNumber(algorithm->evaluationInputHistory_.getSize());
  algorithm->result_.store(inP, outP, 0.0, 0.0, 0.0, 0.0);

  int returnValue = 0;
  if (algorithm->progressCallback_.first)
  {
    algorithm->progressCallback_.first((100.0 * algorithm->evaluationInputHistory_.getSize()) / algorithm->getMaximumEvaluationNumber(), algorithm->progressCallback_.second);
  }
  if (algorithm->stopCallback_.first)
  {
    Bool stop = algorithm->stopCallback_.first(algorithm->stopCallback_.second);
    if (stop)
    {
      // This value is passed to algocobyla. Any non-zero value should work but 1
      // is the most standard value.
      returnValue = 1;
      LOGWARN(OSS() << "Cobyla was stopped by user");
    }
  }
  return returnValue;
}

void Cobyla::setIgnoreFailure(const Bool ignoreFailure)
{
  ignoreFailure_ = ignoreFailure;
}

Bool Cobyla::getIgnoreFailure() const
{
  return ignoreFailure_;
}

END_NAMESPACE_OPENTURNS
