//                                               -*- C++ -*-
/**
 *  @brief Implement a tensorized Fejer & ClenshawCurtis quadrature
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
#include "openturns/FejerAlgorithm.hxx"
#include "openturns/FejerExperiment.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FejerAlgorithm
 */

CLASSNAMEINIT(FejerAlgorithm)

static const Factory<FejerAlgorithm> Factory_FejerAlgorithm;

FejerAlgorithm::FejerAlgorithm(const UnsignedInteger dimension,
                               const IntegrationMethod method)
  : IntegrationAlgorithmImplementation()
  , discretization_(Indices(dimension, ResourceMap::GetAsUnsignedInteger("FejerExperiment-DefaultMarginalNodesNumber")))
  , nodes_(0, 0)
  , weights_(0)
{
  // Generate nodes and weights
  generateNodesAndWeights(method);
}

/* Parameters constructor */
FejerAlgorithm::FejerAlgorithm(const Indices & discretization,
                               const IntegrationMethod method)
  : IntegrationAlgorithmImplementation()
  , discretization_(discretization)
  , nodes_(0, 0)
  , weights_(0)
{
  // Generate nodes and weights
  generateNodesAndWeights(method);
}

/* Virtual constructor */
FejerAlgorithm * FejerAlgorithm::clone() const
{
  return new FejerAlgorithm(*this);
}

/* Compute an approximation of \int_a^b f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an n-D interval.
 */
Point FejerAlgorithm::integrate(const Function & function,
                                const Interval & interval) const
{
  Sample adaptedNodes;
  return integrateWithNodes(function, interval, adaptedNodes);
}

Point FejerAlgorithm::integrateWithNodes(const Function & function,
    const Interval & interval,
    Sample & adaptedNodes) const
{
  const UnsignedInteger inputDimension = discretization_.getSize();
  if (interval.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected an interval of dimension=" << inputDimension << ", got dimension=" << interval.getDimension();
  if (function.getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a function of input dimension=" << inputDimension << ", got input dimension=" << function.getInputDimension();
  // Compute the volume of the interval
  const Scalar volume = interval.getVolume();
  Point integral(function.getOutputDimension(), 0.0);
  if (volume == 0.0) return integral;
  // Adapt the nodes to the bounds of the interval
  const Point halfDelta((interval.getUpperBound() - interval.getLowerBound()) * 0.5);
  // Compute nodes
  adaptedNodes = nodes_ * halfDelta + halfDelta + interval.getLowerBound();
  // Compute the function over the adapted nodes
  const Sample values(function(adaptedNodes));
  // Compute the integral
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
    integral += values[i] * weights_[i];
  return std::pow(0.5, 1.0 * inputDimension) * integral * volume;
}

/* Generate nodes and weights */
void FejerAlgorithm::generateNodesAndWeights(const IntegrationMethod method)
{
  const FejerExperiment experiment(discretization_, static_cast<FejerExperiment::RuleType>(method));
  nodes_ = experiment.generateWithWeights(weights_);
}

/* Discretization accessor */
Indices FejerAlgorithm::getDiscretization() const
{
  return discretization_;
}

/* Nodes accessor */
Sample FejerAlgorithm::getNodes() const
{
  return nodes_;
}

/* Weights accessor */
Point FejerAlgorithm::getWeights() const
{
  return weights_;
}

/* String converter */
String FejerAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FejerAlgorithm::GetClassName()
      << ", discretization=" << discretization_
      << ", nodes=" << nodes_
      << ", weights=" << weights_;
  return oss;
}

/* String converter */
String FejerAlgorithm::__str__(const String & ) const
{
  OSS oss(false);
  oss << FejerAlgorithm::GetClassName()
      << "(" << discretization_ << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void FejerAlgorithm::save(Advocate & adv) const
{
  IntegrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("discretization_", discretization_);
  adv.saveAttribute("nodes_", nodes_);
  adv.saveAttribute("weights_", weights_);
}

/* Method load() reloads the object from the StorageManager */
void FejerAlgorithm::load(Advocate & adv)
{
  IntegrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("discretization_", discretization_);
  adv.loadAttribute("nodes_", nodes_);
  adv.loadAttribute("weights_", weights_);
}

END_NAMESPACE_OPENTURNS
