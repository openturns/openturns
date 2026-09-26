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
#include "openturns/ResourceMap.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussLPQuadrature)

static const Factory<GaussLPQuadrature> Factory_GaussLPQuadrature;

namespace
{

class QuadratureResidual final : public FunctionImplementation
{
public:
  QuadratureResidual()
    : FunctionImplementation()
  {
  }

  QuadratureResidual(const Collection<Function> & orthoFunctions,
                     const Point & moments,
                     const UnsignedInteger size,
                     const UnsignedInteger dimension)
    : FunctionImplementation()
    , orthoFunctions_(orthoFunctions)
    , moments_(moments)
    , size_(size)
    , dimension_(dimension)
  {
  }

  QuadratureResidual * clone() const override
  {
    return new QuadratureResidual(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return size_ + size_ * dimension_;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator()(const Point & x) const override
  {
    const UnsignedInteger nFuns = orthoFunctions_.getSize();
    Point w(x.begin(), x.begin() + size_);
    Sample xs(size_, dimension_);
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      for (UnsignedInteger j = 0; j < dimension_; ++j)
      {
        xs(i, j) = x[size_ + i * dimension_ + j];
      }
    }
    Point phi(nFuns, 0.0);
    for (UnsignedInteger k = 0; k < nFuns; ++k)
    {
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < size_; ++i)
      {
        sum += w[i] * orthoFunctions_[k](xs[i])[0];
      }
      phi[k] = sum - moments_[k];
    }
    const Scalar residual = phi.normSquare();
    return Point(1, residual);
  }

  String __repr__() const override
  {
    return OSS() << "class=" << getClassName();
  }

private:
  Collection<Function> orthoFunctions_;
  Point moments_;
  UnsignedInteger size_;
  UnsignedInteger dimension_;
};

void BuildClusterPath(const Sample & nodes,
                      const Point & weights,
                      Collection<Indices> & path)
{
  Sample workingNodes(nodes);
  Point workingW(weights);
  path.clear();
  while (workingNodes.getSize() > 1)
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
    Indices merge(2);
    merge[0] = i1;
    merge[1] = i2;
    path.add(merge);
    const Scalar w1 = workingW[i1];
    const Scalar w2 = workingW[i2];
    const Scalar newW = w1 + w2;
    const Point newX = workingNodes[i1] * (w1 / newW) + workingNodes[i2] * (w2 / newW);
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
}

void ApplyClusterPath(const Sample & nodes,
                      const Point & weights,
                      const Collection<Indices> & path,
                      const UnsignedInteger targetN,
                      Sample & outNodes,
                      Point & outWeights)
{
  Sample workingNodes(nodes);
  Point workingW(weights);
  const UnsignedInteger pathSize = path.getSize();
  for (UnsignedInteger idx = 0; idx < pathSize; ++idx)
  {
    if (workingNodes.getSize() <= targetN) break;
    const UnsignedInteger i1 = path[idx][0];
    const UnsignedInteger i2 = path[idx][1];
    const Scalar w1 = workingW[i1];
    const Scalar w2 = workingW[i2];
    const Scalar newW = w1 + w2;
    const Point newX = workingNodes[i1] * (w1 / newW) + workingNodes[i2] * (w2 / newW);
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
  outNodes = workingNodes;
  outWeights = workingW;
}

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
    Scalar sum = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      sum += weights[i] * orthoFunctions[k](nodes[i])[0];
    }
    phi[k] = sum - moments[k];
  }
  return phi.normSquare();
}

}

GaussLPQuadrature::GaussLPQuadrature()
  : PersistentObject()
  , orthonormalFunctions_()
  , measure_()
{
}

GaussLPQuadrature::GaussLPQuadrature(const FunctionCollection & orthonormalFunctions,
    const Distribution & measure)
  : PersistentObject()
  , orthonormalFunctions_(orthonormalFunctions)
  , measure_(measure)
{
}

GaussLPQuadrature * GaussLPQuadrature::clone() const
{
  return new GaussLPQuadrature(*this);
}

Sample GaussLPQuadrature::build(const UnsignedInteger n,
                                Point & weightsOut) const
{
  if (n > orthonormalFunctions_.getSize()) throw InvalidArgumentException(HERE) << "Error: n=" << n << " is greater than the size of the orthonormal set=" << orthonormalFunctions_.getSize();
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0";
  const UnsignedInteger dimension = measure_.getDimension();
  Collection<Function> orthoFunctions(n);
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    orthoFunctions[k] = orthonormalFunctions_[k];
  }
  const UnsignedInteger alphaS = ResourceMap::HasKey("GaussLPQuadrature-AlphaS")
    ? ResourceMap::GetAsUnsignedInteger("GaussLPQuadrature-AlphaS")
    : 100;
  const UnsignedInteger Ngauss = ResourceMap::HasKey("GaussLPQuadrature-Ngauss")
    ? ResourceMap::GetAsUnsignedInteger("GaussLPQuadrature-Ngauss")
    : 100;
  const Scalar epsilon = ResourceMap::HasKey("GaussLPQuadrature-Epsilon")
    ? ResourceMap::GetAsScalar("GaussLPQuadrature-Epsilon")
    : 1.0e-5;
  const Interval support(measure_.getRange());
  const Point a(support.getLowerBound());
  const Point b(support.getUpperBound());
  const Indices marginalSizes(dimension, Ngauss);
  const GaussLegendre reference(marginalSizes);
  const Sample refNodes(reference.getNodes());
  const Point refWeights(reference.getWeights());
  const UnsignedInteger totalNodes = refNodes.getSize();
  Sample adaptedNodes(totalNodes, dimension);
  for (UnsignedInteger i = 0; i < totalNodes; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      adaptedNodes(i, j) = refNodes(i, j) * (b[j] - a[j]) + a[j];
    }
  }
  Point moments(n, 0.0);
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    Scalar sum = 0.0;
    for (UnsignedInteger i = 0; i < totalNodes; ++i)
    {
      sum += refWeights[i] * orthoFunctions[k](adaptedNodes[i])[0] * measure_.computePDF(adaptedNodes[i]);
    }
    moments[k] = sum;
  }
  const UnsignedInteger S = std::max(n * alphaS, n + static_cast<UnsignedInteger>(1));
  Sample candidateNodes(S, dimension);
  const Point volumeVec(b - a);
  const Distribution refUniform(Uniform(0.0, 1.0));
  for (UnsignedInteger i = 0; i < S; ++i)
  {
    Point xi(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      xi[j] = refUniform.getRealization()[0];
    }
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      candidateNodes(i, j) = a[j] + xi[j] * volumeVec[j];
    }
  }
  DesignProxy proxy(candidateNodes, orthoFunctions);
  Indices allIndices(n);
  for (UnsignedInteger k = 0; k < n; ++k) allIndices[k] = k;
  Matrix A(proxy.computeDesign(allIndices).transpose());
  Point cost(S, 1.0);
  LinearProblem lpProblem(cost, Interval(Point(S, 0.0), Point(S, 1.0)), A, Interval(moments, moments));
  HiGHS highs(lpProblem);
  highs.run();
  Point alpha(highs.getResult().getOptimalPoint());
  UnsignedInteger nSupport = 0;
  for (UnsignedInteger i = 0; i < S; ++i)
  {
    if (alpha[i] > 0.0) ++nSupport;
  }
  Sample suppNodes(nSupport, dimension);
  Point suppWeights(nSupport);
  UnsignedInteger idx = 0;
  for (UnsignedInteger i = 0; i < S; ++i)
  {
    if (alpha[i] > 0.0)
    {
      suppNodes[idx] = candidateNodes[i];
      suppWeights[idx] = alpha[i];
      ++idx;
    }
  }
  Collection<Indices> clusterPath;
  BuildClusterPath(suppNodes, suppWeights, clusterPath);
  Scalar bestResidual = SpecFunc::MaxScalar;
  Sample bestNodes(0, dimension);
  Point bestWeights(0);
  const UnsignedInteger nMin = 1;
  for (UnsignedInteger nClusters = nMin; nClusters <= nSupport; ++nClusters)
  {
    Sample currentNodes;
    Point currentWeights;
    if (nClusters < nSupport)
    {
      ApplyClusterPath(suppNodes, suppWeights, clusterPath, nClusters, currentNodes, currentWeights);
    }
    else
    {
      currentNodes = suppNodes;
      currentWeights = suppWeights;
    }
    const UnsignedInteger size = currentNodes.getSize();
    const UnsignedInteger flatSize = size * dimension;
    QuadratureResidual residualFunc(orthoFunctions, moments, size, dimension);
    OptimizationProblem optProblem(residualFunc);
    Point lb(size + flatSize);
    Point ub(size + flatSize);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      lb[i] = 0.0;
      ub[i] = 1.0;
    }
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        lb[size + i * dimension + j] = a[j];
        ub[size + i * dimension + j] = b[j];
      }
    }
    optProblem.setBounds(Interval(lb, ub));
    TNC tnc(optProblem);
    const UnsignedInteger maxCalls = std::max(static_cast<UnsignedInteger>(10000), static_cast<UnsignedInteger>(1000) * (size + flatSize));
    tnc.setMaximumCallsNumber(maxCalls);
    tnc.setMaximumIterationNumber(maxCalls / 2);
    Point x0(size + flatSize);
    for (UnsignedInteger i = 0; i < size; ++i) x0[i] = currentWeights[i];
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        x0[size + i * dimension + j] = currentNodes(i, j);
      }
    }
    tnc.setStartingPoint(x0);
    try
    {
      tnc.run();
      const Point xFinal(tnc.getResult().getOptimalPoint());
      const Scalar residual = residualFunc(xFinal)[0];
      if (residual < bestResidual)
      {
        bestResidual = residual;
        bestNodes = Sample(size, dimension);
        bestWeights = Point(size);
        for (UnsignedInteger i = 0; i < size; ++i) bestWeights[i] = xFinal[i];
        for (UnsignedInteger i = 0; i < size; ++i)
        {
          for (UnsignedInteger j = 0; j < dimension; ++j)
          {
            bestNodes(i, j) = xFinal[size + i * dimension + j];
          }
        }
      }
      if (residual < epsilon * epsilon) break;
    }
    catch (...)
    {
      const Scalar residual = ComputeResidual(currentNodes, currentWeights, orthoFunctions, moments);
      if (residual < bestResidual)
      {
        bestResidual = residual;
        bestNodes = currentNodes;
        bestWeights = currentWeights;
      }
    }
  }
  weightsOut = bestWeights;
  return bestNodes;
}

void GaussLPQuadrature::setOrthonormalFunctions(const FunctionCollection & functions)
{
  orthonormalFunctions_ = functions;
}

GaussLPQuadrature::FunctionCollection GaussLPQuadrature::getOrthonormalFunctions() const
{
  return orthonormalFunctions_;
}

void GaussLPQuadrature::setMeasure(const Distribution & measure)
{
  measure_ = measure;
}

Distribution GaussLPQuadrature::getMeasure() const
{
  return measure_;
}

String GaussLPQuadrature::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName();
  return oss;
}

void GaussLPQuadrature::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("orthonormalFunctions_", orthonormalFunctions_);
  adv.saveAttribute("measure_", measure_);
}

void GaussLPQuadrature::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("orthonormalFunctions_", orthonormalFunctions_);
  adv.loadAttribute("measure_", measure_);
}

END_NAMESPACE_OPENTURNS
