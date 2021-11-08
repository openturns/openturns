//                                               -*- C++ -*-
/**
 *  @brief TNC is an actual implementation for OptimizationAlgorithmImplementation using the TNC library
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath> // For HUGE_VAL
#include <cstring> // std::memset

#include "openturns/TNC.hxx"
#include "algotnc.h"
#include "openturns/Point.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(TNC)

static const Factory<TNC> Factory_TNC;


/* Default constructor */
TNC::TNC()
  : OptimizationAlgorithmImplementation()
  , maxCGit_(ResourceMap::GetAsUnsignedInteger("TNC-DefaultMaxCGit"))
  , eta_(ResourceMap::GetAsScalar("TNC-DefaultEta"))
  , stepmx_(ResourceMap::GetAsScalar("TNC-DefaultStepmx"))
  , accuracy_(ResourceMap::GetAsScalar("TNC-DefaultAccuracy"))
  , fmin_(ResourceMap::GetAsScalar("TNC-DefaultFmin"))
  , rescale_(ResourceMap::GetAsScalar("TNC-DefaultRescale"))
  , p_nfeval_(0)
  , ignoreFailure_(false)
{
  // Nothing to do
}

/* Constructor with parameters */
TNC::TNC(const OptimizationProblem & problem)
  : OptimizationAlgorithmImplementation(problem)
  , maxCGit_(ResourceMap::GetAsUnsignedInteger("TNC-DefaultMaxCGit"))
  , eta_(ResourceMap::GetAsScalar("TNC-DefaultEta"))
  , stepmx_(ResourceMap::GetAsScalar("TNC-DefaultStepmx"))
  , accuracy_(ResourceMap::GetAsScalar("TNC-DefaultAccuracy"))
  , fmin_(ResourceMap::GetAsScalar("TNC-DefaultFmin"))
  , rescale_(ResourceMap::GetAsScalar("TNC-DefaultRescale"))
  , p_nfeval_(0)
  , ignoreFailure_(false)
{
  checkProblem(problem);
}

/* Constructor with parameters */
TNC::TNC (const OptimizationProblem & problem,
          const Point & scale,
          const Point & offset,
          const UnsignedInteger maxCGit,
          const Scalar eta,
          const Scalar stepmx,
          const Scalar accuracy,
          const Scalar fmin,
          const Scalar rescale)
  : OptimizationAlgorithmImplementation(problem)
  , scale_(scale)
  , offset_(offset)
  , maxCGit_(maxCGit)
  , eta_(eta)
  , stepmx_(stepmx)
  , accuracy_(accuracy)
  , fmin_(fmin)
  , rescale_(rescale)
  , p_nfeval_(0)
  , ignoreFailure_(false)
{
  checkProblem(problem);
}

/* Virtual constructor */
TNC * TNC::clone() const
{
  return new TNC(*this);
}

/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void TNC::checkProblem(const OptimizationProblem & problem) const
{
  if (problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support multi-objective optimization";
  if (problem.hasInequalityConstraint() || problem.hasEqualityConstraint())
    throw InvalidArgumentException(HERE) << "Error : " << this->getClassName() << " does not support constraints";
  if (!problem.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support non continuous problems";

}

/* Performs the actual computation by calling the TNC algorithm */
void TNC::run()
{
  const UnsignedInteger dimension = getProblem().getDimension();

  Point x(getStartingPoint());
  if (x.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Invalid starting point dimension (" << x.getDimension() << "), expected " << dimension;

  Interval bounds(getProblem().getBounds());
  if (!getProblem().hasBounds())
  {
    bounds = Interval(Point(dimension, 0.0), Point(dimension, 1.0), Interval::BoolCollection(dimension, false), Interval::BoolCollection(dimension, false));
  }
  if (!bounds.contains(x))
  {
    LOGWARN(OSS() << "Starting point is not inside bounds x=" << x.__str__() << " bounds=" << bounds);
  }
  Point low(bounds.getLowerBound());
  Point up(bounds.getUpperBound());
  Interval::BoolCollection finiteLow(bounds.getFiniteLowerBound());
  Interval::BoolCollection finiteUp(bounds.getFiniteUpperBound());
  /* Set the infinite bounds to HUGE_VAL (defined in cmath) with the correct signs */
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (!finiteLow[i]) low[i] = -HUGE_VAL;
    if (!finiteUp[i]) up[i] = HUGE_VAL;
  }

  tnc_message message((getVerbose() ? TNC_MSG_ALL : TNC_MSG_NONE));
  Point scale(getScale());
  Point offset(getOffset());
  double *refScale(scale.getDimension() == 0 ? NULL : &scale[0]);
  double *refOffset(offset.getDimension() == 0 ? NULL : &offset[0]);
  int nfeval = 0;
  p_nfeval_ = &nfeval;

  // clear history
  evaluationInputHistory_ = Sample(0, dimension);
  evaluationOutputHistory_ = Sample(0, 1);
  result_ = OptimizationResult(getProblem());

  Scalar f = -1.0;

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
   * offset    : constant to subtract to each variable
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

  int returnCode = tnc((int)dimension, &(*x.begin()), &f, NULL, TNC::ComputeObjectiveAndGradient, (void*) this, &(*low.begin()), &(*up.begin()), refScale, refOffset, message, getMaxCGit(), getMaximumEvaluationNumber(), getEta(), getStepmx(), getAccuracy(), getFmin(), getMaximumResidualError(), getMaximumAbsoluteError(), getMaximumConstraintError(), getRescale(), &nfeval);
  p_nfeval_ = 0;

  // Update the result
  const UnsignedInteger size = evaluationInputHistory_.getSize();

  Scalar absoluteError = -1.0;
  Scalar relativeError = -1.0;
  Scalar residualError = -1.0;
  Scalar constraintError = -1.0;

  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Point inP(evaluationInputHistory_[i]);
    const Point outP(evaluationOutputHistory_[i]);
    if (i > 0)
    {
      const Point inPM(evaluationInputHistory_[i - 1]);
      const Point outPM(evaluationOutputHistory_[i - 1]);
      absoluteError = (inP - inPM).normInf();
      relativeError = (inP.normInf() > 0.0) ? (absoluteError / inP.normInf()) : -1.0;
      residualError = (std::abs(outP[0]) > 0.0) ? (std::abs(outP[0] - outPM[0]) / std::abs(outP[0])) : -1.0;
    }
    constraintError = 0.0;
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      if (finiteLow[j] && (inP[j] < low[j]))
      {
        constraintError = std::max(constraintError, low[j] - inP[j]);
      }
      if (finiteUp[j] && (up[j] < inP[j]))
      {
        constraintError = std::max(constraintError, inP[j] - up[j]);
      }
    } // for j
    result_.store(inP, Point(1, outP[0]), absoluteError, relativeError, residualError, constraintError);
  } // for i

  /* Store the result */
  result_.setEvaluationNumber(size);
  result_.setOptimalPoint(x);
  const Scalar sign = getProblem().isMinimization() ? 1.0 : -1.0;
  result_.setOptimalValue(Point(1, sign * f));

  if ((returnCode != TNC_LOCALMINIMUM) && (returnCode != TNC_FCONVERGED) && (returnCode != TNC_XCONVERGED) && (returnCode != TNC_USERABORT))
  {
    if (ignoreFailure_)
      LOGWARN(OSS() << "Warning! TNC algorithm failed. The error message is " << tnc_rc_string[returnCode - TNC_MINRC]);
    else
      throw InternalException(HERE) << "Solving problem by TNC method failed (" << tnc_rc_string[returnCode - TNC_MINRC] << ")";
  }
}

/* Scale accessor */
Point TNC::getScale() const
{
  return scale_;
}

void TNC::setScale(const Point & scale)
{
  scale_ = scale;
}

/* Offset accessor */
Point TNC::getOffset() const
{
  return offset_;
}

void TNC::setOffset(const Point & offset)
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
Scalar TNC::getEta() const
{
  return eta_;
}

void TNC::setEta(const Scalar eta)
{
  eta_ = eta;
}

/* Stepmx accessor */
Scalar TNC::getStepmx() const
{
  return stepmx_;
}

void TNC::setStepmx(const Scalar stepmx)
{
  stepmx_ = stepmx;
}

/* Accuracy accessor */
Scalar TNC::getAccuracy() const
{
  return accuracy_;
}

void TNC::setAccuracy(const Scalar accuracy)
{
  accuracy_ = accuracy;
}

/* Fmin accessor */
Scalar TNC::getFmin() const
{
  return fmin_;
}

void TNC::setFmin(const Scalar fmin)
{
  fmin_ = fmin;
}

/* Rescale accessor */
Scalar TNC::getRescale() const
{
  return rescale_;
}

void TNC::setRescale(const Scalar rescale)
{
  rescale_ = rescale;
}

/* String converter */
String TNC::__repr__() const
{
  OSS oss;
  oss << "class=" << TNC::GetClassName()
      << " " << OptimizationAlgorithmImplementation::__repr__()
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
  OptimizationAlgorithmImplementation::save(adv);
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
  OptimizationAlgorithmImplementation::load(adv);
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
 * Wrapper of the Function operator() compatible with
 * TNC signature
 */
int TNC::ComputeObjectiveAndGradient(double *x, double *f, double *g, void *state)
{
  TNC *algorithm = static_cast<TNC *>(state);

  /* Convert the input vector in OpenTURNS format */
  const UnsignedInteger dimension = algorithm->getStartingPoint().getDimension();
  Point inP(dimension);
  std::copy(x, x + dimension, inP.begin());
  const OptimizationProblem problem(algorithm->getProblem());

  // Evaluate the objective function
  Point outP;
  try
  {
    for (UnsignedInteger i = 0; i < inP.getDimension(); ++i)
      if (!SpecFunc::IsNormal(inP[i]))
        throw InvalidArgumentException(HERE) << "TNC got nan value";

    outP = problem.getObjective().operator()(inP);
    *f = problem.isMinimization() ? outP[0] : -outP[0];

    // Here we take the sign into account and convert the result into a Point in one shot
    const Matrix gradient(problem.isMinimization() ? problem.getObjective().gradient(inP) : -1.0 * problem.getObjective().gradient(inP));
    /* Convert the gradient into the output format */
    std::copy(&gradient(0, 0), &gradient(0, 0) + dimension, g);
  }
  catch (...)
  {
    LOGWARN(OSS() << "TNC went to an abnormal point=" << inP.__str__());

    // penalize it
    *f = problem.isMinimization() ? SpecFunc::MaxScalar : -SpecFunc::MaxScalar;
    std::memset(g, 0, dimension);

    // exit gracefully
    return 1;
  }

  // track input/outputs
  algorithm->evaluationInputHistory_.add(inP);
  algorithm->evaluationOutputHistory_.add(outP);

  // update result
  algorithm->result_.setEvaluationNumber(algorithm->evaluationInputHistory_.getSize());
  algorithm->result_.store(inP, outP, 0.0, 0.0, 0.0, 0.0);

  // callbacks
  if (algorithm->progressCallback_.first)
  {
    algorithm->progressCallback_.first((100.0 * algorithm->evaluationInputHistory_.getSize()) / algorithm->getMaximumEvaluationNumber(), algorithm->progressCallback_.second);
  }
  if (algorithm->stopCallback_.first)
  {
    Bool stop = algorithm->stopCallback_.first(algorithm->stopCallback_.second);
    int *p_nfeval = static_cast<int*>(algorithm->p_nfeval_);
    if (p_nfeval)
    {
      if (stop) *p_nfeval = algorithm->getMaximumEvaluationNumber();
    }
    else
      throw InternalException(HERE) << "Null p_nfeval";
  }
  return 0;
}

void TNC::setIgnoreFailure(const Bool ignoreFailure)
{
  ignoreFailure_ = ignoreFailure;
}

Bool TNC::getIgnoreFailure() const
{
  return ignoreFailure_;
}

END_NAMESPACE_OPENTURNS

