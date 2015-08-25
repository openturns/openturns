//                                               -*- C++ -*-
/**
 *  @brief Cobyla is an actual implementation for OptimizationSolverImplementation using the cobyla library
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "Cobyla.hxx"
#include "algocobyla.h"
#include "NumericalPoint.hxx"
#include "PersistentObjectFactory.hxx"
#include "Log.hxx"

BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(Cobyla);

static Factory<Cobyla> RegisteredFactory("Cobyla");

/* Default constructor */
Cobyla::Cobyla()
  : OptimizationSolverImplementation()
{
  // Nothing to do
}

/*
 * @brief  Standard constructor: the problem is defined by a scalar valued function  (in fact, a 1-D vector valued function)
 *         and a level value
 */
Cobyla::Cobyla(const NumericalMathFunction & levelFunction, const Bool verbose)
  : OptimizationSolverImplementation(OptimizationProblem(levelFunction), verbose)
  , specificParameters_()
{
  // Nothing to do
}

Cobyla::Cobyla(const OptimizationProblem & problem,
               const Bool verbose)
  : OptimizationSolverImplementation(problem, verbose)
  , specificParameters_()
{
  // Nothing to do
}

/*
 * @brief  Standard constructor: the problem is defined by a scalar valued function  (in fact, a 1-D vector valued function)
 *         and a level value
 */
Cobyla::Cobyla(const CobylaSpecificParameters & specificParameters,
               const NumericalMathFunction & levelFunction,
               const Bool verbose)
  : OptimizationSolverImplementation(OptimizationProblem(levelFunction), verbose)
  , specificParameters_(specificParameters)
{
  // Nothing to do
}

/* Virtual constructor */
Cobyla * Cobyla::clone() const
{
  return new Cobyla(*this);
}

/* Performs the actual computation by calling the Cobyla algorithm
 */
void Cobyla::run()
{
  UnsignedInteger dimension = getStartingPoint().getDimension();
  int n = dimension;
  int m = getProblem().getInequalityConstraint().getOutputDimension() + 2 * getProblem().getEqualityConstraint().getOutputDimension();

  NumericalPoint x(getStartingPoint());

  /* Compute the objective function at StartingPoint */
  const NumericalScalar sign(getProblem().isMinimization() == true ? 1.0 : -1.0);
  NumericalScalar f = sign * getProblem().getObjective().operator()(x)[0];

  NumericalScalar rhoBeg(specificParameters_.getRhoBeg());
  NumericalScalar rhoEnd(getMaximumAbsoluteError());
  int maxFun = getMaximumIterationsNumber();
  cobyla_message message((getVerbose() ? COBYLA_MSG_INFO : COBYLA_MSG_NONE));

  NumericalScalar absoluteError = -1.0;
  NumericalScalar relativeError = -1.0;
  NumericalScalar residualError = -1.0;
  NumericalScalar constraintError = -1.0;

  // clear result
  setResult(OptimizationSolverImplementationResult(x, NumericalPoint(1,f), 0, absoluteError, relativeError, residualError, constraintError));

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
  int returnCode = cobyla(n, m, &x[0], rhoBeg, rhoEnd, message, &maxFun, Cobyla::ComputeObjectiveAndConstraint, (void*) this);

  // Update the result
  result_.update(x, maxFun);
  UnsignedInteger size = evaluationInputHistory_.getSize();

  for ( UnsignedInteger i = 1; i < size; ++ i )
  {
    NumericalPoint inPM( evaluationInputHistory_[i - 1] );
    NumericalPoint inP( evaluationInputHistory_[i] );
    NumericalPoint outP( evaluationOutputHistory_[i] );
    NumericalPoint outPM( evaluationOutputHistory_[i - 1] );
    absoluteError = (inP - inPM).norm();
    relativeError = absoluteError / inP.norm();
    residualError = fabs(outP[0] - outPM[0]);
    constraintError =  outP[1];
    result_.store( inP, outP, absoluteError, relativeError, residualError, constraintError );
  }

  // check the convergence criteria
  Bool convergence = ((absoluteError < getMaximumAbsoluteError()) && (relativeError < getMaximumRelativeError())) || ((residualError < getMaximumResidualError()) && (constraintError < getMaximumConstraintError()));

  if (returnCode != 0)
  {
    LOGWARN(OSS() << "Warning! The Cobyla algorithm failed to converge. The error message is " << cobyla_rc_string[returnCode - COBYLA_MINRC]);
  }
  else if ( ! convergence )
  {
    LOGWARN(OSS() << "Warning! The Cobyla algorithm could not enforce the convergence criteria");
  }
}

/* Specific parameters accessor */
CobylaSpecificParameters Cobyla::getSpecificParameters() const
{
  return specificParameters_;
}

/* Specific parameters accessor */
void Cobyla::setSpecificParameters(const CobylaSpecificParameters & specificParameters)
{
  specificParameters_ = specificParameters;
}

/* Level function accessor */
NumericalMathFunction Cobyla::getLevelFunction() const
{
  return getProblem().getLevelFunction();
}

/* Level function accessor */
void Cobyla::setLevelFunction(const NumericalMathFunction & levelFunction)
{
  getProblem().setLevelFunction(levelFunction);
}

/* Level value accessor */
NumericalScalar Cobyla::getLevelValue() const
{
  return getProblem().getLevelValue();
}

/* Level value accessor */
void Cobyla::setLevelValue(const NumericalScalar levelValue)
{
  getProblem().setLevelValue(levelValue);
}


/* String converter */
String Cobyla::__repr__() const
{
  OSS oss;
  oss << "class=" << Cobyla::GetClassName()
      << " " << OptimizationSolverImplementation::__repr__()
      << " specificParameters=" << getSpecificParameters();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void Cobyla::save(Advocate & adv) const
{
  OptimizationSolverImplementation::save(adv);
  adv.saveAttribute("specificParameters_", specificParameters_);
}

/* Method load() reloads the object from the StorageManager */
void Cobyla::load(Advocate & adv)
{
  OptimizationSolverImplementation::load(adv);
  adv.loadAttribute("specificParameters_", specificParameters_);
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
  for(int index = 0; index < n; ++index) inPoint[index] = x[index];

  const OptimizationProblem problem(algorithm->getProblem());
  NumericalPoint outPoint(2);

  if ( problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "Error : Cobyla does not support Multi-Ojective Optimization ";

  const NumericalScalar result(problem.getObjective().operator()(inPoint)[0]);

  outPoint[0]= result;

  UnsignedInteger temp=0;
  UnsignedInteger nbIneqConst = problem.getInequalityConstraint().getOutputDimension();
  UnsignedInteger nbEqConst   = problem.getEqualityConstraint().getOutputDimension();
  NumericalPoint constraintValue(nbIneqConst+2*nbEqConst,0.0);

  const NumericalScalar sign(problem.isMinimization() == true ? 1.0 : -1.0);
  *f = sign * result;

    /* Compute the inequality constraints at inPoint */
  if (problem.hasInequalityConstraint())
  {
    NumericalPoint constraintInequalityValue = problem.getInequalityConstraint().operator()(inPoint);
    for(UnsignedInteger index = 0; index < nbIneqConst; ++index) constraintValue[index+temp] = constraintInequalityValue[index];
    temp += nbIneqConst;
  }
  /* Compute the equality constraints at inPoint */
  if (problem.hasEqualityConstraint())
  {
    NumericalPoint constraintEqualityValue = problem.getEqualityConstraint().operator()(inPoint);
    for(UnsignedInteger index = 0; index < nbEqConst; ++index) constraintValue[index+temp] = constraintEqualityValue[index] + algorithm->getMaximumConstraintError();
    temp += nbEqConst;
    for(UnsignedInteger index = 0; index < nbEqConst; ++index) constraintValue[index+temp] = -constraintEqualityValue[index] + algorithm->getMaximumConstraintError();
  }

  /* Convert the constraint vector in double format */
  for(UnsignedInteger index = 0; index < (UnsignedInteger)(constraintValue.getDimension()); ++index) con[index] = constraintValue[index];

  /* Compute constraints norm */
  outPoint[1]= constraintValue.norm();

  // track input/outputs
  algorithm->evaluationInputHistory_.add(inPoint);
  algorithm->evaluationOutputHistory_.add(outPoint);
  return 0;
}

END_NAMESPACE_OPENTURNS
