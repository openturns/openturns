//                                               -*- C++ -*-
/**
 *  @brief EfficientGlobalOptimization or EGO algorithm
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
#include "openturns/EfficientGlobalOptimization.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/KrigingAlgorithm.hxx"
#include "openturns/MultiStart.hxx"
#include "openturns/JointDistribution.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EfficientGlobalOptimization)

static const Factory<EfficientGlobalOptimization> Factory_EfficientGlobalOptimization;

/* Constructor with parameters */
EfficientGlobalOptimization::EfficientGlobalOptimization()
  : OptimizationAlgorithmImplementation()
  , solver_(OptimizationAlgorithm::Build(ResourceMap::GetAsString("EfficientGlobalOptimization-DefaultOptimizationAlgorithm")))
  , multiStartExperimentSize_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartExperimentSize"))
  , multiStartNumber_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartNumber"))
  , parameterEstimationPeriod_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultParameterEstimationPeriod"))
  , correlationLengthFactor_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultCorrelationLengthFactor"))
  , aeiTradeoff_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultAEITradeoff"))
{
}

/* Constructor with parameters */
EfficientGlobalOptimization::EfficientGlobalOptimization(const OptimizationProblem & problem,
    const KrigingResult & krigingResult,
    const Function & noise)
  : OptimizationAlgorithmImplementation(problem)
  , krigingResult_(krigingResult)
  , solver_(OptimizationAlgorithm::Build(ResourceMap::GetAsString("EfficientGlobalOptimization-DefaultOptimizationAlgorithm")))
  , multiStartExperimentSize_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartExperimentSize"))
  , multiStartNumber_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartNumber"))
  , parameterEstimationPeriod_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultParameterEstimationPeriod"))
  , correlationLengthFactor_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultCorrelationLengthFactor"))
  , aeiTradeoff_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultAEITradeoff"))
{
  checkProblem(problem);
  if (krigingResult_.getMetaModel().getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Metamodel must be 1-d";
  if (noise.getEvaluation().getImplementation()->isActualImplementation())
  {
    setMetamodelNoise(noise);
    setNoiseModel(noise);
  }
}


class ExpectedImprovementEvaluation : public EvaluationImplementation
{
public:
  ExpectedImprovementEvaluation (const Scalar optimalValue,
                                 const KrigingResult & metaModelResult,
                                 const Function & noiseModel,
                                 const Bool isMinimization)
    : EvaluationImplementation()
    , optimalValue_(optimalValue)
    , metaModelResult_(metaModelResult)
    , noiseModel_(noiseModel)
    , isMinimization_(isMinimization)
  {
  }

  virtual ExpectedImprovementEvaluation * clone() const
  {
    return new ExpectedImprovementEvaluation(*this);
  }

  Point operator()(const Point & x) const
  {
    return Point(1, computeAsScalar(x));
  }

  Scalar computeAsScalar(const Point & x) const
  {
    const Scalar mx = metaModelResult_.getConditionalMean(x)[0];
    const Scalar fmMk = isMinimization_ ? optimalValue_ - mx : mx - optimalValue_;
    const Scalar sk2 = metaModelResult_.getConditionalMarginalVariance(x);
    const Scalar sk = sqrt(sk2);
    if (!SpecFunc::IsNormal(sk))
      return SpecFunc::LowestScalar;
    const Scalar ratio = fmMk / sk;
    Scalar ei = fmMk * DistFunc::pNormal(ratio) + sk * DistFunc::dNormal(ratio);
    if (noiseModel_.getOutputDimension() == 1) // if provided
    {
      const Scalar noiseVariance = noiseModel_(x)[0];
      if (!(noiseVariance >= 0.0)) throw InvalidArgumentException(HERE) << "Noise model must be positive";
      ei *= (1.0 - sqrt(noiseVariance) / sqrt(noiseVariance + sk2));
    }
    return ei;
  }

  Sample operator()(const Sample & theta) const
  {
    const UnsignedInteger size = theta.getSize();
    // avoid creating size points
    Point theta_i(theta.getDimension());
    Sample outS(size, 1);
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      for (UnsignedInteger j = 0; j < theta_i.getSize(); ++ j)
        theta_i[j] = theta(i, j);
      outS(i, 0) = computeAsScalar(theta_i);
    }
    return outS;
  }

  UnsignedInteger getInputDimension() const
  {
    return metaModelResult_.getMetaModel().getInputDimension();
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  Description getInputDescription() const
  {
    return metaModelResult_.getMetaModel().getInputDescription();
  }

  Description getOutputDescription() const
  {
    return metaModelResult_.getMetaModel().getOutputDescription();
  }

protected:
  Scalar optimalValue_;
  KrigingResult metaModelResult_;
  Function noiseModel_;

  // whether the global problem is a miminization (the improvement criterion is always maximized)
  Bool isMinimization_ = true;
};




void EfficientGlobalOptimization::run()
{
  const OptimizationProblem problem(getProblem());
  const UnsignedInteger dimension = problem.getDimension();
  const Function model(problem.getObjective());
  Sample inputSample(krigingResult_.getInputSample());
  Sample outputSample(krigingResult_.getOutputSample());
  UnsignedInteger size = inputSample.getSize();
  Point noise(size);
  const Bool hasNoise = metamodelNoise_.getEvaluation().getImplementation()->isActualImplementation();
  if (hasNoise)
  {
    Sample noiseSample(metamodelNoise_(inputSample));
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      noise[i] = noiseSample(i, 0);
      if (!(noise[i] >= 0.0)) throw InvalidArgumentException(HERE) << "Noise model must be positive";
    }
  }
  UnsignedInteger evaluationNumber = 0;
  Bool exitLoop = false;

  // select the best feasible point in the initial DOE
  OptimizationResult result(getProblem());
  for (UnsignedInteger index = 0; index < size; ++ index)
    result.store(inputSample[index], outputSample[index], 0.0, 0.0, 0.0, 0.0);
  Point optimalPoint(result.getOptimalPoint());
  Point optimalValue(result.getOptimalValue());
  result = OptimizationResult(getProblem()); // reset to clear history
  LOGINFO(OSS() << "Initial best x=" << optimalPoint << " f(x)=" << optimalValue);

  // compute minimum distance between design points to assess the correlation lengths of the metamodel
  Point minimumDistance(dimension, SpecFunc::MaxScalar);
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
  KrigingResult metaModelResult(krigingResult_);

  while ((!exitLoop) && (evaluationNumber < getMaximumCallsNumber()))
  {
    Scalar optimalValueSubstitute = optimalValue[0];
    if (hasNoise)
    {
      // with noisy objective we don't have access to the real current optimal value
      // so consider a quantile of the kriging prediction: argmin_xi mk(xi) + c * sk(xi)
      optimalValueSubstitute = problem.isMinimization() ? SpecFunc::MaxScalar : SpecFunc::LowestScalar;
      const Sample mx(metaModelResult.getConditionalMean(inputSample));
      for (UnsignedInteger i = 0; i < size; ++ i)
      {
        const Point x(inputSample[i]);
        const Scalar sk2 = metaModelResult.getConditionalMarginalVariance(x);
        const Scalar u = mx(i, 0) + aeiTradeoff_ * sqrt(sk2);
        if ((problem.isMinimization() && (u < optimalValueSubstitute))
            || (!problem.isMinimization() && (u > optimalValueSubstitute)))
        {
          optimalValueSubstitute = u;
        }
      }
    }

    Function improvementObjective(new ExpectedImprovementEvaluation(optimalValueSubstitute, metaModelResult, noiseModel_, problem.isMinimization()));

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
      JointDistribution::DistributionCollection coll;
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        if (!finiteLowerBound[i] || !finiteUpperBound[i])
          throw InvalidArgumentException(HERE) << "Bounds must be finite";
        coll.add(Uniform(lowerBound[i], upperBound[i]));
      }
      const JointDistribution distribution(coll);
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
    expectedImprovement_.add(improvementValue);

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
      const Point scale(metaModelResult.getCovarianceModel().getScale());
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

    if (hasNoise)
    {
      const Point newNoise(metamodelNoise_(newPoint));
      if (!(newNoise[0] >= 0.0)) throw InvalidArgumentException(HERE) << "Noise model must be positive";
      noise.add(newNoise[0]);
    }

    ++ iterationNumber;

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

    if (evaluationNumber > 0)
    {
      KrigingAlgorithm algo(inputSample, outputSample, metaModelResult.getCovarianceModel(), metaModelResult.getBasis());
      LOGINFO(OSS() << "Rebuilding kriging ...");
      algo.setOptimizeParameters((parameterEstimationPeriod_ > 0) && ((evaluationNumber % parameterEstimationPeriod_) == 0));
      if (hasNoise)
        algo.setNoise(noise);
      algo.run();
      LOGINFO(OSS() << "Rebuilding kriging - done");
      metaModelResult = algo.getResult();
    }
  } // while

  krigingResult_ = metaModelResult; // update krigingResult_ to take new points into account
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
  return OSS();
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

/* Expected improvement function */
Sample EfficientGlobalOptimization::getExpectedImprovement() const
{
  return expectedImprovement_;
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
  aeiTradeoff_ = aeiTradeoff;
}

Scalar EfficientGlobalOptimization::getAEITradeoff() const
{
  return aeiTradeoff_;
}

/* metamodel noise accessor */
void EfficientGlobalOptimization::setMetamodelNoise(const Function & noiseModel)
{
  const UnsignedInteger dimension = getProblem().getDimension();
  if (noiseModel.getInputDimension() != dimension) throw InvalidArgumentException(HERE) << "Noise model must be of dimension " << dimension;
  if (noiseModel.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Noise model must be 1-d";
  metamodelNoise_ = noiseModel;
}

Function EfficientGlobalOptimization::getMetamodelNoise() const
{
  return metamodelNoise_;
}

/* optimization noise accessor */
void EfficientGlobalOptimization::setNoiseModel(const Function & noiseModel)
{
  const UnsignedInteger dimension = getProblem().getDimension();
  if (noiseModel.getInputDimension() != dimension) throw InvalidArgumentException(HERE) << "Noise model must be of dimension " << dimension;
  if (noiseModel.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Noise model must be 1-d";
  noiseModel_ = noiseModel;
}

Function EfficientGlobalOptimization::getNoiseModel() const
{
  return noiseModel_;
}


/* Kriging result accessor (especially useful after run() has been called) */
KrigingResult EfficientGlobalOptimization::getKrigingResult() const
{
  return krigingResult_;
}


/* Method save() stores the object through the StorageManager */
void EfficientGlobalOptimization::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("krigingResult_", krigingResult_);
  adv.saveAttribute("solver_", solver_);
  adv.saveAttribute("multiStartExperimentSize_", multiStartExperimentSize_);
  adv.saveAttribute("multiStartNumber_", multiStartNumber_);
  adv.saveAttribute("parameterEstimationPeriod_", parameterEstimationPeriod_);
  adv.saveAttribute("correlationLengthFactor_", correlationLengthFactor_);
  adv.saveAttribute("aeiTradeoff_", aeiTradeoff_);
  adv.saveAttribute("noiseModel_", noiseModel_);
}

/* Method load() reloads the object from the StorageManager */
void EfficientGlobalOptimization::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  adv.loadAttribute("krigingResult_", krigingResult_);
  adv.loadAttribute("solver_", solver_);
  adv.loadAttribute("multiStartExperimentSize_", multiStartExperimentSize_);
  adv.loadAttribute("multiStartNumber_", multiStartNumber_);
  adv.loadAttribute("parameterEstimationPeriod_", parameterEstimationPeriod_);
  adv.loadAttribute("correlationLengthFactor_", correlationLengthFactor_);
  adv.loadAttribute("aeiTradeoff_", aeiTradeoff_);
  adv.loadAttribute("noiseModel_", noiseModel_);
}

END_NAMESPACE_OPENTURNS
