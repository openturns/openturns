//                                               -*- C++ -*-
/**
 *  @brief Basis orthonormalization algorithm
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
#include "openturns/BasisOrthonormalizationAlgorithm.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ExperimentIntegration.hxx"
#include "openturns/ImportanceSamplingExperiment.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/Interval.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/MatrixImplementation.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/ResourceMap.hxx"

#include <algorithm>
#include <cmath>
#include "OrthogonalBasisHelpers.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BasisOrthonormalizationAlgorithm)

static const Factory<BasisOrthonormalizationAlgorithm> Factory_BasisOrthonormalizationAlgorithm;

namespace
{

using namespace OrthogonalBasisHelpers;

/* A fixed set of nodes with measure weights. It is used by the
   default integration when no Gauss product rule is available: the nodes are
   obtained by transporting a standard normal Gauss rule through the
   iso-probabilistic transformation, so the weights already integrate against
   the measure. */
class FixedMeasureExperiment final : public WeightedExperimentImplementation
{
public:
  FixedMeasureExperiment()
    : WeightedExperimentImplementation()
  {
    // Nothing to do
  }

  FixedMeasureExperiment(const Distribution & measure,
                         const Sample & nodes,
                         const Point & weights)
    : WeightedExperimentImplementation(measure, nodes.getSize())
    , nodes_(nodes)
    , weights_(weights)
  {
    // Nothing to do
  }

  FixedMeasureExperiment * clone() const override
  {
    return new FixedMeasureExperiment(*this);
  }

  Sample generateWithWeights(Point & weightsOut) const override
  {
    weightsOut = weights_;
    return nodes_;
  }

  Bool hasUniformWeights() const override
  {
    return false;
  }

  Bool isRandom() const override
  {
    return false;
  }

  String __repr__() const override
  {
    return OSS() << "class=FixedMeasureExperiment"
                 << " size=" << getSize();
  }

  void save(Advocate & adv) const override
  {
    WeightedExperimentImplementation::save(adv);
    adv.saveAttribute("nodes_", nodes_);
    adv.saveAttribute("weights_", weights_);
  }

  void load(Advocate & adv) override
  {
    WeightedExperimentImplementation::load(adv);
    adv.loadAttribute("nodes_", nodes_);
    adv.loadAttribute("weights_", weights_);
  }

private:
  Sample nodes_;
  Point weights_;
};

class GramEvaluation final : public EvaluationImplementation
{
public:
  GramEvaluation()
    : EvaluationImplementation()
  {
    // Nothing to do
  }

  GramEvaluation(const Function & left,
                 const Function & right,
                 const Distribution & measure)
    : EvaluationImplementation()
    , left_(left)
    , right_(right)
    , measure_(measure)
  {
    // Nothing to do
  }

  GramEvaluation * clone() const override
  {
    return new GramEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return measure_.getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator()(const Point & x) const override
  {
    return Point(1, left_(x)[0] * right_(x)[0] * measure_.computePDF(x));
  }

  String __repr__() const override
  {
    return OSS() << "class=" << getClassName();
  }

private:
  Function left_;
  Function right_;
  Distribution measure_;
};

/* The diagonal of the triangular factor scales with the norm of the functions:
   compare it with the largest diagonal so that the rank test is invariant by
   rescaling. */
void CheckTriangularRank(const Matrix & factor, const Scalar epsilon)
{
  const UnsignedInteger size = factor.getNbRows();
  Scalar maxDiagonal = 0.0;
  for (UnsignedInteger j = 0; j < size; ++j)
    maxDiagonal = std::max(maxDiagonal, std::abs(factor(j, j)));
  if (!(maxDiagonal > 0.0))
    throw InvalidArgumentException(HERE) << "Error: the initial functions are linearly dependent";
  for (UnsignedInteger j = 0; j < size; ++j)
  {
    if (!(std::abs(factor(j, j)) > epsilon * maxDiagonal))
      throw InvalidArgumentException(HERE) << "Error: the initial functions are linearly dependent";
  }
}

/* The distribution according to which the experiment nodes are generated.
   ImportanceSamplingExperiment stores the target distribution in the base
   class and the instrumental distribution separately. */
Distribution GetInstrumentalDistribution(const WeightedExperiment & experiment)
{
  const ImportanceSamplingExperiment * importance = dynamic_cast<const ImportanceSamplingExperiment *>(experiment.getImplementation().get());
  if (importance != nullptr) return importance->getImportanceDistribution();
  return experiment.getDistribution();
}

/* True when the support of inner is contained in the support of outer.
   Interval flags describe the true boundedness: an unbounded distribution has
   finite numerical bounds (Normal, Student, ...) but a false flag, so its
   range must be read as an infinite interval. This is a necessary support
   check, not a full measure-compatibility check. */
Bool RangeContains(const Interval & outer,
                   const Interval & inner)
{
  const UnsignedInteger dimension = outer.getDimension();
  if (inner.getDimension() != dimension) return false;
  const Point outerLower(outer.getLowerBound());
  const Point outerUpper(outer.getUpperBound());
  const Point innerLower(inner.getLowerBound());
  const Point innerUpper(inner.getUpperBound());
  const Interval::BoolCollection outerFiniteLower(outer.getFiniteLowerBound());
  const Interval::BoolCollection outerFiniteUpper(outer.getFiniteUpperBound());
  const Interval::BoolCollection innerFiniteLower(inner.getFiniteLowerBound());
  const Interval::BoolCollection innerFiniteUpper(inner.getFiniteUpperBound());
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    if (innerFiniteLower[j])
    {
      // An infinite outer lower bound contains any inner lower bound.
      if (outerFiniteLower[j])
      {
        // The tolerance absorbs the rounding of the numerical bounds returned
        // by getRange(): it must not mask a genuine support mismatch.
        const Scalar tolerance = 1.0e-12 * std::max(1.0, std::max(std::abs(outerLower[j]), std::abs(innerLower[j])));
        if (innerLower[j] < outerLower[j] - tolerance) return false;
      }
    }
    else if (outerFiniteLower[j]) return false;
    if (innerFiniteUpper[j])
    {
      // An infinite outer upper bound contains any inner upper bound.
      if (outerFiniteUpper[j])
      {
        // Same tolerance as for the lower bound.
        const Scalar tolerance = 1.0e-12 * std::max(1.0, std::max(std::abs(outerUpper[j]), std::abs(innerUpper[j])));
        if (innerUpper[j] > outerUpper[j] + tolerance) return false;
      }
    }
    else if (outerFiniteUpper[j]) return false;
  }
  return true;
}

}

BasisOrthonormalizationAlgorithm::BasisOrthonormalizationAlgorithm()
  : PersistentObject()
  , orthogonalBasis_()
  , size_(0)
  , integrationAlgorithm_()
  , orthonormalFunctions_()
  , coefficients_()
{
  // Nothing to do
}

BasisOrthonormalizationAlgorithm::BasisOrthonormalizationAlgorithm(const OrthogonalBasis & orthogonalBasis,
    const UnsignedInteger size)
  : PersistentObject()
  , orthogonalBasis_(orthogonalBasis)
  , size_(size)
  , integrationAlgorithm_()
  , orthonormalFunctions_()
  , coefficients_()
{
  if (size_ == 0)
    throw InvalidArgumentException(HERE) << "Error: the number of basis elements must be > 0";
  const Distribution measure(orthogonalBasis_.getMeasure());
  if (measure.getImplementation().isNull())
    throw InvalidArgumentException(HERE) << "Error: the orthogonal basis must define a measure";
  const UnsignedInteger dimension = measure.getDimension();
  if (dimension == 0)
    throw InvalidArgumentException(HERE) << "Error: the measure of the orthogonal basis must have a positive dimension";
  // A default OrthogonalBasis wraps a bare OrthogonalFunctionFactory, whose
  // build() is not implemented: reject it explicitly instead of surfacing a
  // NotYetImplementedException from the first basis element.
  if (orthogonalBasis_.getImplementation()->getClassName() == "OrthogonalFunctionFactory")
    throw InvalidArgumentException(HERE) << "Error: the orthogonal basis must define its functions";
  // Check that the first size elements are scalar functions of the measure
  // dimension, so that an invalid basis is rejected as early as possible.
  FunctionCollection functions(size_);
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    functions[i] = orthogonalBasis_.build(i);
    if (functions[i].getInputDimension() != dimension)
      throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an input dimension=" << functions[i].getInputDimension() << ", expected an input dimension=" << dimension;
    if (functions[i].getOutputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an output dimension=" << functions[i].getOutputDimension() << ", expected an output dimension=1";
  }
}

BasisOrthonormalizationAlgorithm * BasisOrthonormalizationAlgorithm::clone() const
{
  return new BasisOrthonormalizationAlgorithm(*this);
}

void BasisOrthonormalizationAlgorithm::run()
{
  if (isAlreadyComputed_) return;
  const UnsignedInteger nFuns = size_;
  if (nFuns == 0)
  {
    orthonormalFunctions_ = FunctionPersistentCollection(0);
    coefficients_ = SquareMatrix(0);
    isAlreadyComputed_ = true;
    return;
  }
  const Distribution measure(orthogonalBasis_.getMeasure());
  const UnsignedInteger dimension = measure.getDimension();
  const Interval support(measure.getRange());
  // The basis functions are materialized once: basis.build() may construct a
  // new Function at each call, and they are evaluated many times below.
  FunctionCollection initialFunctions(nFuns);
  for (UnsignedInteger i = 0; i < nFuns; ++i) initialFunctions[i] = orthogonalBasis_.build(i);
  // Effective integration rule. Experiment-derived rules expose their nodes:
  // factorize the weighted design matrix (QR), which preserves the conditioning
  // of the design, unlike the Gram matrix whose condition number is squared.
  // Opaque rules only provide integrals: factorize the Gram matrix (Cholesky).
  const IntegrationAlgorithm effectiveIntegration = hasIntegrationAlgorithm_
      ? integrationAlgorithm_
      : buildDefaultIntegration(measure);
  Matrix RinvMatrix;
  const Scalar epsilon = ResourceMap::GetAsScalar("BasisOrthonormalizationAlgorithm-Epsilon");
  if (!(epsilon >= 0.0)) throw InvalidArgumentException(HERE) << "Error: BasisOrthonormalizationAlgorithm-Epsilon must be >= 0";
  const ExperimentIntegration * experimentRule = dynamic_cast<const ExperimentIntegration *>(effectiveIntegration.getImplementation().get());
  if (experimentRule == nullptr)
  {
    // Gram matrix of the initial functions with respect to the measure
    MatrixImplementation gramImpl(nFuns, nFuns);
    for (UnsignedInteger i = 0; i < nFuns; ++i)
    {
      for (UnsignedInteger j = 0; j <= i; ++j)
      {
        const Function integrand(GramEvaluation(initialFunctions[i], initialFunctions[j], measure));
        const Scalar gij = effectiveIntegration.integrate(integrand, support)[0];
        if (!std::isfinite(gij))
          throw InvalidArgumentException(HERE) << "Error: the Gram matrix entry (" << i << ", " << j << ") is not finite: " << gij;
        gramImpl(i, j) = gij;
        gramImpl(j, i) = gij;
      }
    }
    Matrix factorL;
    try
    {
      factorL = gramImpl.computeCholesky();
    }
    catch (const NotSymmetricDefinitePositiveException &)
    {
      throw InvalidArgumentException(HERE) << "Error: the initial functions are linearly dependent";
    }
    const Matrix factorR(factorL.transpose());
    CheckTriangularRank(factorR, epsilon);
    TriangularMatrix Rtri(*factorR.getImplementation(), false);
    RinvMatrix = Rtri.solveLinearSystemInPlace(IdentityMatrix(nFuns));
  }
  else
  {
    // Weighted design matrix on the rule nodes. The functions are evaluated
    // at the nodes, which sample the target measure.
    const WeightedExperiment ruleExperiment(experimentRule->getWeightedExperiment());
    if (ruleExperiment.getDistribution().getDimension() != dimension)
      throw InvalidArgumentException(HERE) << "Error: the experiment dimension=" << ruleExperiment.getDistribution().getDimension()
                                           << " does not match the measure dimension=" << dimension;
    // The rule must be able to see the whole target support: a support of the
    // instrumental distribution smaller than the target support silently
    // omits part of the measure and produces functions that are orthonormal
    // for a restriction of the target, not for the target itself. A discrete
    // experiment (FixedExperiment and friends) stores the empirical measure
    // supported by its nodes: its range is the convex hull of the nodes, which
    // is legitimately smaller than a continuous target support even when the
    // weights integrate the target exactly.
    const Distribution experimentDistribution(ruleExperiment.getDistribution());
    const Distribution instrumentalDistribution(GetInstrumentalDistribution(ruleExperiment));
    if (!instrumentalDistribution.isDiscrete())
    {
      const Interval instrumentalRange(instrumentalDistribution.getRange());
      const Interval targetRange(measure.getRange());
      if (!RangeContains(instrumentalRange, targetRange))
        throw InvalidArgumentException(HERE) << "Error: the instrumental distribution range=" << instrumentalRange
                                             << " does not contain the target distribution range=" << targetRange
                                             << "; the experiment cannot integrate the target measure";
    }
    // The QR path applies the experiment weights directly, without folding the
    // target density into them: the weights must integrate the target measure.
    // A continuous experiment stores that measure in getDistribution(), so a
    // mismatch with the target would silently produce a basis orthonormal for
    // another measure. A discrete experiment stores the empirical measure
    // supported by its nodes, which differs from the target by construction,
    // and is exempt.
    if (!experimentDistribution.isDiscrete())
    {
      try
      {
        if (experimentDistribution != measure)
          throw InvalidArgumentException(HERE) << "Error: the experiment distribution=" << experimentDistribution
                                               << " does not match the target distribution=" << measure;
      }
      catch (const NotYetImplementedException &)
      {
        // Some distributions do not implement equality: the caller is
        // responsible for the compatibility with the target measure.
      }
    }
    Point weights;
    const Sample nodes(ruleExperiment.generateWithWeights(weights));
    const UnsignedInteger nNodes = nodes.getSize();
    if (weights.getSize() != nNodes)
      throw InvalidArgumentException(HERE) << "Error: the experiment returned " << nNodes << " nodes but " << weights.getSize() << " weights";
    if (nNodes < nFuns)
      throw InvalidArgumentException(HERE) << "Error: the experiment has " << nNodes << " nodes but " << nFuns
                                           << " functions must be orthonormalized; increase the experiment size or use an integration algorithm";
    MatrixImplementation weightedMImpl(nNodes, nFuns);
    for (UnsignedInteger i = 0; i < nNodes; ++i)
    {
      const Point physNode(nodes[i]);
      if (!IsFinitePoint(physNode))
        throw InvalidArgumentException(HERE) << "Error: the experiment node at index=" << i << " is not finite: " << physNode;
      const Scalar weight = weights[i];
      if (!std::isfinite(weight) || (weight < 0.0))
        throw InvalidArgumentException(HERE) << "Error: the experiment weight at index=" << i << " is not finite and non-negative: " << weight;
      const Scalar sqrtW = std::sqrt(weight);
      for (UnsignedInteger j = 0; j < nFuns; ++j)
      {
        weightedMImpl(i, j) = sqrtW * initialFunctions[j](physNode)[0];
      }
    }
    Matrix weightedM(weightedMImpl);
    Matrix R;
    // Only the triangular factor is needed: discarding the orthogonal factor
    // frees it immediately instead of keeping it alive until the end.
    weightedM.computeQR(R);
    CheckTriangularRank(R, epsilon);
    TriangularMatrix Rtri(*R.getImplementation(), false);
    RinvMatrix = Rtri.solveLinearSystemInPlace(IdentityMatrix(nFuns));
  }
  MatrixImplementation coeffImpl(nFuns, nFuns);
  orthonormalFunctions_ = FunctionPersistentCollection(nFuns);
  for (UnsignedInteger j = 0; j < nFuns; ++j)
  {
    const Scalar sign = (RinvMatrix(j, j) > 0.0) ? 1.0 : -1.0;
    // Zero the coefficients relatively to the largest one in the column: an
    // absolute threshold would depend on the scaling of the initial functions.
    Scalar maxCoeff = 0.0;
    for (UnsignedInteger i = 0; i <= j; ++i)
      maxCoeff = std::max(maxCoeff, std::abs(RinvMatrix(i, j)));
    Point coeffs;
    FunctionCollection funcs;
    for (UnsignedInteger i = 0; i <= j; ++i)
    {
      const Scalar cij = sign * RinvMatrix(i, j);
      // Always keep the diagonal term so that the linear combination is never
      // empty, even when the column is dominated by an off-diagonal entry.
      if ((i == j) || (std::abs(cij) > epsilon * maxCoeff))
      {
        coeffs.add(cij);
        funcs.add(initialFunctions[i]);
      }
      coeffImpl(i, j) = cij;
    }
    orthonormalFunctions_[j] = LinearCombinationFunction(funcs, coeffs);
  }
  coefficients_ = SquareMatrix(coeffImpl);
  isAlreadyComputed_ = true;
}

BasisOrthonormalizationAlgorithm::FunctionPersistentCollection BasisOrthonormalizationAlgorithm::getOrthonormalFunctions() const
{
  if (!isAlreadyComputed_)
  {
    const_cast<BasisOrthonormalizationAlgorithm*>(this)->run();
  }
  return orthonormalFunctions_;
}

SquareMatrix BasisOrthonormalizationAlgorithm::getCoefficients() const
{
  if (!isAlreadyComputed_)
  {
    const_cast<BasisOrthonormalizationAlgorithm*>(this)->run();
  }
  return coefficients_;
}

OrthogonalBasis BasisOrthonormalizationAlgorithm::getOrthogonalBasis() const
{
  return orthogonalBasis_;
}

UnsignedInteger BasisOrthonormalizationAlgorithm::getSize() const
{
  return size_;
}

Distribution BasisOrthonormalizationAlgorithm::getMeasure() const
{
  return orthogonalBasis_.getMeasure();
}

void BasisOrthonormalizationAlgorithm::setIntegrationAlgorithm(const IntegrationAlgorithm & integrationAlgorithm)
{
  integrationAlgorithm_ = integrationAlgorithm;
  hasIntegrationAlgorithm_ = true;
  isAlreadyComputed_ = false;
}

IntegrationAlgorithm BasisOrthonormalizationAlgorithm::getIntegrationAlgorithm() const
{
  return integrationAlgorithm_;
}

String BasisOrthonormalizationAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName()
      << " size=" << size_;
  return oss;
}

IntegrationAlgorithm BasisOrthonormalizationAlgorithm::buildDefaultIntegration(const Distribution & distribution)
{
  const UnsignedInteger Ndiscretization = ResourceMap::GetAsUnsignedInteger("BasisOrthonormalizationAlgorithm-DefaultDiscretization");
  if (Ndiscretization == 0) throw InvalidArgumentException(HERE) << "Error: BasisOrthonormalizationAlgorithm-DefaultDiscretization must be > 0";
  const UnsignedInteger maximumNodes = ResourceMap::GetAsUnsignedInteger("BasisOrthonormalizationAlgorithm-MaximumTensorNodes");
  if (maximumNodes == 0) throw InvalidArgumentException(HERE) << "Error: BasisOrthonormalizationAlgorithm-MaximumTensorNodes must be > 0";
  Sample nodes;
  Point weights;
  // First try the exact tensor rule of the measure. It is the most accurate
  // choice when the marginals have a finite moment recurrence. The rule size
  // is capped so that high-dimensional measures do not materialize a huge
  // tensor product, and halved on failure (heavy tails, infinite moments).
  if (!BuildGaussProductRule(distribution, Ndiscretization, maximumNodes, nodes, weights)
      // Otherwise transport a standard normal Gauss rule through the inverse
      // iso-probabilistic transformation: this handles dependent copulas and
      // heavy-tailed marginals without evaluating a high-order recurrence.
      && !BuildTransformedNormalRule(distribution, Ndiscretization, maximumNodes, nodes, weights)
      // Last resort: Gauss-Legendre on the numerical range, with the density
      // and the support volume folded into the weights. A dependent elliptical
      // measure with a non-normal standard distribution has no reliable
      // fallback, see HasUsableGaussLegendreFallback.
      && (!HasUsableGaussLegendreFallback(distribution)
          || !BuildGaussLegendreRule(distribution, Ndiscretization, maximumNodes, nodes, weights)))
    throw InvalidArgumentException(HERE) << "Error: unable to build a measure-adapted integration rule for the distribution=" << distribution
                                         << "; increase BasisOrthonormalizationAlgorithm-MaximumTensorNodes or set an integration algorithm such as ExperimentIntegration";
  return ExperimentIntegration(FixedMeasureExperiment(distribution, nodes, weights));
}

void BasisOrthonormalizationAlgorithm::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("orthogonalBasis_", orthogonalBasis_);
  adv.saveAttribute("size_", size_);
  adv.saveAttribute("integrationAlgorithm_", integrationAlgorithm_);
  adv.saveAttribute("hasIntegrationAlgorithm_", hasIntegrationAlgorithm_);
  adv.saveAttribute("orthonormalFunctions_", orthonormalFunctions_);
  adv.saveAttribute("coefficients_", coefficients_);
  adv.saveAttribute("isAlreadyComputed_", isAlreadyComputed_);
}

void BasisOrthonormalizationAlgorithm::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("orthogonalBasis_", orthogonalBasis_);
  adv.loadAttribute("size_", size_);
  adv.loadAttribute("integrationAlgorithm_", integrationAlgorithm_);
  adv.loadAttribute("hasIntegrationAlgorithm_", hasIntegrationAlgorithm_);
  adv.loadAttribute("orthonormalFunctions_", orthonormalFunctions_);
  adv.loadAttribute("coefficients_", coefficients_);
  adv.loadAttribute("isAlreadyComputed_", isAlreadyComputed_);
}

END_NAMESPACE_OPENTURNS
