//                                               -*- C++ -*-
/**
 *  @brief AbdoRackwitz is an actual implementation for
 *         OptimizationAlgorithmImplementation using the AbdoRackwitz algorithm.
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>
#include "openturns/AbdoRackwitz.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AbdoRackwitz)

static const Factory<AbdoRackwitz> Factory_AbdoRackwitz;

/* Default constructor */
AbdoRackwitz::AbdoRackwitz()
  : OptimizationAlgorithmImplementation()
  , tau_(ResourceMap::GetAsScalar("AbdoRackwitz-DefaultTau"))
  , omega_(ResourceMap::GetAsScalar("AbdoRackwitz-DefaultOmega"))
  , smooth_(ResourceMap::GetAsScalar("AbdoRackwitz-DefaultSmooth"))
{
  initialize();
}

AbdoRackwitz::AbdoRackwitz (const OptimizationProblem & problem,
                            const Scalar tau,
                            const Scalar omega,
                            const Scalar smooth)
  : OptimizationAlgorithmImplementation(problem)
  , tau_(tau)
  , omega_(omega)
  , smooth_(smooth)
{
  initialize();
  checkProblem(problem);
}


AbdoRackwitz::AbdoRackwitz(const OptimizationProblem & problem)
  : OptimizationAlgorithmImplementation(problem)
  , tau_(ResourceMap::GetAsScalar("AbdoRackwitz-DefaultTau"))
  , omega_(ResourceMap::GetAsScalar("AbdoRackwitz-DefaultOmega"))
  , smooth_(ResourceMap::GetAsScalar("AbdoRackwitz-DefaultSmooth"))
{
  initialize();
  checkProblem(problem);
}

/* Virtual constructor */
AbdoRackwitz * AbdoRackwitz::clone() const
{
  return new AbdoRackwitz(*this);
}

void AbdoRackwitz::initialize()
{
  currentSigma_ = 0.0;
  currentLevelValue_ = 0.0;
  currentLambda_ = 0.0;
}

/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void AbdoRackwitz::checkProblem(const OptimizationProblem & problem) const
{
  if (!problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "Error : " << this->getClassName() << " can only solve nearest-point optimization problems";
  if (problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support multi-objective optimization ";
  if (problem.hasBounds())
    throw InvalidArgumentException(HERE) << "Error : " << this->getClassName() << " cannot solve bound-constrained optimization problems";
  if (!problem.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support non continuous problems";

}

/* Line search for globalization of the algorithm */
Scalar AbdoRackwitz::computeLineSearch()
{
  /* Logal copy of the level function and the level value */
  const Function levelFunction(getProblem().getLevelFunction());
  const Scalar levelValue = getProblem().getLevelValue();
  /* Actualize sigma */
  currentSigma_ = std::max(currentSigma_ + 1.0, smooth_ * currentPoint_.norm() / currentGradient_.norm());
  /* Compute penalized scalar objective function at current point */
  const Scalar currentTheta = 0.5 * currentPoint_.normSquare() + currentSigma_ * std::abs(currentLevelValue_ - levelValue);
  /* Min bound for step */
  const Scalar minStep = getMaximumAbsoluteError() / currentDirection_.norm();
  /* Minimum decrease for the penalized objective function */
  const Scalar levelIncrement = omega_ * currentDirection_.dot(currentPoint_ + (currentSigma_ * ((currentLevelValue_ > levelValue) ? 1.0 : -1.0)) * currentGradient_);
  /* Initialization of the line search */
  /* We start with step=1 */
  Scalar step = 1.0;
  Point currentStepPoint;
  Scalar currentStepLevelValue = -1.0;
  Scalar currentStepTheta = -1.0;
  do
  {
    currentStepPoint = currentPoint_ + step * currentDirection_;
    currentStepLevelValue = levelFunction(currentStepPoint)[0];

    currentStepTheta = 0.5 * currentStepPoint.normSquare() + currentSigma_ * std::abs(currentStepLevelValue - levelValue);
    LOGDEBUG(OSS() << "line search step=" << step << " currentStepPoint=" << currentStepPoint << " currentStepLevelValue=" << currentStepLevelValue << " currentStepTheta=" << currentStepTheta);
    step *= tau_;
  }
  while ((step >= minStep) && ( currentStepTheta > currentTheta + step * levelIncrement));
  currentPoint_ = currentStepPoint;
  currentLevelValue_ = currentStepLevelValue;
  /* We went one step beyond */
  return step / tau_;
}


/* Performs the actual computation by using the AbdoRackwitz algorithm
 */
void AbdoRackwitz::run()
{
  initialize();

  /* Get a local copy of the level function */
  const Function levelFunction(getProblem().getLevelFunction());
  /* Get a local copy of the level value */
  const Scalar levelValue = getProblem().getLevelValue();
  /* Current point -> u */
  currentPoint_ = getStartingPoint();
  Bool exitLoop = false;
  UnsignedInteger iterationNumber = 0;
  const UnsignedInteger initialEvaluationNumber = levelFunction.getEvaluationCallsNumber();

  Scalar absoluteError = -1.0;
  Scalar constraintError = -1.0;
  Scalar relativeError = -1.0;
  Scalar residualError = -1.0;

  /* Compute the level function at the current point -> G */
  currentLevelValue_ = levelFunction(currentPoint_)[0];

  UnsignedInteger evaluationNumber = levelFunction.getEvaluationCallsNumber() - initialEvaluationNumber;

  // reset result
  result_ = OptimizationResult(getProblem());
  result_.store(currentPoint_, Point(1, currentLevelValue_), absoluteError, relativeError, residualError, constraintError);

  while ((!exitLoop) && (iterationNumber <= getMaximumIterationNumber()) && (evaluationNumber <= getMaximumEvaluationNumber()))
  {
    /* Go to next iteration */
    ++ iterationNumber;

    /* Compute the level function gradient at the current point -> Grad(G) */
    currentGradient_ = levelFunction.gradient(currentPoint_) * Point(1, 1.0);
    LOGDEBUG(OSS() << "current point=" << currentPoint_ << " current level value=" << currentLevelValue_ << " current gradient=" << currentGradient_);
    /* Compute the current Lagrange multiplier */
    const Scalar normGradientSquared = currentGradient_.normSquare();
    /* In case of a null gradient, throw an internal exception */
    if (!(normGradientSquared > 0))
    {
      throw InternalException(HERE) << "Error in Abdo Rackwitz algorithm: the gradient of the level function is zero at point u=" << currentPoint_;
    }
    /* Lambda = (G - <Grad(G), u>) / ||Grad(G)||^2 */
    currentLambda_ = (currentLevelValue_ - levelValue - currentGradient_.dot(currentPoint_)) / normGradientSquared;
    /* Compute the current direction Du = -Lambda Grad(G) - u */
    /* Be careful! currentGradient_ is an n by 1 matrix, we must multiply it by a 1 by 1
     * vector in order to get an n dimensional equivalente vector
     */
    currentDirection_ = -currentLambda_ * currentGradient_ - currentPoint_;
    /* Perform a line search in the given direction */
    const Scalar alpha = computeLineSearch();

    // update number of evaluations
    evaluationNumber = levelFunction.getEvaluationCallsNumber() - initialEvaluationNumber;

    /* Check if convergence has been achieved */
    absoluteError = std::abs(alpha) * currentDirection_.norm();
    constraintError = std::abs(currentLevelValue_ - levelValue);
    const Scalar pointNorm = currentPoint_.norm();
    if (pointNorm > 0.0)
    {
      relativeError = absoluteError / pointNorm;
    }
    else
    {
      relativeError = -1.0;
    }
    residualError = (currentPoint_ + currentLambda_ * currentGradient_).norm();
    exitLoop = ((absoluteError < getMaximumAbsoluteError()) && (relativeError < getMaximumRelativeError())) || ((residualError < getMaximumResidualError()) && (constraintError < getMaximumConstraintError()));

    // update result
    result_.setEvaluationNumber(evaluationNumber);
    result_.setIterationNumber(iterationNumber);
    result_.store(currentPoint_, Point(1, currentLevelValue_), absoluteError, relativeError, residualError, constraintError);

    LOGINFO(getResult().__repr__());

    // callbacks
    if (progressCallback_.first)
    {
      progressCallback_.first((100.0 * evaluationNumber) / getMaximumEvaluationNumber(), progressCallback_.second);
    }
    if (stopCallback_.first)
    {
      Bool stop = stopCallback_.first(stopCallback_.second);
      if (stop)
      {
        exitLoop = true;
        LOGWARN(OSS() << "AbdoRackwitz was stopped by user");
      }
    }
  }

  /* Check if we converged */
  if (!exitLoop)
  {
    LOGWARN(OSS() << "Warning! The AbdoRackwitz algorithm failed to converge after " << iterationNumber << " iterations, " << evaluationNumber << " evaluations." );
  }
} // run()


/* Tau accessor */
Scalar AbdoRackwitz::getTau() const
{
  return tau_;
}

void AbdoRackwitz::setTau(const Scalar tau)
{
  tau_ = tau;
}

/* Omega accessor */
Scalar AbdoRackwitz::getOmega() const
{
  return omega_;
}

void AbdoRackwitz::setOmega(const Scalar omega)
{
  omega_ = omega;
}

/* Smooth accessor */
Scalar AbdoRackwitz::getSmooth() const
{
  return smooth_;
}

void AbdoRackwitz::setSmooth(const Scalar smooth)
{
  smooth_ = smooth;
}

/* String converter */
String AbdoRackwitz::__repr__() const
{
  OSS oss;
  oss << "class=" << AbdoRackwitz::GetClassName()
      << " " << OptimizationAlgorithmImplementation::__repr__()
      << " tau=" << tau_
      << " omega=" << omega_
      << " smooth=" << smooth_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void AbdoRackwitz::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("tau_", tau_);
  adv.saveAttribute("omega_", omega_);
  adv.saveAttribute("smooth_", smooth_);
  adv.saveAttribute("currentSigma_", currentSigma_);
  adv.saveAttribute("currentPoint_", currentPoint_);
  adv.saveAttribute("currentDirection_", currentDirection_);
  adv.saveAttribute("currentLevelValue_", currentLevelValue_);
  adv.saveAttribute("currentGradient_", currentGradient_);
  adv.saveAttribute("currentLambda_", currentLambda_);
}

/* Method load() reloads the object from the StorageManager */
void AbdoRackwitz::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  adv.loadAttribute("tau_", tau_);
  adv.loadAttribute("omega_", omega_);
  adv.loadAttribute("smooth_", smooth_);
  adv.loadAttribute("currentSigma_", currentSigma_);
  adv.loadAttribute("currentPoint_", currentPoint_);
  adv.loadAttribute("currentDirection_", currentDirection_);
  adv.loadAttribute("currentLevelValue_", currentLevelValue_);
  adv.loadAttribute("currentLambda_", currentLambda_);
}

END_NAMESPACE_OPENTURNS
