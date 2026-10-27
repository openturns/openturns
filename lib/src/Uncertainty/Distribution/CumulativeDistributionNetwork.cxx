//                                               -*- C++ -*-
/**
 *  @brief This class implements the multivariate distribution based on a
 *         bipartite graph and a collection of distributions
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
#include "openturns/CumulativeDistributionNetwork.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Normal.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/Matrix.hxx"

#include <algorithm>
#include <cmath>
#include <limits>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CumulativeDistributionNetwork)

static const Factory<CumulativeDistributionNetwork> Factory_CumulativeDistributionNetwork;

/* Default constructor */
CumulativeDistributionNetwork::CumulativeDistributionNetwork()
  : DistributionImplementation()
  , distributionCollection_(1)
  , graph_()
{
  setName("CumulativeDistributionNetwork");
  // Set an empty range
  setDistributionCollection(DistributionCollection(1));
}

/* Parameters constructor */
CumulativeDistributionNetwork::CumulativeDistributionNetwork(const DistributionCollection & coll,
    const BipartiteGraph & graph)
  : DistributionImplementation()
  , distributionCollection_()
  , graph_(graph)
{
  setName("CumulativeDistributionNetwork");
  // We could NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the same dimension). We do this by calling
  setDistributionCollection( coll );
}

/* Comparison operator */
Bool CumulativeDistributionNetwork::operator ==(const CumulativeDistributionNetwork & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (distributionCollection_ == other.distributionCollection_) && (graph_ == other.graph_);
}

Bool CumulativeDistributionNetwork::equals(const DistributionImplementation & other) const
{
  const CumulativeDistributionNetwork* p_other = dynamic_cast<const CumulativeDistributionNetwork*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String CumulativeDistributionNetwork::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CumulativeDistributionNetwork::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " distributionCollection=" << distributionCollection_
      << " graph=" << graph_;
  return oss;
}

String CumulativeDistributionNetwork::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "([";
  String separator("");
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    oss << separator << distributionCollection_[i] << ")";
    separator = ", ";
  }
  oss << "]" << graph_.__str__() << ")";
  return oss;
}

/* Compute the numerical range of the distribution given the parameters values */
void CumulativeDistributionNetwork::computeRange()
{
  if (distributionCollection_.getSize() == 0) return;
  setRange(computeRange(distributionCollection_, graph_));
}

/* Compute the numerical range of a candidate structure.
   The CDF is the product of the factor CDFs. For a variable shared by several
   factors, the product is zero below the largest finite lower bound and reaches
   one only above the largest upper bound. The support is therefore not the
   intersection of the factor supports: a finite lower bound of any factor
   bounds the support from below, while an unbounded upper bound of any factor
   leaves the support unbounded from above.
   The finite bounds are kept as numerical proxies for the unbounded sides, as
   in the factor ranges: they are used by the numerical integration and by the
   plotting code, while the finite bound flags record the true support. */
Interval CumulativeDistributionNetwork::computeRange(const DistributionCollection & coll,
    const BipartiteGraph & graph) const
{
  const UnsignedInteger size = coll.getSize();
  const UnsignedInteger dim = graph.getBlackNodes().getSize();
  Point lowerBounds(dim, SpecFunc::LowestScalar);
  Point upperBounds(dim,  SpecFunc::Infinity);
  Interval::BoolCollection lowerBoundsFlags(dim, false);
  Interval::BoolCollection upperBoundsFlags(dim, false);
  Indices initialized(dim, 0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Interval cdfRange(coll[i].getRange());
    const UnsignedInteger nodeSize = graph.cend_at(i) - graph.cbegin_at(i);
    for (UnsignedInteger j = 0; j < nodeSize; ++j)
    {
      const UnsignedInteger index = graph(i, j);
      if (index >= dim) throw InvalidArgumentException(HERE) << "Error: the black node " << index << " linked to the red node " << i << " is not a valid variable index (dimension=" << dim << ")";
      const Scalar lowerBound = cdfRange.getLowerBound()[j];
      const Scalar upperBound = cdfRange.getUpperBound()[j];
      const Bool finiteLowerBound = cdfRange.getFiniteLowerBound()[j];
      const Bool finiteUpperBound = cdfRange.getFiniteUpperBound()[j];
      if (!initialized[index])
      {
        lowerBounds[index] = lowerBound;
        upperBounds[index] = upperBound;
        lowerBoundsFlags[index] = finiteLowerBound;
        upperBoundsFlags[index] = finiteUpperBound;
        initialized[index] = 1;
        continue;
      }
      // A finite lower bound of any factor bounds the support from below.
      // Otherwise keep the smallest finite proxy of the unbounded factors.
      if (finiteLowerBound)
      {
        if (lowerBoundsFlags[index])
          lowerBounds[index] = std::max(lowerBounds[index], lowerBound);
        else
        {
          lowerBounds[index] = lowerBound;
          lowerBoundsFlags[index] = true;
        }
      }
      else if (!lowerBoundsFlags[index])
        lowerBounds[index] = std::min(lowerBounds[index], lowerBound);
      // The support is bounded from above only if every factor is. Keep the
      // largest finite proxy in all cases.
      upperBounds[index] = std::max(upperBounds[index], upperBound);
      if (!finiteUpperBound)
        upperBoundsFlags[index] = false;
    }
  } // Red nodes
  return Interval(lowerBounds, upperBounds, lowerBoundsFlags, upperBoundsFlags);
}

/* Rebuild the incidence caches from the collection and the graph.
   The caches are built in local variables and committed only when every
   consistency check has passed, so that a failing setter leaves the network
   unchanged. */
void CumulativeDistributionNetwork::updateStructure(const DistributionCollection & coll,
    const BipartiteGraph & graph)
{
  const UnsignedInteger size = coll.getSize();
  const UnsignedInteger dimension = graph.getBlackNodes().getSize();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: the graph must have at least one black node";
  Collection<Indices> factorVariables(size);
  Collection<Indices> variableFactors(dimension);
  Indices variableMultiplicity(dimension);
  Collection<DistributionCollection> factorUnivariateMarginals(size);
  Bool blockIndependent = true;
  for (UnsignedInteger c = 0; c < size; ++c)
  {
    const UnsignedInteger localDimension = graph.cend_at(c) - graph.cbegin_at(c);
    if (localDimension == 0) throw InvalidArgumentException(HERE) << "Error: the distribution " << c << " has an empty clique";
    Indices variables(localDimension);
    for (UnsignedInteger j = 0; j < localDimension; ++j)
      variables[j] = graph(c, j);
    // Check that the black node numbers are valid and that a clique has no duplicate
    for (UnsignedInteger j = 0; j < localDimension; ++j)
    {
      if (variables[j] >= dimension) throw InvalidArgumentException(HERE) << "Error: the black node " << variables[j] << " linked to the red node " << c << " is not a valid variable index (dimension=" << dimension << ")";
      for (UnsignedInteger k = 0; k < j; ++k)
        if (variables[k] == variables[j]) throw InvalidArgumentException(HERE) << "Error: the black node " << variables[j] << " appears twice in the clique of the red node " << c;
    }
    factorVariables[c] = variables;
    for (UnsignedInteger j = 0; j < localDimension; ++j)
    {
      variableFactors[variables[j]].add(c);
      variableMultiplicity[variables[j]] += 1;
    }
    // Cache the univariate marginals of independent factors, used by the partial derivative engine
    if ((localDimension > 1) && coll[c].hasIndependentCopula())
    {
      const Distribution & distribution = coll[c];
      for (UnsignedInteger j = 0; j < localDimension; ++j)
        factorUnivariateMarginals[c].add(distribution.getMarginal(j));
    }
  }
  // Every variable must belong to at least one factor, otherwise the joint law is improper
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (variableMultiplicity[i] == 0) throw InvalidArgumentException(HERE) << "Error: the black node " << i << " is not linked to any red node";
    if (variableMultiplicity[i] > 1) blockIndependent = false;
  }
  // Commit the caches only after all the checks have passed
  factorVariables_ = factorVariables;
  variableFactors_ = variableFactors;
  variableMultiplicity_ = variableMultiplicity;
  factorUnivariateMarginals_ = factorUnivariateMarginals;
  blockIndependent_ = blockIndependent;
}


/* Distribution collection accessor */
void CumulativeDistributionNetwork::setDistributionCollection(const DistributionCollection & coll)
{
  const UnsignedInteger size = coll.getSize();
  // Check the number of distributions
  if (size != graph_.getRedNodes().getSize()) throw InvalidArgumentException(HERE) << "Error: the given collection of distributions has a size=" << size << " different from the number of red nodes=" << graph_.getRedNodes().getSize();
  // Check the dimension of the distributions
  Bool parallel = true;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const UnsignedInteger nodeSize = graph_.cend_at(i) - graph_.cbegin_at(i);
    if (coll[i].getDimension() != nodeSize) throw InvalidArgumentException(HERE) << "Error: the distribution " << i << " has a dimension=" << coll[i].getDimension() << " which is different from the number of links=" << nodeSize << " starting from red node " << i;
    parallel = parallel && coll[i].getImplementation()->isParallel();
  }
  // Build the candidate structure and range first: any exception leaves the
  // network unchanged.
  const Interval range = (size > 0) ? computeRange(coll, graph_) : Interval();
  updateStructure(coll, graph_);
  // Commit the new collection only after all the checks have passed
  setParallel(parallel);
  distributionCollection_ = coll;
  LOGINFO(OSS() << "graph=" << graph_ << ", dim=" << graph_.getBlackNodes().getSize());
  setDimension(graph_.getBlackNodes().getSize());
  if (size > 0) setRange(range);
}

/* Distribution collection accessor */
CumulativeDistributionNetwork::DistributionCollection CumulativeDistributionNetwork::getDistributionCollection() const
{
  return distributionCollection_;
}

/* Graph accessor */
void CumulativeDistributionNetwork::setGraph(const BipartiteGraph & graph)
{
  const UnsignedInteger size = distributionCollection_.getSize();
  // Check the number of distributions
  if (size != graph.getRedNodes().getSize()) throw InvalidArgumentException(HERE) << "Error: the given graph has a number of red nodes=" << graph.getRedNodes().getSize() << " different from the collection of distributions size=" << size;
  // Check the dimension of the distributions
  Bool parallel = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger nodeSize = graph.cend_at(i) - graph.cbegin_at(i);
    if (distributionCollection_[i].getDimension() != nodeSize) throw InvalidArgumentException(HERE) << "Error: the number of links=" << nodeSize << " starting from red node " << i << " is different from distribution " << i << " dimension=" << distributionCollection_[i].getDimension();
    parallel = parallel && distributionCollection_[i].getImplementation()->isParallel();
  }
  // Build the candidate structure and range first: any exception leaves the
  // network unchanged.
  const Interval range = (size > 0) ? computeRange(distributionCollection_, graph) : Interval();
  updateStructure(distributionCollection_, graph);
  // Commit the new graph only after all the checks have passed
  setParallel(parallel);
  graph_ = graph;
  setDimension(graph_.getBlackNodes().getSize());
  if (size > 0) setRange(range);
}

/* Graph accessor */
BipartiteGraph CumulativeDistributionNetwork::getGraph() const
{
  return graph_;
}

/* Virtual constructor */
CumulativeDistributionNetwork * CumulativeDistributionNetwork::clone() const
{
  return new CumulativeDistributionNetwork(*this);
}


/* Get one realization of the CumulativeDistributionNetwork */
Point CumulativeDistributionNetwork::getRealization() const
{
  // A network whose variables are not shared between factors is a product
  // of independent blocks: sample each block directly.
  if (blockIndependent_)
  {
    const UnsignedInteger dimension = getDimension();
    Point realization(dimension);
    const UnsignedInteger size = distributionCollection_.getSize();
    for (UnsignedInteger c = 0; c < size; ++c)
    {
      const Point localRealization(distributionCollection_[c].getRealization());
      const Indices & variables = factorVariables_[c];
      for (UnsignedInteger j = 0; j < variables.getSize(); ++j)
        realization[variables[j]] = localRealization[j];
    }
    return realization;
  }
  return DistributionImplementation::getRealizationByInversion();
}

/* Get many realizations of the CumulativeDistributionNetwork */
Sample CumulativeDistributionNetwork::getSample(const UnsignedInteger size) const
{
  if (blockIndependent_)
  {
    const UnsignedInteger dimension = getDimension();
    Sample result(size, dimension);
    const UnsignedInteger factorCount = distributionCollection_.getSize();
    for (UnsignedInteger c = 0; c < factorCount; ++c)
    {
      const Sample localSample(distributionCollection_[c].getSample(size));
      const Indices & variables = factorVariables_[c];
      for (UnsignedInteger i = 0; i < size; ++i)
        for (UnsignedInteger j = 0; j < variables.getSize(); ++j)
          result(i, variables[j]) = localSample(i, j);
    }
    result.setName(getName());
    result.setDescription(getDescription());
    return result;
  }
  return DistributionImplementation::getSampleByInversion(size);
}

/* Extract the components of a full dimension point to feed the index distribution */
Point CumulativeDistributionNetwork::reducePoint(const Point & point,
    const UnsignedInteger index) const
{
  const UnsignedInteger size = graph_.cend_at(index) - graph_.cbegin_at(index);
  Point reducedPoint(size);
  for (UnsignedInteger i = 0; i < size; ++i) reducedPoint[i] = point[graph_(index, i)];
  return reducedPoint;
}

/* Compute the log of a local CDF, integrating out infinite coordinates exactly */
Scalar CumulativeDistributionNetwork::computeLocalLogCDF(const UnsignedInteger factorIndex,
    const Point & localPoint) const
{
  const Distribution & distribution = distributionCollection_[factorIndex];
  const UnsignedInteger dimension = distribution.getDimension();
  // A coordinate at -infinity gives a zero CDF
  Indices finitePositions(0);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    if (localPoint[j] == SpecFunc::LowestScalar) return -SpecFunc::Infinity;
    if (localPoint[j] != SpecFunc::Infinity) finitePositions.add(j);
  }
  // All coordinates are at +infinity: the CDF is one
  if (finitePositions.getSize() == 0) return 0.0;
  if (finitePositions.getSize() == dimension)
    return computeFactorLogCDF(distribution, factorUnivariateMarginals_[factorIndex], localPoint);
  // Integrate out the coordinates at +infinity by using the exact marginal
  Point argument(finitePositions.getSize());
  for (UnsignedInteger k = 0; k < finitePositions.getSize(); ++k)
    argument[k] = localPoint[finitePositions[k]];
  const Distribution marginal(distribution.getMarginal(finitePositions));
  return computeFactorLogCDF(marginal, DistributionCollection(0), argument);
}

/* Compute the log of a local CDF at a finite point, factorised when possible */
Scalar CumulativeDistributionNetwork::computeFactorLogCDF(const Distribution & distribution,
    const DistributionCollection & univariateMarginals,
    const Point & localPoint) const
{
  const UnsignedInteger dimension = distribution.getDimension();
  // For an independent factor the log-CDF is a sum of univariate log-CDFs:
  // this keeps the result finite even when their product underflows.
  if ((dimension > 1) && distribution.hasIndependentCopula())
  {
    const Bool cached = (univariateMarginals.getSize() == dimension);
    Scalar logCDF = 0.0;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar cdf = cached ? univariateMarginals[j].computeCDF(localPoint[j])
                        : distribution.getMarginal(j).computeCDF(localPoint[j]);
      if (!(cdf > 0.0)) return -SpecFunc::Infinity;
      logCDF += std::log(cdf);
    }
    return logCDF;
  }
  const Scalar cdf = distribution.computeCDF(localPoint);
  return (cdf > 0.0) ? std::log(cdf) : -SpecFunc::Infinity;
}

/* Compute the log of a local mixed partial derivative (or finite difference) */
Scalar CumulativeDistributionNetwork::computeLocalPartialLog(const UnsignedInteger factorIndex,
    const Point & localPoint,
    const Indices & derivativePositions) const
{
  const Distribution & distribution = distributionCollection_[factorIndex];
  const UnsignedInteger dimension = distribution.getDimension();
  // A coordinate at -infinity gives a zero CDF
  Indices finitePositions(0);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    if (localPoint[j] == SpecFunc::LowestScalar) return -SpecFunc::Infinity;
    if (localPoint[j] != SpecFunc::Infinity) finitePositions.add(j);
  }
  // A derivative taken at +infinity vanishes
  for (UnsignedInteger k = 0; k < derivativePositions.getSize(); ++k)
    if (localPoint[derivativePositions[k]] == SpecFunc::Infinity) return -SpecFunc::Infinity;
  if (finitePositions.getSize() == dimension)
    return computeFactorPartialLog(factorIndex, distribution, factorUnivariateMarginals_[factorIndex], localPoint, derivativePositions);
  // All coordinates are at +infinity: only the CDF remains
  if (finitePositions.getSize() == 0) return 0.0;
  // Integrate out the coordinates at +infinity by using the exact marginal
  Point argument(finitePositions.getSize());
  for (UnsignedInteger k = 0; k < finitePositions.getSize(); ++k)
    argument[k] = localPoint[finitePositions[k]];
  Indices marginalDerivative(derivativePositions.getSize());
  for (UnsignedInteger k = 0; k < derivativePositions.getSize(); ++k)
  {
    UnsignedInteger position = 0;
    while ((position < finitePositions.getSize()) && (finitePositions[position] != derivativePositions[k])) ++position;
    if (position == finitePositions.getSize()) throw InternalException(HERE) << "Error: inconsistent derivative position";
    marginalDerivative[k] = position;
  }
  const Distribution marginal(distribution.getMarginal(finitePositions));
  return computeFactorPartialLog(factorIndex, marginal, DistributionCollection(0), argument, marginalDerivative);
}

/* Compute the log of a local mixed partial derivative (or finite difference)
   at a finite point */
Scalar CumulativeDistributionNetwork::computeFactorPartialLog(const UnsignedInteger factorIndex,
    const Distribution & distribution,
    const DistributionCollection & univariateMarginals,
    const Point & localPoint,
    const Indices & derivativePositions) const
{
  const UnsignedInteger dimension = distribution.getDimension();

  const UnsignedInteger size = derivativePositions.getSize();
  // No derivative: local CDF
  if (size == 0)
    return computeFactorLogCDF(distribution, univariateMarginals, localPoint);
  // Full derivative: local density or probability mass
  if (size == dimension)
    return distribution.computeLogPDF(localPoint);
  // Independent factors: the local derivative factorises exactly
  if (distribution.hasIndependentCopula())
  {
    Indices derivativeFlags(dimension, 0);
    for (UnsignedInteger k = 0; k < size; ++k) derivativeFlags[derivativePositions[k]] = 1;
    const DistributionCollection & marginals = univariateMarginals;
    const Bool cached = (marginals.getSize() == dimension);
    Scalar logValue = 0.0;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      if (derivativeFlags[j])
      {
        const Scalar logPDF = cached ? marginals[j].computeLogPDF(localPoint[j])
                             : distribution.getMarginal(j).computeLogPDF(localPoint[j]);
        if (logPDF == -SpecFunc::Infinity) return -SpecFunc::Infinity;
        logValue += logPDF;
      }
      else
      {
        const Scalar cdf = cached ? marginals[j].computeCDF(localPoint[j])
                          : distribution.getMarginal(j).computeCDF(localPoint[j]);
        if (!(cdf > 0.0)) return -SpecFunc::Infinity;
        logValue += std::log(cdf);
      }
    }
    return logValue;
  }
  // Gaussian factors: exact Schur complement formula
  const Normal *p_normal = dynamic_cast<const Normal *>(distribution.getImplementation().get());
  if (p_normal)
  {
    Bool success = false;
    Scalar logValue = -SpecFunc::Infinity;
    try
    {
      Indices derivativeFlags(dimension, 0);
      for (UnsignedInteger k = 0; k < size; ++k) derivativeFlags[derivativePositions[k]] = 1;
      Indices complement(0);
      for (UnsignedInteger j = 0; j < dimension; ++j)
        if (!derivativeFlags[j]) complement.add(j);
      const UnsignedInteger complementSize = complement.getSize();
      const Point mean(p_normal->getMean());
      const CovarianceMatrix covariance(p_normal->getCovariance());
      CovarianceMatrix covarianceSS(size);
      Matrix covarianceST(size, complementSize);
      CovarianceMatrix covarianceTT(complementSize);
      for (UnsignedInteger a = 0; a < size; ++a)
        for (UnsignedInteger b = 0; b < size; ++b)
          covarianceSS(a, b) = covariance(derivativePositions[a], derivativePositions[b]);
      for (UnsignedInteger a = 0; a < size; ++a)
        for (UnsignedInteger b = 0; b < complementSize; ++b)
          covarianceST(a, b) = covariance(derivativePositions[a], complement[b]);
      for (UnsignedInteger a = 0; a < complementSize; ++a)
        for (UnsignedInteger b = 0; b < complementSize; ++b)
          covarianceTT(a, b) = covariance(complement[a], complement[b]);
      const Matrix solution(covarianceSS.solveLinearSystem(covarianceST));
      const Matrix correction(covarianceST.transpose() * solution);
      CovarianceMatrix conditionalCovariance(complementSize);
      for (UnsignedInteger a = 0; a < complementSize; ++a)
        for (UnsignedInteger b = 0; b <= a; ++b)
          conditionalCovariance(a, b) = covarianceTT(a, b) - correction(a, b);
      Point meanS(size);
      Point meanT(complementSize);
      Point xS(size);
      Point xT(complementSize);
      for (UnsignedInteger a = 0; a < size; ++a)
      {
        meanS[a] = mean[derivativePositions[a]];
        xS[a] = localPoint[derivativePositions[a]];
      }
      for (UnsignedInteger a = 0; a < complementSize; ++a)
      {
        meanT[a] = mean[complement[a]];
        xT[a] = localPoint[complement[a]];
      }
      const Point conditionalMean(meanT + solution.transpose() * (xS - meanS));
      const Normal marginalS(meanS, covarianceSS);
      const Normal conditionalT(conditionalMean, conditionalCovariance);
      const Scalar logDensity = marginalS.computeLogPDF(xS);
      const Scalar conditionalCDF = conditionalT.computeCDF(xT);
      if (std::isfinite(logDensity) && (conditionalCDF > 0.0))
      {
        logValue = logDensity + std::log(conditionalCDF);
        success = true;
      }
    }
    catch (const Exception &)
    {
      // Singular covariance: fall back to a generic local derivative
    }
    if (success) return logValue;
  }
  // Singleton complement: marginal density over the differentiated
  // coordinates times the conditional CDF of the last coordinate
  if (dimension - size == 1)
  {
    UnsignedInteger complementPosition = dimension;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      Bool differentiated = false;
      for (UnsignedInteger k = 0; k < size; ++k)
        if (derivativePositions[k] == j)
        {
          differentiated = true;
          break;
        }
      if (!differentiated)
      {
        complementPosition = j;
        break;
      }
    }
    if (complementPosition == dimension) throw InternalException(HERE) << "Error: inconsistent derivative positions";
    Indices permutation(0);
    for (UnsignedInteger k = 0; k < size; ++k) permutation.add(derivativePositions[k]);
    permutation.add(complementPosition);
    const Distribution permuted(distribution.getMarginal(permutation));
    Point xS(size);
    for (UnsignedInteger k = 0; k < size; ++k) xS[k] = localPoint[derivativePositions[k]];
    Indices marginalIndices(size);
    marginalIndices.fill();
    const Scalar logDensity = permuted.getMarginal(marginalIndices).computeLogPDF(xS);
    if (logDensity == -SpecFunc::Infinity) return -SpecFunc::Infinity;
    const Scalar conditionalCDF = permuted.computeConditionalCDF(localPoint[complementPosition], xS);
    if (!(conditionalCDF > 0.0)) return -SpecFunc::Infinity;
    return logDensity + std::log(conditionalCDF);
  }
  // The public services only request local derivatives over a prefix of the
  // variables. After the cases above, a factor that contains the conditioned
  // variable has a singleton complement and a factor that does not contain it
  // is fully differentiated. Reaching this point would mean that the local
  // assignment cache and the request disagree.
  throw InternalException(HERE) << "Error: inconsistent local derivative configuration for factor " << factorIndex;
}

/* Compute the log of a mixed difference of the joint CDF of integral factors.

   For a joint CDF F = prod_c F_c and a set C of integer variables, the mixed
   backward difference is

     Delta_C F(x) = sum_{R subset C} (-1)^|R|
                      prod_c F_c(x_j - 1_{j in R}, j in C, x_j, j not in C),

   see the inclusion-exclusion form of the product rule for finite differences.
   The sum is a partition function over binary variables R_j, evaluated by
   variable elimination in log domain: each factor table entry is stored as a
   sign and a log-magnitude, so that products of CDFs cannot underflow and the
   sums use a signed log-sum-exp. */
Scalar CumulativeDistributionNetwork::computeDiscretePartialLog(const Point & point,
    const Indices & derivativeIndices) const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger factorCount = distributionCollection_.getSize();
  Indices derivative(dimension, 0);
  Indices derivativeList(0);
  for (UnsignedInteger k = 0; k < derivativeIndices.getSize(); ++k)
  {
    const UnsignedInteger index = derivativeIndices[k];
    if (index >= dimension) throw InvalidArgumentException(HERE) << "Error: the derivative index " << index << " is not in the range [0, " << dimension - 1 << "]";
    if (derivative[index]) continue;
    derivative[index] = 1;
    derivativeList.add(index);
  }
  const UnsignedInteger derivativeSize = derivativeList.getSize();
  if (derivativeSize == 0)
  {
    Scalar logCDF = 0.0;
    for (UnsignedInteger c = 0; c < factorCount; ++c)
    {
      const Scalar factorLogCDF = computeLocalLogCDF(c, reducePoint(point, c));
      if (factorLogCDF == -SpecFunc::Infinity) return -SpecFunc::Infinity;
      logCDF += factorLogCDF;
    }
    return logCDF;
  }
  const UnsignedInteger maximumTerms = ResourceMap::GetAsUnsignedInteger("CumulativeDistributionNetwork-MaximumTerms");

  // A table over the binary variables of a factor scope. Each entry is stored
  // as a sign and the logarithm of its magnitude: value = sign * exp(logAbs),
  // with sign in {-1, 0, 1} and logAbs = -inf when sign == 0. This keeps the
  // products of CDFs from underflowing and lets the inclusion-exclusion sums
  // be evaluated with a signed log-sum-exp.
  struct SignedLog
  {
    Scalar sign;
    Scalar logAbs;

    SignedLog() : sign(0.0), logAbs(-SpecFunc::Infinity) {}
    SignedLog(const Scalar theSign, const Scalar theLogAbs) : sign(theSign), logAbs(theLogAbs) {}

    SignedLog multiply(const SignedLog & other) const
    {
      if ((sign == 0.0) || (other.sign == 0.0)) return SignedLog();
      return SignedLog(sign * other.sign, logAbs + other.logAbs);
    }

    SignedLog add(const SignedLog & other) const
    {
      if (sign == 0.0) return other;
      if (other.sign == 0.0) return *this;
      if (sign == other.sign)
        return SignedLog(sign, SpecFunc::LogSumExp(logAbs, other.logAbs));
      // Opposite signs: log(exp(maximum) - exp(minimum)) with maximum >= minimum.
      // expm1 keeps the difference accurate when the two log-magnitudes are close.
      const Scalar maximum = std::max(logAbs, other.logAbs);
      const Scalar minimum = std::min(logAbs, other.logAbs);
      if (maximum == minimum) return SignedLog();
      return SignedLog((logAbs > other.logAbs) ? sign : other.sign,
                       maximum + std::log(-std::expm1(minimum - maximum)));
    }
  };

  struct LocalFactor
  {
    Indices scope;
    Collection<SignedLog> values;
  };

  Collection<LocalFactor> factors(0);
  SignedLog constant(1.0, 0.0);
  for (UnsignedInteger c = 0; c < factorCount; ++c)
  {
    const Distribution & distribution = distributionCollection_[c];
    const Indices & variables = factorVariables_[c];
    const Point localPoint(reducePoint(point, c));
    // A coordinate at -infinity gives a zero factor CDF. Coordinates at
    // +infinity are integrated out exactly, using the factor marginal.
    Indices finitePositions(0);
    for (UnsignedInteger j = 0; j < localPoint.getDimension(); ++j)
    {
      if (localPoint[j] == SpecFunc::LowestScalar) return -SpecFunc::Infinity;
      if (localPoint[j] != SpecFunc::Infinity) finitePositions.add(j);
    }
    Indices localDerivative(0);
    Bool derivativeAtInfinity = false;
    for (UnsignedInteger j = 0; j < variables.getSize(); ++j)
      if (derivative[variables[j]])
      {
        localDerivative.add(j);
        if (localPoint[j] == SpecFunc::Infinity) derivativeAtInfinity = true;
      }
    // A difference taken at +infinity vanishes
    if (derivativeAtInfinity) return -SpecFunc::Infinity;
    Distribution factorDistribution(distribution);
    Point factorPoint(localPoint);
    Indices factorDerivative(localDerivative);
    if (finitePositions.getSize() < variables.getSize())
    {
      // All coordinates are at +infinity and there is no derivative: CDF=1
      if (finitePositions.getSize() == 0) continue;
      factorDistribution = distribution.getMarginal(finitePositions);
      factorPoint = Point(finitePositions.getSize());
      for (UnsignedInteger k = 0; k < finitePositions.getSize(); ++k)
        factorPoint[k] = localPoint[finitePositions[k]];
      factorDerivative = Indices(localDerivative.getSize());
      for (UnsignedInteger k = 0; k < localDerivative.getSize(); ++k)
      {
        UnsignedInteger position = 0;
        while ((position < finitePositions.getSize()) && (finitePositions[position] != localDerivative[k])) ++position;
        if (position == finitePositions.getSize()) throw InternalException(HERE) << "Error: inconsistent discrete derivative position";
        factorDerivative[k] = position;
      }
    }
    const UnsignedInteger order = factorDerivative.getSize();
    if (order == 0)
    {
      const Scalar cdf = factorDistribution.computeCDF(factorPoint);
      if (!(cdf > 0.0)) return -SpecFunc::Infinity;
      constant = constant.multiply(SignedLog(1.0, std::log(cdf)));
      continue;
    }
    if ((order >= std::numeric_limits<UnsignedInteger>::digits) || ((UnsignedInteger(1) << order) > maximumTerms))
      throw NotYetImplementedException(HERE) << "Error: the factor " << c << " has " << order << " differentiated variables, above CumulativeDistributionNetwork-MaximumTerms=" << maximumTerms;
    LocalFactor factor;
    factor.scope = Indices(order);
    factor.values.resize(UnsignedInteger(1) << order);
    for (UnsignedInteger b = 0; b < order; ++b)
      factor.scope[b] = variables[localDerivative[b]];
    for (UnsignedInteger mask = 0; mask < factor.values.getSize(); ++mask)
    {
      Point argument(factorPoint);
      for (UnsignedInteger b = 0; b < order; ++b)
        if (mask & (UnsignedInteger(1) << b))
          argument[factorDerivative[b]] -= 1.0;
      const Scalar cdf = factorDistribution.computeCDF(argument);
      factor.values[mask] = (cdf > 0.0) ? SignedLog(1.0, std::log(cdf)) : SignedLog();
    }
    factors.add(factor);
  }
  // Sign of each binary variable in the inclusion-exclusion sum
  for (UnsignedInteger k = 0; k < derivativeSize; ++k)
  {
    LocalFactor signFactor;
    signFactor.scope = Indices(1, derivativeList[k]);
    signFactor.values.resize(2);
    signFactor.values[0] = SignedLog(1.0, 0.0);
    signFactor.values[1] = SignedLog(-1.0, 0.0);
    factors.add(signFactor);
  }

  // Variable elimination, choosing the variable with the smallest degree
  Indices eliminated(dimension, 0);
  UnsignedInteger remaining = derivativeSize;
  while (remaining > 0)
  {
    UnsignedInteger variable = dimension;
    UnsignedInteger bestDegree = std::numeric_limits<UnsignedInteger>::max();
    for (UnsignedInteger k = 0; k < derivativeSize; ++k)
    {
      const UnsignedInteger candidate = derivativeList[k];
      if (eliminated[candidate]) continue;
      UnsignedInteger degree = 0;
      for (UnsignedInteger f = 0; f < factors.getSize(); ++f)
      {
        const Indices & scope = factors[f].scope;
        for (UnsignedInteger j = 0; j < scope.getSize(); ++j)
          if (scope[j] == candidate)
          {
            ++degree;
            break;
          }
      }
      if (degree < bestDegree)
      {
        bestDegree = degree;
        variable = candidate;
      }
    }
    if (variable == dimension) throw InternalException(HERE) << "Error: inconsistent discrete factor graph";
    if (bestDegree == 0) return -SpecFunc::Infinity;

    // Gather the factors containing the variable
    Collection<LocalFactor> bucket(0);
    Collection<LocalFactor> kept(0);
    for (UnsignedInteger f = 0; f < factors.getSize(); ++f)
    {
      const Indices & scope = factors[f].scope;
      Bool found = false;
      for (UnsignedInteger j = 0; j < scope.getSize(); ++j)
        if (scope[j] == variable)
        {
          found = true;
          break;
        }
      if (found) bucket.add(factors[f]);
      else kept.add(factors[f]);
    }
    factors = kept;

    // Union of the scopes
    Indices unionScope(0);
    for (UnsignedInteger f = 0; f < bucket.getSize(); ++f)
    {
      const Indices & scope = bucket[f].scope;
      for (UnsignedInteger j = 0; j < scope.getSize(); ++j)
      {
        Bool found = false;
        for (UnsignedInteger u = 0; u < unionScope.getSize(); ++u)
          if (unionScope[u] == scope[j])
          {
            found = true;
            break;
          }
        if (!found) unionScope.add(scope[j]);
      }
    }
    std::sort(unionScope.begin(), unionScope.end());
    const UnsignedInteger unionSize = unionScope.getSize();
    if ((unionSize >= std::numeric_limits<UnsignedInteger>::digits) || ((UnsignedInteger(1) << unionSize) > maximumTerms))
      throw NotYetImplementedException(HERE) << "Error: the discrete variable elimination requires more than CumulativeDistributionNetwork-MaximumTerms=" << maximumTerms << " table entries";
    const UnsignedInteger unionSizeTable = UnsignedInteger(1) << unionSize;
    Collection<SignedLog> product(unionSizeTable, SignedLog(1.0, 0.0));
    for (UnsignedInteger f = 0; f < bucket.getSize(); ++f)
    {
      const Indices & scope = bucket[f].scope;
      Indices mapping(scope.getSize());
      for (UnsignedInteger j = 0; j < scope.getSize(); ++j)
      {
        mapping[j] = unionSize;
        for (UnsignedInteger u = 0; u < unionSize; ++u)
          if (unionScope[u] == scope[j])
          {
            mapping[j] = u;
            break;
          }
        if (mapping[j] == unionSize) throw InternalException(HERE) << "Error: inconsistent discrete factor scope";
      }
      for (UnsignedInteger mask = 0; mask < unionSizeTable; ++mask)
      {
        UnsignedInteger localMask = 0;
        for (UnsignedInteger j = 0; j < scope.getSize(); ++j)
          if (mask & (UnsignedInteger(1) << mapping[j]))
            localMask |= UnsignedInteger(1) << j;
        product[mask] = product[mask].multiply(bucket[f].values[localMask]);
      }
    }

    // Sum over the eliminated variable
    UnsignedInteger position = 0;
    while ((position < unionSize) && (unionScope[position] != variable)) ++position;
    if (position == unionSize) throw InternalException(HERE) << "Error: inconsistent discrete variable elimination";
    Indices reducedScope(0);
    for (UnsignedInteger u = 0; u < unionSize; ++u)
      if (u != position) reducedScope.add(unionScope[u]);
    const UnsignedInteger reducedSize = reducedScope.getSize();
    Collection<SignedLog> reduced(UnsignedInteger(1) << reducedSize, SignedLog());
    const UnsignedInteger lowMask = (UnsignedInteger(1) << position) - 1;
    for (UnsignedInteger mask = 0; mask < unionSizeTable; ++mask)
      if (!(mask & (UnsignedInteger(1) << position)))
      {
        const UnsignedInteger reducedMask = (mask & lowMask) | ((mask >> (position + 1)) << position);
        reduced[reducedMask] = reduced[reducedMask].add(product[mask].add(product[mask | (UnsignedInteger(1) << position)]));
      }
    if (reducedSize == 0) constant = constant.multiply(reduced[0]);
    else
    {
      LocalFactor reducedFactor;
      reducedFactor.scope = reducedScope;
      reducedFactor.values = reduced;
      factors.add(reducedFactor);
    }
    eliminated[variable] = 1;
    --remaining;
  }
  if (!factors.isEmpty()) throw InternalException(HERE) << "Error: discrete variable elimination left uneliminated factors";
  if (constant.sign <= 0.0) return -SpecFunc::Infinity;
  return constant.logAbs;
}

/* Compute the log of a mixed partial derivative of the joint CDF.

   The derivative-sum-product expansion implemented below is described in:
   J. C. Huang and B. J. Frey, "Cumulative distribution networks and the
   derivative-sum-product algorithm", Proceedings of the 24th Conference on
   Uncertainty in Artificial Intelligence (UAI 2008), PMLR R6:290-297, 2008,
   arXiv:1206.3259. The joint CDF is a product of factor CDFs; each
   differentiated variable is assigned to exactly one of the factors that
   contain it, and the resulting assignments are summed in log domain.
   Forest components are processed by dynamic programming, cyclic components
   by explicit enumeration. */
Scalar CumulativeDistributionNetwork::computePartialDerivativeLog(const Point & point,
    const Indices & derivativeIndices,
    const Bool useDifferences) const
{
  if (useDifferences) return computeDiscretePartialLog(point, derivativeIndices);
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger factorCount = distributionCollection_.getSize();
  const UnsignedInteger derivativeSize = derivativeIndices.getSize();
  // No derivative: the joint CDF
  if (derivativeSize == 0)
  {
    Scalar logCDF = 0.0;
    for (UnsignedInteger c = 0; c < factorCount; ++c)
    {
      const Scalar factorLogCDF = computeLocalLogCDF(c, reducePoint(point, c));
      if (factorLogCDF == -SpecFunc::Infinity) return -SpecFunc::Infinity;
      logCDF += factorLogCDF;
    }
    return logCDF;
  }
  Indices derivative(dimension, 0);
  for (UnsignedInteger k = 0; k < derivativeSize; ++k)
  {
    const UnsignedInteger index = derivativeIndices[k];
    if (index >= dimension) throw InvalidArgumentException(HERE) << "Error: the derivative index " << index << " is not in the range [0, " << dimension - 1 << "]";
    derivative[index] = 1;
  }
  const UnsignedInteger maximumTerms = ResourceMap::GetAsUnsignedInteger("CumulativeDistributionNetwork-MaximumTerms");
  // Local points, local derivative tables and constant factors
  Collection<Point> localPoints(factorCount);
  Collection<Indices> forcedPositions(factorCount);
  Collection<Indices> freePositions(factorCount);
  Collection<Point> tables(factorCount);
  Indices active(factorCount, 0);
  Scalar constantLog = 0.0;
  for (UnsignedInteger c = 0; c < factorCount; ++c)
  {
    localPoints[c] = reducePoint(point, c);
    Indices forced(0);
    Indices free(0);
    const Indices & variables = factorVariables_[c];
    for (UnsignedInteger j = 0; j < variables.getSize(); ++j)
    {
      const UnsignedInteger variable = variables[j];
      if (!derivative[variable]) continue;
      if (variableMultiplicity_[variable] == 1) forced.add(j);
      else free.add(j);
    }
    forcedPositions[c] = forced;
    freePositions[c] = free;
    if ((forced.getSize() == 0) && (free.getSize() == 0))
    {
      // This factor is only evaluated, not differentiated: it is a constant
      const Scalar factorLogCDF = computeLocalLogCDF(c, localPoints[c]);
      if (factorLogCDF == -SpecFunc::Infinity) return -SpecFunc::Infinity;
      constantLog += factorLogCDF;
      continue;
    }
    active[c] = 1;
    const UnsignedInteger freeSize = free.getSize();
    if ((freeSize >= std::numeric_limits<UnsignedInteger>::digits) || ((UnsignedInteger(1) << freeSize) > maximumTerms))
      throw NotYetImplementedException(HERE) << "Error: the factor " << c << " has " << freeSize << " free variables, above CumulativeDistributionNetwork-MaximumTerms=" << maximumTerms;
    const UnsignedInteger tableSize = UnsignedInteger(1) << freeSize;
    tables[c].resize(tableSize);
    for (UnsignedInteger mask = 0; mask < tableSize; ++mask)
    {
      Indices positions(forced);
      for (UnsignedInteger b = 0; b < freeSize; ++b)
        if (mask & (UnsignedInteger(1) << b)) positions.add(free[b]);
      tables[c][mask] = computeLocalPartialLog(c, localPoints[c], positions);
    }
  }
  // Position of the free variables in the local derivative tables. The
  // number of free variables is the "no bit" sentinel.
  Collection<Indices> freeBit(factorCount);
  for (UnsignedInteger c = 0; c < factorCount; ++c)
  {
    if (!active[c]) continue;
    freeBit[c] = Indices(factorVariables_[c].getSize(), freePositions[c].getSize());
    for (UnsignedInteger b = 0; b < freePositions[c].getSize(); ++b)
      freeBit[c][freePositions[c][b]] = b;
  }
  // Connected components of the factor graph restricted to derivative variables
  Indices root(factorCount);
  for (UnsignedInteger c = 0; c < factorCount; ++c) root[c] = c;
  const auto findRoot = [&root](UnsignedInteger c)
  {
    while (root[c] != c)
    {
      root[c] = root[root[c]];
      c = root[c];
    }
    return c;
  };
  for (UnsignedInteger k = 0; k < derivativeSize; ++k)
  {
    const UnsignedInteger variable = derivativeIndices[k];
    if (variableMultiplicity_[variable] < 2) continue;
    const Indices & factors = variableFactors_[variable];
    for (UnsignedInteger incidence = 1; incidence < factors.getSize(); ++incidence)
    {
      const UnsignedInteger a = findRoot(factors[0]);
      const UnsignedInteger b = findRoot(factors[incidence]);
      if (a != b) root[b] = a;
    }
  }
  Collection<Indices> groups(factorCount);
  for (UnsignedInteger c = 0; c < factorCount; ++c)
    if (active[c]) groups[findRoot(c)].add(c);
  // Connected components of the factor graph are independent: their
  // derivatives multiply, hence their logarithms add.
  Scalar totalLog = 0.0;
  for (UnsignedInteger groupIndex = 0; groupIndex < factorCount; ++groupIndex)
  {
    const Indices & group = groups[groupIndex];
    if (group.isEmpty()) continue;
    // Variables of the induced component
    Indices inGroupVariable(dimension, 0);
    Indices groupVariables(0);
    UnsignedInteger edges = 0;
    for (UnsignedInteger g = 0; g < group.getSize(); ++g)
    {
      const Indices & variables = factorVariables_[group[g]];
      for (UnsignedInteger j = 0; j < variables.getSize(); ++j)
      {
        const UnsignedInteger variable = variables[j];
        if (derivative[variable] && !inGroupVariable[variable])
        {
          inGroupVariable[variable] = 1;
          groupVariables.add(variable);
          edges += variableMultiplicity_[variable];
        }
      }
    }
    const Bool isTree = (edges + 1 == group.getSize() + groupVariables.getSize());
    Scalar groupLog = -SpecFunc::Infinity;
    if (isTree)
    {
      // Root the tree at the first factor and compute bottom-up messages.
      // Parent indices use the dimension (resp. the factor count) as the
      // "no parent" sentinel; levels are read only for visited nodes.
      Indices factorParentVariable(factorCount, dimension);
      Indices variableParentFactor(dimension, factorCount);
      Indices factorLevel(factorCount);
      Indices variableLevel(dimension);
      Indices factorVisited(factorCount, 0);
      Indices variableVisited(dimension, 0);
      struct TraversalNode
      {
        TraversalNode()
          : isVariable_(false)
          , index_(0)
        {}
        TraversalNode(const Bool isVariable, const UnsignedInteger index)
          : isVariable_(isVariable)
          , index_(index)
        {}
        Bool isVariable_;
        UnsignedInteger index_;
      };
      Collection<TraversalNode> order;
      Indices factorQueue(0);
      const UnsignedInteger rootFactor = group[0];
      factorVisited[rootFactor] = 1;
      factorLevel[rootFactor] = 0;
      order.add(TraversalNode(false, rootFactor));
      factorQueue.add(rootFactor);
      for (UnsignedInteger queueIndex = 0; queueIndex < factorQueue.getSize(); ++queueIndex)
      {
        const UnsignedInteger c = factorQueue[queueIndex];
        const Indices & variables = factorVariables_[c];
        for (UnsignedInteger j = 0; j < variables.getSize(); ++j)
        {
          const UnsignedInteger variable = variables[j];
          if (!derivative[variable] || variableVisited[variable]) continue;
          variableVisited[variable] = 1;
          variableLevel[variable] = factorLevel[c] + 1;
          variableParentFactor[variable] = c;
          order.add(TraversalNode(true, variable));
          const Indices & factors = variableFactors_[variable];
          for (UnsignedInteger incidence = 0; incidence < factors.getSize(); ++incidence)
          {
            const UnsignedInteger other = factors[incidence];
            if ((other == c) || factorVisited[other]) continue;
            factorVisited[other] = 1;
            factorLevel[other] = variableLevel[variable] + 1;
            factorParentVariable[other] = variable;
            order.add(TraversalNode(false, other));
            factorQueue.add(other);
          }
        }
      }
      Point logFactorOut(factorCount, 0.0);
      Point logFactorIn(factorCount, 0.0);
      Point logVariableOut(dimension, 0.0);
      Point logVariableIn(dimension, 0.0);
      for (Collection<TraversalNode>::const_reverse_iterator it = order.rbegin(); it != order.rend(); ++it)
      {
        if (it->isVariable_)
        {
          // Variable to its parent factor: in when differentiated in the parent, out otherwise
          const UnsignedInteger variable = it->index_;
          const Indices & factors = variableFactors_[variable];
          Point outs(0);
          Point ins(0);
          for (UnsignedInteger incidence = 0; incidence < factors.getSize(); ++incidence)
          {
            const UnsignedInteger c = factors[incidence];
            if (c == variableParentFactor[variable]) continue;
            outs.add(logFactorOut[c]);
            ins.add(logFactorIn[c]);
          }
          if (outs.isEmpty())
          {
            logVariableIn[variable] = 0.0;
            logVariableOut[variable] = -SpecFunc::Infinity;
          }
          else
          {
            Scalar sumOut = 0.0;
            for (UnsignedInteger child = 0; child < outs.getSize(); ++child) sumOut += outs[child];
            logVariableIn[variable] = sumOut;
            Scalar total = -SpecFunc::Infinity;
            for (UnsignedInteger chosen = 0; chosen < outs.getSize(); ++chosen)
            {
              Scalar term = ins[chosen];
              for (UnsignedInteger child = 0; child < outs.getSize(); ++child)
                if (child != chosen) term += outs[child];
              total = SpecFunc::LogSumExp(total, term);
            }
            logVariableOut[variable] = total;
          }
        }
        else
        {
          // Factor to its parent variable
          const UnsignedInteger c = it->index_;
          const Indices & variables = factorVariables_[c];
          Indices childVariables(0);
          Indices childBits(0);
          const UnsignedInteger noBit = freePositions[c].getSize();
          for (UnsignedInteger j = 0; j < variables.getSize(); ++j)
          {
            const UnsignedInteger variable = variables[j];
            if (!derivative[variable]) continue;
            if (variable == factorParentVariable[c]) continue;
            if (variableMultiplicity_[variable] == 1) continue;
            const UnsignedInteger bit = freeBit[c][j];
            if (bit == noBit) throw InternalException(HERE) << "Error: inconsistent free variable cache";
            childVariables.add(variable);
            childBits.add(bit);
          }
          const UnsignedInteger parentVariable = factorParentVariable[c];
          const Bool hasParent = (parentVariable != dimension);
          UnsignedInteger parentBit = 0;
          if (hasParent)
          {
            parentBit = noBit;
            for (UnsignedInteger j = 0; j < variables.getSize(); ++j)
              if (variables[j] == parentVariable)
              {
                parentBit = freeBit[c][j];
                break;
              }
            if (parentBit == noBit) throw InternalException(HERE) << "Error: inconsistent parent variable cache";
          }
          const UnsignedInteger childCount = childVariables.getSize();
          if ((childCount >= std::numeric_limits<UnsignedInteger>::digits) || ((UnsignedInteger(1) << childCount) > maximumTerms))
            throw NotYetImplementedException(HERE) << "Error: the factor " << c << " has " << childCount << " free children, above CumulativeDistributionNetwork-MaximumTerms=" << maximumTerms;
          const UnsignedInteger subsetCount = UnsignedInteger(1) << childCount;
          Scalar logOut = -SpecFunc::Infinity;
          Scalar logIn = -SpecFunc::Infinity;
          for (UnsignedInteger subset = 0; subset < subsetCount; ++subset)
          {
            UnsignedInteger tableMask = 0;
            Scalar prefix = 0.0;
            for (UnsignedInteger b = 0; b < childCount; ++b)
            {
              if (subset & (UnsignedInteger(1) << b))
              {
                tableMask |= UnsignedInteger(1) << childBits[b];
                prefix += logVariableIn[childVariables[b]];
              }
              else
                prefix += logVariableOut[childVariables[b]];
            }
            logOut = SpecFunc::LogSumExp(logOut, tables[c][tableMask] + prefix);
            if (hasParent)
              logIn = SpecFunc::LogSumExp(logIn, tables[c][tableMask | (UnsignedInteger(1) << parentBit)] + prefix);
          }
          if (hasParent)
          {
            logFactorOut[c] = logOut;
            logFactorIn[c] = logIn;
          }
          else
            groupLog = logOut;
        }
      }
    }
    else
    {
      // Cyclic component: explicit enumeration of the assignments
      Indices freeVariables(0);
      for (UnsignedInteger position = 0; position < groupVariables.getSize(); ++position)
        if (variableMultiplicity_[groupVariables[position]] > 1) freeVariables.add(groupVariables[position]);
      UnsignedInteger termCount = 1;
      for (UnsignedInteger position = 0; position < freeVariables.getSize(); ++position)
      {
        const UnsignedInteger multiplicity = variableMultiplicity_[freeVariables[position]];
        if (termCount > maximumTerms / multiplicity)
          throw NotYetImplementedException(HERE) << "Error: the cyclic component requires more than CumulativeDistributionNetwork-MaximumTerms=" << maximumTerms << " terms";
        termCount *= multiplicity;
      }
      Collection<Indices> choices(freeVariables.getSize());
      Collection<Indices> choiceBits(freeVariables.getSize());
      for (UnsignedInteger position = 0; position < freeVariables.getSize(); ++position)
      {
        const UnsignedInteger variable = freeVariables[position];
        const Indices & factors = variableFactors_[variable];
        for (UnsignedInteger incidence = 0; incidence < factors.getSize(); ++incidence)
        {
          const UnsignedInteger c = factors[incidence];
          const Indices & variables = factorVariables_[c];
          const UnsignedInteger noBit = freePositions[c].getSize();
          for (UnsignedInteger j = 0; j < variables.getSize(); ++j)
            if (variables[j] == variable)
            {
              const UnsignedInteger bit = freeBit[c][j];
              if (bit == noBit) throw InternalException(HERE) << "Error: inconsistent free variable cache";
              choices[position].add(c);
              choiceBits[position].add(bit);
              break;
            }
        }
      }
      Indices counter(freeVariables.getSize(), 0);
      Indices masks(factorCount, 0);
      if (freeVariables.isEmpty())
      {
        Scalar term = 0.0;
        for (UnsignedInteger g = 0; g < group.getSize(); ++g) term += tables[group[g]][0];
        groupLog = term;
      }
      else
        while (true)
        {
          for (UnsignedInteger g = 0; g < group.getSize(); ++g) masks[group[g]] = 0;
          for (UnsignedInteger position = 0; position < freeVariables.getSize(); ++position)
          {
            const UnsignedInteger c = choices[position][counter[position]];
            masks[c] |= UnsignedInteger(1) << choiceBits[position][counter[position]];
          }
          Scalar term = 0.0;
          for (UnsignedInteger g = 0; g < group.getSize(); ++g) term += tables[group[g]][masks[group[g]]];
          groupLog = SpecFunc::LogSumExp(groupLog, term);
          UnsignedInteger position = 0;
          while (position < counter.getSize())
          {
            ++counter[position];
            if (counter[position] < choices[position].getSize()) break;
            counter[position] = 0;
            ++position;
          }
          if (position == counter.getSize()) break;
        }
    }
    totalLog += groupLog;
  }
  if (totalLog == -SpecFunc::Infinity) return -SpecFunc::Infinity;
  return totalLog + constantLog;
}

/* Compute the log-PDF (continuous) or log-PMF (integral) */
Scalar CumulativeDistributionNetwork::computeLogPDFKernel(const Point & point,
    const Bool useDifferences) const
{
  Indices all(getDimension());
  all.fill();
  return computePartialDerivativeLog(point, all, useDifferences);
}

/* Get the PDF of the CumulativeDistributionNetwork */
Scalar CumulativeDistributionNetwork::computePDF(const Point & point) const
{
  return std::exp(computeLogPDF(point));
}

/* Get the log-PDF of the CumulativeDistributionNetwork */
Scalar CumulativeDistributionNetwork::computeLogPDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if (isContinuous()) return computeLogPDFKernel(point, false);
  if (isIntegral())
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
      if (std::abs(point[j] - std::round(point[j])) > supportEpsilon_) return -SpecFunc::Infinity;
    return computeLogPDFKernel(point, true);
  }
  throw NotYetImplementedException(HERE) << "Error: the PDF of a cumulative distribution network mixing continuous and discrete factors is not implemented";
}

/* Get the CDF of the CumulativeDistributionNetwork */
Scalar CumulativeDistributionNetwork::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  Scalar logCDF = 0.0;
  for (UnsignedInteger c = 0; c < size; ++c)
  {
    const Scalar factorLogCDF = computeLocalLogCDF(c, reducePoint(point, c));
    if (factorLogCDF == -SpecFunc::Infinity) return 0.0;
    logCDF += factorLogCDF;
  }
  return std::min(1.0, std::exp(logCDF));
}

/* Get the CDF of Xi | X1, ..., Xi-1 */
Scalar CumulativeDistributionNetwork::computeConditionalCDF(const Scalar xj, const Point & xcond) const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger conditioningDimension = xcond.getDimension();
  if (conditioningDimension >= dimension) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (conditioningDimension == 0) return getMarginal(UnsignedInteger(0)).computeCDF(Point(1, xj));
  if (hasIndependentCopula()) return getMarginal(conditioningDimension).computeCDF(Point(1, xj));
  Bool useDifferences = false;
  if (isContinuous()) useDifferences = false;
  else if (isIntegral()) useDifferences = true;
  else throw NotYetImplementedException(HERE) << "Error: conditional CDF of a cumulative distribution network mixing continuous and discrete factors is not implemented";
  Point point(dimension, SpecFunc::Infinity);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i) point[i] = xcond[i];
  point[conditioningDimension] = xj;
  Indices derivative(conditioningDimension);
  derivative.fill();
  const Scalar logNumerator = computePartialDerivativeLog(point, derivative, useDifferences);
  if (logNumerator == -SpecFunc::Infinity) return 0.0;
  point[conditioningDimension] = SpecFunc::Infinity;
  const Scalar logDenominator = computePartialDerivativeLog(point, derivative, useDifferences);
  if (logDenominator == -SpecFunc::Infinity) return 0.0;
  const Scalar cdf = std::exp(logNumerator - logDenominator);
  return std::min(1.0, std::max(0.0, cdf));
}

/* Get the PDF of Xi | X1, ..., Xi-1 */
Scalar CumulativeDistributionNetwork::computeConditionalPDF(const Scalar xj, const Point & xcond) const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger conditioningDimension = xcond.getDimension();
  if (conditioningDimension >= dimension) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (conditioningDimension == 0) return getMarginal(UnsignedInteger(0)).computePDF(Point(1, xj));
  if (hasIndependentCopula()) return getMarginal(conditioningDimension).computePDF(Point(1, xj));
  Bool useDifferences = false;
  if (isContinuous()) useDifferences = false;
  else if (isIntegral()) useDifferences = true;
  else throw NotYetImplementedException(HERE) << "Error: conditional PDF of a cumulative distribution network mixing continuous and discrete factors is not implemented";
  if (useDifferences && (std::abs(xj - std::round(xj)) > supportEpsilon_)) return 0.0;
  Point point(dimension, SpecFunc::Infinity);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i) point[i] = xcond[i];
  point[conditioningDimension] = xj;
  Indices derivative(conditioningDimension + 1);
  derivative.fill();
  const Scalar logNumerator = computePartialDerivativeLog(point, derivative, useDifferences);
  if (logNumerator == -SpecFunc::Infinity) return 0.0;
  point[conditioningDimension] = SpecFunc::Infinity;
  Indices conditioningDerivative(conditioningDimension);
  conditioningDerivative.fill();
  const Scalar logDenominator = computePartialDerivativeLog(point, conditioningDerivative, useDifferences);
  if (logDenominator == -SpecFunc::Infinity) return 0.0;
  return std::exp(logNumerator - logDenominator);
}

/* Quantile computation for dimension=1.
   The generic implementation relies on the range bounds. When a variable is
   shared by factors with different supports, the range may be unbounded while
   the CDF is numerically flat far from the origin: the generic Brent solver
   then returns a meaningless huge value. Expand a finite bracket around the
   origin instead, as done for the conditional quantiles. */
Scalar CumulativeDistributionNetwork::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (dimension_ != 1)
    throw InvalidDimensionException(HERE) << "Error: computeScalarQuantile is only defined for dimension=1";
  if (!((prob >= 0.0) && (prob <= 1.0)))
    throw InvalidArgumentException(HERE) << "Error: computeScalarQuantile expected prob to belong to [0,1], but is " << prob;
  const Scalar p = tail ? 1.0 - prob : prob;
  if (p <= 0.0) return getRange().getLowerBound()[0];
  if (p >= 1.0) return getRange().getUpperBound()[0];
  if (isIntegral())
  {
    // Smallest integer with F(x) >= p
    Scalar lower = 0.0;
    Scalar upper = 0.0;
    Scalar width = 1.0;
    UnsignedInteger iteration = 0;
    while ((computeCDF(Point(1, upper)) < p) && (iteration < quantileIterations_))
    {
      lower = upper;
      upper += width;
      width *= 2.0;
      ++iteration;
    }
    width = 1.0;
    iteration = 0;
    while ((computeCDF(Point(1, lower)) >= p) && (iteration < quantileIterations_))
    {
      upper = lower;
      lower -= width;
      width *= 2.0;
      ++iteration;
    }
    if (!(computeCDF(Point(1, lower)) < p) || !(computeCDF(Point(1, upper)) >= p))
      return DistributionImplementation::computeScalarQuantile(p);
    while (upper - lower > 1.0)
    {
      const Scalar middle = std::floor(0.5 * (lower + upper));
      if ((middle <= lower) || (middle >= upper)) break;
      if (computeCDF(Point(1, middle)) >= p) upper = middle;
      else lower = middle;
    }
    return upper;
  }
  Scalar lower = -1.0;
  Scalar upper = 1.0;
  Scalar width = 1.0;
  UnsignedInteger iteration = 0;
  while ((computeCDF(Point(1, lower)) > p) && (iteration < quantileIterations_))
  {
    lower -= width;
    width *= 2.0;
    ++iteration;
  }
  width = 1.0;
  iteration = 0;
  while ((computeCDF(Point(1, upper)) < p) && (iteration < quantileIterations_))
  {
    upper += width;
    width *= 2.0;
    ++iteration;
  }
  for (UnsignedInteger iterationIndex = 0; iterationIndex < quantileIterations_; ++iterationIndex)
  {
    const Scalar middle = 0.5 * (lower + upper);
    if ((middle <= lower) || (middle >= upper)) break;
    if (computeCDF(Point(1, middle)) < p) lower = middle;
    else upper = middle;
  }
  return 0.5 * (lower + upper);
}

/* Get the quantile of Xi | X1, ..., Xi-1 */
Scalar CumulativeDistributionNetwork::computeConditionalQuantile(const Scalar p, const Point & xcond) const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger conditioningDimension = xcond.getDimension();
  if (conditioningDimension >= dimension) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (!((p >= 0.0) && (p <= 1.0))) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level q=" << p << " outside of [0, 1]";
  const Distribution marginal(getMarginal(conditioningDimension));
  if ((conditioningDimension == 0) || hasIndependentCopula() || (p <= 0.0) || (p >= 1.0))
    return marginal.computeScalarQuantile(p);
  // Mixed continuous/discrete networks have no conditional CDF, so they
  // cannot enter the continuous bisection below.
  if (!isContinuous() && !isIntegral())
    throw NotYetImplementedException(HERE) << "Error: conditional quantile of a cumulative distribution network mixing continuous and discrete factors is not implemented";
  if (isIntegral())
  {
    // Smallest integer with F(x | xcond) >= p
    Scalar lower = 0.0;
    Scalar upper = 0.0;
    Scalar width = 1.0;
    UnsignedInteger iteration = 0;
    while ((computeConditionalCDF(upper, xcond) < p) && (iteration < quantileIterations_))
    {
      lower = upper;
      upper += width;
      width *= 2.0;
      ++iteration;
    }
    width = 1.0;
    iteration = 0;
    while ((computeConditionalCDF(lower, xcond) >= p) && (iteration < quantileIterations_))
    {
      upper = lower;
      lower -= width;
      width *= 2.0;
      ++iteration;
    }
    if (!(computeConditionalCDF(lower, xcond) < p) || !(computeConditionalCDF(upper, xcond) >= p))
      return marginal.computeScalarQuantile(p);
    while (upper - lower > 1.0)
    {
      const Scalar middle = std::floor(0.5 * (lower + upper));
      if ((middle <= lower) || (middle >= upper)) break;
      if (computeConditionalCDF(middle, xcond) >= p) upper = middle;
      else lower = middle;
    }
    return upper;
  }
  // Continuous case: expand a bracket around the origin, then bisect
  Scalar lower = -1.0;
  Scalar upper = 1.0;
  Scalar width = 1.0;
  UnsignedInteger iteration = 0;
  while ((computeConditionalCDF(lower, xcond) > p) && (iteration < quantileIterations_))
  {
    lower -= width;
    width *= 2.0;
    ++iteration;
  }
  width = 1.0;
  iteration = 0;
  while ((computeConditionalCDF(upper, xcond) < p) && (iteration < quantileIterations_))
  {
    upper += width;
    width *= 2.0;
    ++iteration;
  }
  if (!(computeConditionalCDF(lower, xcond) <= p) || !(computeConditionalCDF(upper, xcond) >= p))
    return marginal.computeScalarQuantile(p);
  for (UnsignedInteger iterationIndex = 0; iterationIndex < quantileIterations_; ++iterationIndex)
  {
    const Scalar middle = 0.5 * (lower + upper);
    if ((middle <= lower) || (middle >= upper)) break;
    if (computeConditionalCDF(middle, xcond) < p) lower = middle;
    else upper = middle;
  }
  return 0.5 * (lower + upper);
}

/* Get the i-th marginal distribution */
Distribution CumulativeDistributionNetwork::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be lower than the dimension=" << dimension;
  return getMarginal(Indices(1, i));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution CumulativeDistributionNetwork::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger indexSize = indices.getSize();
  if (indexSize == 0) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must not be empty";
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be lower than the dimension=" << dimension << " and must be different";
  Indices full(dimension);
  full.fill();
  if (indices == full) return clone();
  // The marginal of a CDN is a CDN: each factor is replaced by its marginal
  // over the selected variables, and factors with no selected variable vanish.
  DistributionCollection contributors(0);
  Collection<Indices> marginalGraph(0);
  const UnsignedInteger factorCount = distributionCollection_.getSize();
  for (UnsignedInteger c = 0; c < factorCount; ++c)
  {
    const Indices & variables = factorVariables_[c];
    Indices localIndices(0);
    Indices graphPart(0);
    for (UnsignedInteger k = 0; k < indexSize; ++k)
      for (UnsignedInteger j = 0; j < variables.getSize(); ++j)
        if (variables[j] == indices[k])
        {
          localIndices.add(j);
          graphPart.add(k);
          break;
        }
    if (localIndices.getSize() > 0)
    {
      contributors.add(distributionCollection_[c].getMarginal(localIndices));
      marginalGraph.add(graphPart);
    }
  }
  if (contributors.getSize() == 0) throw InternalException(HERE) << "Error: no factor contributes to the marginal distribution";
  Description description(indexSize);
  const Description originalDescription(getDescription());
  for (UnsignedInteger k = 0; k < indexSize; ++k)
    description[k] = originalDescription[indices[k]];
  if (contributors.getSize() == 1)
  {
    Distribution result(contributors[0]);
    result.setDescription(description);
    return result;
  }
  Distribution result(new CumulativeDistributionNetwork(contributors, BipartiteGraph(IndicesCollectionImplementation(marginalGraph))));
  result.setDescription(description);
  return result;
}

/* Check if the distribution is elliptical */
Bool CumulativeDistributionNetwork::isElliptical() const
{
  return (distributionCollection_.getSize() == 1) && distributionCollection_[0].isElliptical();
}

/* Check if the distribution is continuous */
Bool CumulativeDistributionNetwork::isContinuous() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) if (!distributionCollection_[i].isContinuous()) return false;
  return true;
}

/* Check if the distribution is discrete */
Bool CumulativeDistributionNetwork::isDiscrete() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) if (!distributionCollection_[i].isDiscrete()) return false;
  return true;
}

/* Check if the distribution is integral */
Bool CumulativeDistributionNetwork::isIntegral() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) if (!distributionCollection_[i].isIntegral()) return false;
  return true;
}

/* Tell if the distribution has independent copula */
Bool CumulativeDistributionNetwork::hasIndependentCopula() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    if (!distributionCollection_[i].hasIndependentCopula()) return false;
  return true;
}

Point CumulativeDistributionNetwork::getParameter() const
{
  Point point;
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    point.add(distributionCollection_[i].getParameter());
  return point;
}

void CumulativeDistributionNetwork::setParameter(const Point & parameter)
{
  // Check the size before updating any distribution: no partial update on error,
  // and no silently ignored extra values
  if (parameter.getSize() != getParameterDimension()) throw InvalidArgumentException(HERE) << "Error: expected " << getParameterDimension() << " values, got " << parameter.getSize();
  const UnsignedInteger size = distributionCollection_.getSize();
  DistributionCollection newDistributions(size);
  UnsignedInteger globalIndex = 0;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    // Work on a copy: thanks to copy-on-write the stored distribution is left unchanged if a later distribution rejects its slice
    Distribution distribution(distributionCollection_[i]);
    const UnsignedInteger atomParametersDimension = distribution.getParameterDimension();
    Point newParameter(atomParametersDimension);
    std::copy(parameter.begin() + globalIndex, parameter.begin() + globalIndex + atomParametersDimension, newParameter.begin());
    distribution.setParameter(newParameter);
    newDistributions[i] = distribution;
    globalIndex += atomParametersDimension;
  } // atoms
  // All slices accepted: commit the staged updates
  setDistributionCollection(newDistributions);
}

Description CumulativeDistributionNetwork::getParameterDescription() const
{
  Description description;
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Description parameterDescription(distributionCollection_[i].getParameterDescription());
    const UnsignedInteger parameterDimension = parameterDescription.getSize();
    for (UnsignedInteger j = 0; j < parameterDimension; ++ j)
      description.add(OSS() << parameterDescription[j] << "_distribution_" << i);
  }
  return description;
}

/* Method save() stores the object through the StorageManager */
void CumulativeDistributionNetwork::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "distributionCollection_", distributionCollection_ );
  adv.saveAttribute( "graph_", graph_ );
}

/* Method load() reloads the object from the StorageManager */
void CumulativeDistributionNetwork::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "distributionCollection_", distributionCollection_ );
  adv.loadAttribute( "graph_", graph_ );
  // The incidence caches are not serialized: rebuild them from the persistent
  // attributes, so that archives written by previous versions load into the
  // canonical state.
  setDistributionCollection(distributionCollection_);
}

END_NAMESPACE_OPENTURNS
