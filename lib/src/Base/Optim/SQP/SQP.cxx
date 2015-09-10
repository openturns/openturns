//                                               -*- C++ -*-
/**
 *  @brief SQP is an actual implementation for
 *         OptimizationSolverImplementation using the SQP algorithm.
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
#include <sstream>
#include <cmath>

#include "SQP.hxx"
#include "Log.hxx"
#include "PersistentObjectFactory.hxx"
#include "OptimizationProblem.hxx"

BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(SQP);

static Factory<SQP> RegisteredFactory("SQP");

/* Default constructor */
SQP::SQP():
  OptimizationSolverImplementation()
{
  initialize();
}

SQP::SQP(const OptimizationProblem & problem)
  : OptimizationSolverImplementation(problem)
  , specificParameters_()
{
  initialize();
}

/*
 * @brief  Standard constructor: the problem is defined by a scalar valued function  (in fact, a 1-D vector valued fonction)
 *         and a level value
 */
SQP::SQP(const SQPSpecificParameters & specificParameters,
         const OptimizationProblem & problem)
  : OptimizationSolverImplementation(problem)
  , specificParameters_(specificParameters)
{
  initialize();
}

/* Virtual constructor */
SQP * SQP::clone() const
{
  return new SQP(*this);
}

/** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void SQP::checkProblem(const OptimizationProblem & problem) const
{
  if (!problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "Error : " << SQP::GetClassName() << " can only solve nearest-point optimization problems";
  if ( problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support MultiOjective Optimization ";
}

void SQP::initialize()
{
  currentSigma_ = 0.0;
  currentLevelValue_ = 0.0;
  currentLambda_ = 0.0;
}

/* Line search for globalization of the algorithm */
NumericalScalar SQP::computeLineSearch()
{
  /* Local copy of line search parameters */
  const NumericalScalar tau(specificParameters_.getTau());
  const NumericalScalar omega(specificParameters_.getOmega());
  const NumericalScalar smooth(specificParameters_.getSmooth());
  /* Local copy of the level function and the level value */
  const NumericalMathFunction levelFunction(getLevelFunction());
  const NumericalScalar levelValue(getLevelValue());
  /* Actualize sigma */
  currentSigma_ = std::max(currentSigma_ + 1.0, smooth * currentPoint_.norm() / currentGradient_.norm());
  /* Compute penalized scalar objective function at current point */
  NumericalScalar currentTheta(0.5 * currentPoint_.normSquare() + currentSigma_ * fabs(currentLevelValue_ - levelValue));
  /* Min bound for step */
  const NumericalScalar minStep(getMaximumAbsoluteError() / currentDirection_.norm());
  /* Minimum decrease for the penalized objective function */
  const NumericalScalar levelIncrement(omega * dot(currentPoint_ + (currentSigma_ * ((currentLevelValue_ > levelValue) ? 1.0 : -1.0)) * currentGradient_, currentDirection_));
  /* Initialization of the line search */
  /* We start with step=1 */
  NumericalScalar step(1.0);
  NumericalPoint currentStepPoint(currentPoint_.getDimension());
  NumericalScalar currentStepLevelValue;
  NumericalScalar currentStepTheta;

  do
  {
    currentStepPoint = currentPoint_ + step * currentDirection_;
    currentStepLevelValue = levelFunction(currentStepPoint)[0];

    currentStepTheta = 0.5 * currentStepPoint.normSquare() + currentSigma_ * fabs(currentStepLevelValue - levelValue);
    if (getVerbose()) LOGINFO(OSS() << "line search step=" << step << " currentStepPoint=" << currentStepPoint << " currentStepLevelValue=" << currentStepLevelValue << " currentStepTheta=" << currentStepTheta);
    step *= tau;
  }

  while ((step >= minStep) && ( currentStepTheta > currentTheta + step * levelIncrement));

  currentPoint_ = currentStepPoint;

  currentLevelValue_ = currentStepLevelValue;

  /* We went one step beyond */
  return step / tau;
}


/* Performs the actual computation by using the SQP algorithm
 */
void SQP::run()
{
  initialize();
  //system and direction initialization :
  /* Current point -> u */
  currentPoint_ = getStartingPoint();
  const UnsignedInteger dimension(currentPoint_.getDimension());
  currentSystemMatrix_ = SymmetricMatrix(dimension + 1);
  currentSecondMember_ = NumericalPoint(dimension + 1);
  currentDirection_ = NumericalPoint(dimension);


  /* Get a local copy of the level function */
  const NumericalMathFunction levelFunction(getLevelFunction());
  /* Get a local copy of the level value */
  const NumericalScalar levelValue(getLevelValue());

  //Initialize the hessian
  currentHessian_ = levelFunction.hessian(currentPoint_).getSheet(0);


  Bool convergence(false);
  UnsignedInteger iterationNumber = 0;
  NumericalScalar absoluteError(-1.0);
  NumericalScalar constraintError(-1.0);
  NumericalScalar relativeError(-1.0);
  NumericalScalar residualError(-1.0);

  /* Compute the level function at the current point -> G */
  currentLevelValue_ = levelFunction(currentPoint_)[0];

  // reset result
  setResult(OptimizationSolverImplementationResult(currentPoint_, NumericalPoint(1, currentLevelValue_), 0, absoluteError, relativeError, residualError, constraintError));

  while ( (!convergence) && (iterationNumber <= getMaximumIterationsNumber()) )
  {
    /* Go to next iteration */
    ++ iterationNumber;

    /* Compute the level function gradient at the current point -> Grad(G) */
    currentGradient_ = levelFunction.gradient(currentPoint_) * NumericalPoint(1, 1.0);
    /* Compute the current Lagrange multiplier */
    const NumericalScalar normGradientSquared(currentGradient_.normSquare());
    /* In case of a null gradient, throw an internal exception */

    if (normGradientSquared == 0)
    {
      result_.update(currentPoint_, iterationNumber);
      throw InternalException(HERE) << "Error in Abdo SQP algorithm: the gradient of the level function is zero at point u=" << currentPoint_;
    }

    //compute System matrix for evaluation of the direction

    for ( UnsignedInteger i = 0; i < currentPoint_.getDimension(); ++i )
    {
      for ( UnsignedInteger j = 0; j < i + 1; ++j )
      {
        currentSystemMatrix_ ( i, j ) = currentLambda_ * currentHessian_ ( i, j );
      }

      currentSystemMatrix_ ( i, i) += 2;

      currentSystemMatrix_ ( i, currentPoint_.getDimension() ) = currentGradient_[i];
    }

    //compute System second member

    for ( UnsignedInteger i = 0; i < currentPoint_.getDimension(); ++i )
    {
      currentSecondMember_[i] = - currentPoint_[i];
    }

    currentSecondMember_[currentPoint_.getDimension()] = - currentLevelValue_ + levelValue;

    //solve the linear system
    const NumericalPoint Solution ( currentSystemMatrix_.solveLinearSystem ( currentSecondMember_ ) );

    for ( UnsignedInteger i = 0; i < currentPoint_.getDimension(); ++i )
    {
      currentDirection_[i] = Solution[i];
    }

    currentLambda_ = Solution[ currentPoint_.getDimension() ];

    /* Perform a line search in the given direction */
    const NumericalScalar alpha(computeLineSearch());

    /* Check if convergence has been achieved */
    absoluteError = fabs(alpha) * currentDirection_.norm();
    constraintError = fabs(currentLevelValue_ - levelValue);
    const NumericalScalar pointNorm(currentPoint_.norm());

    if (pointNorm > 0.0)
    {
      relativeError = absoluteError / pointNorm;
    }

    else
    {
      relativeError = -1.0;
    }

    residualError = (currentPoint_ + currentLambda_ * currentGradient_).norm();

    convergence = ((absoluteError < getMaximumAbsoluteError()) && (relativeError < getMaximumRelativeError())) || ((residualError < getMaximumResidualError()) && (constraintError < getMaximumConstraintError()));

    // update result
    result_.update(currentPoint_, iterationNumber);
    result_.store(currentPoint_, NumericalPoint(1, currentLevelValue_), absoluteError, relativeError, residualError, constraintError);
    LOGINFO( getResult().__repr__() );
  }

  /* Check if we converged */

  if (!convergence)
  {
    LOGWARN(OSS() << "Warning! The SQP algorithm failed to converge after " << getMaximumIterationsNumber() << " iterations");
  }
} // run()

/* Specific parameters accessor */
SQPSpecificParameters SQP::getSpecificParameters() const
{
  return specificParameters_;
}

/* Specific parameters accessor */
void SQP::setSpecificParameters(const SQPSpecificParameters & specificParameters)
{
  specificParameters_ = specificParameters;
}

/* Level function accessor */
NumericalMathFunction SQP::getLevelFunction() const
{
  return getProblem().getLevelFunction();
}

/* Level function accessor */
void SQP::setLevelFunction(const NumericalMathFunction & levelFunction)
{
  getProblem().setLevelFunction(levelFunction);
}

/* Level value accessor */
NumericalScalar SQP::getLevelValue() const
{
  return getProblem().getLevelValue();
}

/* Level value accessor */
void SQP::setLevelValue(const NumericalScalar levelValue)
{
  getProblem().setLevelValue(levelValue);
}

/* String converter */
String SQP::__repr__() const
{
  OSS oss;
  oss << "class=" << SQP::GetClassName()
      << " " << OptimizationSolverImplementation::__repr__()
      << " specificParameters=" << getSpecificParameters();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SQP::save(Advocate & adv) const
{
  OptimizationSolverImplementation::save(adv);
  adv.saveAttribute("specificParameters_", specificParameters_);
  adv.saveAttribute("currentSigma_", currentSigma_);
  adv.saveAttribute("currentPoint_", currentPoint_);
  adv.saveAttribute("currentDirection_", currentDirection_);
  adv.saveAttribute("currentLevelValue_", currentLevelValue_);
  adv.saveAttribute("currentGradient_", currentGradient_);
  adv.saveAttribute("currentHessian_", currentHessian_);
  adv.saveAttribute("currentSystemMatrix_", currentSystemMatrix_);
  adv.saveAttribute("currentSecondMember_", currentSecondMember_);
  adv.saveAttribute("currentLambda_", currentLambda_);
}

/* Method load() reloads the object from the StorageManager */
void SQP::load(Advocate & adv)
{
  OptimizationSolverImplementation::load(adv);
  adv.loadAttribute("specificParameters_", specificParameters_);
  adv.loadAttribute("currentSigma_", currentSigma_);
  adv.loadAttribute("currentPoint_", currentPoint_);
  adv.loadAttribute("currentDirection_", currentDirection_);
  adv.loadAttribute("currentLevelValue_", currentLevelValue_);
  adv.loadAttribute("currentGradient_", currentGradient_);
  adv.loadAttribute("currentHessian_", currentHessian_);
  adv.loadAttribute("currentSystemMatrix_", currentSystemMatrix_);
  adv.loadAttribute("currentSecondMember_", currentSecondMember_);
  adv.loadAttribute("currentLambda_", currentLambda_);
}

END_NAMESPACE_OPENTURNS
