//                                               -*- C++ -*-
/**
 *  @brief The PointConditionalDistribution distribution
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

#include <cmath>
#include "openturns/PointConditionalDistribution.hxx"
#include "openturns/IteratedQuadrature.hxx"
#include "openturns/ParametricFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SimplicialCubature.hxx"
#include "openturns/CubaIntegration.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/Beta.hxx"
#include "openturns/BlockIndependentCopula.hxx"
#include "openturns/BlockIndependentDistribution.hxx"
#include "openturns/EmpiricalBernsteinCopula.hxx"
#include "openturns/JointDistribution.hxx"
#include "openturns/KernelMixture.hxx"
#include "openturns/Mixture.hxx"
#include "openturns/Normal.hxx"
#include "openturns/Student.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/DomainEvent.hxx"
#include "openturns/PlatformInfo.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SobolSequence.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PointConditionalDistribution)

static const Factory<PointConditionalDistribution> Factory_PointConditionalDistribution;

/* Default constructor */
PointConditionalDistribution::PointConditionalDistribution()
  : DistributionImplementation()
{
  setName("PointConditionalDistribution");
  setDimension(1);
  update();
}

/* Parameters constructor */
PointConditionalDistribution::PointConditionalDistribution(const Distribution & distribution,
    const Indices & conditioningIndices,
    const Point & conditioningValues)
  : DistributionImplementation()
  , distribution_(distribution)
{
  setName("PointConditionalDistribution");
  const UnsignedInteger fullDimension = distribution.getDimension();

  if (conditioningIndices.getSize() != conditioningValues.getSize())
    throw InvalidArgumentException(HERE) << "Conditioning indices/values sizes do not match, got " << conditioningIndices.getSize() << "/" << conditioningValues.getSize();
  if (!conditioningIndices.check(fullDimension))
    throw InvalidArgumentException(HERE) << "Conditioning vector indices (" << conditioningIndices << ") must be less than conditioned distribution dimension (" << fullDimension << " )";

  // reorder indices/values
  Indices permutation(conditioningIndices.getSize());
  permutation.fill();
  std::sort(permutation.begin(), permutation.end(),
            [&](const UnsignedInteger & a, const UnsignedInteger & b) { return (conditioningIndices[a] < conditioningIndices[b]); });
  conditioningIndices_.resize(conditioningIndices.getSize());
  conditioningValues_.resize(conditioningIndices.getSize());
  for (UnsignedInteger i = 0; i < conditioningIndices.getSize(); ++ i)
  {
    conditioningIndices_[i] = conditioningIndices[permutation[i]];
    conditioningValues_[i] = conditioningValues[permutation[i]];
  }

  // it is ok to condition continuous marginals by a discrete one and vice-versa
  const Distribution marginalConditioned(distribution.getMarginal(conditioningIndices.complement(fullDimension)));
  if (!marginalConditioned.isDiscrete() && !marginalConditioned.isContinuous())
    throw NotYetImplementedException(HERE) << "PointConditionalDistribution for mixed continuous/discrete case";

  if (conditioningIndices.getSize())
  {
    const Interval conditioningRange(distribution.getMarginal(conditioningIndices).getRange());
    if (!conditioningRange.contains(conditioningValues))
      throw InvalidArgumentException(HERE) << "Conditioning vector (" << conditioningValues << ") not in conditioning range (" << conditioningRange << ")";
  }

  if (fullDimension == conditioningIndices.getSize() + 1)
  {
    // avoid Cuba in dimension 1
    integrationAlgorithm_ = GaussKronrod();
  }
  else
  {
    if (PlatformInfo::HasFeature("cuba"))
    {
      const String algo(ResourceMap::GetAsString("PointConditionalDistribution-CubaAlgorithm"));
      integrationAlgorithm_ = CubaIntegration(algo);
    }
    else
    {
      integrationAlgorithm_ = SimplicialCubature();
    }
  }
  update();
}

/* Comparison operator */
Bool PointConditionalDistribution::operator ==(const PointConditionalDistribution & other) const
{
  if (this == &other) return true;
  return (distribution_ == other.distribution_) && (conditioningIndices_ == other.conditioningIndices_) && (conditioningValues_ == other.conditioningValues_);
}

Bool PointConditionalDistribution::equals(const DistributionImplementation & other) const
{
  const PointConditionalDistribution* p_other = dynamic_cast<const PointConditionalDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String PointConditionalDistribution::__repr__() const
{
  OSS oss(true);
  oss << "class=" << PointConditionalDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " distribution=" << distribution_
      << " indices=" << conditioningIndices_
      << " values=" << conditioningValues_;
  return oss;
}

String PointConditionalDistribution::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(distribution = " << distribution_ << ", indices = " << conditioningIndices_ << ", values = " << conditioningValues_ << ")";
  return oss;
}

/* Virtual constructor */
PointConditionalDistribution * PointConditionalDistribution::clone() const
{
  return new PointConditionalDistribution(*this);
}

class PointConditionalDistributionUBoundEvaluation : public EvaluationImplementation
{
public:
  PointConditionalDistributionUBoundEvaluation(const Distribution & distribution, const Scalar r)
    : EvaluationImplementation()
    , distribution_(distribution)
    , r_(r)
  {
    // Nothing to do
  }

  PointConditionalDistributionUBoundEvaluation * clone() const override
  {
    return new PointConditionalDistributionUBoundEvaluation(*this);
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

class PointConditionalDistributionVBoundEvaluation : public EvaluationImplementation
{
public:
  PointConditionalDistributionVBoundEvaluation(const Distribution & distribution, const Scalar r)
    : EvaluationImplementation()
    , distribution_(distribution)
    , r_(r)
  {
    // Nothing to do
  }

  PointConditionalDistributionVBoundEvaluation * clone() const override
  {
    return new PointConditionalDistributionVBoundEvaluation(*this);
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


void PointConditionalDistribution::dispatchConditioning(const Collection<Distribution> & distributions, Distribution & simplified) const
{
  Collection<Indices> conditioningIndicesBlocks(distributions.getSize());
  Collection<Point> conditioningValuesBlocks(distributions.getSize());
  // Sort both the conditioning indices and values in increasing order
  const UnsignedInteger conditioningSize = conditioningIndices_.getSize();
  Collection< std::pair<UnsignedInteger, Scalar> > sortedConditioningPairs(conditioningSize);
  for (UnsignedInteger i = 0; i < conditioningSize; ++i)
    sortedConditioningPairs[i] = std::pair<UnsignedInteger, Scalar>(conditioningIndices_[i], conditioningValues_[i]);
  std::sort(sortedConditioningPairs.begin(), sortedConditioningPairs.end());
  UnsignedInteger beginBlock = 0;
  UnsignedInteger endBlock = distributions[0].getDimension();
  UnsignedInteger blockCounter = 0;
  for (UnsignedInteger conditioningCounter = 0; conditioningCounter < conditioningSize; ++conditioningCounter)
  {
    const UnsignedInteger conditioningIndex = sortedConditioningPairs[conditioningCounter].first;
    // Find the block the conditioning index belongs to
    while (conditioningIndex >= endBlock)
    {
      ++ blockCounter;
      // Should never go there
      if (blockCounter == distributions.getSize())
        break;
      beginBlock = endBlock;
      endBlock += distributions[blockCounter].getDimension();
    }
    conditioningIndicesBlocks[blockCounter].add(conditioningIndex - beginBlock);
    conditioningValuesBlocks[blockCounter].add(sortedConditioningPairs[conditioningCounter].second);
  } // for conditioningCounter
  // Now, decide what to do for each block
  Collection<Distribution> newBlocks;
  for (UnsignedInteger i = 0; i < distributions.getSize(); ++i)
  {
    // If the block is fully conditioned, skip it
    if (conditioningIndicesBlocks[i].getSize() == distributions[i].getDimension())
      continue;

    if (conditioningIndicesBlocks[i].getSize() == 0)
    {
      // If the block is not conditioned
      newBlocks.add(distributions[i]);
    }
    else
    {
      const PointConditionalDistribution conditionalBlock(distributions[i], conditioningIndicesBlocks[i], conditioningValuesBlocks[i]);
      newBlocks.add(conditionalBlock.getSimplifiedVersion());
    }
  } // for i
  // Here we return a BlockIndependentDistribution even if the initial collection of distributions was
  // made of copulas only, as the conditioning breaks the copula property
  simplified = (newBlocks.getSize() == 1 ? newBlocks[0] : BlockIndependentDistribution(newBlocks));
}

void PointConditionalDistribution::update()
{
  const UnsignedInteger fullDimension = distribution_.getDimension();
  nonConditioningIndices_ = conditioningIndices_.complement(fullDimension);
  setDimension(nonConditioningIndices_.getSize());
  if (getDimension() == 0)
    throw InvalidArgumentException(HERE) << "Cannot define a conditional distribution by fixing the value of all the components";
  setDescription(distribution_.getDescription().select(nonConditioningIndices_));

  // enable simplified path
  useSimplifiedVersion_ = hasSimplifiedVersion(simplifiedVersion_);
  LOGDEBUG(OSS() << "useSimplifiedVersion_=" << useSimplifiedVersion_);

  // We can postpone the computation of the normalization factor here as we will not need it if there is a simplified version (and it can be costly due to the marginal extraction)
  if (!useSimplifiedVersion_)
    {
      if (conditioningIndices_.getSize())
	logNormalizationFactor_ = distribution_.getMarginal(conditioningIndices_).computeLogPDF(conditioningValues_);
      if (!SpecFunc::IsNormal(logNormalizationFactor_))
	throw InvalidArgumentException(HERE) << "Conditioning vector log PDF value is too low (" << logNormalizationFactor_ << ")";
    }

  // cache marginal for reuse
  if (!useSimplifiedVersion_)
    marginalConditionedDistribution_ = distribution_.getMarginal(nonConditioningIndices_);

  computeRange();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;

  // initialize alias method
  if (!useSimplifiedVersion_ && isDiscrete())
  {
    support_ = getSupport();
    probabilities_ = computePDF(support_).getImplementation()->getData();
    DistFunc::rDiscreteSetup(probabilities_, base_, alias_);
  }

  // initialize ratio of uniforms method, see https://en.wikipedia.org/wiki/Ratio_of_uniforms
  // r_ is a free parameter, could be optimized to maximize the acceptance ratio
  const UnsignedInteger dimension = getDimension();
  if (!useSimplifiedVersion_ && isContinuous() && (dimension <= ResourceMap::GetAsUnsignedInteger("PointConditionalDistribution-SmallDimension"))
    && ResourceMap::GetAsBool("PointConditionalDistribution-InitializeSampling"))
  {
    const Interval bounds(getRange());
    const Point lb(bounds.getLowerBound());
    const Point ub(bounds.getUpperBound());

    // find a feasible starting point
    SobolSequence sequence(dimension);
    Point start;
    const UnsignedInteger candidateNumber = ResourceMap::GetAsUnsignedInteger("PointConditionalDistribution-RatioUniformCandidateNumber");
    for (UnsignedInteger k = 0; k < candidateNumber; ++ k)
    {
      Point candidate(sequence.generate());
      for (UnsignedInteger j = 0; j < dimension; ++ j)
        candidate[j] = lb[j] + candidate[j] * (ub[j] - lb[j]);

      if (SpecFunc::IsNormal(computeLogPDF(candidate)))
      {
        start = candidate;
        break;
      }
    }
    if (!start.getDimension())
      throw InternalException(HERE) << "Could not find a feasible starting point to initialize ration of uniforms U sup";

    // First, the upper bound on U
    const Function objectiveU(new PointConditionalDistributionUBoundEvaluation(*this, r_));
    OptimizationProblem problemU(objectiveU);
    problemU.setMinimization(false);
    problemU.setBounds(bounds);
    OptimizationAlgorithm algo(OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("PointConditionalDistribution-OptimizationAlgorithm")));
    algo.setProblem(problemU);
    algo.setStartingPoint(start);
    algo.run();
    supU_ = std::exp(algo.getResult().getOptimalValue()[0]);
    LOGDEBUG(OSS() << "supU_=" << supU_ << " u*=" << algo.getResult().getOptimalPoint());

    // Second, the lower and upper bounds on V
    const Function objectiveV(new PointConditionalDistributionVBoundEvaluation(*this, r_));
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
        for (UnsignedInteger k = 0; k < candidateNumber; ++ k)
        {
          Point candidate(sequence.generate());
          for (UnsignedInteger j = 0; j < dimension; ++ j)
            candidate[j] = candidate[j] * ub[j];
	  if (SpecFunc::IsNormal(computeLogPDF(candidate)))
          {
            start = candidate;
            break;
          }
        }
        if (!start.getDimension())
          throw InternalException(HERE) << "Could not find a feasible starting point to initialize ration of uniforms V sup";
        problemVI.setBounds(Interval(zero, ub));
        algo.setProblem(problemVI);
        algo.setStartingPoint(start);
        algo.run();
        supV_[i] = std::exp(algo.getResult().getOptimalValue()[0]);
        LOGDEBUG(OSS() << "supV_[" << i << "]=" << supV_[i] << " v*=" << algo.getResult().getOptimalPoint());
      }
      if (lb[i] < 0.0)
      {
        // find a feasible starting point in [lb, 0]
        start.clear();
        for (UnsignedInteger k = 0; k < candidateNumber; ++ k)
        {
          Point candidate(sequence.generate());
          for (UnsignedInteger j = 0; j < dimension; ++ j)
            candidate[j] = candidate[j] * lb[j];
	  if (SpecFunc::IsNormal(computeLogPDF(candidate)))
          {
            start = candidate;
            break;
          }
        }
        if (!start.getDimension())
          throw InternalException(HERE) << "Could not find a feasible starting point to initialize ration of uniforms V inf";
        problemVI.setBounds(Interval(lb, zero));
        algo.setProblem(problemVI);
        algo.setStartingPoint(start);
        algo.run();
        infV_[i] = -std::exp(algo.getResult().getOptimalValue()[0]);
        LOGDEBUG(OSS() << "infV_[" << i << "]=" << infV_[i] << " v*=" << algo.getResult().getOptimalPoint());
      }
    }
  }

  if (!useSimplifiedVersion_ && ResourceMap::GetAsBool("PointConditionalDistribution-InitializeTransformation"))
  {
    // cache reordered marginals
    Indices indices(conditioningIndices_);
    indices.add(nonConditioningIndices_); // initialized in update()
    reorderedDistribution_ = distribution_.getMarginal(indices);

    // cache qI
    Point x(conditioningValues_);
    x.add(getRange().getLowerBound());
    conditioningCDF_ = reorderedDistribution_.computeSequentialConditionalCDF(x);
    conditioningCDF_.resize(conditioningIndices_.getSize());
    LOGDEBUG(OSS() << "conditioningCDF_=" << conditioningCDF_);
  }
}


Point PointConditionalDistribution::decompose(CovarianceMatrix & C) const
{
  const Point mu(distribution_.getMean());
  const CovarianceMatrix cov(distribution_.getCovariance());
  const CovarianceMatrix cxx(distribution_.getMarginal(nonConditioningIndices_).getCovariance());
  const CovarianceMatrix cyy(distribution_.getMarginal(conditioningIndices_).getCovariance());
  const Point mux = mu.select(nonConditioningIndices_);
  const Point muy = mu.select(conditioningIndices_);
  Matrix cxy(conditioningIndices_.getSize(), nonConditioningIndices_.getSize());
  for (UnsignedInteger i = 0; i < conditioningIndices_.getSize(); ++ i)
    for (UnsignedInteger j = 0; j < nonConditioningIndices_.getSize(); ++ j)
      cxy(i, j) = cov(conditioningIndices_[i], nonConditioningIndices_[j]);
  // here we could get the inverse cholesky from Elliptical but we cannot access the covariance scaling factor
  const Point muConditional(mux + cxy.transpose() * cyy.solveLinearSystem(conditioningValues_ - muy));
  C = CovarianceMatrix((cxx - (cxy.transpose() * cyy.solveLinearSystem(cxy))).getImplementation());
  return muConditional;
}

/* Get the simplified version */
Bool PointConditionalDistribution::hasSimplifiedVersion(Distribution & simplified) const
{
  if (!ResourceMap::GetAsBool("PointConditionalDistribution-UseSimplifiedVersion"))
    return false;

  // no conditioning (empty point)
  if (getDimension() == distribution_.getDimension())
  {
    simplified = distribution_;
    return true;
  }

  // conditioning components have no influence on the other components
  if (distribution_.hasIndependentCopula())
  {
    simplified = distribution_.getMarginal(nonConditioningIndices_);
    return true;
  }

  // Normal
  Normal *p_normal = dynamic_cast<Normal *>(distribution_.getImplementation().get());
  if (p_normal)
  {
    CovarianceMatrix C;
    const Point mu(decompose(C));
    simplified = Normal(mu, C);
    return true;
  }

  // Student
  Student *p_student = dynamic_cast<Student *>(distribution_.getImplementation().get());
  if (p_student)
  {
    CovarianceMatrix C;
    const Point mu(decompose(C));
    const Point mY(conditioningValues_ - mu.select(conditioningIndices_));
    const Scalar dy = mY.dot(distribution_.getMarginal(conditioningIndices_).getCovariance().solveLinearSystem(mY));
    const Scalar nu = p_student->getNu();
    const UnsignedInteger py = conditioningIndices_.getSize();
    C = CovarianceMatrix((C * std::sqrt((nu + dy) / (nu + py))).getImplementation());
    simplified = Student(nu + py, mu, C);
    return true;
  }

  // Mixture
  Mixture *p_mixture = dynamic_cast<Mixture *>(distribution_.getImplementation().get());
  if (p_mixture)
  {
    Collection<Distribution> atoms(p_mixture->getDistributionCollection());
    const UnsignedInteger atomsNumber = atoms.getSize();
    const Point weights(p_mixture->getWeights());
    Point newWeights;
    Collection<Distribution> newAtoms;
    for (UnsignedInteger i = 0; i < atomsNumber; ++i)
    {
      const Scalar w = atoms[i].getMarginal(conditioningIndices_).computePDF(conditioningValues_);
      // Add only atoms with nonzero distribution
      if (w > 0.0)
	{
	  newWeights.add(weights[i] * w);
	  newAtoms.add(PointConditionalDistribution(atoms[i], conditioningIndices_, conditioningValues_));
	}
    } // for i
    simplified = Mixture(newAtoms, newWeights);
    return true;
  }

  // Kernel mixture
  KernelMixture *p_kernel_mixture = dynamic_cast<KernelMixture *>(distribution_.getImplementation().get());
  if (p_kernel_mixture)
  {
    const Distribution kernel(p_kernel_mixture->getKernel());
    const Point bandwidth(p_kernel_mixture->getBandwidth());
    const Sample sample(p_kernel_mixture->getInternalSample());
    const UnsignedInteger sampleSize = sample.getSize();
    Collection<Distribution> atoms(sampleSize);
    Point weights(sampleSize, 1.0);
    const UnsignedInteger dimension = getDimension();
    const UnsignedInteger conditioningDimension = conditioningIndices_.getSize();
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      Collection<Distribution> atomComponents(dimension);
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        const UnsignedInteger newJ = nonConditioningIndices_[j];
        const Scalar hJ = bandwidth[newJ];
        atomComponents[j] = kernel * hJ + sample(i, newJ);
      } // j
      atoms[i] = JointDistribution(atomComponents);
      for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
      {
        const UnsignedInteger newJ = conditioningIndices_[j];
        const Scalar hJ = bandwidth[newJ];
        const Scalar xJ = conditioningValues_[j];
        weights[i] *= kernel.computePDF((xJ - sample(i, newJ)) / hJ) / hJ;
      } // j
    } // i
    simplified = Mixture(atoms, weights);
    return true;
  }

  // EmpiricalBernsteinCopula
  EmpiricalBernsteinCopula *p_empirical_bernstein_copula = dynamic_cast<EmpiricalBernsteinCopula *>(distribution_.getImplementation().get());
  if (p_empirical_bernstein_copula)
  {
    const Sample copulaSample(p_empirical_bernstein_copula->getCopulaSample());
    const UnsignedInteger sampleSize = copulaSample.getSize();
    const UnsignedInteger binNumber = p_empirical_bernstein_copula->getBinNumber();
    Collection<Distribution> atoms;
    Point weights;
    const UnsignedInteger dimension = getDimension();
    const UnsignedInteger conditioningDimension = conditioningIndices_.getSize();
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      Scalar logWi = 0.0;
      for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
      {
        const UnsignedInteger newJ = conditioningIndices_[j];
        const Scalar r = std::ceil(binNumber * copulaSample(i, newJ));
        const Scalar xJ = conditioningValues_[j];
        logWi += -SpecFunc::LogBeta(r, binNumber - r + 1.0) + (r - 1.0) * std::log(xJ) + (binNumber - r) * std::log1p(-xJ);
      } // j
      if (SpecFunc::IsNormal(logWi))
	{
	  weights.add(std::exp(logWi));
	  Collection<Distribution> atomComponents(dimension);
	  for (UnsignedInteger j = 0; j < dimension; ++j)
	    {
	      const UnsignedInteger newJ = nonConditioningIndices_[j];
	      const Scalar r = std::ceil(binNumber * copulaSample(i, newJ));
	      atomComponents[j] = Beta(r, binNumber - r + 1.0, 0.0, 1.0);
	    } // j
	  atoms.add(JointDistribution(atomComponents));
	} // isNormal
    } // i
    simplified = Mixture(atoms, weights);
    return true;
  }

  // BlockIndependentDistribution
  BlockIndependentDistribution *p_block_independent_distribution = dynamic_cast<BlockIndependentDistribution *>(distribution_.getImplementation().get());
  if (p_block_independent_distribution)
  {
    dispatchConditioning(p_block_independent_distribution->getDistributionCollection(), simplified);
    return true;
  }

  // BlockIndependentCopula
  BlockIndependentCopula *p_block_independent_copula = dynamic_cast<BlockIndependentCopula *>(distribution_.getImplementation().get());
  if (p_block_independent_copula)
  {
    dispatchConditioning(p_block_independent_copula->getCopulaCollection(), simplified);
    return true;
  }

  // Joint
  JointDistribution *p_joint = dynamic_cast<JointDistribution *>(distribution_.getImplementation().get());
  if (p_joint)
  {
    const Collection<Distribution> marginals(p_joint->getDistributionCollection());
    Point coreConditioniningValues(conditioningIndices_.getSize());
    for (UnsignedInteger i = 0; i < conditioningIndices_.getSize(); ++ i)
      coreConditioniningValues[i] = marginals[conditioningIndices_[i]].computeCDF(conditioningValues_[i]);
    const PointConditionalDistribution conditionalCore(p_joint->getCore(), conditioningIndices_, coreConditioniningValues);
    simplified = JointDistribution(marginals.select(nonConditioningIndices_), conditionalCore);
    return true;
  }

  return false;
}

Distribution PointConditionalDistribution::getSimplifiedVersion() const
{
  return useSimplifiedVersion_ ? simplifiedVersion_ : *this;
}

/* Compute the numerical range of the distribution given the parameters values */
void PointConditionalDistribution::computeRange()
{
  if (useSimplifiedVersion_)
    setRange(simplifiedVersion_.getRange());
  else
    setRange(distribution_.getRange().getMarginal(nonConditioningIndices_));
}

/* Get the support of a distribution that intersects a given interval */
Sample PointConditionalDistribution::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  if (!isDiscrete()) throw NotDefinedException(HERE) << "Error: the support is defined only for discrete distributions.";
  Collection<Sample> marginalSupport(getDimension());
  Indices marginalSize(getDimension());
  for (UnsignedInteger j = 0; j < getDimension(); ++ j)
  {
    marginalSupport[j] = distribution_.getMarginal(nonConditioningIndices_[j]).getSupport(interval.getMarginal(j));
    marginalSize[j] = marginalSupport[j].getSize();
  }
  IndicesCollection tuples(Tuples(marginalSize).generate());
  Sample support(0, getDimension());
  for (UnsignedInteger i = 0; i < tuples.getSize(); ++ i)
  {
    Point x(getDimension());
    for (UnsignedInteger j = 0; j < getDimension(); ++ j)
      x[j] = marginalSupport[j](tuples(i, j), 0);
    if (computePDF(x) > 0.0)
      support.add(x);
  }
  return support;
}

/* Get one realization of the distribution */
Point PointConditionalDistribution::getRealization() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.getRealization();

  if (isDiscrete())
  {
    const UnsignedInteger index = DistFunc::rDiscrete(base_, alias_);
    return support_[index];
  }

  const UnsignedInteger dimension = getDimension();
  if (isContinuous() && dimension <= ResourceMap::GetAsUnsignedInteger("PointConditionalDistribution-SmallDimension"))
  {
    if (!infV_.getSize())
      throw InvalidArgumentException(HERE) << "Sampling was not initialized";

    // Now, the sampling using rejection
    Bool accepted = false;
    Point result(dimension);
    while (!accepted)
    {
      const Scalar u = supU_ * RandomGenerator::Generate();
      const Scalar ur = std::pow(u, r_);
      for (UnsignedInteger i = 0; i < dimension; ++ i)
        result[i] = (infV_[i] + (supV_[i] - infV_[i]) * RandomGenerator::Generate()) / ur;
      accepted = (1.0 + r_ * dimension) * std::log(u) <= computeLogPDF(result);
    }
    return result;
  }

  return DistributionImplementation::getRealization();
}


Point PointConditionalDistribution::expandPoint(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension << ", got dimension=" << point.getDimension();
  Point fullPoint(distribution_.getDimension());
  for (UnsignedInteger i = 0; i < conditioningIndices_.getSize(); ++ i)
    fullPoint[conditioningIndices_[i]] = conditioningValues_[i];
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    fullPoint[nonConditioningIndices_[i]] = point[i];
  return fullPoint;
}

/* Get the DDF of the distribution */
Point PointConditionalDistribution::computeDDF(const Point & point) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeDDF(point);
  else
    return DistributionImplementation::computeDDF(point);
}

/* Get the PDF of the distribution */
Scalar PointConditionalDistribution::computeLogPDF(const Point & point) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeLogPDF(point);
  else
    return distribution_.computeLogPDF(expandPoint(point)) - logNormalizationFactor_;
}

/* Get the PDF of the distribution */
Scalar PointConditionalDistribution::computePDF(const Point & point) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computePDF(point);
  else
    return std::exp(computeLogPDF(point));
}

/* Get the CDF of the distribution */
Scalar PointConditionalDistribution::computeCDF(const Point & point) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeCDF(point);

  return computeProbability(Interval(getRange().getLowerBound(), point));
}

/* Get the probability of an interval */
Scalar PointConditionalDistribution::computeProbability(const Interval & interval) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeProbability(interval);

  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", got " << interval.getDimension();

  Scalar probability = 0.0;
  const Interval intersection(interval.intersect(getRange()));
  if (intersection.isEmpty())
    probability = 0.0;
  else if (intersection == getRange())
    probability = 1.0;
  else
  {
    if (isContinuous())
    {
      // Build the relevant parametric function to be integrated over the remaining parameters
      const ParametricFunction kernel(PDFWrapper(distribution_.getImplementation()->clone()), conditioningIndices_, conditioningValues_);
      probability = integrationAlgorithm_.integrate(kernel, intersection)[0] / std::exp(logNormalizationFactor_);
    }
    else
      probability = computeProbabilityDiscrete(intersection) / std::exp(logNormalizationFactor_);
  }
  return SpecFunc::Clip01(probability);
}

/* Compute the quantile of the distribution */
Scalar PointConditionalDistribution::computeScalarQuantile(const Scalar prob, const Bool tail) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeScalarQuantile(prob, tail);

  return DistributionImplementation::computeScalarQuantile(prob, tail);
}

Point PointConditionalDistribution::computeQuantile(const Scalar prob, const Bool tail) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeQuantile(prob, tail);

  return DistributionImplementation::computeQuantile(prob, tail);
}

/* Check if the distribution is continuous */
Bool PointConditionalDistribution::isContinuous() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.isContinuous();

  return marginalConditionedDistribution_.isContinuous();
}

/* Check if the distribution is discrete */
Bool PointConditionalDistribution::isDiscrete() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.isDiscrete();

  return marginalConditionedDistribution_.isDiscrete();
}

/* Check if the distribution is integral */
Bool PointConditionalDistribution::isIntegral() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.isIntegral();

  return marginalConditionedDistribution_.isIntegral();
}

/* Tell if the distribution is elliptical */
Bool PointConditionalDistribution::isElliptical() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.isElliptical();

  return marginalConditionedDistribution_.isElliptical();
}

/* Tell if the distribution has elliptical copula */
Bool PointConditionalDistribution::hasEllipticalCopula() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.hasEllipticalCopula();

  return marginalConditionedDistribution_.hasEllipticalCopula();
}

/* Tell if the distribution has independent copula */
Bool PointConditionalDistribution::hasIndependentCopula() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.hasIndependentCopula();

  return marginalConditionedDistribution_.hasIndependentCopula();
}

/* Compute the mean of the distribution */
void PointConditionalDistribution::computeMean() const
{
  if (useSimplifiedVersion_)
  {
    mean_ = simplifiedVersion_.getMean();
    isAlreadyComputedMean_ = true;
  }
  else
    DistributionImplementation::computeMean();
}

/* Get the standard deviation of the distribution */
Point PointConditionalDistribution::getStandardDeviation() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.getStandardDeviation();
  else
    return DistributionImplementation::getStandardDeviation();
}

/* Get the skewness of the distribution */
Point PointConditionalDistribution::getSkewness() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.getSkewness();
  else
    return DistributionImplementation::getSkewness();
}

/* Get the kurtosis of the distribution */
Point PointConditionalDistribution::getKurtosis() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.getKurtosis();
  else
    return DistributionImplementation::getKurtosis();
}

/* Compute the covariance of the distribution */
void PointConditionalDistribution::computeCovariance() const
{
  if (useSimplifiedVersion_)
  {
    covariance_ = simplifiedVersion_.getCovariance();
    isAlreadyComputedCovariance_ = true;
  }
  else
    DistributionImplementation::computeCovariance();
}


/* Get the i-th marginal distribution */
Distribution PointConditionalDistribution::getMarginal(const UnsignedInteger index) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.getMarginal(index);

  const UnsignedInteger dimension = getDimension();
  if (index >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (dimension == 1) return clone();

  return getMarginal(Indices({index}));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution PointConditionalDistribution::getMarginal(const Indices & indices) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.getMarginal(indices);

  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";

  Indices full(dimension);
  full.fill();
  if (indices == full) return clone();

  // general case: conditioning then filtered non-conditioning marginals
  Indices newMarginalIndices(conditioningIndices_);
  for (UnsignedInteger i = 0; i < indices.getSize(); ++ i)
    newMarginalIndices.add(nonConditioningIndices_[indices[i]]);
  Indices newConditioningIndices(conditioningIndices_.getSize());
  newConditioningIndices.fill();
  return PointConditionalDistribution(distribution_.getMarginal(newMarginalIndices), newConditioningIndices, conditioningValues_);
}

Point PointConditionalDistribution::getParameter() const
{
  // parameters of the conditioned then conditioning values
  Point parameter(distribution_.getParameter());
  parameter.add(conditioningValues_);
  return parameter;
}

void PointConditionalDistribution::setParameter(const Point & parameter)
{
  Point currentParameter(getParameter());
  if (currentParameter.getSize() != parameter.getSize())
    throw InvalidArgumentException(HERE) << "Parameter size should be " << currentParameter.getSize() << " got " << parameter.getSize();

  // set parameters of the conditioned
  Point conditionedParameter(distribution_.getParameter().getSize());
  if (parameter != currentParameter)
  {
    std::copy(parameter.begin(), parameter.begin() + conditionedParameter.getSize(), conditionedParameter.begin());
    distribution_.setParameter(conditionedParameter);
  }

  // then conditioning values
  std::copy(parameter.begin() + conditionedParameter.getSize(), parameter.end(), conditioningValues_.begin());
  update();
}

Description PointConditionalDistribution::getParameterDescription() const
{
  Description description(distribution_.getParameterDescription());
  description.add(Description::BuildDefault(conditioningIndices_.getSize(), "p_cond_"));
  return description;
}

/* Get the PDF gradient of the distribution */
Point PointConditionalDistribution::computePDFGradient(const Point & point) const
{
  return DistributionImplementation::computePDFGradient(point);
}

/* Get the CDF gradient of the distribution */
Point PointConditionalDistribution::computeCDFGradient(const Point & point) const
{
  return DistributionImplementation::computeCDFGradient(point);
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar PointConditionalDistribution::computeConditionalPDF(const Scalar x, const Point & y) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeConditionalPDF(x, y);

  if (!conditioningCDF_.getSize())
    throw InvalidArgumentException(HERE) << "Transformation was not initialized";
  Point xCond(conditioningValues_);
  xCond.add(y);
  return reorderedDistribution_.computeConditionalPDF(x, xCond);
}

Point PointConditionalDistribution::computeSequentialConditionalPDF(const Point & x) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeSequentialConditionalPDF(x);

  if (!conditioningCDF_.getSize())
    throw InvalidArgumentException(HERE) << "Transformation was not initialized";
  Point xCond(conditioningValues_);
  xCond.add(x);
  Point result(reorderedDistribution_.computeSequentialConditionalPDF(xCond));
  result.erase(result.begin(), result.begin() + conditioningIndices_.getSize());
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar PointConditionalDistribution::computeConditionalCDF(const Scalar x, const Point & y) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeConditionalCDF(x, y);

  if (!conditioningCDF_.getSize())
    throw InvalidArgumentException(HERE) << "Transformation was not initialized";
  Point xCond(conditioningValues_);
  xCond.add(y);
  return reorderedDistribution_.computeConditionalCDF(x, xCond);
}

Point PointConditionalDistribution::computeSequentialConditionalCDF(const Point & y) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeSequentialConditionalCDF(y);

  if (!conditioningCDF_.getSize())
    throw InvalidArgumentException(HERE) << "Transformation was not initialized";
  Point xCond(conditioningValues_);
  xCond.add(y);
  Point result(reorderedDistribution_.computeSequentialConditionalCDF(xCond));
  result.erase(result.begin(), result.begin() + conditioningIndices_.getSize());
  return result;
}

Scalar PointConditionalDistribution::computeConditionalQuantile(const Scalar q, const Point & y) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeConditionalQuantile(q, y);

  if (!conditioningCDF_.getSize())
    throw InvalidArgumentException(HERE) << "Transformation was not initialized";
  Point xCond(conditioningValues_);
  xCond.add(y);
  return reorderedDistribution_.computeConditionalQuantile(q, xCond);
}

Point PointConditionalDistribution::computeSequentialConditionalQuantile(const Point & q) const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.computeSequentialConditionalQuantile(q);

  if (!conditioningCDF_.getSize())
    throw InvalidArgumentException(HERE) << "Transformation was not initialized";
  Point qCond(conditioningCDF_);
  qCond.add(q);
  Point result(reorderedDistribution_.computeSequentialConditionalQuantile(qCond));
  result.erase(result.begin(), result.begin() + conditioningIndices_.getSize());
  return result;
}

/* Get the isoprobabilist transformation */
PointConditionalDistribution::IsoProbabilisticTransformation PointConditionalDistribution::getIsoProbabilisticTransformation() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.getIsoProbabilisticTransformation();

  return DistributionImplementation::getIsoProbabilisticTransformation();
}

/* Get the inverse isoprobabilist transformation */
PointConditionalDistribution::InverseIsoProbabilisticTransformation PointConditionalDistribution::getInverseIsoProbabilisticTransformation() const
{
  if (useSimplifiedVersion_)
    return simplifiedVersion_.getInverseIsoProbabilisticTransformation();

  return DistributionImplementation::getInverseIsoProbabilisticTransformation();
}

Point PointConditionalDistribution::getConditioningValues() const
{
  return conditioningValues_;
}

Indices PointConditionalDistribution::getConditioningIndices() const
{
  return conditioningIndices_;
}

Distribution PointConditionalDistribution::getDistribution() const
{
  return distribution_;
}

/* Integration algorithm accessor */
void PointConditionalDistribution::setIntegrationAlgorithm(const IntegrationAlgorithm & integrationAlgorithm)
{
  integrationAlgorithm_ = integrationAlgorithm;
}

IntegrationAlgorithm PointConditionalDistribution::getIntegrationAlgorithm() const
{
  return integrationAlgorithm_;
}

void PointConditionalDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("distribution_", distribution_);
  adv.saveAttribute("conditioningIndices_", conditioningIndices_);
  adv.saveAttribute("conditioningValues_", conditioningValues_);
}

/* Method load() reloads the object from the StorageManager */
void PointConditionalDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("distribution_", distribution_);
  adv.loadAttribute("conditioningIndices_", conditioningIndices_);
  adv.loadAttribute("conditioningValues_", conditioningValues_);
  update();
}


END_NAMESPACE_OPENTURNS
