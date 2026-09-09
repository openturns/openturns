//                                               -*- C++ -*-
/**
 *  @brief A finite orthonormal set of functions wrt a given distribution.
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
#include "openturns/QRFiniteOrthonormalFunctionFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/DesignProxy.hxx"
#include "openturns/JointDistribution.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/GaussProductExperiment.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/LinearProblem.hxx"
#include "openturns/HiGHS.hxx"
#include "openturns/TNC.hxx"
#include "openturns/Interval.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/MatrixImplementation.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/OptimizationProblem.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/FunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(QRFiniteOrthonormalFunctionFactory)

static const Factory<QRFiniteOrthonormalFunctionFactory> Factory_QRFiniteOrthonormalFunctionFactory;


namespace
{

/** Helper: evaluate the weighted residual for quadrature refinement */
class QuadratureResidual final : public FunctionImplementation
{
public:
  QuadratureResidual()
    : FunctionImplementation()
  {
    // Nothing to do
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
    setInputDimension(size + size * dimension);
    setOutputDimension(1);
    setInputDescription(Description(getInputDimension(), "x"));
    setOutputDescription(Description(1, "residual"));
  }

  QuadratureResidual * clone() const override
  {
    return new QuadratureResidual(*this);
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

/** Greedy clustering path: repeatedly merge smallest-weight point with its nearest neighbor */
void BuildClusterPath(const Sample & nodes,
                      const Point & weights,
                      Collection<Indices> & path)
{
  const UnsignedInteger n = nodes.getSize();
  Sample workingNodes(nodes);
  Point workingW(weights);
  path.clear();
  while (workingNodes.getSize() > 1)
  {
    // Find the point with smallest weight
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
    // Find its nearest neighbor
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
    // Record the merge
    Indices merge(2);
    merge[0] = i1;
    merge[1] = i2;
    path.add(merge);
    // Perform the merge
    const Scalar w1 = workingW[i1];
    const Scalar w2 = workingW[i2];
    const Scalar newW = w1 + w2;
    const Point newX = workingNodes[i1] * (w1 / newW) + workingNodes[i2] * (w2 / newW);
    workingNodes[i1] = newX;
    workingW[i1] = newW;
    // Remove the merged point
    const UnsignedInteger last = workingNodes.getSize() - 1;
    if (i2 != last)
    {
      workingNodes[i2] = workingNodes[last];
      workingW[i2] = workingW[last];
    }
    workingNodes.erase(workingNodes.begin() + last);
    workingW.erase(workingW.begin() + last);
  }
}

/** Apply cluster path to obtain target_N clusters */
void ApplyClusterPath(const Sample & nodes,
                      const Point & weights,
                      const OT::Collection<Indices> & path,
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
    workingNodes.erase(workingNodes.begin() + last);
    workingW.erase(workingW.begin() + last);
  }
  outNodes = workingNodes;
  outWeights = workingW;
}

/** Compute the norm squared of moment residual */
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

} // anonymous namespace


/* Default constructor */
QRFiniteOrthonormalFunctionFactory::QRFiniteOrthonormalFunctionFactory()
  : OrthogonalFunctionFactory()
  , initialFunctions_()
  , experiment_()
  , orthonormalFunctions_()
  , coefficients_()
  , mapping_()
  , isAlreadyComputed_(false)
{
  // Nothing to do
}


/* Constructor */
QRFiniteOrthonormalFunctionFactory::QRFiniteOrthonormalFunctionFactory(const FunctionCollection & functions,
    const Distribution & measure,
    const WeightedExperiment & experiment)
  : OrthogonalFunctionFactory(measure)
  , initialFunctions_()
  , experiment_(experiment)
  , orthonormalFunctions_()
  , coefficients_()
  , mapping_()
  , isAlreadyComputed_(false)
{
  setFunctionsCollection(functions);
}


/* Virtual constructor */
QRFiniteOrthonormalFunctionFactory * QRFiniteOrthonormalFunctionFactory::clone() const
{
  return new QRFiniteOrthonormalFunctionFactory(*this);
}


/* Build the Function of the given index */
Function QRFiniteOrthonormalFunctionFactory::build(const UnsignedInteger index) const
{
  computeOrthonormalBasis();
  if (index >= orthonormalFunctions_.getSize()) throw InvalidArgumentException(HERE) << "Error: the given index=" << index << " is greater than the size of the orthonormal set=" << orthonormalFunctions_.getSize();
  return orthonormalFunctions_[index];
}


/* Build a minimal quadrature rule */
Sample QRFiniteOrthonormalFunctionFactory::buildQuadrature(const UnsignedInteger n,
    Point & weightsOut) const
{
  computeOrthonormalBasis();
  if (n > orthonormalFunctions_.getSize()) throw InvalidArgumentException(HERE) << "Error: n=" << n << " is greater than the size of the orthonormal set=" << orthonormalFunctions_.getSize();
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0";
  const UnsignedInteger dimension = measure_.getDimension();
  // Collect the first n orthonormal functions
  Collection<Function> orthoFunctions(n);
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    orthoFunctions[k] = orthonormalFunctions_[k];
  }
  // Parameters
  const UnsignedInteger alphaS = ResourceMap::GetAsUnsignedInteger("QRFiniteOrthonormalFunctionFactory-AlphaS", 100);
  const UnsignedInteger Ngauss = ResourceMap::GetAsUnsignedInteger("QRFiniteOrthonormalFunctionFactory-Ngauss", 100);
  const Scalar epsilon = ResourceMap::GetAsScalar("QRFiniteOrthonormalFunctionFactory-Epsilon", 1.0e-5);
  // Step 1: compute moments of orthonormal functions wrt the measure
  const Interval support(measure_.getRange());
  const Point a(support.getLowerBound());
  const Point b(support.getUpperBound());
  const Indices marginalSizes(dimension, Ngauss);
  const GaussLegendre reference(marginalSizes);
  const Sample refNodes(reference.getNodes());
  const Point refWeights(reference.getWeights());
  // Map reference nodes from [0,1] to [a,b]
  const UnsignedInteger totalNodes = refNodes.getSize();
  Sample adaptedNodes(totalNodes, dimension);
  for (UnsignedInteger i = 0; i < totalNodes; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      adaptedNodes(i, j) = refNodes(i, j) * (b[j] - a[j]) + a[j];
    }
  }
  // Compute moments: ∫ p_k(x) * pdf(x) dx ≈ Σ_i w_i * p_k(x_i) * pdf(x_i)
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
  // Step 2: generate candidate points uniformly in the support
  const UnsignedInteger S = std::max(n * alphaS, n + 1u);
  Sample candidateNodes(S, dimension);
  const Point volumeVec(b - a);
  for (UnsignedInteger i = 0; i < S; ++i)
  {
    const Point xi = Distribution(Uniform(0.0, 1.0)).getRealization();
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      candidateNodes(i, j) = a[j] + xi[j] * volumeVec[j];
    }
  }
  // Step 3: solve LP
  // Build design matrix: A[k, i] = p_k(x_i)
  DesignProxy proxy(candidateNodes, orthoFunctions);
  Indices allIndices(n);
  for (UnsignedInteger k = 0; k < n; ++k) allIndices[k] = k;
  Matrix A(proxy.computeDesign(allIndices).transpose());
  // min sum_i w_i s.t. A * w = moments, 0 <= w_i <= 1
  Point cost(S, 1.0);
  LinearProblem lpProblem(cost, Interval(Point(S, 0.0), Point(S, 1.0)), A, Interval(moments, moments));
  HiGHS highs(lpProblem);
  highs.run();
  Point alpha(highs.getResult().getOptimalPoint());
  // Extract support: points with positive weight
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
  // Step 4+5: cluster and refine
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
    // Refine via TNC
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
    const UnsignedInteger maxCalls = std::max(10000u, 1000u * (size + flatSize));
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
      // Refinement failed, continue with clustered result
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


/* Functions accessor */
void QRFiniteOrthonormalFunctionFactory::setFunctionsCollection(const FunctionCollection & functions)
{
  const UnsignedInteger size = functions.getSize();
  const UnsignedInteger dimension = measure_.getDimension();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (functions[i].getInputDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an input dimension=" << functions[i].getInputDimension() << ", expected an input dimension=" << dimension;
    if (functions[i].getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an output dimension=" << functions[i].getOutputDimension() << ", expected an output dimension=1";
  }
  initialFunctions_ = functions;
  isAlreadyComputed_ = false;
}


QRFiniteOrthonormalFunctionFactory::FunctionCollection QRFiniteOrthonormalFunctionFactory::getFunctionsCollection() const
{
  return initialFunctions_;
}


/* Experiment accessor */
void QRFiniteOrthonormalFunctionFactory::setExperiment(const WeightedExperiment & experiment)
{
  experiment_ = experiment;
  isAlreadyComputed_ = false;
}


WeightedExperiment QRFiniteOrthonormalFunctionFactory::getExperiment() const
{
  if (!experiment_.getDistribution().getDimension())
  {
    return buildDefaultExperiment(measure_);
  }
  return experiment_;
}


/* Coefficients accessor */
SquareMatrix QRFiniteOrthonormalFunctionFactory::getCoefficients() const
{
  computeOrthonormalBasis();
  return coefficients_;
}


/* String converter */
String QRFiniteOrthonormalFunctionFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName()
      << " functions=" << initialFunctions_
      << " measure=" << measure_
      << " experiment=" << experiment_;
  return oss;
}


String QRFiniteOrthonormalFunctionFactory::__str__(const String & /*offset*/) const
{
  OSS oss(false);
  oss << getClassName()
      << "(functions=" << initialFunctions_
      << ", measure=" << measure_
      << ", experiment=" << experiment_
      << ")";
  return oss;
}


/* Lazy computation of the orthonormal basis */
void QRFiniteOrthonormalFunctionFactory::computeOrthonormalBasis() const
{
  if (isAlreadyComputed_) return;
  const UnsignedInteger nFuns = initialFunctions_.getSize();
  if (nFuns == 0)
  {
    orthonormalFunctions_ = FunctionPersistentCollection(0);
    coefficients_ = SquareMatrix(0);
    isAlreadyComputed_ = true;
    return;
  }
  const UnsignedInteger dimension = measure_.getDimension();
  const Interval support(measure_.getRange());
  const Point a(support.getLowerBound());
  const Point b(support.getUpperBound());
  // Build experiment if none provided
  WeightedExperiment effectiveExperiment(experiment_);
  if (!effectiveExperiment.getDistribution().getDimension())
  {
    effectiveExperiment = buildDefaultExperiment(measure_);
  }
  // Evaluate the initial functions at the experiment nodes
  Point weights;
  Sample nodes(effectiveExperiment.generateWithWeights(weights));
  const UnsignedInteger nNodes = nodes.getSize();
  // Map nodes from reference domain to actual domain
  Sample adaptedNodes(nNodes, dimension);
  const Scalar volume = support.getVolume();
  for (UnsignedInteger i = 0; i < nNodes; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      adaptedNodes(i, j) = nodes(i, j) * ((b[j] - a[j]) * 0.5) + (a[j] + b[j]) * 0.5;
    }
  }
  // Compute PDF and update weights
  for (UnsignedInteger i = 0; i < nNodes; ++i)
  {
    weights[i] *= volume * measure_.computePDF(adaptedNodes[i]);
  }
  // Build the weighted design matrix: M_ij = sqrt(w_i) * f_j(nodes_i)
  MatrixImplementation weightedMImpl(nNodes, nFuns);
  for (UnsignedInteger i = 0; i < nNodes; ++i)
  {
    const Scalar sqrtW = sqrt(weights[i]);
    for (UnsignedInteger j = 0; j < nFuns; ++j)
    {
      weightedMImpl(i, j) = sqrtW * initialFunctions_[j](nodes[i])[0];
    }
  }
  Matrix weightedM(weightedMImpl);
  // QR decomposition
  Matrix R;
  Matrix Q(weightedM.computeQR(R));
  // R is upper triangular (nFuns x nFuns). Invert it.
  TriangularMatrix Rtri(*R.getImplementation(), false);
  Matrix RinvMatrix(Rtri.solveLinearSystemInPlace(IdentityMatrix(nFuns)));
  // Extract coefficients
  MatrixImplementation coeffImpl(nFuns, nFuns);
  mapping_ = buildMapping();
  orthonormalFunctions_ = FunctionPersistentCollection(nFuns);
  const Scalar epsilon = ResourceMap::GetAsScalar("QRFiniteOrthonormalFunctionFactory-Epsilon", 1.0e-11);
  for (UnsignedInteger j = 0; j < nFuns; ++j)
  {
    const Scalar sign = (RinvMatrix(j, j) > 0.0) ? 1.0 : -1.0;
    Point coeffs;
    FunctionCollection funcs;
    for (UnsignedInteger i = 0; i <= j; ++i)
    {
      const Scalar cij = sign * RinvMatrix(i, j);
      if (std::abs(cij) > epsilon)
      {
        coeffs.add(cij);
        funcs.add(initialFunctions_[i]);
      }
      coeffImpl(i, j) = cij;
    }
    LinearCombinationFunction lc(funcs, coeffs);
    orthonormalFunctions_[j] = ComposedFunction(lc, mapping_);
  }
  coefficients_ = SquareMatrix(coeffImpl);
  isAlreadyComputed_ = true;
}


/* Build the mapping from measure domain to reference domain */
Function QRFiniteOrthonormalFunctionFactory::buildMapping() const
{
  const UnsignedInteger dimension = measure_.getDimension();
  const Interval support(measure_.getRange());
  const Point a(support.getLowerBound());
  const Point b(support.getUpperBound());
  Description inputNames(dimension);
  Description formulas(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    inputNames[i] = (OSS() << "x" << i);
    formulas[i] = (OSS() << "2.0 * (x" << i << " - (" << a[i] << ")) / (" << b[i] - a[i] << ") - 1.0");
  }
  return SymbolicFunction(inputNames, formulas);
}


/* Build a default Gauss product experiment for the given measure */
WeightedExperiment QRFiniteOrthonormalFunctionFactory::buildDefaultExperiment(const Distribution & distribution)
{
  const UnsignedInteger dimension = distribution.getDimension();
  const Interval support(distribution.getRange());
  const Point a(support.getLowerBound());
  const Point b(support.getUpperBound());
  Collection<Distribution> uniforms(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    uniforms[i] = Uniform(a[i], b[i]);
  }
  const UnsignedInteger Ndiscretization = ResourceMap::GetAsUnsignedInteger("QRFiniteOrthonormalFunctionFactory-DefaultDiscretization", 128);
  const Indices marginalSizes(dimension, Ndiscretization);
  return GaussProductExperiment(JointDistribution(uniforms), marginalSizes);
}


/* Method save() stores the object through the StorageManager */
void QRFiniteOrthonormalFunctionFactory::save(Advocate & adv) const
{
  OrthogonalFunctionFactory::save(adv);
  adv.saveAttribute("initialFunctions_", initialFunctions_);
  adv.saveAttribute("experiment_", experiment_);
  adv.saveAttribute("orthonormalFunctions_", orthonormalFunctions_);
  adv.saveAttribute("coefficients_", coefficients_);
  adv.saveAttribute("mapping_", mapping_);
  adv.saveAttribute("isAlreadyComputed_", isAlreadyComputed_);
}


/* Method load() reloads the object from the StorageManager */
void QRFiniteOrthonormalFunctionFactory::load(Advocate & adv)
{
  OrthogonalFunctionFactory::load(adv);
  adv.loadAttribute("initialFunctions_", initialFunctions_);
  adv.loadAttribute("experiment_", experiment_);
  adv.loadAttribute("orthonormalFunctions_", orthonormalFunctions_);
  adv.loadAttribute("coefficients_", coefficients_);
  adv.loadAttribute("mapping_", mapping_);
  adv.loadAttribute("isAlreadyComputed_", isAlreadyComputed_);
}


END_NAMESPACE_OPENTURNS
