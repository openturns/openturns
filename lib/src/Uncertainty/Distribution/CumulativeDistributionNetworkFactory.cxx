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

#include "openturns/CumulativeDistributionNetworkFactory.hxx"
#include "openturns/CumulativeDistributionNetwork.hxx"
#include "openturns/Description.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Function.hxx"
#include "openturns/Log.hxx"
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
{
  initializeSolver();
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
  oss << "class=" << this->getClassName()
      << " graph=" << graph_
      << " distributionCollection=" << distributionCollection_
      << " solver=" << solver_;
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

/* Build a distribution using the template parameters */
Distribution CumulativeDistributionNetworkFactory::build() const
{
  return build(getTemplateParameter());
}

/* Build a distribution based on a sample */
Distribution CumulativeDistributionNetworkFactory::build(const Sample & sample) const
{
  Distribution result(build(computeParameter(sample)));
  result.setDescription(sample.getDescription());
  return result;
}

/* Build a distribution based on a set of parameters */
Distribution CumulativeDistributionNetworkFactory::build(const Point & parameter) const
{
  checkConsistency();
  return CumulativeDistributionNetwork(buildFactors(parameter), graph_);
}

/* Build the distribution and the parameter distribution */
DistributionFactoryResult CumulativeDistributionNetworkFactory::buildEstimator(const Sample & sample) const
{
  if (sample.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot estimate a cumulative distribution network from an empty sample";
  return buildBootStrapEstimator(sample);
}

/* Graph accessor */
void CumulativeDistributionNetworkFactory::setGraph(const BipartiteGraph & graph)
{
  graph_ = graph;
}

BipartiteGraph CumulativeDistributionNetworkFactory::getGraph() const
{
  return graph_;
}

/* Distribution template accessor */
void CumulativeDistributionNetworkFactory::setDistributionCollection(const DistributionCollection & distributions)
{
  distributionCollection_ = distributions;
}

CumulativeDistributionNetworkFactory::DistributionCollection CumulativeDistributionNetworkFactory::getDistributionCollection() const
{
  return distributionCollection_;
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
}

/* Method load() reloads the object from the StorageManager */
void CumulativeDistributionNetworkFactory::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
  if (adv.hasAttribute("graph_"))
    adv.loadAttribute("graph_", graph_);
  if (adv.hasAttribute("distributionCollection_"))
    adv.loadAttribute("distributionCollection_", distributionCollection_);
  // Archives written before the solver attributes were introduced are still readable:
  // fall back to the default solver and to the default optimization options.
  if (adv.hasAttribute("solver_"))
    adv.loadAttribute("solver_", solver_);
  else
    initializeSolver();
  if (adv.hasAttribute("optimizationBounds_"))
    adv.loadAttribute("optimizationBounds_", optimizationBounds_);
  else
    optimizationBounds_ = Interval();
  if (adv.hasAttribute("startingPoint_"))
    adv.loadAttribute("startingPoint_", startingPoint_);
  else
    startingPoint_ = Point();
}

END_NAMESPACE_OPENTURNS
