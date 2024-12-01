//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of a RatioOfUniforms generator
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/RatioOfUniforms.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RatioOfUniforms)

static const Factory<RatioOfUniforms> Factory_RatioOfUniforms;

/* Default constructor */
RatioOfUniforms::RatioOfUniforms()
  : RandomVectorImplementation()
{
  // Prepare the ratio of uniforms method
  candidateNumber_ = ResourceMap::GetAsUnsignedInteger("RatioOfUniforms-CandidateNumber");
  optimizationAlgorithm_ = OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("RatioOfUniforms-OptimizationAlgorithm"));
}

/* Constructor with parameters */
RatioOfUniforms::RatioOfUniforms(const Distribution & distribution)
  : RandomVectorImplementation()
{
  // Prepare the ratio of uniforms method
  candidateNumber_ = ResourceMap::GetAsUnsignedInteger("RatioOfUniforms-CandidateNumber");
  optimizationAlgorithm_ = OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("RatioOfUniforms-OptimizationAlgorithm"));
  setDistribution(distribution);
}

/* Virtual constructor */
RatioOfUniforms * RatioOfUniforms::clone() const
{
  return new RatioOfUniforms(*this);
}

/* String converter */
String RatioOfUniforms::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " distribution=" << distribution_;
  return oss;
}

/* Optimization algorithm accessor */
void RatioOfUniforms::setOptimizationAlgorithm(const OptimizationAlgorithm & optimizationAlgorithm)
{
  optimizationAlgorithm_ = optimizationAlgorithm;
}

OptimizationAlgorithm RatioOfUniforms::getOptimizationAlgorithm() const
{
  return optimizationAlgorithm_;
}

/* Candidate number accessor */
void RatioOfUniforms::setCandidateNumber(const UnsignedInteger candidateNumber)
{
  if (candidateNumber == 0)
    throw InvalidArgumentException(HERE) << "Error: the candidate number must be at least 1";
  candidateNumber_ = candidateNumber;
}

UnsignedInteger RatioOfUniforms::getCandidateNumber() const
{
  return candidateNumber_;
}

/* R accessor */
void RatioOfUniforms::setR(const Scalar r)
{
  if (!(r > 0.0))
    throw InvalidArgumentException(HERE) << "Error: the parameter r must be stricly positive, here r=" << r;
  r_ = r;
  initialize();
}

Scalar RatioOfUniforms::getR() const
{
  return r_;
}

/* Acceptance ratio accessor */
Scalar RatioOfUniforms::getAcceptanceRatio() const
{
  if (!isInitialized())
    throw InternalException(HERE) << "Error: RatioOfUniforms was not initialized. Call initialize() to fix it.";
  const UnsignedInteger dimension = distribution_.getDimension();
  Scalar ratio = 1.0 / ((1.0 + r_ * dimension) * supU_);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    ratio /= supV_[i] - infV_[i];
  return ratio;
}

Bool RatioOfUniforms::isInitialized() const
{
  return (infV_.getSize() > 0);
}

class RatioOfUniformsUBoundEvaluation : public EvaluationImplementation
{
public:
  RatioOfUniformsUBoundEvaluation(const Distribution & distribution, const Scalar r)
    : EvaluationImplementation()
    , distribution_(distribution)
    , r_(r)
  {
    // Nothing to do
  }

  RatioOfUniformsUBoundEvaluation * clone() const override
  {
    return new RatioOfUniformsUBoundEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return distribution_.getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator()(const Point & inP) const override
  {
    const UnsignedInteger dimension = distribution_.getDimension();
    Scalar result = distribution_.computeLogPDF(inP) / (1.0 + r_ * dimension);
    result = std::max(-SpecFunc::LogMaxScalar, result);
    return {result};
  }

private:
  Distribution distribution_;
  Scalar r_ = 0.0;

};

class RatioOfUniformsVBoundEvaluation : public EvaluationImplementation
{
public:
  RatioOfUniformsVBoundEvaluation(const Distribution & distribution, const Scalar r)
    : EvaluationImplementation()
    , distribution_(distribution)
    , r_(r)
  {
    // Nothing to do
  }

  RatioOfUniformsVBoundEvaluation * clone() const override
  {
    return new RatioOfUniformsVBoundEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return distribution_.getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return distribution_.getDimension();
  }

  Point operator()(const Point & inP) const override
  {
    const UnsignedInteger dimension = distribution_.getDimension();
    const Scalar value = distribution_.computeLogPDF(inP) * r_ / (1.0 + r_ * dimension);
    Point result(dimension, value);
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      result[i] += std::log(std::abs(inP[i]));
      result[i] = std::max(-SpecFunc::LogMaxScalar, result[i]);
    }
    return result;
  }

private:
  Distribution distribution_;
  Scalar r_ = 0.0;
};

/* Distribution accessor */
void RatioOfUniforms::setDistribution(const Distribution & distribution)
{
  if (!distribution.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: the ratio of uniforms algorithm works only with continuous distributions, here distribution=" << distribution;
  distribution_ = distribution;
  initialize();
}

/* Initialization */
void RatioOfUniforms::initialize()
{
  // r_ is a free parameter, could be optimized to maximize the acceptance ratio
  const UnsignedInteger dimension = distribution_.getDimension();
  const Interval bounds(distribution_.getRange());
  const Point lb(bounds.getLowerBound());
  const Point ub(bounds.getUpperBound());

  // find a feasible starting point
  SobolSequence sequence(dimension);
  Point start;
  for (UnsignedInteger k = 0; k < candidateNumber_; ++ k)
    {
      Point candidate(sequence.generate());
      for (UnsignedInteger j = 0; j < dimension; ++ j)
        candidate[j] = lb[j] + candidate[j] * (ub[j] - lb[j]);
      if (SpecFunc::IsNormal(distribution_.computeLogPDF(candidate)))
	{
	  start = candidate;
	  break;
	}
    } // for k
  if (!start.getDimension())
    throw InternalException(HERE) << "Could not find a feasible starting point to initialize ratio of uniforms U sup";
  
  // First, the upper bound on U
  const Function objectiveU(new RatioOfUniformsUBoundEvaluation(distribution_, r_));
  OptimizationProblem problemU(objectiveU);
  problemU.setMinimization(false);
  problemU.setBounds(bounds);
  optimizationAlgorithm_.setProblem(problemU);
  optimizationAlgorithm_.setStartingPoint(start);
  optimizationAlgorithm_.run();
  supU_ = std::exp(optimizationAlgorithm_.getResult().getOptimalValue()[0]);
  LOGDEBUG(OSS() << "supU_=" << supU_ << " u*=" << optimizationAlgorithm_.getResult().getOptimalPoint());
  
  // Second, the lower and upper bounds on V
  const Function objectiveV(new RatioOfUniformsVBoundEvaluation(distribution_, r_));
  infV_.resize(dimension);
  supV_.resize(dimension);
  const Point zero(dimension, 0.0);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      const Function objectiveVI(objectiveV.getMarginal(i));
      OptimizationProblem problemVI(objectiveVI);
      problemVI.setMinimization(false);
      if (ub[i] > 0.0)
	{
	  // find a feasible starting point in [0, ub]
	  start.clear();
	  for (UnsignedInteger k = 0; k < candidateNumber_; ++ k)
	    {
	      Point candidate(sequence.generate());
	      for (UnsignedInteger j = 0; j < dimension; ++ j)
		candidate[j] = candidate[j] * ub[j];
	      if (SpecFunc::IsNormal(distribution_.computeLogPDF(candidate)))
		{
		  start = candidate;
		  break;
		}
	    } // for k
	  if (!start.getDimension())
	    throw InternalException(HERE) << "Could not find a feasible starting point to initialize ratio of uniforms V sup";
	  problemVI.setBounds(Interval(zero, ub));
	  optimizationAlgorithm_.setProblem(problemVI);
	  optimizationAlgorithm_.setStartingPoint(start);
	  optimizationAlgorithm_.run();
	  supV_[i] = std::exp(optimizationAlgorithm_.getResult().getOptimalValue()[0]);
	  LOGDEBUG(OSS() << "supV_[" << i << "]=" << supV_[i] << " v*=" << optimizationAlgorithm_.getResult().getOptimalPoint());
	} // if ub[i] > 0.0
      if (lb[i] < 0.0)
      {
        // find a feasible starting point in [lb, 0]
        start.clear();
        for (UnsignedInteger k = 0; k < candidateNumber_; ++ k)
	  {
	    Point candidate(sequence.generate());
	    for (UnsignedInteger j = 0; j < dimension; ++ j)
	      candidate[j] = candidate[j] * lb[j];
	    if (SpecFunc::IsNormal(distribution_.computeLogPDF(candidate)))
	      {
		start = candidate;
		break;
	      }
	  } // for k
        if (!start.getDimension())
          throw InternalException(HERE) << "Could not find a feasible starting point to initialize ratio of uniforms V inf";
        problemVI.setBounds(Interval(lb, zero));
        optimizationAlgorithm_.setProblem(problemVI);
        optimizationAlgorithm_.setStartingPoint(start);
        optimizationAlgorithm_.run();
        infV_[i] = -std::exp(optimizationAlgorithm_.getResult().getOptimalValue()[0]);
        LOGDEBUG(OSS() << "infV_[" << i << "]=" << infV_[i] << " v*=" << optimizationAlgorithm_.getResult().getOptimalPoint());
      } // if lb[i] < 0.0
    } // for i
}

/* Sample generation */
Point RatioOfUniforms::getRealization() const
{
  return getSample(1)[0];
}

Sample RatioOfUniforms::getSample(const UnsignedInteger size) const
{
  if (!infV_.getSize())
    throw InternalException(HERE) << "Error: RatioOfUniforms was not initialized. Call initialize() to fix it.";

  // Now, the sampling using rejection
  const UnsignedInteger dimension = distribution_.getDimension();
  Sample sample(size, dimension);
  Point result(dimension);
  for (UnsignedInteger n = 0; n < size; ++n)
    {
      Bool accepted = false;
      while (!accepted)
	{
	  const Scalar u = supU_ * RandomGenerator::Generate();
	    const Scalar ur = std::pow(u, r_);
	    for (UnsignedInteger i = 0; i < dimension; ++ i)
	      result[i] = (infV_[i] + (supV_[i] - infV_[i]) * RandomGenerator::Generate()) / ur;
	    accepted = (1.0 + r_ * dimension) * std::log(u) <= distribution_.computeLogPDF(result);
	  } // !accepted
      sample[n] = result;
    } // for n
  return sample;
}

END_NAMESPACE_OPENTURNS
