//                                               -*- C++ -*-
/**
 *  @brief Estimate the factor distributions of a cumulative distribution
 *         network from data by maximum likelihood
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <algorithm>
#include <cmath>
#include <map>

#include "openturns/CumulativeDistributionNetworkFactory.hxx"
#include "openturns/CumulativeDistributionNetwork.hxx"
#include "openturns/Description.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/FittingTest.hxx"
#include "openturns/Function.hxx"
#include "openturns/JointDistribution.hxx"
#include "openturns/Log.hxx"
#include "openturns/Normal.hxx"
#include "openturns/OptimizationProblem.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CumulativeDistributionNetworkFactory)

static const Factory<CumulativeDistributionNetworkFactory> Factory_CumulativeDistributionNetworkFactory;

typedef Collection<Distribution> DistributionCollection;

/* Build the factor distributions by slicing a concatenated parameter */
static DistributionCollection buildFactorsFromParameter(const DistributionCollection & distributions,
    const Point & parameter)
{
  const UnsignedInteger size = distributions.getSize();
  DistributionCollection factors(size);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localDimension = distributions[i].getParameterDimension();
    if (index + localDimension > parameter.getSize())
      throw InvalidArgumentException(HERE) << "Error: the parameter has size " << parameter.getSize()
                                           << " but the factor " << i << " needs " << localDimension
                                           << " values starting at index " << index;
    Point localParameter(localDimension);
    std::copy(parameter.begin() + index, parameter.begin() + index + localDimension, localParameter.begin());
    index += localDimension;
    Distribution factor(distributions[i]);
    factor.setParameter(localParameter);
    factors[i] = factor;
  }
  if (index != parameter.getSize())
    throw InvalidArgumentException(HERE) << "Error: the parameter has size " << parameter.getSize()
                                         << " but the factor distributions need " << index << " values";
  return factors;
}

/* Fit of a candidate factor on a block of variables */
namespace
{

struct BlockFit
{
  BlockFit()
    : bic_(SpecFunc::Infinity)
  {
    // Nothing to do
  }

  BlockFit(const Distribution & distribution,
           const Scalar bic)
    : distribution_(distribution)
    , bic_(bic)
  {
    // Nothing to do
  }

  Bool isValid() const
  {
    return bic_ < SpecFunc::Infinity;
  }

  Distribution distribution_;
  Scalar bic_;
};

/* Factories that can build a univariate continuous distribution */
DistributionFactory::DistributionFactoryCollection getUnivariateFactories()
{
  DistributionFactory::DistributionFactoryCollection result(0);
  const DistributionFactory::DistributionFactoryCollection factories(DistributionFactory::GetContinuousUniVariateFactories());
  for (UnsignedInteger i = 0; i < factories.getSize(); ++i)
  {
    try
    {
      const Distribution distribution(factories[i].build());
      if ((distribution.getDimension() == 1)
          && distribution.isContinuous()
          && (factories[i].getImplementation()->getClassName() != "HistogramFactory"))
        result.add(factories[i]);
    }
    catch (const Exception &)
    {
      // The factory cannot build a default distribution: skip it
    }
  }
  return result;
}

/* Factories that can build a multivariate continuous distribution which is not a copula */
DistributionFactory::DistributionFactoryCollection getMultiVariateFactories()
{
  DistributionFactory::DistributionFactoryCollection result(0);
  const DistributionFactory::DistributionFactoryCollection factories(DistributionFactory::GetContinuousMultiVariateFactories());
  for (UnsignedInteger i = 0; i < factories.getSize(); ++i)
  {
    try
    {
      const Distribution distribution(factories[i].build());
      if (!distribution.isCopula())
        result.add(factories[i]);
    }
    catch (const Exception &)
    {
      // The factory cannot build a default distribution: skip it
    }
  }
  return result;
}

/* Factories that can build a copula, excluding the non-parametric one */
DistributionFactory::DistributionFactoryCollection getCopulaFactories()
{
  DistributionFactory::DistributionFactoryCollection result(0);
  const DistributionFactory::DistributionFactoryCollection factories(DistributionFactory::GetContinuousMultiVariateFactories());
  for (UnsignedInteger i = 0; i < factories.getSize(); ++i)
  {
    try
    {
      const Distribution distribution(factories[i].build());
      if (distribution.isCopula()
          && (factories[i].getImplementation()->getClassName() != "BernsteinCopulaFactory"))
        result.add(factories[i]);
    }
    catch (const Exception &)
    {
      // The factory cannot build a default distribution: skip it
    }
  }
  return result;
}

/* Best univariate fit of a sample */
BlockFit fitUnivariate(const Sample & sample,
                       const DistributionFactory::DistributionFactoryCollection & factories)
{
  Scalar bic = SpecFunc::Infinity;
  try
  {
    const Distribution distribution(FittingTest::BestModelBIC(sample, factories, bic));
    if (bic < SpecFunc::Infinity)
      return BlockFit(distribution, bic);
  }
  catch (const Exception &)
  {
    // No candidate distribution
  }
  return BlockFit();
}

/* Merge two sorted index sets */
Indices mergeIndices(const Indices & first,
                     const Indices & second)
{
  Indices result(0);
  UnsignedInteger i = 0;
  UnsignedInteger j = 0;
  while ((i < first.getSize()) && (j < second.getSize()))
  {
    if (first[i] < second[j])
      result.add(first[i++]);
    else if (second[j] < first[i])
      result.add(second[j++]);
    else
    {
      // The blocks are disjoint, this case only protects against invalid input
      result.add(first[i++]);
      ++j;
    }
  }
  while (i < first.getSize())
    result.add(first[i++]);
  while (j < second.getSize())
    result.add(second[j++]);
  return result;
}

/* Best fit of a multivariate block, using either a multivariate factory or a
   marginals + copula model */
BlockFit fitBlock(const Sample & sample,
                  const Indices & variables,
                  const Collection<BlockFit> & singletonFits,
                  const DistributionFactory::DistributionFactoryCollection & multiVariateFactories,
                  const DistributionFactory::DistributionFactoryCollection & copulaFactories)
{
  const UnsignedInteger dimension = variables.getSize();
  const Sample blockSample(sample.getMarginal(variables));
  Scalar bestBIC = SpecFunc::Infinity;
  Distribution bestDistribution;
  Bool found = false;

  // Direct multivariate parametric models
  if (multiVariateFactories.getSize() > 0)
  {
    try
    {
      Scalar bic = SpecFunc::Infinity;
      const Distribution distribution(FittingTest::BestModelBIC(blockSample, multiVariateFactories, bic));
      if (bic < bestBIC)
      {
        bestBIC = bic;
        bestDistribution = distribution;
        found = true;
      }
    }
    catch (const Exception &)
    {
      // No direct multivariate candidate
    }
  }

  // Marginals estimated independently, then a copula on the pseudo-observations
  if (copulaFactories.getSize() > 0)
  {
    try
    {
      DistributionCollection marginals(dimension);
      Sample pseudoSample(blockSample.getSize(), dimension);
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        marginals[j] = singletonFits[variables[j]].distribution_;
        for (UnsignedInteger i = 0; i < blockSample.getSize(); ++i)
          pseudoSample(i, j) = SpecFunc::Clip01(marginals[j].computeCDF(blockSample(i, j)));
      }
      Scalar copulaBIC = SpecFunc::Infinity;
      const Distribution copula(FittingTest::BestModelBIC(pseudoSample, copulaFactories, copulaBIC));
      const JointDistribution joint(marginals, copula);
      const Scalar bic = FittingTest::BIC(blockSample, joint, joint.getParameterDimension());
      if (bic < bestBIC)
      {
        bestBIC = bic;
        bestDistribution = joint;
        found = true;
      }
    }
    catch (const Exception &)
    {
      // No marginals + copula candidate
    }
  }

  if (!found)
    return BlockFit();
  return BlockFit(bestDistribution, bestBIC);
}

} // anonymous namespace

/* Mean log-likelihood of the network, as a function of the concatenated parameters */
class NetworkLogLikelihoodEvaluation : public EvaluationImplementation
{
public:
  NetworkLogLikelihoodEvaluation(const Sample & sample,
                                 const BipartiteGraph & graph,
                                 const DistributionCollection & distributions)
    : EvaluationImplementation()
    , sample_(sample)
    , graph_(graph)
    , distributions_(distributions)
  {
    // Nothing to do
  }

  NetworkLogLikelihoodEvaluation * clone() const override
  {
    return new NetworkLogLikelihoodEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    UnsignedInteger dimension = 0;
    for (UnsignedInteger i = 0; i < distributions_.getSize(); ++i)
      dimension += distributions_[i].getParameterDimension();
    return dimension;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Description getInputDescription() const override
  {
    return Description::BuildDefault(getInputDimension(), "theta");
  }

  Description getOutputDescription() const override
  {
    return Description(1, "lh");
  }

  Description getDescription() const override
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  Point operator() (const Point & parameter) const override
  {
    Scalar result = -SpecFunc::LogMaxScalar;
    try
    {
      const DistributionCollection factors(buildFactorsFromParameter(distributions_, parameter));
      const CumulativeDistributionNetwork network(factors, graph_);
      result = network.computeLogPDF(sample_).computeMean()[0];
    }
    catch (const Exception &)
    {
      return {-SpecFunc::LogMaxScalar};
    }
    if (!std::isfinite(result))
      result = -SpecFunc::LogMaxScalar;
    result = std::max(-SpecFunc::LogMaxScalar, result);
    return {result};
  }

private:
  Sample sample_;
  BipartiteGraph graph_;
  DistributionCollection distributions_;
};

/* Default constructor */
CumulativeDistributionNetworkFactory::CumulativeDistributionNetworkFactory()
  : DistributionFactoryImplementation()
  , distributionCollection_(1, Normal())
{
  // The default graph is the single red node [[0]]: provide the matching
  // normal template so that build() can be called on a default factory.
  initializeSolver();
  checkConsistency();
}

/* Parameters constructor */
CumulativeDistributionNetworkFactory::CumulativeDistributionNetworkFactory(const BipartiteGraph & graph,
    const DistributionCollection & distributions)
  : DistributionFactoryImplementation()
  , graph_(graph)
  , distributionCollection_(distributions)
{
  initializeSolver();
  checkConsistency();
}

/* Structure learning constructor */
CumulativeDistributionNetworkFactory::CumulativeDistributionNetworkFactory(const UnsignedInteger minimumFactorNumber,
    const UnsignedInteger maximumFactorNumber,
    const UnsignedInteger maximumFactorDimension)
  : DistributionFactoryImplementation()
  , graph_(0, 0)
  , distributionCollection_()
{
  structureLearning_ = true;
  minimumFactorNumber_ = minimumFactorNumber;
  maximumFactorNumber_ = maximumFactorNumber;
  maximumFactorDimension_ = maximumFactorDimension;
  initializeSolver();
  checkConsistency();
}

/* Initialize the optimization solver using the ResourceMap */
void CumulativeDistributionNetworkFactory::initializeSolver()
{
  const String solverName(ResourceMap::GetAsString("MaximumLikelihoodFactory-DefaultOptimizationAlgorithm"));
  solver_ = OptimizationAlgorithm::GetByName(solverName);
  if (solverName == "TNC")
    solver_.setCheckStatus(false);
  solver_.setMaximumCallsNumber(ResourceMap::GetAsUnsignedInteger("MaximumLikelihoodFactory-MaximumCallsNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumConstraintError"));
}

/* Virtual constructor */
CumulativeDistributionNetworkFactory * CumulativeDistributionNetworkFactory::clone() const
{
  return new CumulativeDistributionNetworkFactory(*this);
}

/* String converter */
String CumulativeDistributionNetworkFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << this->getClassName();
  if (structureLearning_)
    oss << " structureLearning=true"
        << " minimumFactorNumber=" << minimumFactorNumber_
        << " maximumFactorNumber=" << maximumFactorNumber_
        << " maximumFactorDimension=" << maximumFactorDimension_;
  else
    oss << " graph=" << graph_
        << " distributionCollection=" << distributionCollection_;
  oss << " solver=" << solver_;
  return oss;
}

/* String converter */
String CumulativeDistributionNetworkFactory::__str__(const String & ) const
{
  return this->getClassName();
}

/* Check that the graph and the distribution templates are consistent */
void CumulativeDistributionNetworkFactory::checkConsistency() const
{
  if (structureLearning_)
  {
    if (minimumFactorNumber_ == 0)
      throw InvalidArgumentException(HERE) << "Error: the minimum number of factors must be positive";
    if (maximumFactorNumber_ < minimumFactorNumber_)
      throw InvalidArgumentException(HERE) << "Error: the maximum number of factors (" << maximumFactorNumber_
                                           << ") must not be smaller than the minimum number of factors (" << minimumFactorNumber_ << ")";
    if (maximumFactorDimension_ == 0)
      throw InvalidArgumentException(HERE) << "Error: the maximum factor dimension must be positive";
    return;
  }
  const UnsignedInteger size = distributionCollection_.getSize();
  if (size != graph_.getSize())
    throw InvalidArgumentException(HERE) << "Error: the graph has " << graph_.getSize()
                                         << " red nodes but " << size << " distributions were given";
  const UnsignedInteger dimension = graph_.getBlackNodes().getSize();
  Indices multiplicity(dimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localDimension = graph_.cend_at(i) - graph_.cbegin_at(i);
    if (localDimension == 0)
      throw InvalidArgumentException(HERE) << "Error: the clique of the red node " << i << " is empty";
    if (distributionCollection_[i].getDimension() != localDimension)
      throw InvalidArgumentException(HERE) << "Error: the distribution " << i << " has dimension "
                                           << distributionCollection_[i].getDimension()
                                           << " but its clique has " << localDimension << " variables";
    for (UnsignedInteger j = 0; j < localDimension; ++j)
    {
      const UnsignedInteger variable = graph_(i, j);
      if (variable >= dimension)
        throw InvalidArgumentException(HERE) << "Error: the black node " << variable
                                             << " linked to the red node " << i
                                             << " is not a valid variable index (dimension=" << dimension << ")";
      for (UnsignedInteger k = 0; k < j; ++k)
        if (graph_(i, k) == variable)
          throw InvalidArgumentException(HERE) << "Error: the black node " << variable
                                               << " appears twice in the clique of the red node " << i;
      multiplicity[variable] += 1;
    }
  }
  // Every variable must belong to at least one factor, otherwise the network is improper
  for (UnsignedInteger i = 0; i < dimension; ++i)
    if (multiplicity[i] == 0)
      throw InvalidArgumentException(HERE) << "Error: the black node " << i << " is not linked to any red node";
}

/* Concatenate the parameters of the templates */
Point CumulativeDistributionNetworkFactory::getTemplateParameter() const
{
  Point parameter;
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    parameter.add(distributionCollection_[i].getParameter());
  return parameter;
}

/* Build the factor distributions from a concatenated parameter */
CumulativeDistributionNetworkFactory::DistributionCollection CumulativeDistributionNetworkFactory::buildFactors(const Point & parameter) const
{
  if (parameter.getSize() != getParameterDimension())
    throw InvalidArgumentException(HERE) << "Error: expected a parameter of size " << getParameterDimension()
                                         << ", got " << parameter.getSize();
  return buildFactorsFromParameter(distributionCollection_, parameter);
}

/* Optimize the concatenated parameter on a sample */
Point CumulativeDistributionNetworkFactory::computeParameter(const Sample & sample) const
{
  checkConsistency();
  if (sample.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot estimate a cumulative distribution network from an empty sample";
  const UnsignedInteger dimension = graph_.getBlackNodes().getSize();
  if (sample.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the sample dimension=" << sample.getDimension()
                                         << " does not match the network dimension=" << dimension;

  const Point templateParameter(getTemplateParameter());
  const UnsignedInteger parameterDimension = templateParameter.getSize();
  if (parameterDimension == 0)
    return templateParameter;

  // Reject the structures that the exact log-likelihood cannot handle before
  // starting the optimization: otherwise every evaluation returns the same
  // penalized value and the solver silently stops at the starting point.
  const CumulativeDistributionNetwork templateNetwork(buildFactors(templateParameter), graph_);
  if (!templateNetwork.isContinuous() && !templateNetwork.isIntegral())
    throw NotYetImplementedException(HERE) << "Error: maximum likelihood estimation requires a continuous or integral network";
  // Fail fast when the derivative-sum-product enumeration exceeds its limit
  (void) templateNetwork.computeLogPDF(sample[0]);

  // Starting point: explicit value, then the solver one, then the template parameter
  Point startingPoint(startingPoint_);
  if (startingPoint.getSize() == 0)
    startingPoint = solver_.getStartingPoint();
  if (startingPoint.getSize() != parameterDimension)
  {
    if (startingPoint.getSize() != 0)
      LOGINFO(OSS() << "The given starting point=" << startingPoint << " has dimension=" << startingPoint.getSize()
              << " which is different from the expected parameter dimension=" << parameterDimension
              << ". Switching to the template parameter=" << templateParameter);
    startingPoint = templateParameter;
  }

  // Bounds: explicit value, then no bounds at all
  Interval bounds(optimizationBounds_);
  if (bounds.getDimension() != parameterDimension)
  {
    if (bounds.getDimension() != 0)
      LOGINFO(OSS() << "The given bounds have dimension=" << bounds.getDimension()
              << " which is different from the expected parameter dimension=" << parameterDimension
              << ". Switching to unbounded optimization");
    bounds = Interval();
  }

  const NetworkLogLikelihoodEvaluation evaluation(sample, graph_, distributionCollection_);
  const Function logLikelihood(evaluation.clone());
  OptimizationProblem problem(logLikelihood);
  problem.setMinimization(false);
  if (bounds.getDimension() == parameterDimension)
    problem.setBounds(bounds);

  OptimizationAlgorithm solver(solver_);
  solver.setProblem(problem);
  solver.setStartingPoint(startingPoint);
  solver.run();

  const Point optimalParameter(solver.getResult().getOptimalPoint());
  if (optimalParameter.getSize() != parameterDimension)
    throw InvalidArgumentException(HERE) << "Error: the optimization did not yield a feasible parameter";
  return optimalParameter;
}

/* Estimate the graph and the factor distributions on a sample */
Distribution CumulativeDistributionNetworkFactory::buildStructure(const Sample & sample) const
{
  checkConsistency();
  const UnsignedInteger sampleSize = sample.getSize();
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot estimate a cumulative distribution network from an empty sample";
  const UnsignedInteger dimension = sample.getDimension();
  if (dimension == 0)
    throw InvalidArgumentException(HERE) << "Error: the sample must have a positive dimension";
  if (minimumFactorNumber_ > dimension)
    throw InvalidArgumentException(HERE) << "Error: the minimum number of factors (" << minimumFactorNumber_
                                         << ") exceeds the sample dimension (" << dimension << ")";
  const UnsignedInteger maximumFactorNumber = std::min(maximumFactorNumber_, dimension);
  const UnsignedInteger maximumFactorDimension = std::min(maximumFactorDimension_, dimension);
  const UnsignedInteger requiredFactorNumber = (dimension + maximumFactorDimension - 1) / maximumFactorDimension;
  if (maximumFactorNumber < requiredFactorNumber)
    throw InvalidArgumentException(HERE) << "Error: at least " << requiredFactorNumber
                                         << " factors are needed to cover " << dimension
                                         << " variables with factors of dimension at most " << maximumFactorDimension
                                         << ", but the maximum number of factors is " << maximumFactorNumber;

  const DistributionFactory::DistributionFactoryCollection univariateFactories(getUnivariateFactories());
  if (univariateFactories.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Error: no univariate continuous distribution factory is available";
  const DistributionFactory::DistributionFactoryCollection multiVariateFactories(getMultiVariateFactories());
  const DistributionFactory::DistributionFactoryCollection copulaFactories(getCopulaFactories());

  // Initial partition: one singleton factor per variable
  Collection<BlockFit> singletonFits(dimension);
  Scalar score = 0.0;
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    singletonFits[j] = fitUnivariate(sample.getMarginal(j), univariateFactories);
    if (!singletonFits[j].isValid())
      throw InvalidArgumentException(HERE) << "Error: no univariate distribution could be fitted to the variable " << j;
    score += singletonFits[j].bic_;
  }
  Collection<Indices> blocks(dimension);
  Collection<BlockFit> blockFits(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    blocks[j] = Indices(1, j);
    blockFits[j] = singletonFits[j];
  }

  // Greedy agglomeration of the blocks. The BIC of a block-independent network
  // is the sum of the BICs of its blocks, hence the score is additive.
  std::map<Indices, BlockFit> cache;
  while (blocks.getSize() > 1)
  {
    const UnsignedInteger blockNumber = blocks.getSize();
    const Bool mustMerge = blockNumber > maximumFactorNumber;
    if (!mustMerge && (blockNumber <= minimumFactorNumber_))
      break;

    Scalar bestScore = SpecFunc::Infinity;
    UnsignedInteger bestFirst = blockNumber;
    UnsignedInteger bestSecond = blockNumber;
    Indices bestVariables;
    BlockFit bestFit;
    for (UnsignedInteger i = 0; i < blockNumber; ++i)
    {
      for (UnsignedInteger j = i + 1; j < blockNumber; ++j)
      {
        if (blocks[i].getSize() + blocks[j].getSize() > maximumFactorDimension)
          continue;
        const Indices variables(mergeIndices(blocks[i], blocks[j]));
        std::map<Indices, BlockFit>::const_iterator it = cache.find(variables);
        if (it == cache.end())
        {
          const BlockFit fit(fitBlock(sample, variables, singletonFits, multiVariateFactories, copulaFactories));
          it = cache.insert(std::make_pair(variables, fit)).first;
        }
        if (!it->second.isValid())
          continue;
        const Scalar candidateScore = score - blockFits[i].bic_ - blockFits[j].bic_ + it->second.bic_;
        if (candidateScore < bestScore)
        {
          bestScore = candidateScore;
          bestFirst = i;
          bestSecond = j;
          bestVariables = variables;
          bestFit = it->second;
        }
      }
    }

    if (bestFirst == blockNumber)
    {
      if (mustMerge)
        throw InvalidArgumentException(HERE) << "Error: cannot reduce the number of factors to " << maximumFactorNumber
                                             << " with factors of dimension at most " << maximumFactorDimension;
      break;
    }
    // A merge is accepted if it improves the BIC, or if it is required by the
    // maximum number of factors.
    if (!mustMerge && (bestScore >= score))
      break;
    score = bestScore;
    blocks[bestFirst] = bestVariables;
    blockFits[bestFirst] = bestFit;
    blocks.erase(blocks.begin() + bestSecond);
    blockFits.erase(blockFits.begin() + bestSecond);
  }

  const IndicesCollection cliques(blocks);
  const BipartiteGraph graph(cliques);
  DistributionCollection factors(blocks.getSize());
  for (UnsignedInteger i = 0; i < blocks.getSize(); ++i)
    factors[i] = blockFits[i].distribution_;
  return CumulativeDistributionNetwork(factors, graph);
}

/* Build a distribution using the template parameters */
Distribution CumulativeDistributionNetworkFactory::build() const
{
  if (structureLearning_)
    throw InvalidArgumentException(HERE) << "Error: a structure-learning factory must be built from a sample";
  return build(getTemplateParameter());
}

/* Build a distribution based on a sample */
Distribution CumulativeDistributionNetworkFactory::build(const Sample & sample) const
{
  Distribution result;
  if (structureLearning_)
    result = buildStructure(sample);
  else
    result = build(computeParameter(sample));
  result.setDescription(sample.getDescription());
  return result;
}

/* Build a distribution based on a set of parameters */
Distribution CumulativeDistributionNetworkFactory::build(const Point & parameter) const
{
  if (structureLearning_)
    throw InvalidArgumentException(HERE) << "Error: a structure-learning factory must be built from a sample";
  checkConsistency();
  return CumulativeDistributionNetwork(buildFactors(parameter), graph_);
}

/* Build the distribution and the parameter distribution */
DistributionFactoryResult CumulativeDistributionNetworkFactory::buildEstimator(const Sample & sample) const
{
  if (sample.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot estimate a cumulative distribution network from an empty sample";
  if (structureLearning_)
    throw NotYetImplementedException(HERE) << "Error: buildEstimator is not available when the graph is estimated from the sample";
  return buildBootStrapEstimator(sample);
}

/* Graph accessor */
void CumulativeDistributionNetworkFactory::setGraph(const BipartiteGraph & graph)
{
  if (structureLearning_)
    throw InvalidArgumentException(HERE) << "Error: cannot set the graph of a structure-learning factory";
  graph_ = graph;
}

BipartiteGraph CumulativeDistributionNetworkFactory::getGraph() const
{
  if (structureLearning_)
    throw NotDefinedException(HERE) << "Error: the graph is estimated from the sample";
  return graph_;
}

/* Distribution template accessor */
void CumulativeDistributionNetworkFactory::setDistributionCollection(const DistributionCollection & distributions)
{
  if (structureLearning_)
    throw InvalidArgumentException(HERE) << "Error: cannot set the distribution templates of a structure-learning factory";
  distributionCollection_ = distributions;
}

CumulativeDistributionNetworkFactory::DistributionCollection CumulativeDistributionNetworkFactory::getDistributionCollection() const
{
  if (structureLearning_)
    return DistributionCollection(0);
  return distributionCollection_;
}

/* Whether the graph is estimated from the sample */
Bool CumulativeDistributionNetworkFactory::isStructureLearning() const
{
  return structureLearning_;
}

/* Bounds on the graph characteristics used by the structure search */
UnsignedInteger CumulativeDistributionNetworkFactory::getMinimumFactorNumber() const
{
  return minimumFactorNumber_;
}

UnsignedInteger CumulativeDistributionNetworkFactory::getMaximumFactorNumber() const
{
  return maximumFactorNumber_;
}

UnsignedInteger CumulativeDistributionNetworkFactory::getMaximumFactorDimension() const
{
  return maximumFactorDimension_;
}

/* Number of parameters of the network */
UnsignedInteger CumulativeDistributionNetworkFactory::getParameterDimension() const
{
  UnsignedInteger dimension = 0;
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    dimension += distributionCollection_[i].getParameterDimension();
  return dimension;
}

/* Starting point accessor */
void CumulativeDistributionNetworkFactory::setStartingPoint(const Point & startingPoint)
{
  if ((startingPoint.getSize() != 0) && (startingPoint.getSize() != getParameterDimension()))
    throw InvalidArgumentException(HERE) << "Error: the starting point has size " << startingPoint.getSize()
                                         << " but the network has " << getParameterDimension() << " parameters";
  startingPoint_ = startingPoint;
}

Point CumulativeDistributionNetworkFactory::getStartingPoint() const
{
  return startingPoint_;
}

/* Optimization algorithm accessor */
void CumulativeDistributionNetworkFactory::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
}

OptimizationAlgorithm CumulativeDistributionNetworkFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

/* Optimization bounds accessor */
void CumulativeDistributionNetworkFactory::setOptimizationBounds(const Interval & optimizationBounds)
{
  if ((optimizationBounds.getDimension() != 0) && (optimizationBounds.getDimension() != getParameterDimension()))
    throw InvalidArgumentException(HERE) << "Error: the bounds have dimension " << optimizationBounds.getDimension()
                                         << " but the network has " << getParameterDimension() << " parameters";
  optimizationBounds_ = optimizationBounds;
}

Interval CumulativeDistributionNetworkFactory::getOptimizationBounds() const
{
  return optimizationBounds_;
}

/* Method save() stores the object through the StorageManager */
void CumulativeDistributionNetworkFactory::save(Advocate & adv) const
{
  DistributionFactoryImplementation::save(adv);
  adv.saveAttribute("graph_", graph_);
  adv.saveAttribute("distributionCollection_", distributionCollection_);
  adv.saveAttribute("solver_", solver_);
  adv.saveAttribute("optimizationBounds_", optimizationBounds_);
  adv.saveAttribute("startingPoint_", startingPoint_);
  adv.saveAttribute("structureLearning_", structureLearning_);
  adv.saveAttribute("minimumFactorNumber_", minimumFactorNumber_);
  adv.saveAttribute("maximumFactorNumber_", maximumFactorNumber_);
  adv.saveAttribute("maximumFactorDimension_", maximumFactorDimension_);
}

/* Method load() reloads the object from the StorageManager */
void CumulativeDistributionNetworkFactory::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
  adv.loadAttribute("graph_", graph_);
  adv.loadAttribute("distributionCollection_", distributionCollection_);
  adv.loadAttribute("solver_", solver_);
  adv.loadAttribute("optimizationBounds_", optimizationBounds_);
  adv.loadAttribute("startingPoint_", startingPoint_);
  adv.loadAttribute("structureLearning_", structureLearning_);
  adv.loadAttribute("minimumFactorNumber_", minimumFactorNumber_);
  adv.loadAttribute("maximumFactorNumber_", maximumFactorNumber_);
  adv.loadAttribute("maximumFactorDimension_", maximumFactorDimension_);
}

END_NAMESPACE_OPENTURNS
