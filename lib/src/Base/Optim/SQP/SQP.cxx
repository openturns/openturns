//                                               -*- C++ -*-
/**
 *  @brief SQP is an actual implementation for
 *         OptimizationSolverImplementation using the SQP algorithm.
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
#include <sstream>
#include <cmath>

#include "openturns/SQP.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OptimizationProblem.hxx"

BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(SQP);

static const Factory<SQP> Factory_SQP;

/* Default constructor */
SQP::SQP()
  : OptimizationSolverImplementation()
  , tau_(ResourceMap::GetAsNumericalScalar("SQP-DefaultTau"))
  , omega_(ResourceMap::GetAsNumericalScalar("SQP-DefaultOmega"))
  , smooth_(ResourceMap::GetAsNumericalScalar("SQP-DefaultSmooth"))
{
  initialize();
}

SQP::SQP(const OptimizationProblem & problem)
  : OptimizationSolverImplementation(problem)
  , tau_(ResourceMap::GetAsNumericalScalar("SQP-DefaultTau"))
  , omega_(ResourceMap::GetAsNumericalScalar("SQP-DefaultOmega"))
  , smooth_(ResourceMap::GetAsNumericalScalar("SQP-DefaultSmooth"))
{
  initialize();
}

SQP::SQP (const OptimizationProblem & problem,
          const NumericalScalar tau,
          const NumericalScalar omega,
          const NumericalScalar smooth)
  : OptimizationSolverImplementation(problem)
  , tau_(tau)
  , omega_(omega)
  , smooth_(smooth)
{
  initialize();
}

/* Virtual constructor */
SQP * SQP::clone() const
{
  return new SQP(*this);
}

/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void SQP::checkProblem(const OptimizationProblem & problem) const
{
  if (!problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "Error : " << this->getClassName() << " can only solve nearest-point optimization problems";
  if (problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support multi-objective optimization";
  if (problem.hasBounds())
    throw InvalidArgumentException(HERE) << "Error : " << this->getClassName() << " cannot solve bound-constrained optimization problems";
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
  /* Local copy of the level function and the level value */
  const NumericalMathFunction levelFunction(getProblem().getLevelFunction());
  const NumericalScalar levelValue = getProblem().getLevelValue();
  /* Actualize sigma */
  currentSigma_ = std::max(currentSigma_ + 1.0, smooth_ * currentPoint_.norm() / currentGradient_.norm());
  /* Compute penalized scalar objective function at current point */
  NumericalScalar currentTheta = 0.5 * currentPoint_.normSquare() + currentSigma_ * std::abs(currentLevelValue_ - levelValue);
  /* Min bound for step */
  const NumericalScalar minStep = getMaximumAbsoluteError() / currentDirection_.norm();
  /* Minimum decrease for the penalized objective function */
  const NumericalScalar levelIncrement = omega_ * dot(currentPoint_ + (currentSigma_ * ((currentLevelValue_ > levelValue) ? 1.0 : -1.0)) * currentGradient_, currentDirection_);
  /* Initialization of the line search */
  /* We start with step=1 */
  NumericalScalar step = 1.0;
  NumericalPoint currentStepPoint(currentPoint_.getDimension());
  NumericalScalar currentStepLevelValue = -1.0;
  NumericalScalar currentStepTheta = -1.0;

  do
  {
    currentStepPoint = currentPoint_ + step * currentDirection_;
    currentStepLevelValue = levelFunction(currentStepPoint)[0];

    currentStepTheta = 0.5 * currentStepPoint.normSquare() + currentSigma_ * std::abs(currentStepLevelValue - levelValue);
    if (getVerbose()) LOGINFO(OSS() << "line search step=" << step << " currentStepPoint=" << currentStepPoint << " currentStepLevelValue=" << currentStepLevelValue << " currentStepTheta=" << currentStepTheta);
    step *= tau_;
  }

  while ((step >= minStep) && ( currentStepTheta > currentTheta + step * levelIncrement));

  currentPoint_ = currentStepPoint;

  currentLevelValue_ = currentStepLevelValue;

  /* We went one step beyond */
  return step / tau_;
}


/* Performs the actual computation by using the SQP algorithm
 */
void SQP::run()
{
  initialize();
  //system and direction initialization :
  /* Current point -> u */
  currentPoint_ = getStartingPoint();
  const UnsignedInteger dimension = currentPoint_.getDimension();
  currentSystemMatrix_ = SymmetricMatrix(dimension + 1);
  currentSecondMember_ = NumericalPoint(dimension + 1);
  currentDirection_ = NumericalPoint(dimension);


  /* Get a local copy of the level function */
  const NumericalMathFunction levelFunction(getProblem().getLevelFunction());
  /* Get a local copy of the level value */
  const NumericalScalar levelValue = getProblem().getLevelValue();

  //Initialize the hessian
  currentHessian_ = levelFunction.hessian(currentPoint_).getSheet(0);


  Bool convergence = false;
  UnsignedInteger iterationNumber = 0;
  NumericalScalar absoluteError = -1.0;
  NumericalScalar constraintError = -1.0;
  NumericalScalar relativeError = -1.0;
  NumericalScalar residualError = -1.0;

  /* Compute the level function at the current point -> G */
  currentLevelValue_ = levelFunction(currentPoint_)[0];

  // reset result
  result_ = OptimizationResult();
  result_.setProblem(getProblem());
  result_.store(currentPoint_, NumericalPoint(1, currentLevelValue_), absoluteError, relativeError, residualError, constraintError);

  while ( (!convergence) && (iterationNumber <= getMaximumIterationNumber()) )
  {
    /* Go to next iteration */
    ++iterationNumber;

    /* Compute the level function gradient at the current point -> Grad(G) */
    currentGradient_ = levelFunction.gradient(currentPoint_) * NumericalPoint(1, 1.0);
    /* Compute the current Lagrange multiplier */
    const NumericalScalar normGradientSquared = currentGradient_.normSquare();
    /* In case of a null gradient, throw an internal exception */

    if (normGradientSquared == 0)
    {
      throw InternalException(HERE) << "Error in Abdo SQP algorithm: the gradient of the level function is zero at point u=" << currentPoint_;
    }

    //compute System matrix for evaluation of the direction

    for ( UnsignedInteger i = 0; i < dimension; ++i )
    {
      for ( UnsignedInteger j = 0; j < i + 1; ++j )
      {
        currentSystemMatrix_( i, j ) = currentLambda_ * currentHessian_( i, j );
      }

      currentSystemMatrix_( i, i) += 2;

      currentSystemMatrix_( i, dimension ) = currentGradient_[i];
    }

    //compute System second member

    for ( UnsignedInteger i = 0; i < dimension; ++i )
    {
      currentSecondMember_[i] = -currentPoint_[i];
    }

    currentSecondMember_[dimension] = -currentLevelValue_ + levelValue;

    //solve the linear system
    const NumericalPoint solution(currentSystemMatrix_.solveLinearSystem(currentSecondMember_));

    std::copy(solution.begin(), solution.end() - 1, currentDirection_.begin());

    currentLambda_ = solution[dimension];

    /* Perform a line search in the given direction */
    const NumericalScalar alpha = computeLineSearch();

    /* Check if convergence has been achieved */
    absoluteError = std::abs(alpha) * currentDirection_.norm();
    constraintError = std::abs(currentLevelValue_ - levelValue);
    const NumericalScalar pointNorm = currentPoint_.norm();

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
    result_.setIterationNumber(iterationNumber);
    result_.store(currentPoint_, NumericalPoint(1, currentLevelValue_), absoluteError, relativeError, residualError, constraintError);
    result_.setLagrangeMultipliers(NumericalPoint(1, currentLambda_));

    LOGINFO(getResult().__repr__());
  }

  /* Check if we converged */

  if (!convergence)
  {
    LOGWARN(OSS() << "Warning! The SQP algorithm failed to converge after " << getMaximumIterationNumber() << " iterations");
  }

} // run()

/* Tau accessor */
NumericalScalar SQP::getTau() const
{
  return tau_;
}

void SQP::setTau(const NumericalScalar tau)
{
  tau_ = tau;
}

/* Omega accessor */
NumericalScalar SQP::getOmega() const
{
  return omega_;
}

void SQP::setOmega(const NumericalScalar omega)
{
  omega_ = omega;
}

/* Smooth accessor */
NumericalScalar SQP::getSmooth() const
{
  return smooth_;
}

void SQP::setSmooth(const NumericalScalar smooth)
{
  smooth_ = smooth;
}

/* String converter */
String SQP::__repr__() const
{
  OSS oss;
  oss << "class=" << SQP::GetClassName()
      << " " << OptimizationSolverImplementation::__repr__()
      << " tau=" << tau_
      << " omega=" << omega_
      << " smooth=" << smooth_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SQP::save(Advocate & adv) const
{
  OptimizationSolverImplementation::save(adv);
  adv.saveAttribute("tau_", tau_);
  adv.saveAttribute("omega_", omega_);
  adv.saveAttribute("smooth_", smooth_);
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
  adv.loadAttribute("tau_", tau_);
  adv.loadAttribute("omega_", omega_);
  adv.loadAttribute("smooth_", smooth_);
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
