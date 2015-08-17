//                                               -*- C++ -*-
/**
 *  @brief TNC is an actual implementation for a bound-constrained optimization algorithm
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
#include <cmath> // For HUGE_VAL

#include "TNC.hxx"
#include "algotnc.h"
#include "NumericalPoint.hxx"
#include "Matrix.hxx"
#include "Log.hxx"
#include "OSS.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(TNC);

static Factory<TNC> RegisteredFactory("TNC");


/* Default constructor */
TNC::TNC():
  BoundConstrainedAlgorithmImplementation()
{
  // Nothing to do
}

/* Constructor with parameters: no constraint, starting from the origin */
TNC::TNC(const NumericalMathFunction & objectiveFunction,
         const Bool verbose):
  BoundConstrainedAlgorithmImplementation(objectiveFunction, verbose)
{
  // Nothing to do
}

/* Constructor with parameters: bound constraints, starting from the given point */
TNC::TNC(const TNCSpecificParameters & parameters,
         const NumericalMathFunction & objectiveFunction,
         const Interval & boundConstraints,
         const NumericalPoint & startingPoint,
         const OptimizationProblemObsolete optimization,
         const Bool verbose)
  :
  BoundConstrainedAlgorithmImplementation(objectiveFunction, boundConstraints, startingPoint, optimization, verbose),
  specificParameters_(parameters)
{
  // Nothing to do
}

/* Virtual constructor */
TNC * TNC::clone() const
{
  return new TNC(*this);
}

/* Performs the actual computation by calling the TNC algorithm */
void TNC::run()
{
  const UnsignedInteger dimension = getObjectiveFunction().getInputDimension();
  if (dimension == 0) throw InternalException(HERE) << "Error: cannot solve a bound constrained optimization problem with no objective function.";
  NumericalScalar f = 0.0;
  Interval boundConstraints(getBoundConstraints());
  if (boundConstraints.getDimension() != dimension) throw InternalException(HERE) << "Error: cannot solve a bound constrained optimization problem with bounds of dimension=" << boundConstraints.getDimension() << " incompatible with the objective function input dimension=" << dimension << ".";
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
  NumericalPoint x(getStartingPoint());
  if (x.getDimension() != dimension)
  {
    LOGWARN("Warning! The given starting point has a dimension incompatible with the objective function. Using the midpoint of the constraints as a starting point.");
    x = 0.5 * (low + up);
  }
  tnc_message message((getVerbose() ? TNC_MSG_ALL : TNC_MSG_NONE));
  NumericalPoint scale(specificParameters_.getScale());
  NumericalPoint offset(specificParameters_.getOffset());
  double *refScale = scale.getDimension() == 0 ? NULL : &scale[0];
  double *refOffset = offset.getDimension() == 0 ? NULL : &offset[0];
  int nfeval = 0;

  NumericalScalar absoluteError = -1.0;
  NumericalScalar relativeError = -1.0;
  NumericalScalar objectiveError = -1.0;
  NumericalScalar constraintError = -1.0;

  // clear result
  setResult(Result(x, f, getOptimizationProblem(), nfeval, absoluteError, relativeError, objectiveError, constraintError));

  // clear history
  evaluationInputHistory_ = NumericalSample(0, dimension);
  evaluationOutputHistory_.clear();

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
  int returnCode = tnc(int(dimension), &x[0], &f, NULL, TNC::ComputeObjectiveAndConstraint, (void*) this, &low[0], &up[0], refScale, refOffset, message, specificParameters_.getMaxCGit(), getMaximumEvaluationsNumber(), specificParameters_.getEta(), specificParameters_.getStepmx(), specificParameters_.getAccuracy(), specificParameters_.getFmin(), getMaximumObjectiveError(), getMaximumAbsoluteError(), getMaximumConstraintError(), specificParameters_.getRescale(), &nfeval);

  UnsignedInteger size = evaluationInputHistory_.getSize();
  for (UnsignedInteger i = 1; i < size; ++ i)
  {
    NumericalPoint x_m(evaluationInputHistory_[i - 1]);
    NumericalPoint xi(evaluationInputHistory_[i]);
    NumericalPoint y(evaluationOutputHistory_[i]);
    NumericalPoint y_m(evaluationOutputHistory_[i - 1]);
    absoluteError = (xi - x_m).norm();
    relativeError = absoluteError / xi.norm();
    objectiveError = (y - y_m).norm();
    constraintError = 0.0;
    for ( UnsignedInteger j = 0; j < dimension; ++ j )
    {
      if (finiteLow[j] && (xi[j] < low[j]))
      {
        constraintError += low[j] - xi[j];
      }
      if (finiteUp[j] && (up[j] < xi[j]))
      {
        constraintError += xi[j] - up[j];
      }
    }
  }

  /* If it was a maximization problem, we solved the associated minimization problem with -f */
  if (getOptimizationProblem() == Result::MAXIMIZATION) f = -f;

  /* Store the result */
  setResult(Result(x, f, getOptimizationProblem(), nfeval, absoluteError, relativeError, objectiveError, constraintError));

  // check the convergence criteria
  Bool convergence = ((absoluteError < getMaximumAbsoluteError()) && (relativeError < getMaximumRelativeError())) || ((objectiveError < getMaximumObjectiveError()) && (constraintError < getMaximumConstraintError()));

  if ((returnCode != TNC_LOCALMINIMUM) && (returnCode != TNC_FCONVERGED) && (returnCode != TNC_XCONVERGED))
  {
    LOGWARN(OSS() << "Warning! TNC algorithm failed to converge. The error message is " << tnc_rc_string[returnCode - TNC_MINRC]);
  }
  else if ( ! convergence )
  {
    LOGWARN(OSS() << "Warning! The TNC algorithm could not enforce the convergence criteria");
  }
}

/* Specific parameters accessor */
TNCSpecificParameters TNC::getSpecificParameters() const
{
  return specificParameters_;
}

/* Specific parameters accessor */
void TNC::setSpecificParameters(const TNCSpecificParameters & specificParameters)
{
  specificParameters_ = specificParameters;
}

/* String converter */
String TNC::__repr__() const
{
  OSS oss;
  oss << "class=" << TNC::GetClassName()
      << " " << BoundConstrainedAlgorithmImplementation::__repr__()
      << " specificParameters=" << getSpecificParameters();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void TNC::save(Advocate & adv) const
{
  BoundConstrainedAlgorithmImplementation::save(adv);
  adv.saveAttribute("specificParameters_", specificParameters_);
}

/* Method load() reloads the object from the StorageManager */
void TNC::load(Advocate & adv)
{
  BoundConstrainedAlgorithmImplementation::load(adv);
  adv.loadAttribute("specificParameters_", specificParameters_);
}

/*
 * Wrapper of the NumericalMathFunction operator() compatible with
 * TNC signature
 */
int TNC::ComputeObjectiveAndConstraint(double *x, double *f, double *g, void *state)
{
  TNC *algorithm = static_cast<TNC *>(state);

  /* Retreive the objective function */
  NumericalMathFunction objectiveFunction(algorithm->getObjectiveFunction());
  const UnsignedInteger dimension(objectiveFunction.getInputDimension());
  /* Convert the input vector in OpenTURNS format */
  NumericalPoint inPoint(dimension);
  for( UnsignedInteger i = 0; i < dimension; ++ i ) inPoint[i] = x[i];
  /* Change the sign of f if it is a maximization problem */
  const NumericalScalar sign(algorithm->getOptimizationProblem() == Result::MINIMIZATION ? 1.0 : -1.0);
  Matrix objectiveGradient;
  try
  {
    (*f) = sign * objectiveFunction(inPoint)[0];
    objectiveGradient = objectiveFunction.gradient(inPoint);
  }
  catch(...)
  {
    return 1;
  }
  for ( UnsignedInteger i = 0; i < dimension; ++ i ) g[i] = sign * objectiveGradient(i, 0);

  algorithm->evaluationInputHistory_.add(inPoint);
  algorithm->evaluationOutputHistory_.add(NumericalPoint(1, *f));

  return 0;
}

END_NAMESPACE_OPENTURNS
