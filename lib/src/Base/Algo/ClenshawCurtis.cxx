//                                               -*- C++ -*-
/**
 *  @brief Implement a tensorized Gauss-Legendre quadrature
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
#include "openturns/ClenshawCurtis.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Lapack.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ClenshawCurtis
 */

CLASSNAMEINIT(ClenshawCurtis)

static const Factory<ClenshawCurtis> Factory_ClenshawCurtis;

/* Constructor without parameters */
ClenshawCurtis::ClenshawCurtis(const UnsignedInteger dimension)
  : IntegrationAlgorithmImplementation()
  , discretization_(Indices(dimension, ResourceMap::GetAsUnsignedInteger("ClenshawCurtis-DefaultMarginalIntegrationPointsNumber")))
  , nodes_(0, 0)
  , weights_(0)
{
  // Generate nodes and weights
  generateNodesAndWeights();
}

/* Parameters constructor */
ClenshawCurtis::ClenshawCurtis(const Indices & discretization)
  : IntegrationAlgorithmImplementation()
  , discretization_(discretization)
  , nodes_(0, 0)
  , weights_(0)
{
  // Generate nodes and weights
  generateNodesAndWeights();
}

/* Virtual constructor */
ClenshawCurtis * ClenshawCurtis::clone() const
{
  return new ClenshawCurtis(*this);
}

/* Compute an approximation of \int_a^b f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an n-D interval.
 */
Point ClenshawCurtis::integrate(const Function & function,
                               const Interval & interval) const
{
  Sample adaptedNodes;
  return integrateWithNodes(function, interval, adaptedNodes);
}

Point ClenshawCurtis::integrateWithNodes(const Function & function,
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
  adaptedNodes = nodes_ * (interval.getUpperBound() - interval.getLowerBound())/2. + interval.getLowerBound()+(interval.getUpperBound() - interval.getLowerBound())/2.;
  // Compute the function over the adapted nodes
  const Sample values(function(adaptedNodes));
  // Compute the integral
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
    integral += values[i] * weights_[i];
  return std::pow(0.5,inputDimension)*integral * volume;
}

/* Generate nodes and weights */
void ClenshawCurtis::generateNodesAndWeights()
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
        const Scalar theta_k = k*M_PI/(integrationNodesNumber-1);
        Scalar ck = 0;
        if (k == 0 || k == integrationNodesNumber-1)
        {
        ck = 1.;
        }
        else
        {
        ck = 2.;
        }
        // Nodes
        marginalNodes[i][k] = cos(theta_k);
        // Weights
        Scalar terme = 0; 
        for (UnsignedInteger l = 1; l <= (UnsignedInteger)(std::floor((integrationNodesNumber-1)/2.)); ++l)
        {
        Scalar bj = 0;
        if (l<(UnsignedInteger)(std::floor((integrationNodesNumber-1)/2.)))
        {
        bj = 2.;
        }
        else
        {
        bj = 1.;
        }
        terme = terme + bj/(4.*std::pow(l,2)-1)*cos(2*l*theta_k);
        }
        marginalWeights[i][k] = ck/(integrationNodesNumber-1)*(1-terme);
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
Indices ClenshawCurtis::getDiscretization() const
{
  return discretization_;
}

/* Nodes accessor */
Sample ClenshawCurtis::getNodes() const
{
  return nodes_;
}

/* Weights accessor */
Point ClenshawCurtis::getWeights() const
{
  return weights_;
}

/* String converter */
String ClenshawCurtis::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ClenshawCurtis::GetClassName()
      << ", discretization=" << discretization_
      << ", nodes=" << nodes_
      << ", weights=" << weights_;
  return oss;
}

/* String converter */
String ClenshawCurtis::__str__(const String & ) const
{
  OSS oss(false);
  oss << ClenshawCurtis::GetClassName()
      << "(" << discretization_ << ")";
  return oss;
}

END_NAMESPACE_OPENTURNS
