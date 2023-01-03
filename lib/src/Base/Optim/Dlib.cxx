//                                               -*- C++ -*-
/**
 *  @brief Dlib solvers
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

#include "openturns/Dlib.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OSS.hxx"

#ifdef OPENTURNS_HAVE_DLIB
#include "openturns/DlibFunctions.hxx"
#include <dlib/optimization.h>
#ifdef OPENTURNS_HAVE_DLIB_GLOBAL_OPTIMIZATION
#include <dlib/global_optimization.h>
#endif
#endif

BEGIN_NAMESPACE_OPENTURNS

/** DEFINITION OF SEARCH STRATEGY CLASSES **/

/** Search strategies are used for line search algorithms, i.e. conjugate gradient, BFGS, L-BFGS and Newton.
 *  A line search is performed at each iteration of the algorithm, using parameters wolfeRho and wolfeSigma.
 *  The number of iterations of the line search process is limited by maxLineSearchIterations parameter. **/

#ifdef OPENTURNS_HAVE_DLIB
class DlibSearchStrategyImplementation
  : public PersistentObject
{
public:
  DlibSearchStrategyImplementation()
    : PersistentObject()
    , wolfeRho_(ResourceMap::GetAsScalar("Dlib-DefaultWolfeRho"))
    , wolfeSigma_(ResourceMap::GetAsScalar("Dlib-DefaultWolfeSigma"))
    , maxLineSearchIterations_(ResourceMap::GetAsUnsignedInteger("Dlib-DefaultMaxLineSearchIterations"))
  {
    // Nothing to do
  }

  DlibSearchStrategyImplementation(const Scalar wolfeRho,
                                   const Scalar wolfeSigma,
                                   const UnsignedInteger maxLineSearchIterations )
    : PersistentObject()
    , wolfeRho_(wolfeRho)
    , wolfeSigma_(wolfeSigma)
    , maxLineSearchIterations_(maxLineSearchIterations)
  {
    // Check wolfeRho and wolfeSigma
    if (!(wolfeRho < wolfeSigma)) throw InvalidArgumentException(HERE) << "Error: wolfeRho must be lower than wolfeSigma";
  }

  // Virtual constructor
  virtual DlibSearchStrategyImplementation * clone() const
  {
    return new DlibSearchStrategyImplementation(*this);
  }

  /** Accessors to search strategy parameters **/
  virtual double get_wolfe_rho()
  {
    return wolfeRho_;
  }

  virtual double get_wolfe_sigma()
  {
    return wolfeSigma_;
  }

  virtual unsigned long get_max_line_search_iterations ()
  {
    return maxLineSearchIterations_;
  }

  /* Computation of the line search direction */
  /* Virtual function, must be overridden in actual implementation */
  virtual const DlibMatrix get_next_direction ( const DlibMatrix & /*x*/,
      const double /*f_value*/,
      const DlibMatrix & /*funct_derivative*/)
  {
    throw NotYetImplementedException(HERE) << "Error: not yet implemented search strategy";
  }

protected:
  // Parameters used in the line search process
  Scalar wolfeRho_;
  Scalar wolfeSigma_;
  UnsignedInteger maxLineSearchIterations_;
};

class DlibSearchStrategy
  : public TypedInterfaceObject<DlibSearchStrategyImplementation>
{
public:

  DlibSearchStrategy()
    : TypedInterfaceObject<DlibSearchStrategyImplementation>(new DlibSearchStrategyImplementation())
  {
    // Nothing to do
  }

  DlibSearchStrategy(const DlibSearchStrategyImplementation & searchStrategy)
    : TypedInterfaceObject<DlibSearchStrategyImplementation>(searchStrategy.clone())
  {
    // Nothing to do
  }

  /* Accessors to search strategy parameters */
  double get_wolfe_rho()
  {
    return getImplementation()->get_wolfe_rho();
  }
  double get_wolfe_sigma()
  {
    return getImplementation()->get_wolfe_sigma();
  }
  UnsignedInteger get_max_line_search_iterations ()
  {
    return getImplementation()->get_max_line_search_iterations();
  }

  /* Computation of the line search direction */
  DlibMatrix get_next_direction (const DlibMatrix & x,
                                 const double f_value,
                                 const DlibMatrix & funct_derivative)
  {
    return getImplementation()->get_next_direction(x, f_value, funct_derivative);
  }
};

class DlibCgSearchStrategy
  : public DlibSearchStrategyImplementation
{
public:
  DlibCgSearchStrategy( const Scalar wolfeRho,
                        const Scalar wolfeSigma,
                        const UnsignedInteger maxLineSearchIterations)
    : DlibSearchStrategyImplementation(wolfeRho, wolfeSigma, maxLineSearchIterations)
  {
    // Nothing to do
  }

  /* Virtual constructor */
  DlibCgSearchStrategy * clone() const
  {
    return new DlibCgSearchStrategy(*this);
  }

  /* Computation of the line search direction: call to dlib function */
  const DlibMatrix get_next_direction ( const DlibMatrix & x,
                                        const double f_value,
                                        const DlibMatrix & funct_derivative)
  {
    return dlib::cg_search_strategy().get_next_direction(x, f_value, funct_derivative);
  }
};

class DlibBfgsSearchStrategy
  : public DlibSearchStrategyImplementation
{
public:
  DlibBfgsSearchStrategy(const Scalar wolfeRho,
                         const Scalar wolfeSigma,
                         const UnsignedInteger maxLineSearchIterations)
    : DlibSearchStrategyImplementation(wolfeRho, wolfeSigma, maxLineSearchIterations)
  {
    // Nothing to do
  }

  /* Virtual constructor */
  DlibBfgsSearchStrategy * clone() const
  {
    return new DlibBfgsSearchStrategy(*this);
  }

  /* Computation of the line search direction: call to dlib function */
  const DlibMatrix get_next_direction ( const DlibMatrix & x,
                                        const double f_value,
                                        const DlibMatrix & funct_derivative)
  {
    return dlib::bfgs_search_strategy().get_next_direction(x, f_value, funct_derivative);
  }
};

/** In addition to the previous features, L-BFGS algorithm allows the user
 *  to define the maximum amount of memory to use during the process **/
class DlibLbfgsSearchStrategy
  : public DlibSearchStrategyImplementation
{
public:
  DlibLbfgsSearchStrategy(const Scalar wolfeRho,
                          const Scalar wolfeSigma,
                          const UnsignedInteger maxLineSearchIterations,
                          const UnsignedInteger maxSize
                         )
    : DlibSearchStrategyImplementation(wolfeRho, wolfeSigma, maxLineSearchIterations)
    , maxSize_(maxSize)
  {
    // Nothing to do
  }

  /* Virtual constructor */
  DlibLbfgsSearchStrategy * clone() const
  {
    return new DlibLbfgsSearchStrategy(*this);
  }

  /* Computation of the line search direction: call to dlib function */
  const DlibMatrix get_next_direction ( const DlibMatrix & x,
                                        const double f_value,
                                        const DlibMatrix & funct_derivative)
  {
    return dlib::lbfgs_search_strategy(maxSize_).get_next_direction(x, f_value, funct_derivative);
  }

private:
  // Maximum amount of memory to use
  UnsignedInteger maxSize_;
};

/** Newton algorithm requires the user to provide the Hessian matrix of the objective function. **/
class DlibNewtonSearchStrategy
  : public DlibSearchStrategyImplementation
{
public:
  DlibNewtonSearchStrategy( const Scalar wolfeRho,
                            const Scalar wolfeSigma,
                            const UnsignedInteger maxLineSearchIterations,
                            const Function objectiveFunction
                          )
    : DlibSearchStrategyImplementation(wolfeRho, wolfeSigma, maxLineSearchIterations)
    , hessian_(objectiveFunction.getHessian())
  {
    // Nothing to do
  }

  /* Virtual constructor */
  DlibNewtonSearchStrategy * clone() const
  {
    return new DlibNewtonSearchStrategy(*this);
  }

  /* Computation of the line search direction: call to dlib function */
  const DlibMatrix get_next_direction ( const DlibMatrix & x,
                                        const double f_value,
                                        const DlibMatrix & funct_derivative)
  {
    return dlib::newton_search_strategy_obj<DlibHessian>(hessian_).get_next_direction(x, f_value, funct_derivative);
  }

private:
  // Hessian matrix (as a function) of the objective function
  DlibHessian hessian_;
};

/**                               => End of search strategy classes definitions **/

/** DEFINITION OF DLIBSTOPSTRATEGY CLASS **/
class DlibStopStrategy
{
public:
  DlibStopStrategy( const Dlib& dlibAlgorithm,
                    OptimizationResult& optimizationResult,
                    const DlibFunction& objectiveFunction)
    : dlibAlgorithm_(dlibAlgorithm)
    , optimizationResult_(optimizationResult)
    , objectiveFunction_(objectiveFunction)
    , lastInput_(Point(dlibAlgorithm_.getProblem().getDimension()))
    , lastOutput_(Point(1))
  {
    // Nothing to do
  }

  bool should_continue_search(const DlibMatrix & x,
                              const double funct_value,
                              const DlibMatrix & )
  {
    optimizationResult_.setEvaluationNumber(objectiveFunction_.getEvaluationNumber());

    Point xPoint(x.size());
    std::copy(x.begin(), x.end(), xPoint.begin());
    Point fxPoint(1, funct_value);

    Scalar absoluteError = dlibAlgorithm_.getMaximumAbsoluteError();
    Scalar relativeError = dlibAlgorithm_.getMaximumRelativeError();
    Scalar residualError = dlibAlgorithm_.getMaximumResidualError();
    Scalar constraintError = 0.0;

    if (optimizationResult_.getIterationNumber() > 0)
    {
      absoluteError = (xPoint - lastInput_).norm();
      relativeError = (xPoint - lastInput_).norm() / xPoint.norm();
      residualError = (fxPoint - lastOutput_).norm();
      constraintError = 0.0;
    }

    // Compute stop criterion
    bool stopSearch =  ((absoluteError < dlibAlgorithm_.getMaximumAbsoluteError())
                        && (relativeError < dlibAlgorithm_.getMaximumRelativeError())
                        && (residualError < dlibAlgorithm_.getMaximumResidualError()))
                       || (optimizationResult_.getIterationNumber() >= dlibAlgorithm_.getMaximumIterationNumber())
                       || (objectiveFunction_.getEvaluationNumber() >= dlibAlgorithm_.getMaximumEvaluationNumber());

    lastInput_ = xPoint;
    lastOutput_ = fxPoint;

    optimizationResult_.store(lastInput_,
                              lastOutput_,
                              absoluteError,
                              relativeError,
                              residualError,
                              constraintError);

    if (!stopSearch) optimizationResult_.setIterationNumber(optimizationResult_.getIterationNumber() + 1);

    return !stopSearch;
  }

private:
  const Dlib & dlibAlgorithm_;
  OptimizationResult & optimizationResult_;
  const DlibFunction & objectiveFunction_;
  Point lastInput_;
  Point lastOutput_;
};

/**                               => End of stop strategy class definition **/
#endif

CLASSNAMEINIT(Dlib)

static const Factory<Dlib> Factory_Dlib;

Description Dlib::GetAlgorithmNames()
{
  static Description algoNames;

  if (!algoNames.getSize())
  {
    algoNames.add("cg");
    algoNames.add("bfgs");
    algoNames.add("lbfgs");
    algoNames.add("newton");
#ifdef OPENTURNS_HAVE_DLIB_GLOBAL_OPTIMIZATION
    algoNames.add("global");
#endif
    algoNames.add("least_squares");
    algoNames.add("least_squares_lm");
    algoNames.add("trust_region");
  }
  return algoNames;
}

/** CONSTRUCTORS */
Dlib::Dlib()
  : OptimizationAlgorithmImplementation()
  , algoName_("bfgs")
  , wolfeRho_(ResourceMap::GetAsScalar("Dlib-DefaultWolfeRho"))
  , wolfeSigma_(ResourceMap::GetAsScalar("Dlib-DefaultWolfeSigma"))
  , maxLineSearchIterations_(ResourceMap::GetAsUnsignedInteger("Dlib-DefaultMaxLineSearchIterations"))
  , initialTrustRegionRadius_(ResourceMap::GetAsScalar("Dlib-DefaultInitialTrustRegionRadius"))
{
}


Dlib::Dlib(const String & algoName)
  : OptimizationAlgorithmImplementation()
  , algoName_(algoName)
  , wolfeRho_(ResourceMap::GetAsScalar("Dlib-DefaultWolfeRho"))
  , wolfeSigma_(ResourceMap::GetAsScalar("Dlib-DefaultWolfeSigma"))
  , maxLineSearchIterations_(ResourceMap::GetAsUnsignedInteger("Dlib-DefaultMaxLineSearchIterations"))
  , initialTrustRegionRadius_(ResourceMap::GetAsScalar("Dlib-DefaultInitialTrustRegionRadius"))
{
  setAlgorithmName(algoName);
}


Dlib::Dlib(const OptimizationProblem & problem,
           const String & algoName)
  : OptimizationAlgorithmImplementation(problem)
  , wolfeRho_(ResourceMap::GetAsScalar("Dlib-DefaultWolfeRho"))
  , wolfeSigma_(ResourceMap::GetAsScalar("Dlib-DefaultWolfeSigma"))
  , maxLineSearchIterations_(ResourceMap::GetAsUnsignedInteger("Dlib-DefaultMaxLineSearchIterations"))
  , initialTrustRegionRadius_(ResourceMap::GetAsScalar("Dlib-DefaultInitialTrustRegionRadius"))
{
  setAlgorithmName(algoName);
  checkProblem(problem);
}

// Virtual constructor
Dlib * Dlib::clone() const
{
  return new Dlib(*this);
}

/** ACCESSORS */
String Dlib::getAlgorithmName()
{
  return algoName_;
}

void Dlib::setAlgorithmName(const String algoName)
{
  if (!GetAlgorithmNames().contains(algoName))
    throw NotYetImplementedException(HERE) << "Unknown Dlib algorithm name: " << algoName;

  algoName_ = algoName;
}

Scalar Dlib::getWolfeRho() const
{
  return wolfeRho_;
}

void Dlib::setWolfeRho(const Scalar wolfeRho)
{
  if (!(wolfeRho > 0.0))
    throw InvalidArgumentException(HERE) << "Error: wolfeRho parameter must be strictly positive";

  wolfeRho_ = wolfeRho;

  if (!(wolfeRho_ < wolfeSigma_))
    LOGWARN(OSS() << "Warning: wolfeRho must be strictly lower than wolfeSigma. Please adjust either of the parameters.");
}

Scalar Dlib::getWolfeSigma() const
{
  return wolfeSigma_;
}

void Dlib::setWolfeSigma(const Scalar wolfeSigma)
{
  if (!(wolfeSigma > 0.0))
    throw InvalidArgumentException(HERE) << "Error: wolfeSigma parameter must be strictly positive";

  wolfeSigma_ = wolfeSigma;

  if (!(wolfeRho_ < wolfeSigma_))
    LOGWARN(OSS() << "Warning: wolfeRho must be strictly lower than wolfeSigma. Please adjust either of the parameters.");
}

UnsignedInteger Dlib::getMaxLineSearchIterations() const
{
  return maxLineSearchIterations_;
}

void Dlib::setMaxLineSearchIterations(const UnsignedInteger maxLineSearchIterations)
{
  maxLineSearchIterations_ = maxLineSearchIterations;
}

UnsignedInteger Dlib::getMaxSize() const
{
  return maxSize_;
}

void Dlib::setMaxSize(const UnsignedInteger maxSize)
{
  maxSize_ = maxSize;
}

Scalar Dlib::getInitialTrustRegionRadius() const
{
  return initialTrustRegionRadius_;
}

void Dlib::setInitialTrustRegionRadius(const Scalar radius)
{
  if (!(radius > 0.0))
    throw InvalidArgumentException(HERE) << "Error: trust region radius must be positive.";
  initialTrustRegionRadius_ = radius;
}

/** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void Dlib::checkProblem(const OptimizationProblem & problem) const
{
  // Cannot solve multi-objective problems
  if (problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "Error: " << algoName_ << " does not support multi-objective optimization";

  // Cannot solve problems with equality/inequality constraints
  if (problem.hasInequalityConstraint())
    throw InvalidArgumentException(HERE) << "Error: " << algoName_ << " algorithm does not support inequality constraints";
  if (problem.hasEqualityConstraint())
    throw InvalidArgumentException(HERE) << "Error: " << algoName_ << " algorithm does not support equality constraints";

  // Cannot solve non continuous problems
  if (!problem.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support non continuous problems";

  // "global" requires finite bounds
  if (algoName_ == "global")
  {
    if (!problem.hasBounds())
      throw InvalidArgumentException(HERE) << "Error: " << algoName_ << " algorithm requires problem to have bounds.";
    else
    {
      Interval::BoolCollection finiteLowerBound(problem.getBounds().getFiniteLowerBound());
      Interval::BoolCollection finiteUpperBound(problem.getBounds().getFiniteUpperBound());
      for (UnsignedInteger i = 0; i < problem.getDimension(); ++ i)
      {
        if (!finiteLowerBound[i] || !finiteUpperBound[i])
          throw InvalidArgumentException(HERE) << "Error: " << algoName_ << " algorithm requires problem to have finite bounds.";
      }
    }
  }

  // Only "least_squares" and "least_squares_lm" support least squares problems
  if (problem.hasResidualFunction() && !(algoName_ == "least_squares" || algoName_ == "least_squares_lm"))
    throw InvalidArgumentException(HERE) << "Error: " << algoName_ << " algorithm does not support least squares problems.";

  // "least_squares", "least_squares_lm" and "trust_region" require non bounded variables
  if (problem.hasBounds() && (algoName_ == "least_squares" || algoName_ == "least_squares_lm" || algoName_ == "trust_region"))
    throw InvalidArgumentException(HERE) << "Error: " << algoName_ << " algorithm cannot solve bounded problems.";

#ifndef OPENTURNS_HAVE_DLIB
  throw NotYetImplementedException(HERE) << "No Dlib support";
#endif
}

/** RUN FUNCTION **/
// Performs the actual computation by calling dlib library solvers
void Dlib::run()
{
#ifdef OPENTURNS_HAVE_DLIB
  Point startingPoint(getStartingPoint());

  /* Various checks */
  // Check search strategy
  if (!(GetAlgorithmNames().contains(algoName_)))
    throw NotYetImplementedException(HERE) << "Error: unknown algorithm (" << algoName_ << ")" ;
  // Check input dimension
  const UnsignedInteger dimension = getProblem().getDimension();
  if (startingPoint.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: Invalid starting point dimension (" << startingPoint.getDimension() << ", expected " << dimension << ")";

  /** OBJECTIVE FUNCTION: convert objective function to DlibFunction */
  DlibFunction objectiveDlibFunction(getProblem().getObjective());
  DlibGradient objectiveDlibGradient(objectiveDlibFunction.getGradient());

  /** STARTING POINT: Convert startingPoint to dlib::matrix */
  DlibVector optimPoint(dimension, 1);
  for (UnsignedInteger i = 0; i < startingPoint.getDimension(); ++ i)
    optimPoint(i, 0) = startingPoint[i];

  /** BOUNDS **/
  DlibVector lb(dimension, 1);
  DlibVector ub(dimension, 1);

  if (getProblem().hasBounds())
  {
    // Extraction of bounds as vectors
    Interval bounds(getProblem().getBounds());

    if (algoName_ != "global" && !bounds.contains(startingPoint))
      throw InvalidArgumentException(HERE) << "Error: starting point is not inside bounds x=" << startingPoint.__str__() << " bounds=" << bounds;

    Interval::BoolCollection finiteLowerBound(bounds.getFiniteLowerBound());
    Interval::BoolCollection finiteUpperBound(bounds.getFiniteUpperBound());
    Point lowerBound(bounds.getLowerBound());
    Point upperBound(bounds.getUpperBound());

    std::copy(lowerBound.begin(), lowerBound.end(), lb.begin());
    std::copy(upperBound.begin(), upperBound.end(), ub.begin());
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      if (!finiteLowerBound[i]) lb(i) = SpecFunc::LowestScalar;
      if (!finiteUpperBound[i]) ub(i) =  SpecFunc::MaxScalar;
    }
  }
  else
  {
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      lb(i) = SpecFunc::LowestScalar;
      ub(i) = SpecFunc::MaxScalar;
    }
  }

  // initialize result
  result_ = OptimizationResult(getProblem());

  /** SWITCH BETWEEN ALGORITHMS **/
  if (algoName_ == "cg"
    || algoName_ == "bfgs"
    || algoName_ == "lbfgs"
    || algoName_ == "newton")
  {
    // Create searchStrategy
    DlibSearchStrategy searchStrategy;
    if (algoName_ == "cg")
      searchStrategy = DlibCgSearchStrategy(wolfeRho_,
                                            wolfeSigma_,
                                            maxLineSearchIterations_);
    else if (algoName_ == "bfgs")
      searchStrategy = DlibBfgsSearchStrategy(wolfeRho_,
                                              wolfeSigma_,
                                              maxLineSearchIterations_);
    else if (algoName_ == "lbfgs")
      searchStrategy = DlibLbfgsSearchStrategy( wolfeRho_,
                       wolfeSigma_,
                       maxLineSearchIterations_,
                       maxSize_);
    else if (algoName_ == "newton")
      searchStrategy = DlibNewtonSearchStrategy(wolfeRho_,
                       wolfeSigma_,
                       maxLineSearchIterations_,
                       objectiveDlibFunction );

    // Create stopStrategy
    DlibStopStrategy stopStrategy(*this,
                                  result_,
                                  objectiveDlibFunction);

    // Switch on problem type
    if (getProblem().isMinimization())
    {
      if (getProblem().hasBounds())
        dlib::find_min_box_constrained (searchStrategy,
                                        stopStrategy,
                                        objectiveDlibFunction,
                                        objectiveDlibGradient,
                                        optimPoint,
                                        lb,
                                        ub);
      else
        dlib::find_min ( searchStrategy,
                         stopStrategy,
                         objectiveDlibFunction,
                         objectiveDlibGradient,
                         optimPoint,
                         SpecFunc::LowestScalar );
    }
    else
    {
      if (getProblem().hasBounds())
        dlib::find_max_box_constrained (searchStrategy,
                                        stopStrategy,
                                        objectiveDlibFunction,
                                        objectiveDlibGradient,
                                        optimPoint,
                                        lb,
                                        ub);
      else
        dlib::find_max (searchStrategy,
                        stopStrategy,
                        objectiveDlibFunction,
                        objectiveDlibGradient,
                        optimPoint,
                        SpecFunc::MaxScalar);
    }
  } // CG, BFGS/LBFGS, Newton
#ifdef OPENTURNS_HAVE_DLIB_GLOBAL_OPTIMIZATION
  else if (algoName_ == "global")
  {
    // Declare result and lambda function
    dlib::function_evaluation globalOptimResult;
    auto objectiveLambdaFunction = [&](dlib::matrix<double, 0, 1> input)
    {
      return objectiveDlibFunction(input);
    };

    const std::vector<bool> is_integer_variable(dimension, false);
    if (getProblem().isMinimization())
      globalOptimResult = dlib::find_min_global(objectiveLambdaFunction,
                          lb,
                          ub,
                          is_integer_variable,
                          dlib::max_function_calls(getMaximumEvaluationNumber()),
                          std::chrono::nanoseconds(dlib::FOREVER),
                          getMaximumAbsoluteError());
    else
      globalOptimResult = dlib::find_max_global(objectiveLambdaFunction,
                          lb,
                          ub,
                          is_integer_variable,
                          dlib::max_function_calls(getMaximumEvaluationNumber()),
                          std::chrono::nanoseconds(dlib::FOREVER),
                          getMaximumAbsoluteError());

    // Reconstruction of OptimizationResult
    Sample inputHistory(objectiveDlibFunction.getInputHistory());
    Sample outputHistory(objectiveDlibFunction.getOutputHistory());

    Point optimalPoint(dimension);
    std::copy(globalOptimResult.x.begin(), globalOptimResult.x.end(), optimalPoint.begin());

    result_.store(inputHistory[0],
                  outputHistory[0],
                  0.0,
                  0.0,
                  0.0,
                  0.0);

    for (UnsignedInteger i = 1; i < objectiveDlibFunction.getEvaluationNumber(); ++ i) // Iterations 2 to last
      result_.store(  inputHistory[i],
                      outputHistory[i],
                      (inputHistory[i] - inputHistory[i - 1]).norm(),
                      (inputHistory[i] - inputHistory[i - 1]).norm() / Point(inputHistory[i]).norm(),
                      (outputHistory[i] - outputHistory[i - 1]).norm(),
                      0.0);

    result_.setOptimalPoint(optimalPoint);
    result_.setOptimalValue(Point(1, globalOptimResult.y));
    result_.setEvaluationNumber(objectiveDlibFunction.getEvaluationNumber());
  }
#endif
  else if (algoName_ == "least_squares")
  {
    // Create stopStrategy
    DlibFunction residualDlibFunction(getProblem().getResidualFunction());
    DlibStopStrategy stopStrategy(*this,
                                  result_,
                                  residualDlibFunction);

    // Create lambda functions to add a first variable as required by dlib::solve_least_squares
    auto augmentedResidualFunction = [&](int i, dlib::matrix<double, 0, 1> params)
    {
      return residualDlibFunction(i, params);
    };

    auto augmentedResidualDerivative = [&](int i, dlib::matrix<double, 0, 1> params)
    {
      return residualDlibFunction.gradient(i, params);
    };

    // Create dummy 'list', as required by dlib
    std::vector<int> list(residualDlibFunction.getOutputDimension());
    for (UnsignedInteger i = 0; i < residualDlibFunction.getOutputDimension(); ++ i)
      list[i] = i;

    // Create parameters vector
    DlibVector params(dimension, 1);
    std::copy(startingPoint.begin(), startingPoint.end(), params.begin());

    // Call to dlib::solve_least_squares: modification of params
    dlib::solve_least_squares(stopStrategy,
                              augmentedResidualFunction,
                              augmentedResidualDerivative,
                              list,
                              params,
                              initialTrustRegionRadius_);
  }
  else if (algoName_ == "least_squares_lm")
  {
    // Create stopStrategy
    DlibFunction residualDlibFunction(getProblem().getResidualFunction());
    DlibStopStrategy stopStrategy(*this,
                                  result_,
                                  residualDlibFunction);

    // Create lambda functions to add a first variable as required by dlib::solve_least_squares
    auto augmentedResidualFunction = [&](int i, dlib::matrix<double, 0, 1> params)
    {
      return residualDlibFunction(i, params);
    };

    auto augmentedResidualDerivative = [&](int i, dlib::matrix<double, 0, 1> params)
    {
      return residualDlibFunction.gradient(i, params);
    };

    // Create dummy 'list', as required by dlib
    std::vector<double> list(residualDlibFunction.getOutputDimension());
    for (UnsignedInteger i = 0; i < residualDlibFunction.getOutputDimension(); ++ i)
      list[i] = i;

    // Create parameters vector
    DlibVector params(dimension, 1);
    std::copy(startingPoint.begin(), startingPoint.end(), params.begin());

    // Call to dlib::solve_least_squares: modification of params
    dlib::solve_least_squares_lm(stopStrategy,
                                 augmentedResidualFunction,
                                 augmentedResidualDerivative,
                                 list,
                                 params,
                                 initialTrustRegionRadius_
                                );
  }
  else if (algoName_ == "trust_region")
  {
    // Create stopStrategy
    DlibStopStrategy stopStrategy(*this,
                                  result_,
                                  objectiveDlibFunction);

    // Convert optimPoint to DlibFunction::column_vector
    DlibFunction::column_vector optimizer(dimension, 1);
    for (UnsignedInteger i = 0; i < dimension; ++i)
      optimizer(i, 0) = optimPoint(i, 0);

    // Call to find_min/max_trust_region()
    if (getProblem().isMinimization())
      dlib::find_min_trust_region(  stopStrategy,
                                    objectiveDlibFunction,
                                    optimizer,
                                    initialTrustRegionRadius_);
    else
      dlib::find_max_trust_region( stopStrategy,
                                   objectiveDlibFunction,
                                   optimizer,
                                   initialTrustRegionRadius_
                                 );
  }
  else
    throw NotYetImplementedException(HERE) << "Error: unknown strategy " << algoName_;
#endif  // dlib_FOUND
}

/* String converter */
String Dlib::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " " << OptimizationAlgorithmImplementation::__repr__()
      << " algorithm=" << algoName_;
  return oss;
}

/* String converter */
String Dlib::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << getClassName()
      << " algorithm=" << algoName_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void Dlib::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("algoName_", algoName_);
  adv.saveAttribute("wolfeRho_", wolfeRho_);
  adv.saveAttribute("wolfeSigma_", wolfeSigma_);
  adv.saveAttribute("maxLineSearchIterations_", maxLineSearchIterations_);
  adv.saveAttribute("maxSize_", maxSize_);
  adv.saveAttribute("initialTrustRegionRadius_", initialTrustRegionRadius_);
}

/* Method load() reloads the object from the StorageManager */
void Dlib::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  adv.loadAttribute("algoName_", algoName_);
  adv.loadAttribute("wolfeRho_", wolfeRho_);
  adv.loadAttribute("wolfeSigma_", wolfeSigma_);
  adv.loadAttribute("maxLineSearchIterations_", maxLineSearchIterations_);
  adv.loadAttribute("maxSize_", maxSize_);
  adv.loadAttribute("initialTrustRegionRadius_", initialTrustRegionRadius_);
}

END_NAMESPACE_OPENTURNS

