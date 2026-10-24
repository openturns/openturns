//                                               -*- C++ -*-
/**
 *  @brief Predictor-corrector continuation of a one dimensional fibre
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
#include <vector>
#include "openturns/Indices.hxx"
#include "openturns/LevelSetContinuation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SquareMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Parameter constructor */
LevelSetContinuation::LevelSetContinuation(const Function & function,
                                           const Function & density,
                                           const Interval & searchInterval,
                                           const Point & level)
  : function_(function)
  , density_(density)
  , searchInterval_(searchInterval)
  , level_(level)
{
  const UnsignedInteger inputDimension = function.getInputDimension();
  const UnsignedInteger outputDimension = function.getOutputDimension();
  if ((inputDimension < 2) || (outputDimension + 1 != inputDimension))
    throw InvalidArgumentException(HERE)
        << "Error: the continuation expects a map from R^n to R^(n-1) with n >= 2, here input dimension="
        << inputDimension << " and output dimension=" << outputDimension;
  if (searchInterval.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE)
        << "Error: the search interval dimension=" << searchInterval.getDimension()
        << " does not match the input dimension=" << inputDimension;
  if ((density.getInputDimension() != inputDimension)
      || (density.getOutputDimension() != 1))
    throw InvalidArgumentException(HERE)
        << "Error: the density must be a scalar function of the input dimension, here input dimension="
        << density.getInputDimension() << " and output dimension=" << density.getOutputDimension();
  if (level.getSize() != outputDimension)
    throw InvalidArgumentException(HERE)
        << "Error: the level dimension=" << level.getSize()
        << " does not match the output dimension=" << outputDimension;
}

/* Jacobian of the map at the given point */
Matrix LevelSetContinuation::getJacobian(const Point & point, Bool & available) const
{
  available = false;
  const UnsignedInteger inputDimension = function_.getInputDimension();
  const UnsignedInteger outputDimension = function_.getOutputDimension();
  Matrix jacobian;
  try
  {
    jacobian = function_.gradient(point);
    if ((jacobian.getNbRows() == inputDimension)
        && (jacobian.getNbColumns() == outputDimension))
    {
      available = true;
      return jacobian;
    }
  }
  catch (const Exception &)
  {
    // Fall through to finite differences
  }
  const Scalar step = std::sqrt(SpecFunc::ScalarEpsilon) * std::max(1.0, point.norm());
  jacobian = Matrix(inputDimension, outputDimension);
  Point plusPoint(point);
  Point minusPoint(point);
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    plusPoint[i] += step;
    minusPoint[i] -= step;
    Point plusValue;
    Point minusValue;
    try
    {
      plusValue = function_(plusPoint);
      minusValue = function_(minusPoint);
    }
    catch (const Exception &)
    {
      return Matrix();
    }
    if ((plusValue.getSize() != outputDimension)
        || (minusValue.getSize() != outputDimension))
      return Matrix();
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      jacobian(i, j) = (plusValue[j] - minusValue[j]) / (2.0 * step);
    plusPoint[i] = point[i];
    minusPoint[i] = point[i];
  }
  available = true;
  return jacobian;
}

/* Generalized cross product of the n - 1 columns of an n x (n - 1)
   Jacobian. The component i is the signed determinant of the matrix obtained
   by deleting row i. For n = 2 this is the rotated gradient, for n = 3 the
   usual cross product of the two gradient vectors, and for n = 4 the vector
   of signed 3 x 3 minors used by the co-area formula. */
Point LevelSetContinuation::computeCoAreaVector(const Matrix & jacobian)
{
  const UnsignedInteger inputDimension = jacobian.getNbRows();
  const UnsignedInteger outputDimension = jacobian.getNbColumns();
  if ((inputDimension < 2) || (outputDimension + 1 != inputDimension))
    return Point();
  Point coAreaVector(inputDimension);
  for (UnsignedInteger removedRow = 0; removedRow < inputDimension; ++removedRow)
  {
    SquareMatrix minorMatrix(outputDimension);
    UnsignedInteger minorRow = 0;
    for (UnsignedInteger row = 0; row < inputDimension; ++row)
    {
      if (row == removedRow)
        continue;
      for (UnsignedInteger column = 0; column < outputDimension; ++column)
        minorMatrix(minorRow, column) = jacobian(row, column);
      ++minorRow;
    }
    const Scalar sign = (removedRow % 2 == 0) ? 1.0 : -1.0;
    coAreaVector[removedRow] = sign * minorMatrix.computeDeterminant();
  }
  return coAreaVector;
}

/* Co-area factor of a one dimensional fibre */
Scalar LevelSetContinuation::computeCoAreaFactor(const Matrix & jacobian)
{
  return computeCoAreaVector(jacobian).norm();
}

/* Co-area factor of a one dimensional fibre */
Scalar LevelSetContinuation::getCoAreaFactor(const Matrix & jacobian) const
{
  return computeCoAreaFactor(jacobian);
}

/* Unit tangent of a one dimensional fibre */
Point LevelSetContinuation::getTangent(const Matrix & jacobian, Bool & available) const
{
  available = false;
  Point tangent(computeCoAreaVector(jacobian));
  const Scalar norm = tangent.norm();
  if (!(norm > 0.0) || !std::isfinite(norm))
    return Point();
  tangent /= norm;
  available = true;
  return tangent;
}

/* Project a point onto the fibre */
Bool LevelSetContinuation::project(Point & point) const
{
  const Scalar tolerance = 1.0e-12 * std::max(1.0, level_.norm());
  for (UnsignedInteger iteration = 0; iteration < 8; ++iteration)
  {
    Point residual;
    try
    {
      residual = function_(point) - level_;
    }
    catch (const Exception &)
    {
      return false;
    }
    if (residual.norm() <= tolerance)
      return true;
    Bool available = false;
    const Matrix jacobian(getJacobian(point, available));
    if (!available)
      return false;
    const UnsignedInteger outputDimension = residual.getSize();
    const UnsignedInteger inputDimension = point.getSize();
    SquareMatrix normal(outputDimension);
    Point rightHandSide(outputDimension);
    for (UnsignedInteger i = 0; i < outputDimension; ++i)
    {
      for (UnsignedInteger j = 0; j < outputDimension; ++j)
      {
        Scalar sum = 0.0;
        for (UnsignedInteger k = 0; k < inputDimension; ++k)
          sum += jacobian(k, i) * jacobian(k, j);
        normal(i, j) = sum;
      }
      rightHandSide[i] = residual[i];
    }
    Point parameterIncrement;
    try
    {
      parameterIncrement = normal.solveLinearSystem(rightHandSide);
    }
    catch (const Exception &)
    {
      return false;
    }
    Point correction(inputDimension, 0.0);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      for (UnsignedInteger j = 0; j < outputDimension; ++j)
        correction[i] += jacobian(i, j) * parameterIncrement[j];
    point -= correction;
  }
  Point residual;
  try
  {
    residual = function_(point) - level_;
  }
  catch (const Exception &)
  {
    return false;
  }
  return residual.norm() <= 10.0 * tolerance;
}

/* Right-hand side of the arc-length ODE */
Bool LevelSetContinuation::evaluateRHS(const Point & point,
                                       Point & tangent,
                                       Scalar & integrand,
                                       Bool & singular) const
{
  Bool available = false;
  const Matrix jacobian(getJacobian(point, available));
  if (!available)
    return false;
  const Scalar coAreaFactor = getCoAreaFactor(jacobian);
  if (!(coAreaFactor > 0.0) || !std::isfinite(coAreaFactor))
  {
    singular = true;
    return false;
  }
  tangent = getTangent(jacobian, available);
  if (!available)
  {
    singular = true;
    return false;
  }
  Point densityValue;
  try
  {
    densityValue = density_(point);
  }
  catch (const Exception &)
  {
    return false;
  }
  if (densityValue.getSize() != 1)
    return false;
  const Scalar antecedentPDF = densityValue[0];
  integrand = (antecedentPDF > 0.0) ? antecedentPDF / coAreaFactor : 0.0;
  if (!std::isfinite(integrand))
    return false;
  return true;
}

/* One RK4 step followed by a Newton projection */
Bool LevelSetContinuation::RK4Step(Point & point,
                                  Scalar & integral,
                                  const Scalar step,
                                  const Scalar orientation,
                                  Point & tangentEnd,
                                  Bool & singular) const
{
  Point k1x;
  Point k2x;
  Point k3x;
  Point k4x;
  Scalar k1i = 0.0;
  Scalar k2i = 0.0;
  Scalar k3i = 0.0;
  Scalar k4i = 0.0;
  if (!evaluateRHS(point, k1x, k1i, singular))
    return false;
  Point stage(point);
  for (UnsignedInteger i = 0; i < point.getSize(); ++i)
    stage[i] += 0.5 * step * orientation * k1x[i];
  if (!evaluateRHS(stage, k2x, k2i, singular))
    return false;
  stage = point;
  for (UnsignedInteger i = 0; i < point.getSize(); ++i)
    stage[i] += 0.5 * step * orientation * k2x[i];
  if (!evaluateRHS(stage, k3x, k3i, singular))
    return false;
  stage = point;
  for (UnsignedInteger i = 0; i < point.getSize(); ++i)
    stage[i] += step * orientation * k3x[i];
  if (!evaluateRHS(stage, k4x, k4i, singular))
    return false;
  Point predicted(point);
  for (UnsignedInteger i = 0; i < point.getSize(); ++i)
    predicted[i] += step * orientation
                    * (k1x[i] + 2.0 * k2x[i] + 2.0 * k3x[i] + k4x[i]) / 6.0;
  const Scalar predictedIntegral = integral
                                   + step * (k1i + 2.0 * k2i + 2.0 * k3i + k4i) / 6.0;
  if (!project(predicted))
    return false;
  point = predicted;
  integral = predictedIntegral;
  tangentEnd = k4x;
  return true;
}

/* Trace one direction from a seed */
LevelSetContinuation::Result LevelSetContinuation::traceDirection(const Point & seed,
                                                                  const Scalar orientation,
                                                                  const Scalar initialStep,
                                                                  const Scalar minimumStep,
                                                                  Bool & closed,
                                                                  Scalar & length,
                                                                  Collection<Point> & path) const
{
  Result result;
  closed = false;
  length = 0.0;
  Point point(seed);
  if (!project(point))
    return result;
  // The mesh seed is only a P1 approximation of the fibre and project() may
  // move it. Closure must be measured against the projected starting point,
  // otherwise a closed fibre is traced forever without returning to the
  // original approximate seed.
  const Point startPoint(point);
  path.add(startPoint);
  Point tangent;
  Scalar integrand = 0.0;
  Bool singular = false;
  if (!evaluateRHS(point, tangent, integrand, singular))
  {
    result.status = singular ? SINGULAR : FAILURE;
    return result;
  }
  const Scalar boxMargin = 1.0e-12 * std::max(1.0,
                           (searchInterval_.getUpperBound() - searchInterval_.getLowerBound()).norm());
  Scalar step = initialStep;
  Point previousTangent(tangent);
  const Point initialTangent(tangent);
  const UnsignedInteger maximumSteps = 100000;
  for (UnsignedInteger iteration = 0; iteration < maximumSteps; ++iteration)
  {
    Bool accepted = false;
    for (UnsignedInteger attempt = 0; attempt < 16; ++attempt)
    {
      Point candidate(point);
      Scalar candidateIntegral = result.density;
      Point candidateTangent;
      Bool stepSingular = false;
      if (!RK4Step(candidate, candidateIntegral, step, orientation,
                   candidateTangent, stepSingular))
      {
        result.status = stepSingular ? SINGULAR : FAILURE;
        return result;
      }
      const Point lowerBound(searchInterval_.getLowerBound());
      const Point upperBound(searchInterval_.getUpperBound());
      Bool inside = true;
      for (UnsignedInteger i = 0; i < candidate.getSize(); ++i)
        if ((candidate[i] < lowerBound[i] - boxMargin)
            || (candidate[i] > upperBound[i] + boxMargin))
        {
          inside = false;
          break;
        }
      if (!inside)
      {
        step *= 0.5;
        if (step < minimumStep)
        {
          result.status = SUCCESS;
          return result;
        }
        continue;
      }
      Scalar cosine = 0.0;
      for (UnsignedInteger i = 0; i < point.getSize(); ++i)
        cosine += previousTangent[i] * candidateTangent[i];
      cosine = std::max(-1.0, std::min(1.0, cosine));
      const Scalar angle = std::acos(cosine);
      if ((angle > 0.25) && (step > minimumStep))
      {
        step *= 0.5;
        continue;
      }
      // The ODE is parameterized by arc length: the RK4 increment already
      // advances the integral by step, and the projection only removes the
      // normal drift. Scaling by the chord would reintroduce an O(h^2) error.
      point = candidate;
      result.density = candidateIntegral;
      length += step;
      path.add(point);
      previousTangent = candidateTangent;
      accepted = true;
      break;
    }
    if (!accepted)
      return result;
    Scalar returnCosine = 0.0;
    for (UnsignedInteger i = 0; i < point.getSize(); ++i)
      returnCosine += previousTangent[i] * initialTangent[i];
    const Scalar returnDistance = (point - startPoint).norm();
    // The first two accepted steps cannot be a return: the first ends one
    // step away from the seed, and the second has only travelled two steps.
    // This lower bound lets a small loop whose circumference is only a few
    // steps long close after one loop, unlike a fixed multiple of the initial
    // step, while the distance test keeps the closure local.
    if ((iteration > 0) && (returnCosine > 0.0)
        && (length > 2.0 * step)
        && (returnDistance <= std::max(1.0e-12, 1.1 * step)))
    {
      if (returnDistance <= std::max(1.0e-12, 1.0e-6 * initialStep))
      {
        result.status = SUCCESS;
        closed = true;
        return result;
      }
      step = std::max(1.0e-14, 0.5 * returnDistance);
      continue;
    }
  }
  return result;
}

/* Trace one connected component from one of its mesh segments */
LevelSetContinuation::Status LevelSetContinuation::traceComponent(const Segment & segment,
                                                                  const Scalar initialStep,
                                                                  const Scalar minimumStep,
                                                                  Scalar & density,
                                                                  Collection<Point> & path) const
{
  density = 0.0;
  Point direction(segment.second - segment.first);
  const Scalar directionNorm = direction.norm();
  if (!(directionNorm > 0.0))
    return FAILURE;
  direction /= directionNorm;
  Point tangent;
  Scalar integrand = 0.0;
  Bool singular = false;
  if (!evaluateRHS(segment.first, tangent, integrand, singular))
    return singular ? SINGULAR : FAILURE;
  Scalar cosine = 0.0;
  for (UnsignedInteger i = 0; i < direction.getSize(); ++i)
    cosine += direction[i] * tangent[i];
  const Scalar orientation = (cosine >= 0.0) ? 1.0 : -1.0;
  Bool closed = false;
  Scalar forwardLength = 0.0;
  const Result forward = traceDirection(segment.first, orientation, initialStep,
                                        minimumStep, closed, forwardLength, path);
  if (forward.status != SUCCESS)
    return forward.status;
  if (closed)
  {
    density = forward.density;
    return SUCCESS;
  }
  Bool backwardClosed = false;
  Scalar backwardLength = 0.0;
  const Result backward = traceDirection(segment.first, -orientation, initialStep,
                                         minimumStep, backwardClosed,
                                         backwardLength, path);
  if (backward.status != SUCCESS)
    return backward.status;
  density = forward.density + backward.density;
  return SUCCESS;
}

/* Refine the fibre described by the mesh segments and integrate the density */
LevelSetContinuation::Result LevelSetContinuation::computeDensity(const Collection<Segment> & segments,
                                                                  const Scalar meshStep,
                                                                  const Scalar endpointTolerance) const
{
  Result result;
  if (segments.isEmpty())
    return result;
  const UnsignedInteger inputDimension = function_.getInputDimension();
  const Scalar scale = std::max(1.0,
                                (searchInterval_.getUpperBound() - searchInterval_.getLowerBound()).norm());
  const Scalar tolerance = std::max(endpointTolerance, 1.0e-12 * scale);
  // OpenTURNS has no associative container: std::map indexes the spatial
  // hash and the connected components, while the stored sequences are Indices.
  // The hash key has one signed integer per coordinate, so that the same code
  // handles the curve in R^2, R^3 and R^4.
  typedef std::vector<SignedInteger> HashKey;
  std::map<HashKey, Indices> buckets;
  Collection<Point> nodes;
  Indices parent;
  Indices degree;
  const auto findRoot = [&](UnsignedInteger index) -> UnsignedInteger
  {
    UnsignedInteger root = index;
    while (parent[root] != root)
      root = parent[root];
    while (parent[index] != index)
    {
      const UnsignedInteger next = parent[index];
      parent[index] = root;
      index = next;
    }
    return root;
  };
  const auto getNode = [&](const Point & point) -> UnsignedInteger
  {
    HashKey base(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      base[i] = static_cast<SignedInteger>(std::floor(point[i] / tolerance));
    // Visit the 3^n neighbouring cells in the spatial hash. The current point
    // belongs to the cell indexed by base, and two points closer than the
    // tolerance can only be in adjacent cells.
    UnsignedInteger neighborCount = 1;
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      neighborCount *= 3;
    for (UnsignedInteger neighborIndex = 0; neighborIndex < neighborCount; ++neighborIndex)
    {
      HashKey key(base);
      UnsignedInteger code = neighborIndex;
      for (UnsignedInteger i = 0; i < inputDimension; ++i)
      {
        key[i] += static_cast<SignedInteger>(code % 3) - 1;
        code /= 3;
      }
      const auto bucket = buckets.find(key);
      if (bucket == buckets.end())
        continue;
      for (const UnsignedInteger nodeIndex : bucket->second)
        if ((nodes[nodeIndex] - point).norm() <= tolerance)
          return nodeIndex;
    }
    const UnsignedInteger nodeIndex = nodes.getSize();
    nodes.add(point);
    parent.add(nodeIndex);
    degree.add(0);
    buckets[base].add(nodeIndex);
    return nodeIndex;
  };
  Indices segmentFirstNodes;
  Indices segmentSecondNodes;
  for (const Segment & segment : segments)
  {
    if ((segment.second - segment.first).norm() <= tolerance)
      continue;
    const UnsignedInteger firstNode = getNode(segment.first);
    const UnsignedInteger secondNode = getNode(segment.second);
    if (firstNode == secondNode)
      continue;
    const UnsignedInteger firstRoot = findRoot(firstNode);
    const UnsignedInteger secondRoot = findRoot(secondNode);
    if (firstRoot != secondRoot)
      parent[secondRoot] = firstRoot;
    ++degree[firstNode];
    ++degree[secondNode];
    segmentFirstNodes.add(firstNode);
    segmentSecondNodes.add(secondNode);
  }
  if (segmentFirstNodes.isEmpty())
    return result;
  // A regular one dimensional fibre is a chain (degree one at its two ends)
  // or a closed loop (degree two everywhere). A branching node is a saddle
  // or a tangential contact: keep the mesh result for the whole evaluation.
  for (const UnsignedInteger nodeDegree : degree)
    if ((nodeDegree != 1) && (nodeDegree != 2))
      return result;
  std::map<UnsignedInteger, Indices> components;
  for (UnsignedInteger segmentIndex = 0; segmentIndex < segmentFirstNodes.getSize(); ++segmentIndex)
  {
    const UnsignedInteger root = findRoot(segmentFirstNodes[segmentIndex]);
    components[root].add(segmentIndex);
  }
  const Scalar initialStep = std::max(1.0e-12, 0.25 * meshStep);
  const Scalar minimumStep = std::max(1.0e-12, meshStep * 1.0e-6);
  Scalar density = 0.0;
  // The P1 mesh may split the approximation of one regular fibre into several
  // disconnected fragments, especially when the fibre is coarse relative to
  // the mesh. Tracing each fragment would then integrate the same actual
  // fibre several times. After a component has been traced, its path samples
  // cover the whole actual fibre it belongs to; skip any fragment whose
  // projected seed is already on that path.
  Collection<Point> tracedPoints;
  const Scalar seedMergeTolerance = std::max(initialStep, 4.0 * endpointTolerance);
  for (const auto & component : components)
  {
    const UnsignedInteger firstSegment = component.second[0];
    const UnsignedInteger firstNode = segmentFirstNodes[firstSegment];
    const UnsignedInteger secondNode = segmentSecondNodes[firstSegment];
    Segment seed;
    seed.first = nodes[firstNode];
    seed.second = nodes[secondNode];
    Point projectedSeed(seed.first);
    if (project(projectedSeed))
    {
      Bool alreadyTraced = false;
      for (const Point & tracedPoint : tracedPoints)
        if ((tracedPoint - projectedSeed).norm() <= seedMergeTolerance)
        {
          alreadyTraced = true;
          break;
        }
      if (alreadyTraced)
        continue;
    }
    Scalar componentDensity = 0.0;
    Collection<Point> componentPath;
    const Status status = traceComponent(seed, initialStep,
                                         minimumStep, componentDensity,
                                         componentPath);
    if (status != SUCCESS)
    {
      result.status = status;
      return result;
    }
    density += componentDensity;
    for (const Point & tracedPoint : componentPath)
      tracedPoints.add(tracedPoint);
  }
  result.status = SUCCESS;
  result.density = density;
  if (!std::isfinite(density))
    result.status = FAILURE;
  return result;
}

END_NAMESPACE_OPENTURNS
