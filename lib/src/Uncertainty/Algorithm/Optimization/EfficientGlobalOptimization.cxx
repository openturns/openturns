//                                               -*- C++ -*-
/**
 *  @brief EfficientGlobalOptimization or EGO algorithm
 *
 *  Copyright 2005-2027 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/EfficientGlobalOptimization.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/MultiStart.hxx"
#include "openturns/JointDistribution.hxx"
#include "openturns/MultivariateUniform.hxx"
#include "openturns/GaussianProcessFitter.hxx"
#include "openturns/GaussianProcessRegression.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EfficientGlobalOptimization)

static const Factory<EfficientGlobalOptimization> Factory_EfficientGlobalOptimization;

/* Constructor with parameters */
EfficientGlobalOptimization::EfficientGlobalOptimization()
  : OptimizationAlgorithmImplementation()
  , solver_(OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("EfficientGlobalOptimization-DefaultOptimizationAlgorithm")))
  , multiStartExperimentSize_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartExperimentSize"))
  , multiStartNumber_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartNumber"))
  , parameterEstimationPeriod_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultParameterEstimationPeriod"))
  , correlationLengthFactor_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultCorrelationLengthFactor"))
  , criterion_(ExpectedImprovement())
{
  // Nothing to do
}

EfficientGlobalOptimization::EfficientGlobalOptimization(const OptimizationProblem & problem,
    const GaussianProcessRegressionResult & gprResult)
  : OptimizationAlgorithmImplementation(problem)
  , gprResult_(gprResult)
  , solver_(OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("EfficientGlobalOptimization-DefaultOptimizationAlgorithm")))
  , multiStartExperimentSize_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartExperimentSize"))
  , multiStartNumber_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartNumber"))
  , parameterEstimationPeriod_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultParameterEstimationPeriod"))
  , correlationLengthFactor_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultCorrelationLengthFactor"))
  , criterion_(ExpectedImprovement())
{
  checkProblem(problem);
  if (gprResult_.getMetaModel().getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "GPR metamodel must be 1-d";
}


void EfficientGlobalOptimization::run()
{
  const OptimizationProblem problem(getProblem());
  const UnsignedInteger dimension = problem.getDimension();
  const Function model(problem.getObjective());
  Sample inputSample;
  Sample outputSample;
  inputSample = gprResult_.getInputSample();
  outputSample = gprResult_.getOutputSample();
  UnsignedInteger size = inputSample.getSize();
  Point noise(size);
  const Bool hasNoise = noiseFunction_.getEvaluation().getImplementation()->isActualImplementation();
  if (hasNoise)
  {
    Sample noiseSample(noiseFunction_(inputSample));
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      noise[i] = noiseSample(i, 0);
      if (!(noise[i] >= 0.0)) throw InvalidArgumentException(HERE) << "Noise-induced variance must be nonnegative, but is " << noise[i] << " at point " << inputSample[i];
    }
  }
  UnsignedInteger evaluationNumber = 0;
  Bool exitLoop = false;
  hasConverged_ = false;
  // The criterion history belongs to the run, like the result: a second run
  // must not inherit the values of the first one.
  criterionValues_ = Sample();

  // select the best feasible point in the initial DOE
  OptimizationResult result(getProblem());
  for (UnsignedInteger index = 0; index < size; ++ index)
    result.store(inputSample[index], outputSample[index], 0.0, 0.0, 0.0, 0.0);
  Point optimalPoint(result.getOptimalPoint());
  Point optimalValue(result.getOptimalValue());
  result = OptimizationResult(getProblem()); // reset to clear history
  LOGINFO(OSS() << "Initial best x=" << optimalPoint << " f(x)=" << optimalValue);

  // compute minimum distance between design points to assess the correlation lengths of the metamodel
  Point minimumDistance(dimension, SpecFunc::Infinity);
  if (!hasNoise)
  {
    for (UnsignedInteger i1 = 0; i1 < size; ++ i1)
    {
      for (UnsignedInteger i2 = 0; i2 < i1; ++ i2)
      {
        for (UnsignedInteger j = 0; j < dimension; ++ j)
        {
          Scalar distance = std::abs(inputSample(i1, j) - inputSample(i2, j));
          if (distance < minimumDistance[j])
          {
            minimumDistance[j] = distance;
          }
        }
      }
    }
  }

  UnsignedInteger iterationNumber = 0;
  // use the provided kriging result at first iteration

  GaussianProcessRegressionResult gprResult(gprResult_);

  while ((!exitLoop) && (evaluationNumber < getMaximumCallsNumber()))
  {
    // Hand the current state over to the enrichment criterion: it must build
    // the objective to maximize from this iteration, not from the initial one
    criterion_.setGaussianProcessRegression(gprResult);
    criterion_.setInputSample(inputSample);
    criterion_.setOptimalValue(optimalValue[0]);
    criterion_.setMinimization(problem.isMinimization());
    criterion_.setNoiseFunction(noiseFunction_);
    Function improvementObjective(criterion_.buildObjective());
    if (improvementObjective.getInputDimension() != dimension)
      throw InvalidArgumentException(HERE) << "The objective built by the criterion must have input dimension " << dimension
                                           << ", got " << improvementObjective.getInputDimension();
    if (improvementObjective.getOutputDimension() != 1)
      throw InvalidArgumentException(HERE) << "The objective built by the criterion must have output dimension 1, got " << improvementObjective.getOutputDimension();

    // use multi-start to optimize the improvement criterion when using the default solver
    OptimizationAlgorithm solver(solver_);
    if (useDefaultSolver_ && problem.hasBounds())
    {
      // Sample uniformly into the bounds
      const Interval bounds(problem.getBounds());
      const Point lowerBound(bounds.getLowerBound());
      const Point upperBound(bounds.getUpperBound());
      const Interval::BoolCollection finiteLowerBound(bounds.getFiniteLowerBound());
      const Interval::BoolCollection finiteUpperBound(bounds.getFiniteUpperBound());
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        if (!finiteLowerBound[i] || !finiteUpperBound[i])
          throw InvalidArgumentException(HERE) << "Bounds must be finite";
      }
      const MultivariateUniform distribution(lowerBound, upperBound);
      Sample improvementExperiment(distribution.getSample(multiStartExperimentSize_));
      // retain best P/N points as starting points
      improvementExperiment.stack(improvementObjective(improvementExperiment));
      Indices inputs(dimension);
      inputs.fill();
      const Sample sortedImprovement(improvementExperiment.sortAccordingToAComponent(dimension).getMarginal(inputs));
      // handle multiStartExperimentSize_ < multiStartNumber_
      const UnsignedInteger pointNumber = std::min(multiStartNumber_, multiStartExperimentSize_);
      const Sample startingPoints(sortedImprovement, multiStartExperimentSize_ - pointNumber, multiStartExperimentSize_);
      solver = MultiStart(solver, startingPoints);
    }

    // build improvement criterion optimization problem
    OptimizationProblem maximizeImprovement(improvementObjective);
    maximizeImprovement.setMinimization(false);
    if (problem.hasBounds())
      maximizeImprovement.setBounds(problem.getBounds());
    solver.setProblem(maximizeImprovement);
    try
    {
      // If the solver is single start, we can use its setStartingPoint method
      solver.setStartingPoint(optimalPoint);
    }
    catch (const NotDefinedException &) // setStartingPoint is not defined for the solver
    {
      // Nothing to do if setStartingPoint is not defined
    }
    solver.run();
    const OptimizationResult improvementResult(solver.getResult());

    // store improvement
    const Point improvementValue(improvementResult.getOptimalValue());
    if (!improvementValue.getDimension())
      throw InvalidArgumentException(HERE) << "optimization in EGO did not yield feasible points";
    criterionValues_.add(improvementValue);

    const Point newPoint(improvementResult.getOptimalPoint());
    const Point newValue(model(newPoint));
    ++ evaluationNumber;

    LOGINFO(OSS() << "New point x=" << newPoint << " f(x)=" << newValue << " evaluations=" << evaluationNumber);
    result.store(newPoint, newValue, 0.0, 0.0, 0.0, 0.0);

    optimalPoint = result.getOptimalPoint();
    optimalValue = result.getOptimalValue();
    LOGINFO(OSS() << "Optimum so far x=" << result.getOptimalPoint() << " f(x)=" << optimalValue);

    // update minimum distance stopping criterion
    if (!hasNoise)
    {
      // update minimum distance according to the new point
      for (UnsignedInteger i = 0; i < size; ++ i)
      {
        for (UnsignedInteger j = 0; j < dimension; ++ j)
        {
          Scalar distance = std::abs(inputSample(i, j) - newPoint[j]);
          if (distance < minimumDistance[j])
          {
            minimumDistance[j] = distance;
          }
        }
      }

      // when a correlation length becomes smaller than the minimal distance between design point for a single component
      // that means the model tends to be noisy, and the original EGO formulation is not adapted anymore
      const Point scale(gprResult.getCovarianceModel().getScale());
      for (UnsignedInteger j = 0; j < dimension; ++ j)
      {
        const Bool minDistStop = scale[j] < minimumDistance[j] / correlationLengthFactor_;
        if (minDistStop) LOGINFO(OSS() << "Stopped algorithm over the minimum distance criterion");
        exitLoop = exitLoop || minDistStop;
      }
    }

    // add new point to design
    inputSample.add(newPoint);
    outputSample.add(newValue);
    ++ size;
    ++ iterationNumber;

    if (hasNoise)
    {
      const Point newNoise(noiseFunction_(newPoint));
      if (!(newNoise[0] >= 0.0)) throw InvalidArgumentException(HERE) << "Noise function must be nonnegative";
      noise.add(newNoise[0]);
    }

    // callbacks
    if (progressCallback_.first)
    {
      progressCallback_.first((100.0 * evaluationNumber) / getMaximumCallsNumber(), progressCallback_.second);
    }
    if (stopCallback_.first)
    {
      Bool stop = stopCallback_.first(stopCallback_.second);
      if (stop)
      {
        exitLoop = true;
        LOGWARN(OSS() << "EGO was stopped by user");
      }
    }

    // criterion-declared convergence, checked once the new point is measured
    if (criterion_.checkConvergence(improvementValue[0]))
    {
      exitLoop = true;
      hasConverged_ = true;
      LOGINFO(OSS() << "Stopped algorithm over the criterion convergence test, criterion value=" << improvementValue[0]);
    }

    if (evaluationNumber > 0)
    {
      GaussianProcessRegression algo;
      if ((parameterEstimationPeriod_ > 0) && ((evaluationNumber % parameterEstimationPeriod_) == 0))
      {
        GaussianProcessFitter fitter(inputSample, outputSample, gprResult.getCovarianceModel(), gprResult.getBasis());
        if (hasNoise)
          fitter.setNoise(noise);
        fitter.run();
        algo = GaussianProcessRegression(fitter.getResult());
      }
      else
      {
        algo = GaussianProcessRegression(inputSample, outputSample, gprResult.getCovarianceModel(), gprResult.getBasis()[0]);
      }
      algo.run();
      gprResult = algo.getResult();
    }
  } // while

  // update result to take new points into account
  gprResult_ = gprResult;

  result.setIterationNumber(iterationNumber);
  setResult(result);
}


/* Virtual constructor */
EfficientGlobalOptimization * EfficientGlobalOptimization::clone() const
{
  return new EfficientGlobalOptimization(*this);
}

/* String converter */
String EfficientGlobalOptimization::__repr__() const
{
  OSS oss;
  oss << "class=" << EfficientGlobalOptimization::GetClassName()
      << " " << OptimizationAlgorithmImplementation::__repr__()
      << " criterion=" << criterion_
      << " multiStartExperimentSize=" << multiStartExperimentSize_
      << " multiStartNumber=" << multiStartNumber_
      << " parameterEstimationPeriod=" << parameterEstimationPeriod_
      << " correlationLengthFactor=" << correlationLengthFactor_
      << " hasConverged=" << (hasConverged_ ? "true" : "false");
  return oss;
}

/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void EfficientGlobalOptimization::checkProblem(const OptimizationProblem & problem) const
{
  if (problem.getObjective().getOutputDimension() > 1)
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support multi-objective optimization";
  if (problem.hasInequalityConstraint() || problem.hasEqualityConstraint())
    throw InvalidArgumentException(HERE) << "Error : " << this->getClassName() << " does not support constraints";
  if (!problem.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: " << this->getClassName() << " does not support non continuous problems";
}


void EfficientGlobalOptimization::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
  useDefaultSolver_ = false;
}

OptimizationAlgorithm EfficientGlobalOptimization::getOptimizationAlgorithm() const
{
  return solver_;
}

/* Size of the design to draw starting points */
UnsignedInteger EfficientGlobalOptimization::getMultiStartExperimentSize() const
{
  return multiStartExperimentSize_;
}

void EfficientGlobalOptimization::setMultiStartExperimentSize(const UnsignedInteger multiStartExperimentSize)
{
  multiStartExperimentSize_ = multiStartExperimentSize;
}


/* Number of starting points for the criterion optim */
UnsignedInteger EfficientGlobalOptimization::getMultiStartNumber() const
{
  return multiStartNumber_;
}

void EfficientGlobalOptimization::setMultiStartNumber(const UnsignedInteger multiStartNumber)
{
  multiStartNumber_ = multiStartNumber;
}

/* Parameter estimation period accessor */
UnsignedInteger EfficientGlobalOptimization::getParameterEstimationPeriod() const
{
  return parameterEstimationPeriod_;
}


void EfficientGlobalOptimization::setParameterEstimationPeriod(const UnsignedInteger parameterEstimationPeriod)
{
  parameterEstimationPeriod_ = parameterEstimationPeriod;
}

/* Enrichment criterion accessor */
void EfficientGlobalOptimization::setCriterion(const EfficientGlobalOptimizationCriterion & criterion)
{
  // Take a private copy: the algorithm mutates the criterion at each iteration,
  // and that must not be visible through the handle the caller kept.
  criterion_ = EfficientGlobalOptimizationCriterion(criterion.getImplementation()->clone());
  // Keep the noise function of the algorithm authoritative: a criterion
  // replaced after setNoiseFunction() must still see it.
  criterion_.setNoiseFunction(noiseFunction_);
}

EfficientGlobalOptimizationCriterion EfficientGlobalOptimization::getCriterion() const
{
  return criterion_;
}

/* Value of the criterion at each iteration */
Sample EfficientGlobalOptimization::getCriterionValues() const
{
  return criterionValues_;
}

Sample EfficientGlobalOptimization::getExpectedImprovement() const
{
  return criterionValues_;
}

/* Whether the loop ended on the criterion convergence test */
Bool EfficientGlobalOptimization::getHasConverged() const
{
  return hasConverged_;
}

/* correlation length stopping criterion factor accessor */
void EfficientGlobalOptimization::setCorrelationLengthFactor(const Scalar correlationLengthFactor)
{
  correlationLengthFactor_ = correlationLengthFactor;
}

Scalar EfficientGlobalOptimization::getCorrelationLengthFactor() const
{
  return correlationLengthFactor_;
}


/* AEI tradeoff constant accessor */
void EfficientGlobalOptimization::setAEITradeoff(const Scalar aeiTradeoff)
{
  const ExpectedImprovement * p_expectedImprovement = dynamic_cast<const ExpectedImprovement *>(criterion_.getImplementation().get());
  if (p_expectedImprovement == nullptr)
    throw InvalidArgumentException(HERE) << "setAEITradeoff() applies to the expected improvement criterion only, the current criterion is "
                                         << criterion_.getImplementation()->getClassName();
  // Copy on write: getCriterion() hands out a handle that shares its
  // implementation, and a change of the tradeoff must not be visible through
  // it. clone() keeps the dynamic type, so a derived criterion is not sliced.
  EfficientGlobalOptimizationCriterion newCriterion(criterion_.getImplementation()->clone());
  dynamic_cast<ExpectedImprovement *>(newCriterion.getImplementation().get())->setAEITradeoff(aeiTradeoff);
  criterion_ = newCriterion;
}

Scalar EfficientGlobalOptimization::getAEITradeoff() const
{
  const ExpectedImprovement * p_expectedImprovement = dynamic_cast<const ExpectedImprovement *>(criterion_.getImplementation().get());
  if (p_expectedImprovement == nullptr)
    throw InvalidArgumentException(HERE) << "getAEITradeoff() applies to the expected improvement criterion only, the current criterion is "
                                         << criterion_.getImplementation()->getClassName();
  return p_expectedImprovement->getAEITradeoff();
}

void EfficientGlobalOptimization::setNoiseFunction(const Function & noiseFunction)
{
  const UnsignedInteger dimension = getProblem().getDimension();
  if (noiseFunction.getInputDimension() != dimension) throw InvalidArgumentException(HERE) << "Noise function must be of dimension " << dimension;
  if (noiseFunction.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Noise function must be 1-d";
  noiseFunction_ = noiseFunction;
  criterion_.setNoiseFunction(noiseFunction);
}

Function EfficientGlobalOptimization::getNoiseFunction() const
{
  return noiseFunction_;
}

/* GP result accessor (especially useful after run() has been called) */
GaussianProcessRegressionResult EfficientGlobalOptimization::getGaussianProcessRegressionResult() const
{
  return gprResult_;
}

/* Method save() stores the object through the StorageManager */
void EfficientGlobalOptimization::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("gprResult_", gprResult_);
  adv.saveAttribute("solver_", solver_);
  adv.saveAttribute("multiStartExperimentSize_", multiStartExperimentSize_);
  adv.saveAttribute("multiStartNumber_", multiStartNumber_);
  adv.saveAttribute("parameterEstimationPeriod_", parameterEstimationPeriod_);
  adv.saveAttribute("correlationLengthFactor_", correlationLengthFactor_);
  adv.saveAttribute("noiseFunction_", noiseFunction_);
  adv.saveAttribute("criterion_", criterion_);
  adv.saveAttribute("hasConverged_", hasConverged_);
  adv.saveAttribute("criterionValues_", criterionValues_);
}

/* Method load() reloads the object from the StorageManager */
void EfficientGlobalOptimization::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  adv.loadAttribute("gprResult_", gprResult_);
  adv.loadAttribute("solver_", solver_);
  adv.loadAttribute("multiStartExperimentSize_", multiStartExperimentSize_);
  adv.loadAttribute("multiStartNumber_", multiStartNumber_);
  adv.loadAttribute("parameterEstimationPeriod_", parameterEstimationPeriod_);
  adv.loadAttribute("correlationLengthFactor_", correlationLengthFactor_);
  if (adv.hasAttribute("noiseFunction_"))
    adv.loadAttribute("noiseFunction_", noiseFunction_);
  if (adv.hasAttribute("criterion_"))
    adv.loadAttribute("criterion_", criterion_);
  else
    // Files written before the criterion became pluggable hold the AEI trade
    // off on the algorithm itself: it belongs to the default criterion now.
    criterion_ = ExpectedImprovement();
  if (adv.hasAttribute("aeiTradeoff_"))
  {
    Scalar legacyAEITradeoff = 0.0;
    adv.loadAttribute("aeiTradeoff_", legacyAEITradeoff);
    ExpectedImprovement * p_expectedImprovement = dynamic_cast<ExpectedImprovement *>(criterion_.getImplementation().get());
    if (p_expectedImprovement != nullptr)
      p_expectedImprovement->setAEITradeoff(legacyAEITradeoff);
  }
  if (adv.hasAttribute("hasConverged_"))
    adv.loadAttribute("hasConverged_", hasConverged_);
  if (adv.hasAttribute("criterionValues_"))
    adv.loadAttribute("criterionValues_", criterionValues_);
  criterion_.setNoiseFunction(noiseFunction_);
}

END_NAMESPACE_OPENTURNS
