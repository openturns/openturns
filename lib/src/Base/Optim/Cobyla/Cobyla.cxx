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
#include "openturns/Cobyla.hxx"
#include "algocobyla.h"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Cobyla);

static const Factory<Cobyla> Factory_Cobyla;

/* Default constructor */
Cobyla::Cobyla()
  : OptimizationSolverImplementation()
  , rhoBeg_(ResourceMap::GetAsNumericalScalar("Cobyla-DefaultRhoBeg"))
{
  // Nothing to do
}

Cobyla::Cobyla(const OptimizationProblem & problem)
  : OptimizationSolverImplementation(problem)
  , rhoBeg_(ResourceMap::GetAsNumericalScalar("Cobyla-DefaultRhoBeg"))
{
  // Nothing to do
}

Cobyla::Cobyla(const OptimizationProblem & problem,
               const NumericalScalar rhoBeg)
  : OptimizationSolverImplementation(problem)
  , rhoBeg_(rhoBeg)
{
  // Nothing to do
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
}

/* Performs the actual computation by calling the Cobyla algorithm
 */
void Cobyla::run()
{
  const UnsignedInteger dimension = getProblem().getDimension();
  int n(dimension);
  int m(getProblem().getInequalityConstraint().getOutputDimension() + 2 * getProblem().getEqualityConstraint().getOutputDimension());

  NumericalPoint x(getStartingPoint());
  if (x.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Invalid starting point dimension (" << x.getDimension() << "), expected " << dimension;

  if (getProblem().hasBounds())
  {
    Interval bounds(getProblem().getBounds());
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      if (bounds.getFiniteLowerBound()[i]) ++m;
      if (bounds.getFiniteUpperBound()[i]) ++m;
    }
  }

  NumericalScalar rhoEnd = getMaximumAbsoluteError();
  int maxFun(getMaximumIterationNumber());
  cobyla_message message((getVerbose() ? COBYLA_MSG_INFO : COBYLA_MSG_NONE));

  // initialize history
  evaluationInputHistory_ = NumericalSample(0, dimension);
  evaluationOutputHistory_ = NumericalSample(0, 2);

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
  int returnCode(ot_cobyla(n, m, &x[0], rhoBeg_, rhoEnd, message, &maxFun, Cobyla::ComputeObjectiveAndConstraint, (void*) this));

  result_ = OptimizationResult();
  result_.setProblem(getProblem());

  // Update the result
  UnsignedInteger size = evaluationInputHistory_.getSize();

  NumericalScalar absoluteError = -1.0;
  NumericalScalar relativeError = -1.0;
  NumericalScalar residualError = -1.0;
  NumericalScalar constraintError = -1.0;

  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const NumericalPoint inP(evaluationInputHistory_[i]);
    const NumericalPoint outP(evaluationOutputHistory_[i]);
    if (i > 0)
    {
      const NumericalPoint inPM(evaluationInputHistory_[i - 1]);
      const NumericalPoint outPM(evaluationOutputHistory_[i - 1]);
      absoluteError = (inP - inPM).normInf();
      relativeError = absoluteError / inP.normInf();
      residualError = std::abs(outP[0] - outPM[0]);
      constraintError = outP[1];
    }
    result_.store(inP, NumericalPoint(1, outP[0]), absoluteError, relativeError, residualError, constraintError);
  }

  result_.setOptimalPoint(x);
  const UnsignedInteger index = evaluationInputHistory_.find(x);
  NumericalScalar bestValue = evaluationOutputHistory_[index][0];
  result_.setOptimalValue(NumericalPoint(1, bestValue));
  result_.setIterationNumber(maxFun);
  result_.setLagrangeMultipliers(computeLagrangeMultipliers(x));

  // check the convergence criteria
  const Bool convergence = ((absoluteError < getMaximumAbsoluteError()) && (relativeError < getMaximumRelativeError())) || ((residualError < getMaximumResidualError()) && (constraintError < getMaximumConstraintError()));

  if (returnCode != 0)
  {
    LOGWARN(OSS() << "Warning! The Cobyla algorithm failed to converge. The error message is " << cobyla_rc_string[returnCode - COBYLA_MINRC]);
  }
  else if ( ! convergence )
  {
    LOGWARN(OSS() << "Warning! The Cobyla algorithm could not enforce the convergence criteria");
  }
}

/* RhoBeg accessor */
NumericalScalar Cobyla::getRhoBeg() const
{
  return rhoBeg_;
}

void Cobyla::setRhoBeg(const NumericalScalar rhoBeg)
{
  rhoBeg_ = rhoBeg;
}

/* String converter */
String Cobyla::__repr__() const
{
  OSS oss;
  oss << "class=" << Cobyla::GetClassName()
      << " " << OptimizationSolverImplementation::__repr__()
      << " rhoBeg=" << rhoBeg_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void Cobyla::save(Advocate & adv) const
{
  OptimizationSolverImplementation::save(adv);
  adv.saveAttribute("rhoBeg_", rhoBeg_);
}

/* Method load() reloads the object from the StorageManager */
void Cobyla::load(Advocate & adv)
{
  OptimizationSolverImplementation::load(adv);
  adv.loadAttribute("rhoBeg_", rhoBeg_);
}

/*
 * Wrapper of the NumericalMathFunction operator() compatible with
 * cobyla signature
 */
int Cobyla::ComputeObjectiveAndConstraint(int n,
    int m,
    double *x,
    double *f,
    double *con,
    void *state)
{
  Cobyla *algorithm = static_cast<Cobyla *>(state);

  /* Convert the input vector in OpenTURNS format */
  NumericalPoint inPoint(n);
  memcpy(&inPoint[0], &x[0], n * sizeof(NumericalScalar));

  const OptimizationProblem problem(algorithm->getProblem());
  NumericalPoint outPoint(2);

  NumericalScalar result = problem.getObjective().operator()(inPoint)[0];
  // cobyla freezes when dealing with MaxNumericalScalar
  if (std::abs(result) == SpecFunc::MaxNumericalScalar) result /= 1.0e3;
  outPoint[0] = result;

  const NumericalScalar sign = problem.isMinimization() ? 1.0 : -1.0;
  *f = sign * result;

  UnsignedInteger shift = 0;
  UnsignedInteger nbIneqConst = problem.getInequalityConstraint().getOutputDimension();
  UnsignedInteger nbEqConst = problem.getEqualityConstraint().getOutputDimension();
  NumericalPoint constraintValue(nbIneqConst + 2 * nbEqConst);

  /* Compute the inequality constraints at inPoint */
  if (problem.hasInequalityConstraint())
  {
    const NumericalPoint constraintInequalityValue(problem.getInequalityConstraint().operator()(inPoint));
    for(UnsignedInteger index = 0; index < nbIneqConst; ++index) constraintValue[index + shift] = constraintInequalityValue[index];
    shift += nbIneqConst;
  }

  /* Compute the equality constraints at inPoint */
  if (problem.hasEqualityConstraint())
  {
    const NumericalPoint constraintEqualityValue = problem.getEqualityConstraint().operator()(inPoint);
    for(UnsignedInteger index = 0; index < nbEqConst; ++index) constraintValue[index + shift] = constraintEqualityValue[index] + algorithm->getMaximumConstraintError();
    shift += nbEqConst;
    for(UnsignedInteger index = 0; index < nbEqConst; ++index) constraintValue[index + shift] = -constraintEqualityValue[index] + algorithm->getMaximumConstraintError();
  }

  /* Compute the bound constraints at inPoint */
  if (problem.hasBounds())
  {
    const Interval bounds(problem.getBounds());
    for (UnsignedInteger index = 0; index < bounds.getDimension(); ++index)
    {
      if (bounds.getFiniteLowerBound()[index])
        constraintValue.add(inPoint[index] - bounds.getLowerBound()[index]);
      if (bounds.getFiniteUpperBound()[index])
        constraintValue.add(bounds.getUpperBound()[index] - inPoint[index]);
    }
  }

  /* Convert the constraint vector in double format */
  memcpy(&con[0], &constraintValue[0], constraintValue.getDimension() * sizeof(NumericalScalar));

  // only take violated constraints into account to compute error
  for (UnsignedInteger j = 0; j < constraintValue.getDimension(); ++ j)
  {
    if (constraintValue[j] > 0.0) constraintValue[j] = 0.0;
  }
  outPoint[1] = constraintValue.normInf();

  // track input/outputs
  algorithm->evaluationInputHistory_.add(inPoint);
  algorithm->evaluationOutputHistory_.add(outPoint);
  return 0;
}

END_NAMESPACE_OPENTURNS
