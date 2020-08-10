//                                               -*- C++ -*-
/**
 *  @brief Implement a tensorized Fejer & ClenshawCurtis quadrature
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
#include "openturns/FejerAlgorithm.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Lapack.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FejerAlgorithm
 */

CLASSNAMEINIT(FejerAlgorithm)

static const Factory<FejerAlgorithm> Factory_FejerAlgorithm;

FejerAlgorithm::FejerAlgorithm(const UnsignedInteger dimension,
                               const IntegrationMethod method)
    : IntegrationAlgorithmImplementation()
    , discretization_(Indices(dimension, ResourceMap::GetAsUnsignedInteger("FejerAlgorithm-DefaultMarginalIntegrationPointsNumber")))
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
  const Point halfDelta( (interval.getUpperBound() - interval.getLowerBound()) / 2.0 );
  // Compute nodes
  adaptedNodes = nodes_ * halfDelta + halfDelta + interval.getLowerBound();
  // Compute the function over the adapted nodes
  const Sample values(function(adaptedNodes));
  // Compute the integral
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
    integral += values[i] * weights_[i];
  return std::pow(0.5, 1.0 * inputDimension) * integral * volume;
}

void FejerAlgorithm::generateNodesAndWeightsCleanshawCurtis(Collection<Point> & marginalNodes,
                                                            Collection<Point> & marginalWeights)
{
  const UnsignedInteger dimension = discretization_.getSize();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger integrationNodesNumber = discretization_[i];
    if (integrationNodesNumber == 0)
      throw InvalidArgumentException(HERE) << "Error: the discretization must be positive, here discretization[" << i << "] is null.";
    // Check if we already computed this 1D rule
    // We use the value 'dimension' as a guard
    UnsignedInteger indexAlreadyComputed = dimension;
    for (UnsignedInteger j = 0; j < i; ++j)
      if (discretization_[j] == integrationNodesNumber)
      {
        indexAlreadyComputed = j;
        break;
      }
    // If indexAlreadyComputed < dimension we found a match
    if (indexAlreadyComputed < dimension)
    {
      marginalNodes[i] = marginalNodes[indexAlreadyComputed];
      marginalWeights[i] = marginalWeights[indexAlreadyComputed];
    } // A match found
    else
    {
      marginalNodes[i] = Point(integrationNodesNumber);
      marginalWeights[i] = Point(integrationNodesNumber);
      for (UnsignedInteger k = 0; k < integrationNodesNumber; ++k)
      {
        const Scalar theta_k = k * M_PI / (integrationNodesNumber - 1);
        Scalar ck = 2.0;
        if (k == 0 || k == (integrationNodesNumber - 1))
          ck = 1.0;
        // Nodes
        marginalNodes[i][k] = std::cos(theta_k);
        // Weights
        Scalar term = 0;
        UnsignedInteger halfNodeNumber = (integrationNodesNumber - 1) / 2;
        for (UnsignedInteger l = 1; l <= halfNodeNumber; ++l)
        {
          Scalar bj = 1.0;
          if (l < halfNodeNumber)
            bj = 2.0;
          term += bj / (4.0 * l * l - 1) * std::cos(2 * l * theta_k);
        }
        marginalWeights[i][k] = ck / (integrationNodesNumber - 1) * (1 - term);
      }
    } // No match found
  }   // For i
}

void FejerAlgorithm::generateNodesAndWeightsFejerType1(Collection<Point> & marginalNodes,
                                                       Collection<Point> & marginalWeights)
{
  const UnsignedInteger dimension = discretization_.getSize();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger integrationNodesNumber = discretization_[i];
    // Check if we already computed this 1D rule
    // We use the value 'dimension' as a guard
    UnsignedInteger indexAlreadyComputed = dimension;
    for (UnsignedInteger j = 0; j < i; ++j)
      if (discretization_[j] == integrationNodesNumber)
      {
        indexAlreadyComputed = j;
        break;
      }
    // If indexAlreadyComputed < dimension we found a match
    if (indexAlreadyComputed < dimension)
    {
      marginalNodes[i] = marginalNodes[indexAlreadyComputed];
      marginalWeights[i] = marginalWeights[indexAlreadyComputed];
    } // A match found
    else
    {
      marginalNodes[i] = Point(integrationNodesNumber);
      marginalWeights[i] = Point(integrationNodesNumber);

      for (UnsignedInteger k = 0; k < integrationNodesNumber; ++k)
      {
        Scalar sum_term = 0;
        // Nodes
        const Scalar theta_ = (k + 1.0 / 2.) * M_PI / (integrationNodesNumber);
        marginalNodes[i][k] = std::cos(theta_);
        // Weights
        UnsignedInteger end_sum = static_cast<UnsignedInteger>(std::floor((integrationNodesNumber) / 2));
        for (UnsignedInteger index_sum = 1; index_sum <= end_sum; ++index_sum)
        {
          sum_term += (1.0 / (4. * index_sum * index_sum - 1)) * std::cos(2 * index_sum * theta_);
        }
        marginalWeights[i][k] = (2.0 / integrationNodesNumber) * (1.0 - 2.0 * sum_term);
      }

    } // No match found
  }   // For i
}

void FejerAlgorithm::generateNodesAndWeightsFejerType2(Collection<Point> & marginalNodes,
                                                       Collection<Point> & marginalWeights)
{
  // First, generate the 1D marginal rules over [0, 1]
  const UnsignedInteger dimension = discretization_.getSize();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger integrationNodesNumber = discretization_[i];
    // Check if we already computed this 1D rule
    // We use the value 'dimension' as a guard
    UnsignedInteger indexAlreadyComputed = dimension;
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      if (discretization_[j] == integrationNodesNumber)
      {
        indexAlreadyComputed = j;
        break;
      }
    }
    // If indexAlreadyComputed < dimension we found a match
    if (indexAlreadyComputed < dimension)
    {
      marginalNodes[i] = marginalNodes[indexAlreadyComputed];
      marginalWeights[i] = marginalWeights[indexAlreadyComputed];
    } // A match found
    else
    {
      marginalNodes[i] = Point(integrationNodesNumber);
      marginalWeights[i] = Point(integrationNodesNumber);

      for (UnsignedInteger k = 0; k < integrationNodesNumber; ++k)
      {
        const Scalar theta_k = k * M_PI / (integrationNodesNumber - 1);
        Scalar sum_sinus = 0.0;
        const UnsignedInteger halfNodesNumber = (integrationNodesNumber - 1) / 2;
        for (UnsignedInteger iter_ = 1; iter_ <= halfNodesNumber; ++iter_)
            sum_sinus += std::sin((2 * iter_ - 1) * theta_k) / (2 * iter_ - 1);
        // Nodes
        marginalNodes[i][k] = std::cos(theta_k);
        // Weights
        marginalWeights[i][k] = 4.0 / (integrationNodesNumber - 1) * std::sin(theta_k) * sum_sinus;
      }
    } // No match found
  }   // For i
}

/* Generate nodes and weights */
void FejerAlgorithm::generateNodesAndWeights(const IntegrationMethod method)
{
  // First, generate the 1D marginal rules over [0, 1]
  const UnsignedInteger dimension = discretization_.getSize();
  if (dimension == 0)
    throw InvalidArgumentException(HERE) << "Error: expected a positive dimension";
   for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger integrationNodesNumber = discretization_[i];
    if (integrationNodesNumber == 0)
      throw InvalidArgumentException(HERE) << "Error: the discretization must be positive, here discretization[" << i << "] is null.";
  } 
  Collection<Point> marginalNodes(dimension);
  Collection<Point> marginalWeights(dimension);
  switch (method)
  {
    case FEJERTYPE1:
      generateNodesAndWeightsFejerType1(marginalNodes, marginalWeights);
      break;
    case FEJERTYPE2:
      generateNodesAndWeightsFejerType2(marginalNodes, marginalWeights);
      break;
    case CLENSHAWCURTIS:
      generateNodesAndWeightsCleanshawCurtis(marginalNodes, marginalWeights);
      break;
    default:
      throw InvalidArgumentException(HERE) << "Error: invalid side argument for method, must be FEJERTYPE1, FEJERTYPE2 or  CLEANSHAWCURTIS, here method=" << method;
  } /* end switch */
  // Now we have marginal nodes & weights,
  // we generate the nD rule over [0, 1]^n
  IndicesCollection allTuples(Tuples(discretization_).generate());
  const UnsignedInteger size = allTuples.getSize();
  nodes_ = Sample(size, dimension);
  weights_ = Point(size, 1.0);

  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      nodes_(i, j) = marginalNodes[j][allTuples(i, j)];
      weights_[i] *= marginalWeights[j][allTuples(i, j)];
    }
  } // tuples
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

END_NAMESPACE_OPENTURNS
