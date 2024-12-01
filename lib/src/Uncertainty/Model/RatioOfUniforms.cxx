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
#include "openturns/SymbolicFunction.hxx"
#include "openturns/MultiStart.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RatioOfUniforms)

static const Factory<RatioOfUniforms> Factory_RatioOfUniforms;

/* Default constructor */
RatioOfUniforms::RatioOfUniforms()
  : RandomVectorImplementation()
{
  // Prepare the ratio of uniforms method
  candidateNumber_ = ResourceMap::GetAsUnsignedInteger("RatioOfUniforms-CandidateNumber");
  maximumMultistart_ = ResourceMap::GetAsUnsignedInteger("RatioOfUniforms-MaximumMultistart");
  optimizationAlgorithm_ = OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("RatioOfUniforms-OptimizationAlgorithm"));
  setLogUnscaledPDFAndRange(SymbolicFunction("x", "0.0"), Interval(0.0, 1.0), true);
}

/* Constructor with parameters */
RatioOfUniforms::RatioOfUniforms(const Function & logUnscaledPDF,
				 const Interval & range,
				 const Bool isScaled)
  : RandomVectorImplementation()
{
  // Prepare the ratio of uniforms method
  candidateNumber_ = ResourceMap::GetAsUnsignedInteger("RatioOfUniforms-CandidateNumber");
  maximumMultistart_ = ResourceMap::GetAsUnsignedInteger("RatioOfUniforms-MaximumMultistart");
  optimizationAlgorithm_ = OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("RatioOfUniforms-OptimizationAlgorithm"));
  setLogUnscaledPDFAndRange(logUnscaledPDF, range, isScaled);
}

/* Constructor with parameters */
RatioOfUniforms::RatioOfUniforms(const Distribution & distribution)
  : RatioOfUniforms(distribution.getLogPDF(), distribution.getRange(), true)
{
  // Nothing to do
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
      << " logUnscaledPDF=" << logUnscaledPDF_
      << " range=" << range_
      << " r=" << r_
      << " supU=" << supU_
      << " infV=" << infV_
      << " supV=" << supV_
      << " acceptanceRatio=" << acceptanceRatio_
      << " optimizationAlgorithm=" << optimizationAlgorithm_;
  return oss;
}

String RatioOfUniforms::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName()
      << "(logUnscaledPDF=" << logUnscaledPDF_
      << ", range=" << range_
      << ", r=" << r_
      << ", supU=" << supU_
      << ", infV=" << infV_
      << ", supV=" << supV_
      << ", acceptanceRatio=" << acceptanceRatio_
      << " optimizationAlgorithm=" << optimizationAlgorithm_.getImplementation()->getClassName()
      << ")";
  return oss;
}

/* LogPDF and range accessor */
void RatioOfUniforms::setLogUnscaledPDFAndRange(const Function & logUnscaledPDF,
						const Interval & range,
						const Bool isScaled)
{
  if (logPDF.getInputDimension() != range.getDimension()) throw InvalidArgumentException(HERE) << "Error: the log unscaled PDF input dimension must match the range dimension, here log unscaled PDF input dimension=" << logPDF.getInputDimension() << " and range dimension=" << range.getDimension();
  if (logPDF.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the log unscaled PDF output dimension must be equal to 1, here log unscaled PDF output dimension=" << logPDF.getOutputDimension();
  logUnscaledPDF_ = logUnscaledPDF;
  range_ = range;
  isScaled_ = isScaled;
  (void) initialize();
}

Function RatioOfUniforms::getLogUnscaledPDF() const
{
  return logUnscaledPDF_;
}

Interval RatioOfUniforms::getRange() const
{
  return range_;
}

/* Bounds accessors */
Scalar RatioOfUniforms::getSupU() const
{
  return supU_;
}

Point RatioOfUniforms::getInfV() const
{
  return infV_;
}

Point RatioOfUniforms::getSupV() const
{
  return supV_;
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
  (void) initialize();
}

Scalar RatioOfUniforms::getR() const
{
  return r_;
}

/* C accessor */
Scalar RatioOfUniforms::getC() const
{
  if (isScaled_)
    return 1.0;
  if (!isInitialized())
    throw InternalException(HERE) << "Error: RatioOfUniforms was not initialized. Call initialize() to fix it.";
  const UnsignedInteger dimension = range_.getDimension();
  Scalar c = 1.0 / (acceptanceRatio_ * (1.0 + r_ * dimension) * supU_);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    c /= supV_[i] - infV_[i];
  return c;
}

/* Acceptance ratio accessor */
Scalar RatioOfUniforms::getAcceptanceRatio() const
{
  if (!isInitialized())
    throw InternalException(HERE) << "Error: RatioOfUniforms was not initialized. Call initialize() to fix it.";
  return acceptanceRatio_;
}

Bool RatioOfUniforms::isInitialized() const
{
  return (infV_.getSize() > 0);
}

class RatioOfUniformsUBoundEvaluation : public EvaluationImplementation
{
public:
  RatioOfUniformsUBoundEvaluation(const Function & logPDF,
				  const Interval & range,
				  const Scalar r)
    : EvaluationImplementation()
    , logPDF_(logPDF)
    , range_(range)
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
    return range_.getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator()(const Point & inP) const override
  {
    const UnsignedInteger dimension = range_.getDimension();
    Scalar result = logPDF_(inP)[0] / (1.0 + r_ * dimension);
    result = std::max(-SpecFunc::LogMaxScalar, result);
    return {result};
  }

private:
  Function logPDF_;
  Interval range_;
  Scalar r_ = 0.0;

};

class RatioOfUniformsVBoundEvaluation : public EvaluationImplementation
{
public:
  RatioOfUniformsVBoundEvaluation(const Function & logPDF,
				  const Interval & range,
				  const Scalar r)
    : EvaluationImplementation()
    , logPDF_(logPDF)
    , range_(range)
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
    return range_.getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return range_.getDimension();
  }

  Point operator()(const Point & inP) const override
  {
    const UnsignedInteger dimension = range_.getDimension();
    const Scalar value = logPDF_(inP)[0] * r_ / (1.0 + r_ * dimension);
    Point result(dimension, value);
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      result[i] += std::log(std::abs(inP[i]));
      result[i] = std::max(-SpecFunc::LogMaxScalar, result[i]);
    }
    return result;
  }

private:
  Function logPDF_;
  Interval range_;
  Scalar r_ = 0.0;
};

/* Initialization */
Collection<MultiStart> RatioOfUniforms::initialize()
{
  // r_ is a free parameter, could be optimized to maximize the acceptance ratio
  const UnsignedInteger dimension = range_.getDimension();
  const Point lb(range_.getLowerBound());
  const Point ub(range_.getUpperBound());

  // find a feasible starting point
  Collection<MultiStart> allMultiStarts(0);
  SobolSequence sequence(dimension);
  Sample startingPointsU(0, dimension);
  for (UnsignedInteger k = 0; k < candidateNumber_; ++ k)
    {
      Point candidate(sequence.generate());
      for (UnsignedInteger j = 0; j < dimension; ++ j)
        candidate[j] = lb[j] + candidate[j] * (ub[j] - lb[j]);
      if (SpecFunc::IsNormal(logPDF_(candidate)[0]))
	startingPointsU.add(candidate);
    } // for k
  if (!startingPointsU.getSize())
    throw InternalException(HERE) << "Could not find a feasible starting point to initialize ratio of uniforms U sup";

  // First, the upper bound on U
  const Function objectiveU(new RatioOfUniformsUBoundEvaluation(logPDF_, range_, r_));
  OptimizationProblem problemU(objectiveU);
  problemU.setMinimization(false);
  problemU.setBounds(range_);
  optimizationAlgorithm_.setProblem(problemU);
  MultiStart multistart(optimizationAlgorithm_, startingPointsU);
  multistart.run();
  allMultiStarts.add(multistart);
  supU_ = std::exp(multistart.getResult().getOptimalValue()[0]);
  LOGDEBUG(OSS() << "supU_=" << supU_ << " u*=" << multistart.getResult().getOptimalPoint());

  // Second, the lower and upper bounds on V
  const Function objectiveV(new RatioOfUniformsVBoundEvaluation(logPDF_, range_, r_));
  infV_.resize(dimension);
  supV_.resize(dimension);
  const Point zero(dimension, 0.0);
  Sample startingPointsUB(0, dimension);
  Sample startingPointsLB(0, dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      const Function objectiveVI(objectiveV.getMarginal(i));
      OptimizationProblem problemVI(objectiveVI);
      problemVI.setMinimization(false);
      if (ub[i] > 0.0)
	{
	  // find a feasible starting point in [0, ub]
	  if (!startingPointsUB.getSize())
	    {
	      for (UnsignedInteger k = 0; k < candidateNumber_; ++ k)
		{
		  Point candidate(sequence.generate());
		  for (UnsignedInteger j = 0; j < dimension; ++ j)
		    candidate[j] = candidate[j] * ub[j];
		  if (SpecFunc::IsNormal(logPDF_(candidate)[0]))
		    startingPointsUB.add(candidate);
		} // for k
	    } // if (!startingPointsUB.getSize())
	  if (!startingPointsUB.getSize())
	    throw InternalException(HERE) << "Could not find a feasible starting point to initialize ratio of uniforms V sup";
	  problemVI.setBounds(Interval(zero, ub));
	  optimizationAlgorithm_.setProblem(problemVI);
	  multistart = MultiStart(optimizationAlgorithm_, startingPointsUB);
	  multistart.run();
	  allMultiStarts.add(multistart);
	  supV_[i] = std::exp(multistart.getResult().getOptimalValue()[0]);
	  LOGDEBUG(OSS() << "supV_[" << i << "]=" << supV_[i] << " v*=" << multistart.getResult().getOptimalPoint());
	} // if ub[i] > 0.0
      if (lb[i] < 0.0)
	{
	  // find a feasible starting point in [lb, 0]
	  if (!startingPointsLB.getSize())
	    {
	      for (UnsignedInteger k = 0; k < candidateNumber_; ++ k)
		{
		  Point candidate(sequence.generate());
		  for (UnsignedInteger j = 0; j < dimension; ++ j)
		    candidate[j] = candidate[j] * lb[j];
		  if (SpecFunc::IsNormal(logPDF_(candidate)[0]))
		    startingPointsLB.add(candidate);
		} // for k
	    } // (!startingPointsLB.getSize())
	  if (!startingPointsLB.getSize())
	    throw InternalException(HERE) << "Could not find a feasible starting point to initialize ratio of uniforms V inf";
	  problemVI.setBounds(Interval(lb, zero));
	  optimizationAlgorithm_.setProblem(problemVI);
	  multistart = MultiStart(optimizationAlgorithm_, startingPointsLB);
	  multistart.run();
	  allMultiStarts.add(multistart);
	  infV_[i] = -std::exp(multistart.getResult().getOptimalValue()[0]);
	  LOGDEBUG(OSS() << "infV_[" << i << "]=" << infV_[i] << " v*=" << multistart.getResult().getOptimalPoint());
	} // if lb[i] < 0.0
    } // for i
  LOGDEBUG(OSS() << "supU multistart points=\n" << startingPointsSupU << "\ninfV multistart points=\n" << startingPointsInfV << "\nsupV multistart points=\n" << startingPointsSupV);
  if (isScaled_)
    {
      acceptanceRatio_ = 1.0 / ((1.0 + r_ * dimension) * supU_);
      for (UnsignedInteger i = 0; i < dimension; ++i)
	acceptanceRatio_ /= supV_[i] - infV_[i];
    }
  else
    {
      const UnsignedInteger size = ResourceMap::GetAsUnsignedInteger("RatioOfUniforms-NormalizationSampleSize");
      
      UnsignedInteger tryNumber;
      RandomGeneratorState initialState(RandomGenerator::GetState());
      RandomGenerator::SetSeed(size);
      (void) getSampleWithTryNumber(size, tryNumber);
      RandomGenerator::SetState(initialState);
      acceptanceRatio_ = (1.0 * size) / tryNumber;
    }
  if (!(acceptanceRatio_ <= 1.0))
    throw InternalException(HERE) << "Error: the acceptance ratio=" << acceptanceRatio_ << " is greater than 1, the computation of the (U, V) bounds is wrong. Here, supU=" << supU_ << ", infV=" << infV_ << ", supV=" << supV_;
  return allMultiStarts;
}

/* Sample generation */
Point RatioOfUniforms::getRealization() const
{
  return getSample(1)[0];
}

Sample RatioOfUniforms::getSample(const UnsignedInteger size) const
{
  UnsignedInteger tryNumber;
  return getSampleWithTryNumber(size, tryNumber);
}

Sample RatioOfUniforms::getSampleWithTryNumber(const UnsignedInteger size,
					       UnsignedInteger & tryNumberOut) const
{
  if (!infV_.getSize())
    throw InternalException(HERE) << "Error: RatioOfUniforms was not initialized. Call initialize() to fix it.";

  // Now, the sampling using rejection
  const UnsignedInteger dimension = range_.getDimension();
  Sample sample(size, dimension);
  Point result(dimension);
  tryNumberOut = 0;
  for (UnsignedInteger n = 0; n < size; ++n)
    {
      Bool accepted = false;
      while (!accepted)
	{
	  ++tryNumberOut;
	  const Scalar u = supU_ * RandomGenerator::Generate();
	  const Scalar ur = std::pow(u, r_);
	  for (UnsignedInteger i = 0; i < dimension; ++ i)
	    result[i] = (infV_[i] + (supV_[i] - infV_[i]) * RandomGenerator::Generate()) / ur;
	  accepted = range_.contains(result) && ((1.0 + r_ * dimension) * std::log(u) <= logUnscaledPDF_(result)[0]);
	} // !accepted
      sample[n] = result;
    } // for n
  return sample;
}

END_NAMESPACE_OPENTURNS
