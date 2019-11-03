//                                               -*- C++ -*-
/**
 *  @brief OPT++ solver implementation
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/OPTppImplementation.hxx"

#include <NonLinearInequality.h>
#include <NonLinearEquation.h>

#include <OptBaNewton.h>
#include <OptBaQNewton.h>
#include <OptBCEllipsoid.h>
#include <OptBCFDNewton.h>
#include <OptBCNewton.h>
#include <OptBCQNewton.h>
#include <OptCG.h>
#include <OptConstrFDNewton.h>
#include <OptConstrNewton.h>
#include <OptConstrQNewton.h>
#include <OptDHNIPS.h>
#include <OptFDNewton.h>
#include <OptFDNIPS.h>
#include <OptLBFGS.h>
#include <OptNewton.h>
#include <OptNIPS.h>
#include <OptPDS.h>
#include <OptQNIPS.h>
#include <OptQNewton.h>


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OPTppImplementation)

static const Factory<OPTppImplementation> Factory_OPTppImplementation;

OPTppImplementation* OPTppImplementation::P_current_opt_ = 0;
Description OPTppImplementation::AlgorithmNames_;


void OPTppImplementation::InitializeProblem(int /*ndim*/, NEWMAT::ColumnVector& x)
{
  if (!P_current_opt_)
    throw InternalException(HERE) << "P_current_opt_ is null";
  const UnsignedInteger dimension = P_current_opt_->getProblem().getDimension();
  const Point startingPoint(P_current_opt_->getStartingPoint());
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    x(i + 1) = startingPoint[i];// starts at 1!
}


void OPTppImplementation::ComputeObjective(int /*ndim*/, const NEWMAT::ColumnVector & x, double & fx, int & result, void* v)
{
  OPTppImplementation *algorithm = static_cast<OPTppImplementation *>(v);

  const UnsignedInteger dimension = algorithm->getProblem().getDimension();
  Point inP(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    inP[i] = x(i + 1);// starts at 1!

  const Scalar sign = algorithm->getProblem().isMinimization() ? 1.0 : -1.0;

  // evaluation
  const Point outP(algorithm->getProblem().getObjective()(inP));
  fx = sign * outP[0];
  result = OPTPP::NLPFunction;

  // track input/outputs
  algorithm->evaluationInputHistory_.add(inP);
  algorithm->evaluationOutputHistory_.add(outP);

  // callbacks
  if (algorithm->progressCallback_.first)
  {
    algorithm->progressCallback_.first((100.0 * algorithm->evaluationInputHistory_.getSize()) / algorithm->getMaximumEvaluationNumber(), algorithm->progressCallback_.second);
  }
  if (algorithm->stopCallback_.first)
  {
    const Bool stop = algorithm->stopCallback_.first(algorithm->stopCallback_.second);
    if (stop)
      throw InternalException(HERE) << "User stopped OPTpp";
  }
}


void OPTppImplementation::ComputeObjectiveGradient(int mode, int /*ndim*/, const NEWMAT::ColumnVector & x, double & fx, NEWMAT::ColumnVector & g, int & result, void* v)
{
  OPTppImplementation *algorithm = static_cast<OPTppImplementation *>(v);

  const UnsignedInteger dimension = algorithm->getProblem().getDimension();
  Point inP(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    inP[i] = x(i + 1);// starts at 1!

  const Scalar sign = algorithm->getProblem().isMinimization() ? 1.0 : -1.0;

  // evaluation
  if (mode & OPTPP::NLPFunction)
  {
    const Point outP(algorithm->getProblem().getObjective()(inP));
    fx = sign * outP[0];
    result = OPTPP::NLPFunction;

    // track input/outputs
    algorithm->evaluationInputHistory_.add(inP);
    algorithm->evaluationOutputHistory_.add(outP);
  }

  // gradient
  if (mode & OPTPP::NLPGradient)
  {
    const Matrix grad(algorithm->getProblem().getObjective().gradient(inP));
    for (UnsignedInteger i = 0; i < dimension; ++ i)
      g(i + 1) = sign * grad(i, 0);// starts at 1!
    result = OPTPP::NLPGradient;
  }

  // callbacks
  if (algorithm->progressCallback_.first)
  {
    algorithm->progressCallback_.first((100.0 * algorithm->evaluationInputHistory_.getSize()) / algorithm->getMaximumEvaluationNumber(), algorithm->progressCallback_.second);
  }
  if (algorithm->stopCallback_.first)
  {
    const Bool stop = algorithm->stopCallback_.first(algorithm->stopCallback_.second);
    if (stop)
      throw InternalException(HERE) << "User stopped OPTpp";
  }
}


void OPTppImplementation::ComputeObjectiveGradientHessian(int mode, int /*ndim*/, const NEWMAT::ColumnVector & x, double & fx, NEWMAT::ColumnVector & g, NEWMAT::SymmetricMatrix & H, int & result, void* v)
{
  OPTppImplementation *algorithm = static_cast<OPTppImplementation *>(v);

  const UnsignedInteger dimension = algorithm->getProblem().getDimension();
  Point inP(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    inP[i] = x(i + 1);// starts at 1!

  const Scalar sign = algorithm->getProblem().isMinimization() ? 1.0 : -1.0;

  // evaluation
  if (mode & OPTPP::NLPFunction)
  {
    const Point outP(algorithm->getProblem().getObjective()(inP));
    fx = sign * outP[0];
    result = OPTPP::NLPFunction;

    // track input/outputs
    algorithm->evaluationInputHistory_.add(inP);
    algorithm->evaluationOutputHistory_.add(outP);
  }

  // gradient
  if (mode & OPTPP::NLPGradient)
  {
    const Matrix grad(algorithm->getProblem().getObjective().gradient(inP));
    for (UnsignedInteger i = 0; i < dimension; ++ i)
      g(i + 1) = sign * grad(i, 0);// starts at 1!
    result = OPTPP::NLPGradient;
  }

  // hessian
  if (mode & OPTPP::NLPHessian)
  {
    const SymmetricTensor hess(algorithm->getProblem().getObjective().hessian(inP));
    for (UnsignedInteger i = 0; i < dimension; ++ i)
      for (UnsignedInteger j = 0; j < i; ++ j)
        H(i + 1, j + 1) = sign * hess(i, j, 0);// starts at 1!
    result = OPTPP::NLPHessian;
  }

  // callbacks
  if (algorithm->progressCallback_.first)
  {
    algorithm->progressCallback_.first((100.0 * algorithm->evaluationInputHistory_.getSize()) / algorithm->getMaximumEvaluationNumber(), algorithm->progressCallback_.second);
  }
  if (algorithm->stopCallback_.first)
  {
    const Bool stop = algorithm->stopCallback_.first(algorithm->stopCallback_.second);
    if (stop)
      throw InternalException(HERE) << "User stopped OPTpp";
  }
}


void OPTppImplementation::ComputeInequalityConstraint(int mode, int /*ndim*/, const NEWMAT::ColumnVector & x, NEWMAT::ColumnVector & fx, NEWMAT::Matrix & g, OPTPP::OptppArray<NEWMAT::SymmetricMatrix> & H, int & result)
{
  if (!P_current_opt_)
    throw InternalException(HERE) << "P_current_opt_ is null";
  OPTppImplementation *algorithm = P_current_opt_;
  const UnsignedInteger dimension = algorithm->getProblem().getDimension();
  Point inP(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    inP[i] = x(i + 1);// starts at 1!

  const Function constraint(algorithm->getProblem().getInequalityConstraint());
  const UnsignedInteger constraintDimension = constraint.getOutputDimension();

  // evaluation
  if (mode & OPTPP::NLPFunction)
  {
    const Point outP(constraint(inP));
    algorithm->inequalityConstraintHistory_.add(outP);
    for (UnsignedInteger i = 0; i < constraintDimension; ++ i)
      fx(i + 1) = outP[i];// starts at 1!
    result = OPTPP::NLPFunction;
  }

  // gradient
  if (mode & OPTPP::NLPGradient)
  {
    const Matrix grad(constraint.gradient(inP));
    for (UnsignedInteger i = 0; i < dimension; ++ i)
      for (UnsignedInteger j = 0; j < constraintDimension; ++ j)
        g(i + 1, j + 1) = grad(i, j);// starts at 1!
    result = OPTPP::NLPGradient;
  }

  if (mode & OPTPP::NLPHessian)
  {
    const SymmetricTensor hessian(constraint.hessian(inP));
    for (UnsignedInteger k = 0; k < constraintDimension; ++ k)
    {
      NEWMAT::SymmetricMatrix Htmp(dimension);
      for (UnsignedInteger i = 0; i < dimension; ++ i)
        for (UnsignedInteger j = 0; j < dimension; ++ j)
          Htmp(i + 1, j + 1) = hessian(i, j, k);// starts at 1!
      H[k] = Htmp;
    }
    result = OPTPP::NLPHessian;
  }
}


void OPTppImplementation::ComputeEqualityConstraint(int mode, int /*ndim*/, const NEWMAT::ColumnVector & x, NEWMAT::ColumnVector & fx, NEWMAT::Matrix & g, OPTPP::OptppArray<NEWMAT::SymmetricMatrix> & H, int & result)
{
  if (!P_current_opt_)
    throw InternalException(HERE) << "P_current_opt_ is null";
  OPTppImplementation *algorithm = P_current_opt_;
  const UnsignedInteger dimension = algorithm->getProblem().getDimension();
  Point inP(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    inP[i] = x(i + 1);// starts at 1!

  const Function constraint(algorithm->getProblem().getEqualityConstraint());
  const UnsignedInteger constraintDimension = constraint.getOutputDimension();

  // evaluation
  if (mode & OPTPP::NLPFunction)
  {
    const Point outP(constraint(inP));
    algorithm->equalityConstraintHistory_.add(outP);
    for (UnsignedInteger i = 0; i < constraintDimension; ++ i)
      fx(i + 1) = outP[i];// starts at 1!
    result = OPTPP::NLPFunction;
  }

  // gradient
  if (mode & OPTPP::NLPGradient)
  {
    const Matrix grad(constraint.gradient(inP));
    for (UnsignedInteger i = 0; i < dimension; ++ i)
      for (UnsignedInteger j = 0; j < constraintDimension; ++ j)
        g(i + 1, j + 1) = grad(i, j);// starts at 1!
    result = OPTPP::NLPGradient;
  }

  if (mode & OPTPP::NLPHessian)
  {
    const SymmetricTensor hessian(constraint.hessian(inP));
    for (UnsignedInteger k = 0; k < constraintDimension; ++ k)
    {
      NEWMAT::SymmetricMatrix Htmp(dimension);
      for (UnsignedInteger i = 0; i < dimension; ++ i)
        for (UnsignedInteger j = 0; j < dimension; ++ j)
          Htmp(i + 1, j + 1) = hessian(i, j, k);// starts at 1!
      H[k] = Htmp;
    }
    result = OPTPP::NLPHessian;
  }
}


OPTPP::BoundConstraint * OPTppImplementation::buildBoundConstraints()
{
  const Interval bounds(getProblem().getBounds());
  const UnsignedInteger dimension = getProblem().getDimension();
  const Interval::BoolCollection finiteLowerBound(bounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(bounds.getFiniteUpperBound());
  const Point lowerBound(bounds.getLowerBound());
  const Point upperBound(bounds.getUpperBound());
  NEWMAT::ColumnVector lb(dimension);
  NEWMAT::ColumnVector ub(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    lb(i + 1) = lowerBound[i];
    ub(i + 1) = upperBound[i];
    if (!finiteLowerBound[i]) lb(i + 1) = -SpecFunc::MaxScalar;
    if (!finiteUpperBound[i]) ub(i + 1) =  SpecFunc::MaxScalar;
  }
  OPTPP::BoundConstraint * boundConstraint = new OPTPP::BoundConstraint(dimension, lb, ub);
  return boundConstraint;
}


void OPTppImplementation::addConstraints(OPTPP::NLP0 * nlp)
{
  const UnsignedInteger dimension = getProblem().getDimension();
  OPTPP::OptppArray<OPTPP::Constraint> constraintArray;
  if (getProblem().hasBounds())
  {
    constraintArray.append(buildBoundConstraints());
  }
  if (getProblem().hasInequalityConstraint())
  {
    const UnsignedInteger inequalityDimension = getProblem().getInequalityConstraint().getOutputDimension();
    OPTPP::NLP* nlpCon = new OPTPP::NLP(new OPTPP::NLF2(dimension, inequalityDimension, OPTppImplementation::ComputeInequalityConstraint, OPTppImplementation::InitializeProblem));
    const Bool stdForm = true;
    OPTPP::Constraint inequality = new OPTPP::NonLinearInequality(nlpCon, stdForm, inequalityDimension);
    constraintArray.append(inequality);
  }
  if (getProblem().hasEqualityConstraint())
  {
    const UnsignedInteger equalityDimension = getProblem().getEqualityConstraint().getOutputDimension();
    OPTPP::NLP* nlpCon = new OPTPP::NLP(new OPTPP::NLF2(dimension, equalityDimension, OPTppImplementation::ComputeEqualityConstraint, OPTppImplementation::InitializeProblem));
    OPTPP::Constraint equality = new OPTPP::NonLinearEquation(nlpCon, equalityDimension);
    constraintArray.append(equality);
  }
  if (constraintArray.length() > 0)
  {
    OPTPP::CompoundConstraint * compoundConstraint = new OPTPP::CompoundConstraint(constraintArray);
    nlp->setConstraints(compoundConstraint);
  }
}


OPTPP::NLP0 * OPTppImplementation::buildNLP0()
{
  const UnsignedInteger dimension = getProblem().getDimension();

  OPTPP::NLP0 *nlp = new OPTPP::NLF0(dimension, OPTppImplementation::ComputeObjective, OPTppImplementation::InitializeProblem, this);
  p_nlp_ = Pointer<OPTPP::NLPBase>(nlp);
  addConstraints(nlp);
  return nlp;
}


OPTPP::NLP1 * OPTppImplementation::buildNLP1()
{
  const UnsignedInteger dimension = getProblem().getDimension();

  OPTPP::NLP1 *nlp = new OPTPP::NLF1(dimension, OPTppImplementation::ComputeObjectiveGradient, OPTppImplementation::InitializeProblem, this);
  p_nlp_ = Pointer<OPTPP::NLPBase>(nlp);
  addConstraints(nlp);
  return nlp;
}


OPTPP::NLP2 * OPTppImplementation::buildNLP2()
{
  const UnsignedInteger dimension = getProblem().getDimension();

  OPTPP::NLP2 *nlp = new OPTPP::NLF2(dimension, OPTppImplementation::ComputeObjectiveGradientHessian, OPTppImplementation::InitializeProblem, this);
  p_nlp_ = Pointer<OPTPP::NLPBase>(nlp);
  addConstraints(nlp);
  return nlp;
}


/* Static method to initialize the algorithm names/codes pairing in nlopt */
void OPTppImplementation::InitializeAlgorithmNames()
{
  if (!AlgorithmNames_.isEmpty()) return;
  AlgorithmNames_.add("OptBaNewton");
  AlgorithmNames_.add("OptBaQNewton");
  // OptBCEllipsoid can exit(0)
//   AlgorithmNames_.add("OptBCEllipsoid");
  AlgorithmNames_.add("OptBCFDNewton");
  AlgorithmNames_.add("OptBCNewton");
  AlgorithmNames_.add("OptBCQNewton");
  AlgorithmNames_.add("OptCG");
  AlgorithmNames_.add("OptConstrFDNewton");
  AlgorithmNames_.add("OptConstrNewton");
  AlgorithmNames_.add("OptConstrQNewton");
  AlgorithmNames_.add("OptDHNIPS");
  AlgorithmNames_.add("OptFDNewton");
  AlgorithmNames_.add("OptFDNIPS");
  AlgorithmNames_.add("OptGSS");
  AlgorithmNames_.add("OptLBFGS");
  AlgorithmNames_.add("OptNewton");
  AlgorithmNames_.add("OptNIPS");
  AlgorithmNames_.add("OptPDS");
  AlgorithmNames_.add("OptQNewton");
  AlgorithmNames_.add("OptQNIPS");
}

/* Static methods to access algorithm names and codes */
Description OPTppImplementation::GetAlgorithmNames()
{
  InitializeAlgorithmNames();
  return AlgorithmNames_;
}


/* Default constructor */
OPTppImplementation::OPTppImplementation()
  : OptimizationAlgorithmImplementation()
  , minimumStep_(ResourceMap::GetAsScalar("OPTpp-DefaultMinimumStep"))
  , maximumStep_(ResourceMap::GetAsScalar("OPTpp-DefaultMaximumStep"))
  , lineSearchTolerance_(ResourceMap::GetAsScalar("OPTpp-DefaultLineSearchTolerance"))
  , maximumLineSearchIterationNumber_(ResourceMap::GetAsUnsignedInteger("OPTpp-DefaultMaximumLineSearchIterationNumber"))
  , trustRegionSize_(ResourceMap::GetAsScalar("OPTpp-DefaultTrustRegionSize"))
  , p_opt_(0)
{
}


OPTppImplementation::OPTppImplementation(const OptimizationProblem & problem)
  : OptimizationAlgorithmImplementation(problem)
  , minimumStep_(ResourceMap::GetAsScalar("OPTpp-DefaultMinimumStep"))
  , maximumStep_(ResourceMap::GetAsScalar("OPTpp-DefaultMaximumStep"))
  , lineSearchTolerance_(ResourceMap::GetAsScalar("OPTpp-DefaultLineSearchTolerance"))
  , maximumLineSearchIterationNumber_(ResourceMap::GetAsUnsignedInteger("OPTpp-DefaultMaximumLineSearchIterationNumber"))
  , trustRegionSize_(ResourceMap::GetAsScalar("OPTpp-DefaultTrustRegionSize"))
  , p_opt_(0)
{
  checkProblem(problem);
}


/* Virtual constructor */
OPTppImplementation * OPTppImplementation::clone() const
{
  return new OPTppImplementation(*this);
}


/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void OPTppImplementation::checkProblem(const OptimizationProblem & problem) const
{
  if (problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << getClassName() << " does not support multi-objective optimization";
  if (problem.hasResidualFunction())
    throw InvalidArgumentException(HERE) << getClassName() << " does not support least-square problems";
  if (!problem.isContinuous())
    throw InvalidArgumentException(HERE) << getClassName() << " does not support non continuous problems";

}


Pointer<OPTPP::OptimizeClass> OPTppImplementation::instanciateSolver()
{
  throw NotYetImplementedException(HERE);
}


/* Performs the actual computation by calling the OPTpp library
 */
void OPTppImplementation::run()
{
  const UnsignedInteger dimension = getProblem().getDimension();
  Point startingPoint(getStartingPoint());
  if (startingPoint.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Invalid starting point dimension (" << startingPoint.getDimension() << "), expected " << dimension;

  if (getProblem().hasBounds())
  {
    Interval bounds(getProblem().getBounds());
    if (!bounds.contains(startingPoint))
      throw InvalidArgumentException(HERE) << "Starting point is not inside bounds x=" << startingPoint.__str__() << " bounds=" << bounds;
  }

  // initialize history
  evaluationInputHistory_ = Sample(0, dimension);
  evaluationOutputHistory_ = Sample(0, 1);
  equalityConstraintHistory_ = Sample(0, getProblem().getEqualityConstraint().getOutputDimension());
  inequalityConstraintHistory_ = Sample(0, getProblem().getInequalityConstraint().getOutputDimension());

  Pointer<OPTPP::OptimizeClass> algo = instanciateSolver();
  // generic parameters
  algo->setStepTol(getMaximumAbsoluteError());
  algo->setGradTol(getMaximumRelativeError());
  algo->setFcnTol(getMaximumResidualError());
  algo->setConTol(getMaximumConstraintError());
  algo->setMaxFeval(getMaximumEvaluationNumber());
  algo->setMaxIter(getMaximumIterationNumber());

  // optpp-specific parameters
  algo->setMaxStep(getMaximumStep());
  algo->setMinStep(getMinimumStep());
  algo->setLineSearchTol(getLineSearchTolerance());
  algo->setMaxBacktrackIter(getMaximumLineSearchIterationNumber());

  // no TRSize accessor in the top class :[ we set it here for intermediate classes
  OPTPP::OptNewtonLike * newton = dynamic_cast<OPTPP::OptNewtonLike *>(algo.get());
  if (newton)
    newton->setTRSize(getTrustRegionSize());
  OPTPP::OptBCNewtonLike * bcNewton = dynamic_cast<OPTPP::OptBCNewtonLike *>(algo.get());
  if (bcNewton)
    bcNewton->setTRSize(getTrustRegionSize());
  OPTPP::OptConstrNewtonLike * constrNewton = dynamic_cast<OPTPP::OptConstrNewtonLike *>(algo.get());
  if (constrNewton)
    constrNewton->setTRSize(getTrustRegionSize());

  p_opt_ = algo.get();
  P_current_opt_ = this;
  algo->optimize();
  p_opt_ = 0;
  algo->cleanup();

  // there is no accessor to the status message, only an undocumented return code
  // -1: Step does not satisfy sufficient decrease condition
  // -4: Maximum number of iterations or fevals
  int returnCode = algo->getReturnCode();
  LOGINFO(OSS() << "OPTpp returned the code " << returnCode);

  if (evaluationOutputHistory_.computeVariance()[0] < SpecFunc::ScalarEpsilon)
    throw InternalException(HERE) << getClassName() << " could not make progress";

  if (!p_nlp_)
    throw InternalException(HERE) << "p_nlp_ is null";
  const Scalar optimalValue = getProblem().isMinimization() ? p_nlp_->getF() : -p_nlp_->getF();
  NEWMAT::ColumnVector xc(p_nlp_->getXc());
  Point optimizer(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    optimizer[i] = xc(i + 1);// starts at 1!

  OptimizationResult result(dimension, 1);
  result.setProblem(getProblem());

  const UnsignedInteger size = evaluationInputHistory_.getSize();

  Scalar absoluteError = -1.0;
  Scalar relativeError = -1.0;
  Scalar residualError = -1.0;
  Scalar constraintError = -1.0;

  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Point inP(evaluationInputHistory_[i]);
    const Point outP(evaluationOutputHistory_[i]);
    constraintError = 0.0;
    if (getProblem().hasBounds())
    {
      const Interval bounds(getProblem().getBounds());
      for (UnsignedInteger j = 0; j < dimension; ++ j)
      {
        if (bounds.getFiniteLowerBound()[j])
          constraintError = std::max(constraintError, bounds.getLowerBound()[j] - inP[j]);
        if (bounds.getFiniteUpperBound()[j])
          constraintError = std::max(constraintError, inP[j] - bounds.getUpperBound()[j]);
      }
    }
    if (getProblem().hasEqualityConstraint())
    {
      const Point g(equalityConstraintHistory_[i]);
      constraintError = std::max(constraintError, g.normInf());
    }
    if (getProblem().hasInequalityConstraint())
    {
      Point h(inequalityConstraintHistory_[i]);
      for (UnsignedInteger k = 0; k < getProblem().getInequalityConstraint().getOutputDimension(); ++ k)
      {
        h[k] = std::min(h[k], 0.0);// convention h(x)>=0 <=> admissibility
      }
      constraintError = std::max(constraintError, h.normInf());
    }
    if (i > 0)
    {
      const Point inPM(evaluationInputHistory_[i - 1]);
      const Point outPM(evaluationOutputHistory_[i - 1]);
      absoluteError = (inP - inPM).normInf();
      relativeError = (inP.normInf() > 0.0) ? (absoluteError / inP.normInf()) : -1.0;
      residualError = (std::abs(outP[0]) > 0.0) ? (std::abs(outP[0] - outPM[0]) / std::abs(outP[0])) : -1.0;
    }
    result.store(inP, outP, absoluteError, relativeError, residualError, constraintError);
  }

  result.setEvaluationNumber(size);
  result.setOptimalPoint(optimizer);
  result.setOptimalValue(Point(1, optimalValue));
  result.setLagrangeMultipliers(computeLagrangeMultipliers(optimizer));
  setResult(result);
}


/* Minimum step accessor */
void OPTppImplementation::setMinimumStep(const Scalar minimumStep)
{
  minimumStep_ = minimumStep;
}

Scalar OPTppImplementation::getMinimumStep() const
{
  return minimumStep_;
}

/* Maximum step accessor */
void OPTppImplementation::setMaximumStep(const Scalar maximumStep)
{
  maximumStep_ = maximumStep;
}

Scalar OPTppImplementation::getMaximumStep() const
{
  return maximumStep_;
}

/* Maximum line search tolerance accessor */
void OPTppImplementation::setLineSearchTolerance(const Scalar lineSearchTolerance)
{
  lineSearchTolerance_ = lineSearchTolerance;
}

Scalar OPTppImplementation::getLineSearchTolerance() const
{
  return lineSearchTolerance_;
}

/* Maximum line search iteration number accessor */
void OPTppImplementation::setMaximumLineSearchIterationNumber(const UnsignedInteger maximumLineSearchIterationNumber)
{
  maximumLineSearchIterationNumber_ = maximumLineSearchIterationNumber;
}

UnsignedInteger OPTppImplementation::getMaximumLineSearchIterationNumber() const
{
  return maximumLineSearchIterationNumber_;
}

/* Trust region size accessor */
void OPTppImplementation::setTrustRegionSize(const Scalar trustRegionSize)
{
  trustRegionSize_ = trustRegionSize;
}

Scalar OPTppImplementation::getTrustRegionSize() const
{
  return trustRegionSize_;
}

/* String converter */
String OPTppImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " " << OptimizationAlgorithmImplementation::__repr__();
  return oss;
}

/* String converter */
String OPTppImplementation::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << getClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void OPTppImplementation::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("minimumStep_", minimumStep_);
  adv.saveAttribute("maximumStep_", maximumStep_);
  adv.saveAttribute("lineSearchTolerance_", lineSearchTolerance_);
  adv.saveAttribute("maximumLineSearchIterationNumber_", maximumLineSearchIterationNumber_);
  adv.saveAttribute("trustRegionSize_", trustRegionSize_);
}

/* Method load() reloads the object from the StorageManager */
void OPTppImplementation::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  adv.loadAttribute("minimumStep_", minimumStep_);
  adv.loadAttribute("maximumStep_", maximumStep_);
  adv.loadAttribute("lineSearchTolerance_", lineSearchTolerance_);
  adv.loadAttribute("maximumLineSearchIterationNumber_", maximumLineSearchIterationNumber_);
  adv.loadAttribute("trustRegionSize_", trustRegionSize_);
}

#define IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(CName) \
CLASSNAMEINIT(CName) \
static const Factory<CName> Factory_ ## CName; \
CName::CName() : OPTppImplementation() {} \
CName::CName(const OptimizationProblem & problem) : OPTppImplementation(problem) {} \
CName * CName::clone() const { return new CName(*this); }

IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptBaNewton)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptBaQNewton)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptBCEllipsoid)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptBCFDNewton)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptBCNewton)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptBCQNewton)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptCG)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptConstrFDNewton)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptConstrNewton)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptConstrQNewton)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptDHNIPS)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptFDNewton)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptFDNIPS)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptLBFGS)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptNewton)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptNIPS)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptPDS)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptQNewton)
IMPLEMENT_OPTPP_SOLVER_IMPLEMENTATION(OptQNIPS)

#define IMPLEMENT_NLP1_INSTANTIATION(CName) \
Pointer<OPTPP::OptimizeClass> CName::instanciateSolver() { \
  Pointer<OPTPP::OptimizeClass> solver = new OPTPP::CName(buildNLP1()); \
  return solver; }
#define IMPLEMENT_NLP2_INSTANTIATION(CName) \
Pointer<OPTPP::OptimizeClass> CName::instanciateSolver() { \
  Pointer<OPTPP::OptimizeClass> solver = new OPTPP::CName(buildNLP2()); \
  return solver; }

IMPLEMENT_NLP2_INSTANTIATION(OptBaNewton)
IMPLEMENT_NLP2_INSTANTIATION(OptBaQNewton)
IMPLEMENT_NLP1_INSTANTIATION(OptBCEllipsoid)
IMPLEMENT_NLP1_INSTANTIATION(OptBCFDNewton)
IMPLEMENT_NLP2_INSTANTIATION(OptBCNewton)
IMPLEMENT_NLP1_INSTANTIATION(OptBCQNewton)
IMPLEMENT_NLP1_INSTANTIATION(OptCG)
IMPLEMENT_NLP1_INSTANTIATION(OptConstrFDNewton)
IMPLEMENT_NLP2_INSTANTIATION(OptConstrNewton)
IMPLEMENT_NLP1_INSTANTIATION(OptConstrQNewton)
IMPLEMENT_NLP2_INSTANTIATION(OptDHNIPS)
IMPLEMENT_NLP1_INSTANTIATION(OptFDNewton)
IMPLEMENT_NLP1_INSTANTIATION(OptFDNIPS)
IMPLEMENT_NLP1_INSTANTIATION(OptLBFGS)
IMPLEMENT_NLP2_INSTANTIATION(OptNewton)
IMPLEMENT_NLP2_INSTANTIATION(OptNIPS)

Pointer<OPTPP::OptimizeClass> OptPDS::instanciateSolver()
{
  OPTPP::OptPDS * solver = new OPTPP::OptPDS(buildNLP1());
  solver->setTRSize(getTrustRegionSize());
  return solver;
}

IMPLEMENT_NLP1_INSTANTIATION(OptQNewton)
IMPLEMENT_NLP1_INSTANTIATION(OptQNIPS)

#define IMPLEMENT_CHECKPROBLEM_GENERAL(CName) \
void CName::checkProblem(const OptimizationProblem & problem) const { \
  OPTppImplementation::checkProblem(problem); }

#define IMPLEMENT_CHECKPROBLEM_BOUNDS(CName) \
void CName::checkProblem(const OptimizationProblem & problem) const { \
  OPTppImplementation::checkProblem(problem); \
  if (problem.hasInequalityConstraint()) \
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support inequality constraints"; \
  if (problem.hasEqualityConstraint()) \
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support equality constraints"; \
}

#define IMPLEMENT_CHECKPROBLEM_FORCEBOUNDS(CName) \
void CName::checkProblem(const OptimizationProblem & problem) const { \
  OPTppImplementation::checkProblem(problem); \
  if (!problem.hasBounds()) \
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " requires bound constraints"; \
  if (problem.hasInequalityConstraint()) \
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support inequality constraints"; \
  if (problem.hasEqualityConstraint()) \
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support equality constraints"; \
}

#define IMPLEMENT_CHECKPROBLEM_UNCONSTRAINED(CName) \
void CName::checkProblem(const OptimizationProblem & problem) const { \
  OPTppImplementation::checkProblem(problem); \
  if (problem.hasBounds()) \
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support bound constraints"; \
  if (problem.hasInequalityConstraint()) \
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support inequality constraints"; \
  if (problem.hasEqualityConstraint()) \
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support equality constraints"; \
}

IMPLEMENT_CHECKPROBLEM_FORCEBOUNDS(OptBaNewton)
IMPLEMENT_CHECKPROBLEM_FORCEBOUNDS(OptBaQNewton)
IMPLEMENT_CHECKPROBLEM_FORCEBOUNDS(OptBCEllipsoid)
IMPLEMENT_CHECKPROBLEM_UNCONSTRAINED(OptBCFDNewton)
IMPLEMENT_CHECKPROBLEM_FORCEBOUNDS(OptBCNewton)
IMPLEMENT_CHECKPROBLEM_FORCEBOUNDS(OptBCQNewton)
IMPLEMENT_CHECKPROBLEM_UNCONSTRAINED(OptCG)
IMPLEMENT_CHECKPROBLEM_UNCONSTRAINED(OptConstrFDNewton)
IMPLEMENT_CHECKPROBLEM_UNCONSTRAINED(OptConstrNewton)
IMPLEMENT_CHECKPROBLEM_UNCONSTRAINED(OptConstrQNewton)
IMPLEMENT_CHECKPROBLEM_BOUNDS(OptDHNIPS)
IMPLEMENT_CHECKPROBLEM_UNCONSTRAINED(OptFDNewton)
IMPLEMENT_CHECKPROBLEM_GENERAL(OptFDNIPS)
IMPLEMENT_CHECKPROBLEM_UNCONSTRAINED(OptLBFGS)
IMPLEMENT_CHECKPROBLEM_UNCONSTRAINED(OptNewton)
IMPLEMENT_CHECKPROBLEM_GENERAL(OptNIPS)
IMPLEMENT_CHECKPROBLEM_UNCONSTRAINED(OptPDS)
IMPLEMENT_CHECKPROBLEM_UNCONSTRAINED(OptQNewton)
IMPLEMENT_CHECKPROBLEM_GENERAL(OptQNIPS)

END_NAMESPACE_OPENTURNS

