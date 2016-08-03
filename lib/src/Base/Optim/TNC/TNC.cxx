//                                               -*- C++ -*-
/**
 *  @brief TNC is an actual implementation for OptimizationSolverImplementation using the TNC library
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
#include <cmath> // For HUGE_VAL

#include "openturns/TNC.hxx"
#include "algotnc.h"
#include "openturns/NumericalPoint.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(TNC);

static const Factory<TNC> Factory_TNC;


/* Default constructor */
TNC::TNC()
  : OptimizationSolverImplementation()
  , maxCGit_(ResourceMap::GetAsUnsignedInteger("TNC-DefaultMaxCGit"))
  , eta_(ResourceMap::GetAsNumericalScalar("TNC-DefaultEta"))
  , stepmx_(ResourceMap::GetAsNumericalScalar("TNC-DefaultStepmx"))
  , accuracy_(ResourceMap::GetAsNumericalScalar("TNC-DefaultAccuracy"))
  , fmin_(ResourceMap::GetAsNumericalScalar("TNC-DefaultFmin"))
  , rescale_(ResourceMap::GetAsNumericalScalar("TNC-DefaultRescale"))
{
  // Nothing to do
}

/* Constructor with parameters */
TNC::TNC(const OptimizationProblem & problem)
  : OptimizationSolverImplementation(problem)
  , maxCGit_(ResourceMap::GetAsUnsignedInteger("TNC-DefaultMaxCGit"))
  , eta_(ResourceMap::GetAsNumericalScalar("TNC-DefaultEta"))
  , stepmx_(ResourceMap::GetAsNumericalScalar("TNC-DefaultStepmx"))
  , accuracy_(ResourceMap::GetAsNumericalScalar("TNC-DefaultAccuracy"))
  , fmin_(ResourceMap::GetAsNumericalScalar("TNC-DefaultFmin"))
  , rescale_(ResourceMap::GetAsNumericalScalar("TNC-DefaultRescale"))
{
  // Nothing to do
}

/* Constructor with parameters */
TNC::TNC (const OptimizationProblem & problem,
          const NumericalPoint & scale,
          const NumericalPoint & offset,
          const UnsignedInteger maxCGit,
          const NumericalScalar eta,
          const NumericalScalar stepmx,
          const NumericalScalar accuracy,
          const NumericalScalar fmin,
          const NumericalScalar rescale)
  : OptimizationSolverImplementation(problem)
  , scale_(scale)
  , offset_(offset)
  , maxCGit_(maxCGit)
  , eta_(eta)
  , stepmx_(stepmx)
  , accuracy_(accuracy)
  , fmin_(fmin)
  , rescale_(rescale)
{
  // Nothing to do
}

/* Virtual constructor */
TNC * TNC::clone() const
{
  return new TNC(*this);
}

/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void TNC::checkProblem(const OptimizationProblem & problem) const
{
  if (problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support level-function optimization";
  if (problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support multi-objective optimization";
  if (problem.hasInequalityConstraint() || problem.hasEqualityConstraint())
    throw InvalidArgumentException(HERE) << "Error : " << this->getClassName() << " does not support constraints";
}

/* Performs the actual computation by calling the TNC algorithm */
void TNC::run()
{
  const UnsignedInteger dimension(getStartingPoint().getDimension());
  Interval boundConstraints(getProblem().getBounds());
  if (!getProblem().hasBounds())
  {
    boundConstraints = Interval(NumericalPoint(dimension, 0.0), NumericalPoint(dimension, 1.0), Interval::BoolCollection(dimension, false), Interval::BoolCollection(dimension, false));
  }

  NumericalPoint x(getStartingPoint());

  NumericalPoint low(boundConstraints.getLowerBound());
  NumericalPoint up(boundConstraints.getUpperBound());
  Interval::BoolCollection finiteLow(boundConstraints.getFiniteLowerBound());
  Interval::BoolCollection finiteUp(boundConstraints.getFiniteUpperBound());
  /* Set the infinite bounds to HUGE_VAL (defined in cmath) with the correct signs */
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (!finiteLow[i]) low[i] = -HUGE_VAL;
    if (!finiteUp[i]) up[i] = HUGE_VAL;
  }

  tnc_message message((getVerbose() ? TNC_MSG_ALL : TNC_MSG_NONE));
  NumericalPoint scale(getScale());
  NumericalPoint offset(getOffset());
  double *refScale(scale.getDimension() == 0 ? NULL : &scale[0]);
  double *refOffset(offset.getDimension() == 0 ? NULL : &offset[0]);
  int nfeval(0);

  // clear history
  evaluationInputHistory_ = NumericalSample(0.0, dimension);
  evaluationOutputHistory_ = NumericalSample(0.0, 2);

  NumericalScalar f = -1.0;

  /*
   * tnc : minimize a function with variables subject to bounds, using
   *       gradient information.
   *
   * n         : number of variables (must be >= 0)
   * x         : on input, initial estimate ; on output, the solution
   * f         : on output, the function value at the solution
   * g         : on output, the gradient value at the solution
   *             g should be an allocated vector of size n or NULL,
   *             in which case the gradient value is not returned.
   * function  : the function to minimize (see tnc_function)
   * state     : used by function (see tnc_function)
   * low, up   : the bounds
   *             set low[i] to -HUGE_VAL to remove the lower bound
   *             set up[i] to HUGE_VAL to remove the upper bound
   *             if low == NULL, the lower bounds are removed.
   *             if up == NULL, the upper bounds are removed.
   * scale     : scaling factors to apply to each variable
   *             if NULL, the factors are up-low for interval bounded variables
   *             and 1+|x] for the others.
   * offset    : constant to substract to each variable
   *             if NULL, the constant are (up+low)/2 for interval bounded
   *             variables and x for the others.
   * messages  : see the tnc_message enum
   * maxCGit   : max. number of hessian*vector evaluation per main iteration
   *             if maxCGit == 0, the direction chosen is -gradient
   *             if maxCGit < 0, maxCGit is set to max(1,min(50,n/2))
   * maxnfeval : max. number of function evaluation
   * eta       : severity of the line search. if < 0 or > 1, set to 0.25
   * stepmx    : maximum step for the line search. may be increased during call
   *             if too small, will be set to 10.0
   * accuracy  : relative precision for finite difference calculations
   *             if <= machine_precision, set to sqrt(machine_precision)
   * fmin      : minimum function value estimate
   * ftol      : precision goal for the value of f in the stoping criterion
   *             if ftol < 0.0, ftol is set to accuracy
   * xtol      : precision goal for the value of x in the stopping criterion
   *             (after applying x scaling factors)
   *             if xtol < 0.0, xtol is set to sqrt(machine_precision)
   * pgtol     : precision goal for the value of the projected gradient in the
   *             stopping criterion (after applying x scaling factors)
   *             if pgtol < 0.0, pgtol is set to 1e-2 * sqrt(accuracy)
   *             setting it to 0.0 is not recommended
   * rescale   : f scaling factor (in log10) used to trigger f value rescaling
   *             if 0, rescale at each iteration
   *             if a big value, never rescale
   *             if < 0, rescale is set to 1.3
   * nfeval    : on output, the number of function evaluations.
   *             ignored if nfeval==NULL.
   *
   * The tnc function returns a code defined in the tnc_rc enum.
   * On output, x, f and g may be very slightly out of sync because of scaling.
   *
   */

  int returnCode(tnc(int(dimension), &x[0], &f, NULL, TNC::ComputeObjectiveAndGradient, (void*) this, &low[0], &up[0], refScale, refOffset, message, getMaxCGit(), getMaximumIterationNumber(), getEta(), getStepmx(), getAccuracy(), getFmin(), getMaximumResidualError(), getMaximumAbsoluteError(), getMaximumConstraintError(), getRescale(), &nfeval));

  result_ = OptimizationResult();
  result_.setProblem(getProblem());

  // Update the result
  const UnsignedInteger size = evaluationInputHistory_.getSize();

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
    }
    constraintError = 0.0;
    for ( UnsignedInteger j = 0; j < dimension; ++j )
    {
      if (finiteLow[j] && (inP[j] < low[j]))
      {
        constraintError += low[j] - inP[j];
      }
      if (finiteUp[j] && (up[j] < inP[j]))
      {
        constraintError += inP[j] - up[j];
      }
    } // for j
    result_.store(inP, NumericalPoint(1, outP[0]), absoluteError, relativeError, residualError, constraintError);
  } // for i

  /* Store the result */
  result_.setOptimalPoint(x);
  const NumericalScalar sign = getProblem().isMinimization() ? 1.0 : -1.0;
  result_.setOptimalValue(NumericalPoint(1, sign * f));
  result_.setLagrangeMultipliers(computeLagrangeMultipliers(x));

  // check the convergence criteria
  const Bool convergence(((absoluteError < getMaximumAbsoluteError()) && (relativeError < getMaximumRelativeError())) || ((residualError < getMaximumResidualError()) && (constraintError < getMaximumConstraintError())));

  if ((returnCode != TNC_LOCALMINIMUM) && (returnCode != TNC_FCONVERGED) && (returnCode != TNC_XCONVERGED))
  {
    LOGWARN(OSS() << "Warning! TNC algorithm failed to converge. The error message is " << tnc_rc_string[returnCode - TNC_MINRC]);
  }
  else if ( ! convergence )
  {
    LOGWARN(OSS() << "Warning! The TNC algorithm could not enforce the convergence criteria");
  }
}

/* Scale accessor */
NumericalPoint TNC::getScale() const
{
  return scale_;
}

void TNC::setScale(const NumericalPoint & scale)
{
  scale_ = scale;
}

/* Offset accessor */
NumericalPoint TNC::getOffset() const
{
  return offset_;
}

void TNC::setOffset(const NumericalPoint & offset)
{
  offset_ = offset;
}

/* MaxCGit accessor */
UnsignedInteger TNC::getMaxCGit() const
{
  return maxCGit_;
}

void TNC::setMaxCGit(const UnsignedInteger maxCGit)
{
  maxCGit_ = maxCGit;
}

/* Eta accessor */
NumericalScalar TNC::getEta() const
{
  return eta_;
}

void TNC::setEta(const NumericalScalar eta)
{
  eta_ = eta;
}

/* Stepmx accessor */
NumericalScalar TNC::getStepmx() const
{
  return stepmx_;
}

void TNC::setStepmx(const NumericalScalar stepmx)
{
  stepmx_ = stepmx;
}

/* Accuracy accessor */
NumericalScalar TNC::getAccuracy() const
{
  return accuracy_;
}

void TNC::setAccuracy(const NumericalScalar accuracy)
{
  accuracy_ = accuracy;
}

/* Fmin accessor */
NumericalScalar TNC::getFmin() const
{
  return fmin_;
}

void TNC::setFmin(const NumericalScalar fmin)
{
  fmin_ = fmin;
}

/* Rescale accessor */
NumericalScalar TNC::getRescale() const
{
  return rescale_;
}

void TNC::setRescale(const NumericalScalar rescale)
{
  rescale_ = rescale;
}

/* String converter */
String TNC::__repr__() const
{
  OSS oss;
  oss << "class=" << TNC::GetClassName()
      << " " << OptimizationSolverImplementation::__repr__()
      << " scale=" << scale_
      << " offset=" << offset_
      << " maxCGit=" << maxCGit_
      << " eta=" << eta_
      << " stepmx=" << stepmx_
      << " accuracy=" << accuracy_
      << " fmin=" << fmin_
      << " rescale=" << rescale_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void TNC::save(Advocate & adv) const
{
  OptimizationSolverImplementation::save(adv);
  adv.saveAttribute("scale_", scale_);
  adv.saveAttribute("offset_", offset_);
  adv.saveAttribute("maxCGit_", maxCGit_);
  adv.saveAttribute("eta_", eta_);
  adv.saveAttribute("stepmx_", stepmx_);
  adv.saveAttribute("accuracy_", accuracy_);
  adv.saveAttribute("fmin_", fmin_);
  adv.saveAttribute("rescale_", rescale_);
}

/* Method load() reloads the object from the StorageManager */
void TNC::load(Advocate & adv)
{
  OptimizationSolverImplementation::load(adv);
  adv.loadAttribute("scale_", scale_);
  adv.loadAttribute("offset_", offset_);
  adv.loadAttribute("maxCGit_", maxCGit_);
  adv.loadAttribute("eta_", eta_);
  adv.loadAttribute("stepmx_", stepmx_);
  adv.loadAttribute("accuracy_", accuracy_);
  adv.loadAttribute("fmin_", fmin_);
  adv.loadAttribute("rescale_", rescale_);
}

/*
 * Wrapper of the NumericalMathFunction operator() compatible with
 * TNC signature
 */
int TNC::ComputeObjectiveAndGradient(double *x, double *f, double *g, void *state)
{
  TNC *algorithm = static_cast<TNC *>(state);

  /* Convert the input vector in OpenTURNS format */
  const UnsignedInteger dimension(algorithm->getStartingPoint().getDimension());
  NumericalPoint inPoint(dimension);
  memcpy(&inPoint[0], &x[0], dimension * sizeof(NumericalScalar));
  const OptimizationProblem problem(algorithm->getProblem());

  /* Used for history purpose. We store the value of the objective function in the first component and the norm of its gradient in the second component. */
  NumericalPoint outPoint(2);

  /* Compute the objective function at inPoint */
  const NumericalScalar result(problem.getObjective().operator()(inPoint)[0]);
  outPoint[0] = result;

  const NumericalScalar sign(problem.isMinimization() ? 1.0 : -1.0);
  *f = sign * result;

  NumericalPoint objectiveGradient;
  try
  {
    // Here we take the sign into account and convert the result into a NumericalPoint in one shot
    objectiveGradient = problem.getObjective().gradient(inPoint) * NumericalPoint(1, sign);
  }
  catch(...)
  {
    return 1;
  }

  /* Convert the gradient into the output format */
  memcpy(&g[0], &objectiveGradient[0], dimension * sizeof(NumericalScalar));

  outPoint[1] = objectiveGradient.norm();

  // track input/outputs
  algorithm->evaluationInputHistory_.add(inPoint);
  algorithm->evaluationOutputHistory_.add(outPoint);

  return 0;
}

END_NAMESPACE_OPENTURNS

