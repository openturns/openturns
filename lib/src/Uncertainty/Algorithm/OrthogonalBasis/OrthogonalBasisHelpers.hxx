//                                               -*- C++ -*-
/**
 *  @brief Shared helpers for the orthogonal basis integration rules
 *
 *  This private header is shared by BasisOrthonormalizationAlgorithm and
 *  GaussLPQuadrature. The default integration rules and their validation are
 *  common to both classes, so the helpers are defined once. They are inline
 *  because the header is included by several translation units, and the
 *  include guard keeps a single definition in unity builds.
 *
 *  Copyright 2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ORTHOGONALBASISHELPERS_HXX
#define OPENTURNS_ORTHOGONALBASISHELPERS_HXX

#include <algorithm>
#include <cmath>
#include "openturns/OTtypes.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Function.hxx"
#include "openturns/Normal.hxx"
#include "openturns/GaussProductExperiment.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace OrthogonalBasisHelpers
{

inline Bool IsFinitePoint(const Point & point)
{
  for (UnsignedInteger i = 0; i < point.getDimension(); ++i)
    if (!std::isfinite(point[i])) return false;
  return true;
}

inline Bool IsFiniteSample(const Sample & sample)
{
  for (UnsignedInteger i = 0; i < sample.getSize(); ++i)
    if (!IsFinitePoint(sample[i])) return false;
  return true;
}

inline Bool IsFiniteRule(const Sample & nodes,
                         const Point & weights)
{
  if ((nodes.getSize() == 0) || (nodes.getSize() != weights.getDimension())) return false;
  if (!IsFiniteSample(nodes)) return false;
  for (UnsignedInteger i = 0; i < weights.getDimension(); ++i)
    if (!std::isfinite(weights[i]) || (weights[i] < 0.0)) return false;
  return true;
}

/* True when base^exponent does not exceed limit, computed without overflow. */
inline Bool PowerWithinBudget(const UnsignedInteger base,
                              const UnsignedInteger exponent,
                              const UnsignedInteger limit)
{
  if (base == 0) return false;
  UnsignedInteger nodeNumber = 1;
  for (UnsignedInteger j = 0; j < exponent; ++j)
  {
    if (nodeNumber > limit / base) return false;
    nodeNumber *= base;
  }
  return true;
}

/* Largest marginal order whose full tensor rule stays within maximumNodes.
   The requested order is kept when the tensor rule is small enough, so that
   low-dimensional rules retain their full accuracy. Returns 0 when even the
   order-2 tensor rule exceeds the budget: the caller then falls back to
   another rule instead of materializing an unattainable tensor product. */
inline UnsignedInteger ComputeMarginalSize(const UnsignedInteger requestedSize,
                                           const UnsignedInteger dimension,
                                           const UnsignedInteger maximumNodes)
{
  if (maximumNodes == 0) throw InvalidArgumentException(HERE) << "Error: the maximum number of tensor nodes must be > 0";
  const UnsignedInteger requested = std::max<UnsignedInteger>(2, requestedSize);
  if (!PowerWithinBudget(2, dimension, maximumNodes)) return 0;
  if (PowerWithinBudget(requested, dimension, maximumNodes)) return requested;
  // 2 is attainable and requested is not: binary search the largest order.
  UnsignedInteger lower = 2;
  UnsignedInteger upper = requested;
  while (lower + 1 < upper)
  {
    const UnsignedInteger middle = lower + (upper - lower) / 2;
    if (PowerWithinBudget(middle, dimension, maximumNodes)) lower = middle;
    else upper = middle;
  }
  return lower;
}

/* Tensor Gauss product rule built directly from the measure. The marginal
   order is halved on failure: heavy tails can overflow the recurrence used to
   build the orthogonal polynomials. */
inline Bool BuildGaussProductRule(const Distribution & distribution,
                                  const UnsignedInteger requestedSize,
                                  const UnsignedInteger maximumNodes,
                                  Sample & nodes,
                                  Point & weights)
{
  if (!distribution.hasIndependentCopula()) return false;
  const UnsignedInteger dimension = distribution.getDimension();
  UnsignedInteger marginalSize = ComputeMarginalSize(requestedSize, dimension, maximumNodes);
  if (marginalSize == 0) return false;
  while (true)
  {
    try
    {
      const GaussProductExperiment experiment(distribution, Indices(dimension, marginalSize));
      Point candidateWeights;
      const Sample candidateNodes(experiment.generateWithWeights(candidateWeights));
      if (IsFiniteRule(candidateNodes, candidateWeights))
      {
        nodes = candidateNodes;
        weights = candidateWeights;
        return true;
      }
    }
    catch (const Exception &)
    {
      // Try a smaller order.
    }
    if (marginalSize <= 2) break;
    marginalSize /= 2;
  }
  return false;
}

/* Measure-adapted rule obtained by transporting a standard normal Gauss
   product rule through the inverse iso-probabilistic transformation. The
   standard normal weights are also the weights of the transformed nodes. */
inline Bool BuildTransformedNormalRule(const Distribution & distribution,
                                       const UnsignedInteger requestedSize,
                                       const UnsignedInteger maximumNodes,
                                       Sample & nodes,
                                       Point & weights)
{
  const UnsignedInteger dimension = distribution.getDimension();
  const UnsignedInteger marginalSize = ComputeMarginalSize(requestedSize, dimension, maximumNodes);
  if (marginalSize == 0) return false;
  try
  {
    // The standard normal weights are valid for the transformed nodes only
    // when the standard space of the distribution is the standard normal
    // distribution: a non-Gaussian elliptical distribution preserves its
    // elliptical type in the standard space, so the inverse Nataf map
    // expects another measure than N(0, I).
    if (distribution.getStandardDistribution().getImplementation()->getClassName() != "Normal") return false;
    const Normal standardNormal(dimension);
    const GaussProductExperiment normalExperiment(standardNormal, Indices(dimension, marginalSize));
    Point candidateWeights;
    const Sample standardNodes(normalExperiment.generateWithWeights(candidateWeights));
    if (!IsFiniteRule(standardNodes, candidateWeights)) return false;
    const Function transformation(distribution.getInverseIsoProbabilisticTransformation());
    const Sample candidateNodes(transformation(standardNodes));
    if ((candidateNodes.getSize() != standardNodes.getSize())
        || (candidateNodes.getDimension() != dimension)
        || !IsFiniteSample(candidateNodes)) return false;
    nodes = candidateNodes;
    weights = candidateWeights;
    return true;
  }
  catch (const Exception &)
  {
    return false;
  }
}

/* Last-resort rule: Gauss-Legendre on the finite numerical range of the
   measure, with the density and the support volume folded into the weights.
   This rule is unsuitable for a dependent elliptical measure whose standard
   distribution is not the standard normal one: truncating the heavy tails
   loses the moments to cancellation in the folded density. */
inline Bool HasUsableGaussLegendreFallback(const Distribution & distribution)
{
  if (!distribution.isElliptical() || distribution.hasIndependentCopula()) return true;
  try
  {
    return distribution.getStandardDistribution().getImplementation()->getClassName() == "Normal";
  }
  catch (const Exception &)
  {
    return true;
  }
}

inline Bool BuildGaussLegendreRule(const Distribution & distribution,
                                   const UnsignedInteger requestedSize,
                                   const UnsignedInteger maximumNodes,
                                   Sample & nodes,
                                   Point & weights)
{
  const UnsignedInteger dimension = distribution.getDimension();
  const Interval support(distribution.getRange());
  const Scalar volume = support.getVolume();
  if (!(volume > 0.0) || !std::isfinite(volume)) return false;
  const Point a(support.getLowerBound());
  const Point b(support.getUpperBound());
  const UnsignedInteger marginalSize = ComputeMarginalSize(requestedSize, dimension, maximumNodes);
  if (marginalSize == 0) return false;
  try
  {
    const GaussLegendre reference(Indices(dimension, marginalSize));
    const Sample referenceNodes(reference.getNodes());
    const Point referenceWeights(reference.getWeights());
    if ((referenceNodes.getSize() == 0) || (referenceNodes.getSize() != referenceWeights.getDimension())) return false;
    Sample candidateNodes(referenceNodes.getSize(), dimension);
    for (UnsignedInteger i = 0; i < referenceNodes.getSize(); ++i)
    {
      Point x(dimension);
      for (UnsignedInteger j = 0; j < dimension; ++j)
        x[j] = a[j] + referenceNodes(i, j) * (b[j] - a[j]);
      candidateNodes[i] = x;
    }
    // Evaluate the density in one batch: a point-by-point evaluation is much
    // slower for large tensor rules.
    const Sample density(distribution.computePDF(candidateNodes));
    if (density.getSize() != candidateNodes.getSize()) return false;
    Point candidateWeights(referenceNodes.getSize());
    for (UnsignedInteger i = 0; i < referenceNodes.getSize(); ++i)
      candidateWeights[i] = referenceWeights[i] * volume * density(i, 0);
    if (!IsFiniteRule(candidateNodes, candidateWeights)) return false;
    nodes = candidateNodes;
    weights = candidateWeights;
    return true;
  }
  catch (const Exception &)
  {
    return false;
  }
}

} // namespace OrthogonalBasisHelpers

END_NAMESPACE_OPENTURNS

#endif // OPENTURNS_ORTHOGONALBASISHELPERS_HXX
