//                                               -*- C++ -*-
/**
 *  @brief Implement a tensorized Gauss-Legendre quadrature
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/GaussLegendre.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Lapack.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussLegendre
 */

CLASSNAMEINIT(GaussLegendre)

static const Factory<GaussLegendre> Factory_GaussLegendre;

/* Constructor without parameters */
GaussLegendre::GaussLegendre(const UnsignedInteger dimension)
  : IntegrationAlgorithmImplementation()
  , discretization_(Indices(dimension, ResourceMap::GetAsUnsignedInteger("GaussLegendre-DefaultMarginalIntegrationPointsNumber")))
  , nodes_(0, 0)
  , weights_(0)
{
  // Generate nodes and weights
  generateNodesAndWeights();
}

/* Parameters constructor */
GaussLegendre::GaussLegendre(const Indices & discretization)
  : IntegrationAlgorithmImplementation()
  , discretization_(discretization)
  , nodes_(0, 0)
  , weights_(0)
{
  // Generate nodes and weights
  generateNodesAndWeights();
}

/* Virtual constructor */
GaussLegendre * GaussLegendre::clone() const
{
  return new GaussLegendre(*this);
}

/* Compute an approximation of \int_a^b f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an n-D interval.
 */
Point GaussLegendre::integrate(const Function & function,
                               const Interval & interval) const
{
  Sample adaptedNodes;
  return integrateWithNodes(function, interval, adaptedNodes);
}

Point GaussLegendre::integrateWithNodes(const Function & function,
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
  adaptedNodes = nodes_ * (interval.getUpperBound() - interval.getLowerBound()) + interval.getLowerBound();
  // Compute the function over the adapted nodes
  const Sample values(function(adaptedNodes));
  // Compute the integral
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
    integral += values[i] * weights_[i];
  return integral * volume;
}

/* Generate nodes and weights */
void GaussLegendre::generateNodesAndWeights()
{
  // First, generate the 1D marginal rules over [0, 1]
  const UnsignedInteger dimension = discretization_.getSize();
  if (!(dimension > 0)) throw InvalidArgumentException(HERE) << "Error: expected a positive dimension, not " << dimension;
  Collection<Point> marginalNodes(dimension);
  Collection<Point> marginalWeights(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger integrationNodesNumber(discretization_[i]);
    if (!(integrationNodesNumber > 0)) throw InvalidArgumentException(HERE) << "Error: the discretization must be positive, here discretization[" << i << "] has " << integrationNodesNumber << "nodes.";
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
      // First, build a symmetric tridiagonal matrix whose eigenvalues are the nodes of the
      // gauss integration rule
      char jobz('V');
      int ljobz(1);
      Point d(integrationNodesNumber);
      Point e(integrationNodesNumber);
      for (UnsignedInteger k = 1; k < integrationNodesNumber; ++k) e[k - 1] = 0.5 / std::sqrt(1.0 - std::pow(2.0 * k, -2));
      int ldz(integrationNodesNumber);
      SquareMatrix z(integrationNodesNumber);
      Point work(2 * integrationNodesNumber - 2);
      int info;
      int n = static_cast<int>(integrationNodesNumber);
      dstev_(&jobz, &n, &d[0], &e[0], &z(0, 0), &ldz, &work[0], &info, &ljobz);
      if (info != 0) throw InternalException(HERE) << "Lapack DSTEV: error code=" << info;
      for (UnsignedInteger k = 0; k < integrationNodesNumber; ++k)
      {
        // Nodes
        marginalNodes[i][k] = 0.5 * (1.0 + d[k]);
        // Weights
        marginalWeights[i][k] = std::pow(z(0, k), 2);
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
Indices GaussLegendre::getDiscretization() const
{
  return discretization_;
}

/* Nodes accessor */
Sample GaussLegendre::getNodes() const
{
  return nodes_;
}

/* Weights accessor */
Point GaussLegendre::getWeights() const
{
  return weights_;
}

/* String converter */
String GaussLegendre::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GaussLegendre::GetClassName()
      << ", discretization=" << discretization_
      << ", nodes=" << nodes_
      << ", weights=" << weights_;
  return oss;
}

/* String converter */
String GaussLegendre::__str__(const String & ) const
{
  OSS oss(false);
  oss << GaussLegendre::GetClassName()
      << "(" << discretization_ << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void GaussLegendre::save(Advocate & adv) const
{
  IntegrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("discretization_", discretization_);
  adv.saveAttribute("nodes_", nodes_);
  adv.saveAttribute("weights_", weights_);
}

/* Method load() reloads the object from the StorageManager */
void GaussLegendre::load(Advocate & adv)
{
  IntegrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("discretization_", discretization_);
  adv.loadAttribute("nodes_", nodes_);
  adv.loadAttribute("weights_", weights_);
}

END_NAMESPACE_OPENTURNS
