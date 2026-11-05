//                                               -*- C++ -*-
/**
 *  @brief Gauss-LP quadrature rule builder
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
#include "openturns/GaussLPQuadrature.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/DesignProxy.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/GaussProductExperiment.hxx"
#include "openturns/Normal.hxx"
#include "openturns/LinearProblem.hxx"
#include "openturns/HiGHS.hxx"
#include "openturns/TNC.hxx"
#include "openturns/OptimizationProblem.hxx"
#include "openturns/OptimizationResult.hxx"
#include "openturns/Interval.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/FunctionImplementation.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/GradientImplementation.hxx"
#include "openturns/Gradient.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Log.hxx"
#include "openturns/Distribution.hxx"

#include <algorithm>
#include <cmath>
#include "OrthogonalBasisHelpers.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussLPQuadrature)

static const Factory<GaussLPQuadrature> Factory_GaussLPQuadrature;

namespace
{

using namespace OrthogonalBasisHelpers;

/* Measure-adapted rule used for the moment kernel. */
Bool BuildMeasureAdaptedRule(const Distribution & distribution,
                             const UnsignedInteger requestedSize,
                             const UnsignedInteger maximumNodes,
                             Sample & nodes,
                             Point & weights)
{
  return BuildGaussProductRule(distribution, requestedSize, maximumNodes, nodes, weights)
         || BuildTransformedNormalRule(distribution, requestedSize, maximumNodes, nodes, weights)
         || (HasUsableGaussLegendreFallback(distribution)
             && BuildGaussLegendreRule(distribution, requestedSize, maximumNodes, nodes, weights));
}

/* Squared norm of the moment reproduction error. The target functions
   are evaluated in one batch at the nodes: a point-by-point evaluation costs
   one virtual call per node and per function. */
Scalar ComputeResidual(const Sample & nodes,
                       const Point & weights,
                       const Collection<Function> & orthoFunctions,
                       const Point & moments)
{
  const UnsignedInteger nFuns = orthoFunctions.getSize();
  const UnsignedInteger size = nodes.getSize();
  Point phi(nFuns, 0.0);
  for (UnsignedInteger k = 0; k < nFuns; ++k)
  {
    const Sample values(orthoFunctions[k](nodes));
    Scalar sum = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      sum += weights[i] * values(i, 0);
    }
    phi[k] = sum - moments[k];
  }
  return phi.normSquare();
}

/* Unpack the optimization variable into the quadrature rule. The first
   fixedNodeCount nodes are anchors: their coordinates are not optimization
   variables and only the coordinates of the added nodes are stored in the
   variable. */
void UnpackQuadratureRule(const Point & x,
                          const UnsignedInteger size,
                          const UnsignedInteger dimension,
                          const Sample & fixedNodes,
                          Point & weights,
                          Sample & nodes)
{
  const UnsignedInteger fixedNodeCount = fixedNodes.getSize();
  if (fixedNodeCount > size)
    throw InvalidArgumentException(HERE) << "Error: the number of fixed nodes=" << fixedNodeCount
                                         << " is greater than the rule size=" << size;
  const UnsignedInteger variableNodeCount = size - fixedNodeCount;
  const UnsignedInteger expectedDimension = size + variableNodeCount * dimension;
  if (x.getDimension() != expectedDimension)
    throw InvalidArgumentException(HERE) << "Error: expected a rule variable of dimension " << expectedDimension
                                         << ", got " << x.getDimension();
  weights = Point(x.begin(), x.begin() + size);
  nodes = Sample(size, dimension);
  for (UnsignedInteger i = 0; i < fixedNodeCount; ++i)
    nodes[i] = fixedNodes[i];
  UnsignedInteger index = size;
  for (UnsignedInteger i = fixedNodeCount; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      nodes(i, j) = x[index];
      ++index;
    }
  }
}

class QuadratureResidualEvaluation final : public EvaluationImplementation
{
public:
  QuadratureResidualEvaluation()
    : EvaluationImplementation()
  {
    // Nothing to do
  }

  QuadratureResidualEvaluation(const Collection<Function> & orthoFunctions,
                               const Point & moments,
                               const UnsignedInteger size,
                               const UnsignedInteger dimension,
                               const Sample & fixedNodes)
    : EvaluationImplementation()
    , orthoFunctions_(orthoFunctions)
    , moments_(moments)
    , size_(size)
    , dimension_(dimension)
    , fixedNodes_(fixedNodes)
  {
    // Nothing to do
  }

  QuadratureResidualEvaluation * clone() const override
  {
    return new QuadratureResidualEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    const UnsignedInteger variableNodeCount = size_ - fixedNodes_.getSize();
    return size_ + variableNodeCount * dimension_;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator()(const Point & x) const override
  {
    Point weights;
    Sample nodes;
    UnpackQuadratureRule(x, size_, dimension_, fixedNodes_, weights, nodes);
    return Point(1, ComputeResidual(nodes, weights, orthoFunctions_, moments_));
  }

  String __repr__() const override
  {
    return OSS() << "class=" << getClassName();
  }

private:
  Collection<Function> orthoFunctions_;
  Point moments_;
  UnsignedInteger size_ = 0;
  UnsignedInteger dimension_ = 0;
  Sample fixedNodes_;
};

/* Analytic gradient of the squared moment residual.
   With r_k = sum_i w_i phi_k(x_i) - m_k,
     dJ/dw_i = 2 sum_k r_k phi_k(x_i),
     dJ/dx_{i,a} = 2 w_i sum_k r_k d_a phi_k(x_i).
   The coordinates of the first fixedNodeCount nodes are anchors and are not
   optimization variables, so the corresponding entries are omitted. */
class QuadratureResidualGradient final : public GradientImplementation
{
public:
  QuadratureResidualGradient()
    : GradientImplementation()
  {
    // Nothing to do
  }

  QuadratureResidualGradient(const Collection<Function> & orthoFunctions,
                             const Point & moments,
                             const UnsignedInteger size,
                             const UnsignedInteger dimension,
                             const Sample & fixedNodes)
    : GradientImplementation()
    , orthoFunctions_(orthoFunctions)
    , moments_(moments)
    , size_(size)
    , dimension_(dimension)
    , fixedNodes_(fixedNodes)
  {
    // Nothing to do
  }

  QuadratureResidualGradient * clone() const override
  {
    return new QuadratureResidualGradient(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    const UnsignedInteger variableNodeCount = size_ - fixedNodes_.getSize();
    return size_ + variableNodeCount * dimension_;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Matrix gradient(const Point & x) const override
  {
    Point weights;
    Sample nodes;
    UnpackQuadratureRule(x, size_, dimension_, fixedNodes_, weights, nodes);
    const UnsignedInteger nFuns = orthoFunctions_.getSize();
    Matrix values(nFuns, size_);
    Point residual(nFuns, 0.0);
    for (UnsignedInteger k = 0; k < nFuns; ++k)
    {
      const Sample functionValues(orthoFunctions_[k](nodes));
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < size_; ++i)
      {
        values(k, i) = functionValues(i, 0);
        sum += weights[i] * values(k, i);
      }
      residual[k] = sum - moments_[k];
    }
    Matrix result(x.getDimension(), 1);
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      Scalar value = 0.0;
      for (UnsignedInteger k = 0; k < nFuns; ++k)
        value += residual[k] * values(k, i);
      result(i, 0) = 2.0 * value;
    }
    const UnsignedInteger fixedNodeCount = fixedNodes_.getSize();
    for (UnsignedInteger k = 0; k < nFuns; ++k)
    {
      const Sample nodeGradients(orthoFunctions_[k].gradient(nodes));
      if (nodeGradients.getDimension() != dimension_)
        throw InternalException(HERE) << "Error: the gradient of function " << k << " has dimension "
                                      << nodeGradients.getDimension() << ", expected " << dimension_;
      for (UnsignedInteger i = fixedNodeCount; i < size_; ++i)
      {
        const Scalar factor = 2.0 * weights[i] * residual[k];
        const UnsignedInteger offset = size_ + (i - fixedNodeCount) * dimension_;
        for (UnsignedInteger j = 0; j < dimension_; ++j)
          result(offset + j, 0) += factor * nodeGradients(i, j);
      }
    }
    return result;
  }

  String __repr__() const override
  {
    return OSS() << "class=" << getClassName();
  }

private:
  Collection<Function> orthoFunctions_;
  Point moments_;
  UnsignedInteger size_ = 0;
  UnsignedInteger dimension_ = 0;
  Sample fixedNodes_;
};

/* Greedy clustering of Jakeman and Narayan: repeatedly merge the least
   weighted node with its nearest neighbor, replacing both by their weighted
   barycenter, until targetSize nodes remain. */
void ClusterNodes(const Sample & nodes,
                  const Point & weights,
                  const UnsignedInteger targetSize,
                  Sample & resultNodes,
                  Point & resultWeights)
{
  Sample workingNodes(nodes);
  Point workingW(weights);
  while (workingNodes.getSize() > targetSize)
  {
    UnsignedInteger i1 = 0;
    Scalar minW = workingW[0];
    for (UnsignedInteger i = 1; i < workingNodes.getSize(); ++i)
    {
      if (workingW[i] < minW)
      {
        minW = workingW[i];
        i1 = i;
      }
    }
    const Point x1(workingNodes[i1]);
    UnsignedInteger i2 = (i1 == 0 ? 1 : 0);
    Scalar dMin = (workingNodes[i2] - x1).normSquare();
    for (UnsignedInteger i = 0; i < workingNodes.getSize(); ++i)
    {
      if (i == i1) continue;
      const Scalar d = (workingNodes[i] - x1).normSquare();
      if (d < dMin)
      {
        dMin = d;
        i2 = i;
      }
    }
    const Scalar w1 = workingW[i1];
    const Scalar w2 = workingW[i2];
    const Scalar newW = w1 + w2;
    // A zero merged weight carries no mass, so any finite position gives the
    // same rule; the midpoint avoids dividing by zero when both weights vanish.
    const Point newX = (newW > 0.0)
                       ? workingNodes[i1] * (w1 / newW) + workingNodes[i2] * (w2 / newW)
                       : (workingNodes[i1] + workingNodes[i2]) * 0.5;
    workingNodes[i1] = newX;
    workingW[i1] = newW;
    const UnsignedInteger last = workingNodes.getSize() - 1;
    if (i2 != last)
    {
      workingNodes[i2] = workingNodes[last];
      workingW[i2] = workingW[last];
    }
    workingNodes.erase(last);
    workingW.erase(last);
  }
  resultNodes = workingNodes;
  resultWeights = workingW;
}

Point ComputeMoments(const Collection<Function> & orthoFunctions,
                     const Sample & nodes,
                     const Point & weights)
{
  const UnsignedInteger nFuns = orthoFunctions.getSize();
  const UnsignedInteger size = nodes.getSize();
  if ((size == 0) || (size != weights.getDimension()))
    throw InvalidArgumentException(HERE) << "Error: inconsistent moment quadrature rule";
  Point moments(nFuns, 0.0);
  for (UnsignedInteger k = 0; k < nFuns; ++k)
  {
    const Sample values(orthoFunctions[k](nodes));
    Scalar sum = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      sum += weights[i] * values(i, 0);
    }
    if (!std::isfinite(sum))
      throw InvalidArgumentException(HERE) << "Error: the moment of function " << k << " is not finite: " << sum;
    moments[k] = sum;
  }
  return moments;
}

/* Design matrix of the target functions at the given nodes. Its rows are the
   functions and its columns the nodes, as expected by the linear program. */
Matrix BuildDesignMatrix(const Sample & nodes,
                         const Collection<Function> & functions,
                         const UnsignedInteger n)
{
  const UnsignedInteger size = nodes.getSize();
  DesignProxy proxy(nodes, functions);
  Indices allIndices(n);
  for (UnsignedInteger k = 0; k < n; ++k) allIndices[k] = k;
  Matrix A(proxy.computeDesign(allIndices).transpose());
  if ((A.getNbRows() != n) || (A.getNbColumns() != size))
    throw InvalidArgumentException(HERE) << "Error: inconsistent design matrix in the linear program";
  return A;
}

/* Solve a linear program with the configured HiGHS solver and fall back once
   to the configured fallback solver when the first attempt fails. An empty
   fallback disables the retry. The solvers are selected per HiGHS instance:
   the ResourceMap is only read, never modified. */
Point SolveLinearProgram(const Point & cost,
                         const Interval & bounds,
                         const Matrix & constraintCoefficients,
                         const Interval & constraintBounds,
                         const String & primarySolver,
                         const String & fallbackSolver,
                         const Scalar maximumConstraintError)
{
  const auto runSolver = [&](const String & solverName) -> Point
  {
    LinearProblem lpProblem(cost, bounds, constraintCoefficients, constraintBounds);
    HiGHS highs(lpProblem, solverName);
    highs.setMaximumConstraintError(maximumConstraintError);
    highs.run();
    return highs.getResult().getOptimalPoint();
  };
  Point alpha;
  try
  {
    alpha = runSolver(primarySolver);
  }
  catch (const Exception &)
  {
    if (fallbackSolver.empty() || (fallbackSolver == primarySolver)) throw;
    LOGWARN(OSS() << "GaussLPQuadrature: HiGHS solve failed with solver="
                  << primarySolver << ", retrying with solver=" << fallbackSolver);
    alpha = runSolver(fallbackSolver);
  }
  if ((alpha.getDimension() != cost.getDimension()) || !IsFinitePoint(alpha))
    throw InvalidArgumentException(HERE) << "Error: the linear program did not return a finite weight vector";
  return alpha;
}

/* Minimum total weight reproducing the moments from the given nodes. */
Point SolveMinWeightLP(const Sample & nodes,
                       const Collection<Function> & functions,
                       const Point & moments,
                       const Point & lowerBounds,
                       const String & primarySolver,
                       const String & fallbackSolver,
                       const Scalar maximumConstraintError)
{
  const UnsignedInteger size = nodes.getSize();
  const UnsignedInteger n = moments.getDimension();
  if (lowerBounds.getDimension() != size)
    throw InvalidArgumentException(HERE) << "Error: inconsistent lower bounds in the linear program";
  const Matrix A(BuildDesignMatrix(nodes, functions, n));
  const Point cost(size, 1.0);
  return SolveLinearProgram(cost,
                            Interval(lowerBounds, Point(size, 1.0)),
                            A,
                            Interval(moments, moments),
                            primarySolver,
                            fallbackSolver,
                            maximumConstraintError);
}

/* Minimum total weight reproducing the moments from the given nodes, with all
   the anchors forced to a positive weight. A first linear program maximizes
   the smallest anchor weight; the second one minimizes the total weight with
   the anchor weights bounded from below by this positive optimum. */
Point SolveForcedMinWeightLP(const Sample & nodes,
                             const Collection<Function> & functions,
                             const Point & moments,
                             const Indices & anchorIndices,
                             Scalar & anchorLowerBound,
                             const String & primarySolver,
                             const String & fallbackSolver,
                             const Scalar maximumConstraintError)
{
  const UnsignedInteger size = nodes.getSize();
  const UnsignedInteger n = moments.getDimension();
  const UnsignedInteger anchorCount = anchorIndices.getSize();
  if (anchorCount == 0)
  {
    anchorLowerBound = 0.0;
    return SolveMinWeightLP(nodes, functions, moments, Point(size, 0.0),
                            primarySolver, fallbackSolver, maximumConstraintError);
  }
  const Matrix A(BuildDesignMatrix(nodes, functions, n));
  // Variables: the node weights followed by the common lower bound t.
  // Constraints: the moment equalities, then w_i - t >= 0 for each anchor.
  Matrix extendedA(n + anchorCount, size + 1);
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    for (UnsignedInteger i = 0; i < size; ++i)
      extendedA(k, i) = A(k, i);
    extendedA(k, size) = 0.0;
  }
  for (UnsignedInteger j = 0; j < anchorCount; ++j)
  {
    extendedA(n + j, anchorIndices[j]) = 1.0;
    extendedA(n + j, size) = -1.0;
  }
  Point cost(size + 1, 0.0);
  cost[size] = -1.0;
  Point constraintLower(n + anchorCount, 0.0);
  Point constraintUpper(n + anchorCount, SpecFunc::MaxScalar);
  for (UnsignedInteger k = 0; k < n; ++k)
    constraintLower[k] = constraintUpper[k] = moments[k];
  const Point alphaExtended(SolveLinearProgram(cost,
                                               Interval(Point(size + 1, 0.0), Point(size + 1, 1.0)),
                                               extendedA,
                                               Interval(constraintLower, constraintUpper),
                                               primarySolver,
                                               fallbackSolver,
                                               maximumConstraintError));
  const Scalar minimumAnchorWeight = alphaExtended[size];
  if (!(minimumAnchorWeight > 0.0))
    throw InvalidArgumentException(HERE) << "Error: the anchors cannot all carry a positive weight while reproducing the moments"
                                         << "; increase GaussLPQuadrature-AlphaS or GaussLPQuadrature-Margin";
  Point lowerBounds(size, 0.0);
  for (UnsignedInteger j = 0; j < anchorCount; ++j)
    lowerBounds[anchorIndices[j]] = minimumAnchorWeight;
  anchorLowerBound = minimumAnchorWeight;
  return SolveMinWeightLP(nodes, functions, moments, lowerBounds,
                          primarySolver, fallbackSolver, maximumConstraintError);
}

/* Extract the strictly positive weights of a linear-programming solution. */
void ExtractSupport(const Sample & nodes,
                    const Point & alpha,
                    const Scalar weightTolerance,
                    Sample & supportNodes,
                    Point & supportWeights)
{
  const UnsignedInteger size = nodes.getSize();
  UnsignedInteger supportSize = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
    if (alpha[i] > weightTolerance) ++supportSize;
  if (supportSize == 0)
    throw InvalidArgumentException(HERE) << "Error: the linear program found no positive weight"
                                         << "; increase GaussLPQuadrature-AlphaS or GaussLPQuadrature-Margin";
  supportNodes = Sample(supportSize, nodes.getDimension());
  supportWeights = Point(supportSize);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (alpha[i] > weightTolerance)
    {
      supportNodes[index] = nodes[i];
      supportWeights[index] = alpha[i];
      ++index;
    }
  }
}

/* Extract the support of a forced linear program. The first anchorCount nodes
   are the anchors and are retained by identity: a positive weight is a hard
   requirement, not a numerical threshold. The added nodes are subject to the
   usual numerical zero threshold. */
void ExtractForcedSupport(const Sample & nodes,
                          const Point & alpha,
                          const UnsignedInteger anchorCount,
                          const Scalar weightTolerance,
                          Sample & supportNodes,
                          Point & supportWeights)
{
  const UnsignedInteger size = nodes.getSize();
  if (anchorCount > size)
    throw InvalidArgumentException(HERE) << "Error: inconsistent anchor count in the linear program";
  for (UnsignedInteger j = 0; j < anchorCount; ++j)
    if (!(alpha[j] > 0.0))
      throw InvalidArgumentException(HERE) << "Error: the linear program returned a non-positive anchor weight="
                                           << alpha[j]
                                           << "; increase GaussLPQuadrature-AlphaS or GaussLPQuadrature-Margin";
  UnsignedInteger supportSize = anchorCount;
  for (UnsignedInteger i = anchorCount; i < size; ++i)
    if (alpha[i] > weightTolerance) ++supportSize;
  supportNodes = Sample(supportSize, nodes.getDimension());
  supportWeights = Point(supportSize);
  for (UnsignedInteger j = 0; j < anchorCount; ++j)
  {
    supportNodes[j] = nodes[j];
    supportWeights[j] = alpha[j];
  }
  UnsignedInteger index = anchorCount;
  for (UnsignedInteger i = anchorCount; i < size; ++i)
  {
    if (alpha[i] > weightTolerance)
    {
      supportNodes[index] = nodes[i];
      supportWeights[index] = alpha[i];
      ++index;
    }
  }
}

/* Working box used to sample the candidate nodes: the numerical range of the
   measure, with the infinite components replaced by the extreme coordinates
   of the measure-adapted moment rule. The candidates are then drawn uniformly
   over this box, independently of the target measure. */
void BuildCandidateBox(const Distribution & measure,
                       const Sample & momentNodes,
                       Point & lowerBound,
                       Point & upperBound)
{
  const UnsignedInteger dimension = measure.getDimension();
  const Interval range(measure.getRange());
  lowerBound = range.getLowerBound();
  upperBound = range.getUpperBound();
  const Interval::BoolCollection finiteLowerBound(range.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(range.getFiniteUpperBound());
  if (momentNodes.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Error: the moment rule is empty";
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    if (!finiteLowerBound[j] || !std::isfinite(lowerBound[j]))
    {
      Scalar value = momentNodes(0, j);
      for (UnsignedInteger i = 1; i < momentNodes.getSize(); ++i)
        value = std::min(value, momentNodes(i, j));
      lowerBound[j] = value;
    }
    if (!finiteUpperBound[j] || !std::isfinite(upperBound[j]))
    {
      Scalar value = momentNodes(0, j);
      for (UnsignedInteger i = 1; i < momentNodes.getSize(); ++i)
        value = std::max(value, momentNodes(i, j));
      upperBound[j] = value;
    }
    if (!(lowerBound[j] <= upperBound[j]))
      throw InvalidArgumentException(HERE) << "Error: invalid candidate box in dimension " << j
                                           << ": lower bound=" << lowerBound[j]
                                           << " is greater than upper bound=" << upperBound[j];
  }
}

/* Sample candidate nodes uniformly over the box. The convex combination form
   avoids overflowing when the box is very wide. */
Sample GenerateUniformCandidates(const Point & lowerBound,
                                 const Point & upperBound,
                                 const UnsignedInteger size)
{
  const UnsignedInteger dimension = lowerBound.getDimension();
  Sample candidates(size, dimension);
  // Draw the uniforms in one batch and fill the sample column-wise, as the
  // Sample storage does. This keeps the candidate stream consistent with a
  // direct measure sample for a uniform measure.
  const Point uniforms(RandomGenerator::Generate(size * dimension));
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar u = uniforms[j * size + i];
      candidates(i, j) = (1.0 - u) * lowerBound[j] + u * upperBound[j];
    }
  }
  if (!IsFiniteSample(candidates))
    throw InvalidArgumentException(HERE) << "Error: the candidate box did not produce a finite sample";
  return candidates;
}

/* Remove duplicate anchors and check that each retained anchor has a positive
   target density. */
Sample PrepareAnchors(const Sample & anchors,
                      const Distribution & measure)
{
  const UnsignedInteger dimension = measure.getDimension();
  if (anchors.getSize() == 0)
    return Sample(0, dimension);
  if (anchors.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the anchors have dimension=" << anchors.getDimension()
                                         << ", expected " << dimension;
  if (!IsFiniteSample(anchors))
    throw InvalidArgumentException(HERE) << "Error: the anchors must be finite";
  Sample result(0, dimension);
  for (UnsignedInteger i = 0; i < anchors.getSize(); ++i)
  {
    Bool duplicate = false;
    for (UnsignedInteger j = 0; j < result.getSize(); ++j)
    {
      if (anchors[i] == result[j])
      {
        duplicate = true;
        break;
      }
    }
    if (duplicate)
    {
      LOGWARN(OSS() << "GaussLPQuadrature: duplicate anchor " << anchors[i] << " removed");
      continue;
    }
    const Scalar pdf = measure.computePDF(anchors[i]);
    if (!(pdf > 0.0))
      throw InvalidArgumentException(HERE) << "Error: the anchor " << anchors[i]
                                           << " has a non-positive target PDF=" << pdf;
    result.add(anchors[i]);
  }
  return result;
}

}

GaussLPQuadrature::GaussLPQuadrature()
  : PersistentObject()
  , functions_()
  , measure_()
  , size_(0)
  , anchors_()
{
  // Nothing to do
}

GaussLPQuadrature::GaussLPQuadrature(const FunctionCollection & functions,
                                     const Distribution & measure)
  : PersistentObject()
  , functions_()
  , measure_()
  , size_(0)
  , anchors_()
{
  setMeasureAndFunctions(measure, functions);
}

GaussLPQuadrature::GaussLPQuadrature(const FunctionCollection & functions,
                                     const Distribution & measure,
                                     const Sample & anchors)
  : PersistentObject()
  , functions_()
  , measure_()
  , size_(0)
  , anchors_()
{
  setMeasureAndFunctions(measure, functions);
  setAnchors(anchors);
}

GaussLPQuadrature * GaussLPQuadrature::clone() const
{
  return new GaussLPQuadrature(*this);
}

Sample GaussLPQuadrature::build(const UnsignedInteger n,
                                Point & weightsOut) const
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0";
  if (functions_.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Error: the functions and the measure must be set before building a rule";
  if (measure_.getImplementation().isNull())
    throw InvalidArgumentException(HERE) << "Error: the measure must be defined";
  if (n > size_)
    throw InvalidArgumentException(HERE) << "Error: n=" << n << " is greater than the number of functions=" << size_;
  const UnsignedInteger dimension = measure_.getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: the measure dimension must be > 0";
  Collection<Function> functions(n);
  for (UnsignedInteger k = 0; k < n; ++k)
    functions[k] = functions_[k];

  const UnsignedInteger alphaS = ResourceMap::GetAsUnsignedInteger("GaussLPQuadrature-AlphaS");
  if (alphaS == 0) throw InvalidArgumentException(HERE) << "Error: GaussLPQuadrature-AlphaS must be > 0";
  const UnsignedInteger margin = ResourceMap::GetAsUnsignedInteger("GaussLPQuadrature-Margin");
  if (margin == 0) throw InvalidArgumentException(HERE) << "Error: GaussLPQuadrature-Margin must be > 0";
  const UnsignedInteger Ngauss = ResourceMap::GetAsUnsignedInteger("GaussLPQuadrature-Ngauss");
  if (Ngauss == 0) throw InvalidArgumentException(HERE) << "Error: GaussLPQuadrature-Ngauss must be > 0";
  const UnsignedInteger maximumNodes = ResourceMap::GetAsUnsignedInteger("GaussLPQuadrature-MaximumTensorNodes");
  if (maximumNodes == 0) throw InvalidArgumentException(HERE) << "Error: GaussLPQuadrature-MaximumTensorNodes must be > 0";
  const Scalar epsilon = ResourceMap::GetAsScalar("GaussLPQuadrature-Epsilon");
  if (!(epsilon > 0.0)) throw InvalidArgumentException(HERE) << "Error: GaussLPQuadrature-Epsilon must be > 0";
  const Scalar maximumAbsoluteError = ResourceMap::GetAsScalar("GaussLPQuadrature-MaximumAbsoluteError");
  const Scalar maximumRelativeError = ResourceMap::GetAsScalar("GaussLPQuadrature-MaximumRelativeError");
  const Scalar maximumObjectiveError = ResourceMap::GetAsScalar("GaussLPQuadrature-MaximumObjectiveError");
  const Scalar maximumConstraintError = ResourceMap::GetAsScalar("GaussLPQuadrature-MaximumConstraintError");
  const String primarySolver = ResourceMap::GetAsString("GaussLPQuadrature-Solver");
  if (primarySolver.empty()) throw InvalidArgumentException(HERE) << "Error: GaussLPQuadrature-Solver must be non-empty";
  const String fallbackSolver = ResourceMap::GetAsString("GaussLPQuadrature-FallbackSolver");

  // Moment kernel: a measure-adapted quadrature rule, not a Lebesgue rule on
  // the numerical range. Heavy-tailed and dependent measures are supported.
  Sample momentNodes;
  Point momentWeights;
  if (!BuildMeasureAdaptedRule(measure_, Ngauss, maximumNodes, momentNodes, momentWeights))
    throw InvalidArgumentException(HERE) << "Error: unable to build a measure-adapted rule for the moments"
                                         << "; increase GaussLPQuadrature-MaximumTensorNodes or GaussLPQuadrature-Ngauss";
  const Point moments(ComputeMoments(functions, momentNodes, momentWeights));

  // The candidate nodes are drawn uniformly over the working box. For an
  // unbounded measure the box is the one of the moment rule.
  Point lowerBound;
  Point upperBound;
  BuildCandidateBox(measure_, momentNodes, lowerBound, upperBound);

  const UnsignedInteger anchorCount = anchors_.getSize();
  // The linear-programming weights lie in [0, 1]: a weight below the working
  // precision is a numerical zero and does not belong to the support.
  const Scalar weightTolerance = SpecFunc::Precision;
  // Linear-programming feasibility is only defined up to its own tolerance.
  const Scalar feasibilityTolerance = std::sqrt(SpecFunc::Precision);

  // When there are enough anchors to determine the rule, first try a rule
  // supported by anchors only: it needs no added point at all. The anchors are
  // then used as a candidate pool and the linear program selects the subset
  // reproducing the moments.
  if ((anchorCount > 0) && (anchorCount >= n))
  {
    try
    {
      const Point alpha(SolveMinWeightLP(anchors_, functions, moments, Point(anchorCount, 0.0),
                                         primarySolver, fallbackSolver, maximumConstraintError));
      Sample anchorRuleNodes;
      Point anchorRuleWeights;
      ExtractSupport(anchors_, alpha, weightTolerance, anchorRuleNodes, anchorRuleWeights);
      LOGINFO(OSS() << "GaussLPQuadrature: anchor-only rule with " << anchorRuleNodes.getSize() << " node(s)");
      weightsOut = anchorRuleWeights;
      return anchorRuleNodes;
    }
    catch (const Exception & linearProgramError)
    {
      LOGINFO(OSS() << "GaussLPQuadrature: no anchor-only rule (" << linearProgramError.what()
                    << "), adding candidate nodes");
    }
  }

  // Candidate nodes: S = max(n * AlphaS, n + Margin). The margin is a
  // heuristic: it ensures that small values of AlphaS still give the linear
  // program a few more columns than moment constraints.
  const UnsignedInteger S = std::max<UnsignedInteger>(n * alphaS, n + margin);
  const Sample uniformCandidates(GenerateUniformCandidates(lowerBound, upperBound, S));

  // Build the node set. The anchors always come first so that the support
  // extraction can retain them by identity.
  const auto buildNodes = [&](const Sample & candidates) -> Sample
  {
    const UnsignedInteger candidateSize = candidates.getSize();
    Sample nodes(anchorCount + candidateSize, dimension);
    for (UnsignedInteger i = 0; i < anchorCount; ++i)
      nodes[i] = anchors_[i];
    for (UnsignedInteger i = 0; i < candidateSize; ++i)
      nodes[anchorCount + i] = candidates[i];
    return nodes;
  };
  Indices anchorIndices(anchorCount);
  for (UnsignedInteger i = 0; i < anchorCount; ++i)
    anchorIndices[i] = i;

  Sample fullNodes;
  Point fullWeights;
  Scalar anchorLowerBound = 0.0;
  const auto solveOnCandidates = [&](const Sample & candidates) -> Bool
  {
    const Sample nodes(buildNodes(candidates));
    try
    {
      if (anchorCount > 0)
        fullWeights = SolveForcedMinWeightLP(nodes, functions, moments, anchorIndices, anchorLowerBound,
                                             primarySolver, fallbackSolver, maximumConstraintError);
      else
        fullWeights = SolveMinWeightLP(nodes, functions, moments, Point(nodes.getSize(), 0.0),
                                       primarySolver, fallbackSolver, maximumConstraintError);
      fullNodes = nodes;
      return true;
    }
    catch (const Exception & linearProgramError)
    {
      LOGINFO(OSS() << "GaussLPQuadrature: linear program failed on " << candidates.getSize()
                    << " candidate node(s): " << linearProgramError.what());
      return false;
    }
  };

  Sample suppNodes;
  Point suppWeights;
  if (solveOnCandidates(uniformCandidates))
  {
    if (anchorCount > 0)
      ExtractForcedSupport(fullNodes, fullWeights, anchorCount, weightTolerance, suppNodes, suppWeights);
    else
      ExtractSupport(fullNodes, fullWeights, weightTolerance, suppNodes, suppWeights);
  }
  else
  {
    // The uniform candidates may not span the moments, especially for
    // high-degree systems. Fall back to a measure-adapted tensor rule of
    // roughly n nodes: a tensor rule of order ceil(n^(1/d)) per dimension. For
    // polynomial bases it reproduces the moments accurately and gives the
    // refinement a well-conditioned start.
    const UnsignedInteger order = std::max<UnsignedInteger>(2, static_cast<UnsignedInteger>(std::ceil(std::pow(1.0 * n, 1.0 / dimension))));
    Sample fallbackNodes;
    Point fallbackWeights;
    if (!BuildMeasureAdaptedRule(measure_, order, maximumNodes, fallbackNodes, fallbackWeights))
      throw InvalidArgumentException(HERE) << "Error: the linear program failed on the sampled candidates and no measure-adapted fallback rule could be built"
                                           << "; increase GaussLPQuadrature-MaximumTensorNodes or GaussLPQuadrature-Ngauss";
    if (anchorCount > 0)
    {
      if (!solveOnCandidates(fallbackNodes))
        throw InvalidArgumentException(HERE) << "Error: no rule could be built with all the anchors;"
                                             << " increase GaussLPQuadrature-AlphaS or GaussLPQuadrature-Margin";
      ExtractForcedSupport(fullNodes, fullWeights, anchorCount, weightTolerance, suppNodes, suppWeights);
    }
    else
    {
      if (fallbackNodes.getSize() < n)
        throw InvalidArgumentException(HERE) << "Error: the linear program failed and the measure-adapted fallback rule has "
                                             << fallbackNodes.getSize() << " nodes, fewer than the " << n
                                             << " functions to integrate";
      suppNodes = fallbackNodes;
      suppWeights = fallbackWeights;
    }
    LOGWARN(OSS() << "GaussLPQuadrature: the linear program on the sampled candidates failed, starting from a measure-adapted rule with "
                  << suppNodes.getSize() << " node(s)");
  }
  if (!IsFiniteRule(suppNodes, suppWeights))
    throw InvalidArgumentException(HERE) << "Error: the starting rule is not a valid quadrature rule";

  // The support of the starting rule gives the natural rule size. The anchors
  // are kept by identity and are never clustered: only the added nodes take
  // part in the condensation. A basic feasible solution of the linear program
  // has at most n positive weights; an interior-point solution can have more,
  // in which case the added nodes are clustered down.
  const UnsignedInteger supportSize = suppNodes.getSize();
  if (supportSize < anchorCount)
    throw InvalidArgumentException(HERE) << "Error: the linear program support does not contain all the anchors";
  const Bool useClusterPath = ResourceMap::GetAsBool("GaussLPQuadrature-ClusterPath");
  // A rule with more than n nodes is not minimal, unless the anchors already
  // impose more than n nodes.
  const UnsignedInteger maximumRuleSize = (anchorCount >= n)
                                          ? supportSize
                                          : std::min<UnsignedInteger>(supportSize, n);
  // If added nodes are present, keep at least one of them: the anchors alone
  // have already been tried (or are not sufficient), and clustering the last
  // added node into an anchor would change the anchor coordinates or weights.
  const UnsignedInteger addedCount = supportSize - anchorCount;
  // The clustering path starts from the smallest rule and adds one added node
  // at a time; without the path only the largest rule is refined.
  const UnsignedInteger minimumRuleSize = anchorCount
                                          + (useClusterPath
                                             ? ((addedCount > 0) ? 1 : 0)
                                             : (maximumRuleSize - anchorCount));

  const Interval support(measure_.getRange());
  Point a(support.getLowerBound());
  Point b(support.getUpperBound());
  // The numerical range of an unbounded measure is only a working box, and
  // the starting rule may contain nodes beyond it. Extend the box so that
  // every node of the clustering path always lies inside the bounds.
  for (UnsignedInteger i = 0; i < supportSize; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      a[j] = std::min(a[j], suppNodes(i, j));
      b[j] = std::max(b[j], suppNodes(i, j));
    }
  }

  // Cluster the added nodes of the linear-programming support down to the
  // requested size. The anchors are copied verbatim into the starting rule.
  Sample anchorSupport(anchorCount, dimension);
  Point anchorSupportWeights(anchorCount);
  Sample addedSupport(addedCount, dimension);
  Point addedSupportWeights(addedCount);
  for (UnsignedInteger i = 0; i < anchorCount; ++i)
  {
    anchorSupport[i] = suppNodes[i];
    anchorSupportWeights[i] = suppWeights[i];
  }
  for (UnsignedInteger i = 0; i < addedCount; ++i)
  {
    addedSupport[i] = suppNodes[anchorCount + i];
    addedSupportWeights[i] = suppWeights[anchorCount + i];
  }
  const auto buildStartingRule = [&](const UnsignedInteger ruleSize,
                                     Sample & ruleNodes,
                                     Point & ruleWeights)
  {
    const UnsignedInteger targetAddedSize = ruleSize - anchorCount;
    Sample clusteredNodes;
    Point clusteredWeights;
    if (addedCount > targetAddedSize)
      ClusterNodes(addedSupport, addedSupportWeights, targetAddedSize, clusteredNodes, clusteredWeights);
    else
    {
      clusteredNodes = addedSupport;
      clusteredWeights = addedSupportWeights;
    }
    ruleNodes = Sample(ruleSize, dimension);
    ruleWeights = Point(ruleSize);
    for (UnsignedInteger i = 0; i < anchorCount; ++i)
    {
      ruleNodes[i] = anchorSupport[i];
      ruleWeights[i] = anchorSupportWeights[i];
    }
    for (UnsignedInteger i = 0; i < targetAddedSize; ++i)
    {
      ruleNodes[anchorCount + i] = clusteredNodes[i];
      ruleWeights[anchorCount + i] = clusteredWeights[i];
    }
  };

  Scalar bestResidual = SpecFunc::MaxScalar;
  Sample bestNodes(0, dimension);
  Point bestWeights(0);
  for (UnsignedInteger ruleSize = minimumRuleSize; ruleSize <= maximumRuleSize; ++ruleSize)
  {
    Sample currentNodes;
    Point currentWeights;
    buildStartingRule(ruleSize, currentNodes, currentWeights);
    if ((currentNodes.getSize() != ruleSize) || (currentWeights.getDimension() != ruleSize))
      throw InvalidArgumentException(HERE) << "Error: the clustering step did not produce a valid rule of size=" << ruleSize;

    const UnsignedInteger variableNodeCount = ruleSize - anchorCount;
    const UnsignedInteger variableSize = ruleSize + variableNodeCount * dimension;
    Function residualFunc(QuadratureResidualEvaluation(functions, moments, ruleSize, dimension, anchorSupport));
    // The analytic gradient avoids the 2 * variableSize residual evaluations of
    // a finite-difference gradient and the associated cancellation error.
    residualFunc.setGradient(QuadratureResidualGradient(functions, moments, ruleSize, dimension, anchorSupport));
    OptimizationProblem optProblem(residualFunc);
    Point lb(variableSize);
    Point ub(variableSize);
    for (UnsignedInteger i = 0; i < ruleSize; ++i)
    {
      lb[i] = 0.0;
      ub[i] = 1.0;
    }
    for (UnsignedInteger i = 0; i < anchorCount; ++i)
    {
      // The anchors keep a positive weight: the max-min linear program gives
      // the strongest possible common lower bound.
      lb[i] = anchorLowerBound;
    }
    UnsignedInteger index = ruleSize;
    for (UnsignedInteger i = anchorCount; i < ruleSize; ++i)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        lb[index] = a[j];
        ub[index] = b[j];
        ++index;
      }
    }
    optProblem.setBounds(Interval(lb, ub));
    const auto packRule = [&](const Sample & nodes, const Point & weights) -> Point
    {
      Point x(variableSize);
      for (UnsignedInteger i = 0; i < ruleSize; ++i) x[i] = weights[i];
      UnsignedInteger flatIndex = ruleSize;
      for (UnsignedInteger i = anchorCount; i < ruleSize; ++i)
      {
        for (UnsignedInteger j = 0; j < dimension; ++j)
          x[flatIndex++] = nodes(i, j);
      }
      return x;
    };
    TNC tnc(optProblem);
    // The analytic gradient costs one function evaluation per iteration:
    // allow at least 10000 calls and 1000 calls per optimization variable so
    // that the refinement is not truncated on small problems. Each iteration
    // costs at least two calls, hence the iteration budget is half the call
    // budget.
    const UnsignedInteger maxCalls = std::max<UnsignedInteger>(10000, 1000 * variableSize);
    tnc.setMaximumCallsNumber(maxCalls);
    tnc.setMaximumIterationNumber(maxCalls / 2);
    // Dedicated tuning of the refinement tolerances: the generic optimization
    // defaults are too loose to reproduce the moments to machine precision.
    tnc.setMaximumAbsoluteError(maximumAbsoluteError);
    tnc.setMaximumRelativeError(maximumRelativeError);
    tnc.setMaximumResidualError(maximumObjectiveError);
    tnc.setMaximumConstraintError(maximumConstraintError);
    tnc.setStartingPoint(packRule(currentNodes, currentWeights));

    const auto storeCandidate = [&](const Point & x) -> Bool
    {
      if ((x.getDimension() != variableSize) || !IsFinitePoint(x)) return false;
      Point refinedWeights;
      Sample refinedNodes;
      try
      {
        UnpackQuadratureRule(x, ruleSize, dimension, anchorSupport, refinedWeights, refinedNodes);
      }
      catch (const Exception &)
      {
        return false;
      }
      for (UnsignedInteger i = 0; i < ruleSize; ++i)
      {
        // TNC may violate the bounds by a few ulps: reject clearly infeasible
        // candidates and clamp the small violations to the bounds.
        if ((x[i] < -1.0e-12) || (x[i] > 1.0 + 1.0e-12)) return false;
        if ((i < anchorCount) && (x[i] < anchorLowerBound - feasibilityTolerance)) return false;
        refinedWeights[i] = std::min<Scalar>(1.0, std::max<Scalar>(0.0, x[i]));
        if (i < anchorCount)
          refinedWeights[i] = std::max<Scalar>(anchorLowerBound, refinedWeights[i]);
      }
      if (!IsFiniteRule(refinedNodes, refinedWeights)) return false;
      Scalar residual = 0.0;
      try
      {
        residual = ComputeResidual(refinedNodes, refinedWeights, functions, moments);
      }
      catch (const Exception &)
      {
        return false;
      }
      if (!std::isfinite(residual)) return false;
      if (residual < bestResidual)
      {
        bestResidual = residual;
        bestNodes = refinedNodes;
        bestWeights = refinedWeights;
      }
      return true;
    };

    LOGDEBUG(OSS() << "GaussLPQuadrature: trying " << ruleSize << " node(s)");
    // Keep the clustered rule as a valid fallback, then try to improve it.
    storeCandidate(packRule(currentNodes, currentWeights));
    // When the starting rule already meets the tolerance, refining it would
    // only add numerical noise and consume the call budget: keep it as is.
    if (bestResidual > epsilon * epsilon)
    {
      try
      {
        tnc.run();
        const Point xFinal(tnc.getResult().getOptimalPoint());
        storeCandidate(xFinal);
      }
      catch (const Exception &)
      {
        // The clustered fallback is already stored.
      }
    }
    // Stop at the first rule on the path that meets the tolerance: it is the
    // smallest rule found by the greedy clustering sequence.
    if (bestResidual <= epsilon * epsilon)
    {
      LOGDEBUG(OSS() << "GaussLPQuadrature: converged with " << ruleSize << " node(s)");
      break;
    }
  }
  if (bestNodes.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Error: the local refinement did not produce a valid quadrature rule";
  if (bestWeights.getDimension() != bestNodes.getSize())
    throw InvalidArgumentException(HERE) << "Error: the local refinement did not produce consistent nodes and weights";
  if (!IsFiniteRule(bestNodes, bestWeights))
    throw InvalidArgumentException(HERE) << "Error: the local refinement returned an invalid quadrature rule";
  // The residual is a squared norm, while the tolerance applies to the norm
  // itself.
  if (bestResidual > epsilon * epsilon)
    LOGWARN(OSS() << "GaussLPQuadrature: residual norm=" << std::sqrt(bestResidual) << " is larger than the requested tolerance=" << epsilon);
  weightsOut = bestWeights;
  return bestNodes;
}

GaussLPQuadratureRule GaussLPQuadrature::buildRule(const UnsignedInteger n) const
{
  Point weights;
  const Sample nodes(build(n, weights));
  return GaussLPQuadratureRule(nodes, weights, getMeasure());
}

void GaussLPQuadrature::setAnchors(const Sample & anchors)
{
  if (size_ == 0)
  {
    // The builder has no functions and no measure yet: the anchors will be
    // validated when the measure is set.
    anchors_ = anchors;
    return;
  }
  anchors_ = PrepareAnchors(anchors, measure_);
}

Sample GaussLPQuadrature::getAnchors() const
{
  return anchors_;
}

void GaussLPQuadrature::setMeasureAndFunctions(const Distribution & measure,
                                               const FunctionCollection & functions)
{
  if (functions.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Error: the number of functions must be > 0";
  if (measure.getImplementation().isNull())
    throw InvalidArgumentException(HERE) << "Error: the measure must be defined";
  const UnsignedInteger dimension = measure.getDimension();
  if (dimension == 0)
    throw InvalidArgumentException(HERE) << "Error: the measure dimension must be > 0";
  for (UnsignedInteger k = 0; k < functions.getSize(); ++k)
  {
    if (functions[k].getInputDimension() != dimension)
      throw InvalidArgumentException(HERE) << "Error: function " << k << " has input dimension=" << functions[k].getInputDimension()
                                           << ", expected " << dimension;
    if (functions[k].getOutputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: function " << k << " has output dimension=" << functions[k].getOutputDimension()
                                           << ", expected 1";
  }
  // Preserve and revalidate the current anchors before modifying the builder:
  // a rejected measure change leaves the builder unchanged.
  Sample validatedAnchors(anchors_);
  if (validatedAnchors.getSize() > 0)
    validatedAnchors = PrepareAnchors(validatedAnchors, measure);
  functions_ = FunctionPersistentCollection(functions);
  measure_ = measure;
  size_ = functions.getSize();
  anchors_ = validatedAnchors;
}

GaussLPQuadrature::FunctionCollection GaussLPQuadrature::getFunctionsCollection() const
{
  return functions_;
}

UnsignedInteger GaussLPQuadrature::getSize() const
{
  return size_;
}

Distribution GaussLPQuadrature::getMeasure() const
{
  return measure_;
}

String GaussLPQuadrature::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName()
      << " functions=" << functions_.getSize()
      << " measure=" << measure_
      << " size=" << size_
      << " anchors=" << anchors_.getSize();
  return oss;
}

void GaussLPQuadrature::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("functions_", functions_);
  adv.saveAttribute("measure_", measure_);
  adv.saveAttribute("size_", size_);
  adv.saveAttribute("anchors_", anchors_);
}

void GaussLPQuadrature::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("functions_", functions_);
  adv.loadAttribute("measure_", measure_);
  adv.loadAttribute("size_", size_);
  if (adv.hasAttribute("anchors_"))
    adv.loadAttribute("anchors_", anchors_);
  else
    anchors_ = Sample();
  // Revalidate the persisted anchors against the persisted measure when the
  // builder was configured.
  if ((size_ > 0) && (anchors_.getSize() > 0))
    anchors_ = PrepareAnchors(anchors_, measure_);
}

END_NAMESPACE_OPENTURNS
