//                                               -*- C++ -*-
/**
 *  @brief Implement a tensorized Fejer1 quadrature
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/Fejer1.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Lapack.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Fejer1
 */

CLASSNAMEINIT(Fejer1)

static const Factory<Fejer1> Factory_Fejer1;

/* Constructor without parameters */
Fejer1::Fejer1(const UnsignedInteger dimension)
  : IntegrationAlgorithmImplementation()
  , discretization_(Indices(dimension, ResourceMap::GetAsUnsignedInteger("Fejer1-DefaultMarginalIntegrationPointsNumber")))
  , nodes_(0, 0)
  , weights_(0)
{
  // Generate nodes and weights
  generateNodesAndWeights();
}

/* Parameters constructor */
Fejer1::Fejer1(const Indices & discretization)
  : IntegrationAlgorithmImplementation()
  , discretization_(discretization)
  , nodes_(0, 0)
  , weights_(0)
{
  // Generate nodes and weights
  generateNodesAndWeights();
}

/* Virtual constructor */
Fejer1 * Fejer1::clone() const
{
  return new Fejer1(*this);
}

/* Compute an approximation of \int_a^b f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an n-D interval.
 */
Point Fejer1::integrate(const Function & function,
                               const Interval & interval) const
{
  Sample adaptedNodes;
  return integrateWithNodes(function, interval, adaptedNodes);
}

Point Fejer1::integrateWithNodes(const Function & function,
                                        const Interval & interval,
                                        Sample & adaptedNodes) const
{
  const UnsignedInteger inputDimension = discretization_.getSize();
  if (interval.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected an interval of dimension=" << inputDimension << ", got dimension=" << interval.getDimension();
  if (function.getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a function of input dimension=" << inputDimension << ", got input dimension=" << function.getInputDimension();
  // Compute the volume of the interval
  const Scalar volume = interval.getVolume();
  Point integral(function.getOutputDimension());
  if (volume == 0.0) return integral;
  // Adapt the nodes to the bounds of the interval
  adaptedNodes = nodes_ * (interval.getUpperBound() - interval.getLowerBound())/2. + (interval.getUpperBound() - interval.getLowerBound())/2.+interval.getLowerBound();
  // Compute the function over the adapted nodes
  const Sample values(function(adaptedNodes));
  // Compute the integral
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
    integral += values[i] * weights_[i];
  return std::pow(0.5,inputDimension)*integral * volume;
}

/* Generate nodes and weights */
void Fejer1::generateNodesAndWeights()
{
  // First, generate the 1D marginal rules over [0, 1]
  const UnsignedInteger dimension = discretization_.getSize();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive dimension";
  Collection<Point> marginalNodes(dimension);
  Collection<Point> marginalWeights(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger integrationNodesNumber(discretization_[i]);
    if (integrationNodesNumber == 0) throw InvalidArgumentException(HERE) << "Error: the discretization must be positive, here discretization[" << i << "] is null.";
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
	Scalar sum_term=0;        
	// Nodes
	const Scalar theta_ = (k+1./2.) * M_PI/(integrationNodesNumber);	
        marginalNodes[i][k] = cos(theta_);
        // Weights
        UnsignedInteger end_sum = static_cast<UnsignedInteger>(std::floor((integrationNodesNumber)/2));
	for (UnsignedInteger index_sum = 1; index_sum<=end_sum; ++index_sum)
	{
		sum_term = sum_term + (1./(4.*std::pow(index_sum,2)-1))*cos(2*index_sum*theta_);
	}
        marginalWeights[i][k] = (2./(integrationNodesNumber))*(1.-2.*sum_term);
      }
      
    } // No match found
  } // For i
  // Now, generate the nD rule over [0, 1]^n
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
Indices Fejer1::getDiscretization() const
{
  return discretization_;
}

/* Nodes accessor */
Sample Fejer1::getNodes() const
{
  return nodes_;
}

/* Weights accessor */
Point Fejer1::getWeights() const
{
  return weights_;
}

/* String converter */
String Fejer1::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Fejer1::GetClassName()
      << ", discretization=" << discretization_
      << ", nodes=" << nodes_
      << ", weights=" << weights_;
  return oss;
}

/* String converter */
String Fejer1::__str__(const String & ) const
{
  OSS oss(false);
  oss << Fejer1::GetClassName()
      << "(" << discretization_ << ")";
  return oss;
}

END_NAMESPACE_OPENTURNS
