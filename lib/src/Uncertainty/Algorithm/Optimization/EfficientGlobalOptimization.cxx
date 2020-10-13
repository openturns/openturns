//                                               -*- C++ -*-
/**
 *  @brief EfficientGlobalOptimization or EGO algorithm
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Cobyla.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/KrigingAlgorithm.hxx"
#include "openturns/MultiStart.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EfficientGlobalOptimization)

static const Factory<EfficientGlobalOptimization> Factory_EfficientGlobalOptimization;

/* Constructor with parameters */
EfficientGlobalOptimization::EfficientGlobalOptimization()
  : OptimizationAlgorithmImplementation()
  , solver_(new Cobyla)
  , useDefaultSolver_(true)
  , multiStartExperimentSize_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartExperimentSize"))
  , multiStartNumber_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartNumber"))
  , parameterEstimationPeriod_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultParameterEstimationPeriod"))
  , improvementFactor_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultImprovementFactor"))
  , correlationLengthFactor_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultCorrelationLengthFactor"))
  , aeiTradeoff_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultAEITradeoff"))
{
}

/* Constructor with parameters */
EfficientGlobalOptimization::EfficientGlobalOptimization(const OptimizationProblem & problem,
    const KrigingResult & krigingResult)
  : OptimizationAlgorithmImplementation(problem)
  , krigingResult_(krigingResult)
  , solver_(new Cobyla)
  , useDefaultSolver_(true)
  , multiStartExperimentSize_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartExperimentSize"))
  , multiStartNumber_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartNumber"))
  , parameterEstimationPeriod_(ResourceMap::GetAsUnsignedInteger("EfficientGlobalOptimization-DefaultParameterEstimationPeriod"))
  , improvementFactor_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultImprovementFactor"))
  , correlationLengthFactor_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultCorrelationLengthFactor"))
  , aeiTradeoff_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultAEITradeoff"))
{
  checkProblem(problem);
  if (krigingResult_.getMetaModel().getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Metamodel must be 1-d";
}


class ExpectedImprovementEvaluation : public EvaluationImplementation
{
public:
  ExpectedImprovementEvaluation (const Scalar optimalValue,
                                 const KrigingResult & metaModelResult,
                                 const Function & noiseModel)
    : EvaluationImplementation()
    , optimalValue_(optimalValue)
    , metaModelResult_(metaModelResult)
    , noiseModel_(noiseModel)
  {
  }

  virtual ExpectedImprovementEvaluation * clone() const
  {
    return new ExpectedImprovementEvaluation(*this);
  }

  Point operator()(const Point & x) const
  {
    const Scalar mx = metaModelResult_.getConditionalMean(x)[0];
    const Scalar fmMk = optimalValue_ - mx;
    const Scalar sk2 = metaModelResult_.getConditionalMarginalVariance(x);
    const Scalar sk = sqrt(sk2);
    if (!SpecFunc::IsNormal(sk)) return Point(1, SpecFunc::LowestScalar);
    const Scalar ratio = fmMk / sk;
    Scalar ei = fmMk * DistFunc::pNormal(ratio) + sk * DistFunc::dNormal(ratio);
    if (noiseModel_.getOutputDimension() == 1) // if provided
    {
      const Scalar noiseVariance = noiseModel_(x)[0];
      if (!(noiseVariance >= 0.0)) throw InvalidArgumentException(HERE) << "Noise model must be positive";
      ei *= (1.0 - sqrt(noiseVariance) / sqrt(noiseVariance + sk2));
    }
    return Point(1, ei);
  }

  Sample operator()(const Sample & theta) const
  {
    const UnsignedInteger size = theta.getSize();
    Sample outS(size, 1);
    for (UnsignedInteger i = 0; i < size; ++ i)
      outS[i] = operator()(theta[i]);
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
  const Bool hasNoise = model.getOutputDimension() == 2;
  if (hasNoise)
  {
    // always use 2nd marginal to evaluate noise at initial design and new points
    Sample noiseSample(model.getMarginal(1)(inputSample));
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      noise[i] = noiseSample(i, 0);
      if (!(noise[i] >= 0.0)) throw InvalidArgumentException(HERE) << "Noise model must be positive";
    }

    // use noise model for criterion optimization if provided, else fallback to objective 2nd marginal
    if (noiseModel_.getOutputDimension() != 1)
      noiseModel_ = model.getMarginal(1);
  }
  UnsignedInteger evaluationNumber = 0;
  Bool exitLoop = false;

  // select the best feasible point
  Point optimizer;
  Scalar optimalValue = problem.isMinimization() ? SpecFunc::MaxScalar : SpecFunc::LowestScalar;
  Point optimizerPrev; // previous optimizer
  Scalar optimalValuePrev = optimalValue;// previous optimal value
  for (UnsignedInteger index = 0; index < size; ++ index)
  {
    if (!problem.hasBounds() || (problem.hasBounds() && problem.getBounds().contains(inputSample[index])))
      if ((problem.isMinimization() && (outputSample(index, 0) < optimalValue))
          || (!problem.isMinimization() && (outputSample(index, 0) > optimalValue)))
      {
        optimizerPrev = optimizer;
        optimalValuePrev = optimalValue;

        optimizer = inputSample[index];
        optimalValue = outputSample(index, 0);
      }
  }

  LOGINFO(OSS() << "Optimum so far x=" << optimizer << " f(x)=" << optimalValue);

  // we need the second best to compute convergence criteria
  if (optimizerPrev.getDimension() == 0)
  {
    // then the optimum was the first
    for (UnsignedInteger index = 1; index < size; ++ index)
    {
      if (!problem.hasBounds() || (problem.hasBounds() && problem.getBounds().contains(inputSample[index])))
        if ((problem.isMinimization() && (outputSample(index, 0) < optimalValuePrev))
            || (!problem.isMinimization() && (outputSample(index, 0) > optimalValuePrev)))
        {
          optimizerPrev = inputSample[index];
          optimalValuePrev = outputSample(index, 0);
        }
    }
  }

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

  OptimizationResult result(dimension);
  result.setProblem(getProblem());

  UnsignedInteger iterationNumber = 0;
  while ((!exitLoop) && (evaluationNumber < getMaximumEvaluationNumber()))
  {
    // use the provided kriging result at first iteration
    KrigingResult metaModelResult(krigingResult_);
    if (evaluationNumber > 0)
    {
      KrigingAlgorithm algo(inputSample, outputSample, metaModelResult.getCovarianceModel(), krigingResult_.getBasisCollection());
      LOGINFO(OSS() << "Rebuilding kriging ...");
      algo.setOptimizeParameters((parameterEstimationPeriod_ > 0) && ((evaluationNumber % parameterEstimationPeriod_) == 0));
      if (hasNoise)
        algo.setNoise(noise);
      algo.run();
      LOGINFO(OSS() << "Rebuilding kriging - done");
      metaModelResult = algo.getResult();
    }

    Scalar optimalValueSubstitute = optimalValue;
    if (hasNoise)
    {
      // with noisy objective we dont have access to the real current optimal value
      // so consider a quantile of the kriging prediction: argmin_xi mk(xi) + c * sk(xi)
      optimalValueSubstitute = problem.isMinimization() ? SpecFunc::MaxScalar : SpecFunc::LowestScalar;
      const Point mx(metaModelResult.getConditionalMean(inputSample));
      for (UnsignedInteger i = 0; i < size; ++ i)
      {
        const Point x(inputSample[i]);
        const Scalar sk2 = metaModelResult.getConditionalMarginalVariance(x);
        const Scalar u = mx[i] + aeiTradeoff_ * sqrt(sk2);
        if ((problem.isMinimization() && (u < optimalValueSubstitute))
            || (!problem.isMinimization() && (u > optimalValueSubstitute)))
        {
          optimalValueSubstitute = u;
        }
      }
    }

    Function improvementObjective(new ExpectedImprovementEvaluation(optimalValueSubstitute, metaModelResult, noiseModel_));

    // use multi-start to optimize the improvement criterion when using the default solver
    if (useDefaultSolver_ && problem.hasBounds())
    {
      // Sample uniformly into the bounds
      const Interval bounds(problem.getBounds());
      const Point lowerBound(bounds.getLowerBound());
      const Point upperBound(bounds.getUpperBound());
      const Interval::BoolCollection finiteLowerBound(bounds.getFiniteLowerBound());
      const Interval::BoolCollection finiteUpperBound(bounds.getFiniteUpperBound());
      ComposedDistribution::DistributionCollection coll;
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        if (!finiteLowerBound[i] || !finiteUpperBound[i])
          throw InvalidArgumentException(HERE) << "Bounds must be finite";
        coll.add(Uniform(lowerBound[i], upperBound[i]));
      }
      const ComposedDistribution distribution(coll);
      Sample improvementExperiment(distribution.getSample(multiStartExperimentSize_));
      // retain best P/N points as starting points
      improvementExperiment.stack(improvementObjective(improvementExperiment));
      Indices inputs(dimension);
      inputs.fill();
      const Sample sortedImprovement(improvementExperiment.sortAccordingToAComponent(dimension).getMarginal(inputs));
      // handle multiStartExperimentSize_ < multiStartNumber_
      const UnsignedInteger pointNumber = std::min(multiStartNumber_, multiStartExperimentSize_);
      const Sample startingPoints(sortedImprovement, multiStartExperimentSize_ - pointNumber, multiStartExperimentSize_);
      setOptimizationAlgorithm(MultiStart(solver_, startingPoints));
    }

    // build improvement criterion optimization problem
    OptimizationProblem maximizeImprovement(improvementObjective);
    maximizeImprovement.setMinimization(false);
    if (problem.hasBounds())
      maximizeImprovement.setBounds(problem.getBounds());
    solver_.setProblem(maximizeImprovement);
    solver_.setStartingPoint(optimizer);
    solver_.run();
    const OptimizationResult improvementResult(solver_.getResult());

    // store improvement
    Point improvementValue(improvementResult.getOptimalValue());
    expectedImprovement_.add(improvementValue);

    const Point newPoint(improvementResult.getOptimalPoint());
    const Point newOutput(model(newPoint));
    ++ evaluationNumber;
    const Point newValue(Point(1, newOutput[0]));// noise can be provided on the 2nd marginal

    LOGINFO(OSS() << "New point x=" << newPoint << " f(x)=" << newValue << " evaluations=" << evaluationNumber);

    // is the new point better ?
    if ((problem.isMinimization() && (newValue[0] < optimalValue))
        || (!problem.isMinimization() && (newValue[0] > optimalValue)))
    {
      optimizerPrev = optimizer;
      optimalValuePrev = optimalValue;

      optimizer = newPoint;
      optimalValue = newValue[0];
      LOGINFO(OSS() << "Optimum so far x=" << optimizer << " f(x)=" << optimalValue);
    }

    // algorithm is global so compute convergence criteria on the last 2 optimum instead of last 2 points
    const Scalar absoluteError = (optimizer - optimizerPrev).normInf();
    const Scalar relativeError = absoluteError / optimizer.normInf();
    const Scalar residualError = std::abs(optimalValue - optimalValuePrev);
    const Scalar constraintError = -1.0;

    result.store(newPoint, newValue, absoluteError, relativeError, residualError, constraintError);

    // general convergence criteria
    exitLoop = ((absoluteError < getMaximumAbsoluteError()) && (relativeError < getMaximumRelativeError())) || ((residualError < getMaximumResidualError()) && (constraintError < getMaximumConstraintError()));

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

    // improvement stopping criterion
    const Bool improvementStop = (improvementValue[0] < improvementFactor_ * std::abs(optimalValue));
    if (improvementStop) LOGINFO(OSS() << "Stopped algorithm over the improvement criterion");
    exitLoop = exitLoop || improvementStop;

    // add new point to design
    inputSample.add(newPoint);
    outputSample.add(newValue);
    ++ size;

    if (hasNoise)
    {
      if (!(newOutput[1] >= 0.0)) throw InvalidArgumentException(HERE) << "Noise model must be positive";
      noise.add(newOutput[1]);
    }

    ++ iterationNumber;

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
        LOGWARN(OSS() << "EGO was stopped by user");
      }
    }
  }
  result.setOptimalPoint(optimizer);
  result.setOptimalValue(Point(1, optimalValue));
  result.setEvaluationNumber(evaluationNumber);
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
  if (problem.getObjective().getOutputDimension() > 2) // 2nd marginal can be used as noise
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

/* improvement criterion factor accessor */
void EfficientGlobalOptimization::setImprovementFactor(const Scalar improvementFactor)
{
  improvementFactor_ = improvementFactor;
}

Scalar EfficientGlobalOptimization::getImprovementFactor() const
{
  return improvementFactor_;
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

/* Method save() stores the object through the StorageManager */
void EfficientGlobalOptimization::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("krigingResult_", krigingResult_);
  adv.saveAttribute("solver_", solver_);
  adv.saveAttribute("multiStartExperimentSize_", multiStartExperimentSize_);
  adv.saveAttribute("multiStartNumber_", multiStartNumber_);
  adv.saveAttribute("parameterEstimationPeriod_", parameterEstimationPeriod_);
  adv.saveAttribute("improvementFactor_", improvementFactor_);
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
  adv.loadAttribute("improvementFactor_", improvementFactor_);
  adv.loadAttribute("correlationLengthFactor_", correlationLengthFactor_);
  adv.loadAttribute("aeiTradeoff_", aeiTradeoff_);
  adv.loadAttribute("noiseModel_", noiseModel_);
}

END_NAMESPACE_OPENTURNS
