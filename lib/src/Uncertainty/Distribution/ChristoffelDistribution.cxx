//                                               -*- C++ -*-
/**
 *  @brief The Christoffel distribution associated to an orthonormal basis
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
#include <cmath>
#include <algorithm>
#include <functional>
#include <limits>
#include <vector>
#include <utility>
#include "openturns/ChristoffelDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/MultiStart.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/Log.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
#include "openturns/OrthogonalProductFunctionFactory.hxx"
#include "openturns/UniVariateFunctionFamily.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/GradientImplementation.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Unnormalized tensor slice t -> mu_j(t) * S_j(t) for 1D quadrature */
class ChristoffelTensorSliceEvaluation: public EvaluationImplementation
{
public:
  ChristoffelTensorSliceEvaluation(const ChristoffelDistribution & distribution,
                                   const Point & partialProducts,
                                   const UnsignedInteger index)
    : EvaluationImplementation()
    , distribution_(distribution)
    , marginal_(distribution.getMeasure().getMarginal(index))
    , partialProducts_(partialProducts)
    , index_(index)
  {
    // Nothing to do
  }

  ChristoffelTensorSliceEvaluation * clone() const override
  {
    return new ChristoffelTensorSliceEvaluation(*this);
  }

  Point operator()(const Point & point) const override
  {
    const Scalar density = marginal_.computePDF(Point(1, point[0]));
    if (density == 0.0) return Point(1, 0.0);
    return Point(1, density * distribution_.computePartialChristoffel(partialProducts_, index_, point[0]));
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

private:
  const ChristoffelDistribution & distribution_;
  // fixed per slice: built once instead of at every quadrature node
  Distribution marginal_;
  Point partialProducts_;
  UnsignedInteger index_;
}; // class ChristoffelTensorSliceEvaluation

CLASSNAMEINIT(ChristoffelDistribution)

static const Factory<ChristoffelDistribution> Factory_ChristoffelDistribution;

/* Positive, finite safety factor shared by the rejection and tensor samplers */
static Scalar CheckedKnSafetyFactor()
{
  const Scalar safety = ResourceMap::GetAsScalar("ChristoffelDistribution-KnSafetyFactor");
  if (!(safety > 0.0) || !std::isfinite(safety))
    throw InvalidArgumentException(HERE) << "Error: expected a positive finite Christoffel stability safety factor, here factor=" << safety;
  return safety;
}

/* ResourceMap values that change the stability factor estimate */
static String KnCacheKey()
{
  OSS oss;
  oss << ResourceMap::GetAsUnsignedInteger("ChristoffelDistribution-KnSamplingSize") << "|"
      << ResourceMap::GetAsBool("ChristoffelDistribution-ExactKn") << "|"
      << ResourceMap::GetAsUnsignedInteger("ChristoffelDistribution-KnMaximumMultiStart") << "|"
      << ResourceMap::GetAsScalar("ChristoffelDistribution-KnSafetyFactor") << "|"
      << ResourceMap::GetAsString("ChristoffelDistribution-OptimizationAlgorithm");
  return oss;
}

/* Default constructor: 1D Legendre basis of size 1, a valid state for plotting */
ChristoffelDistribution::ChristoffelDistribution()
  : DistributionImplementation()
  , orthogonalBasis_()
  , size_(0)
{
  setName("ChristoffelDistribution");
  const OrthogonalProductPolynomialFactory factory(Collection<Distribution>(1, Uniform(-1.0, 1.0)));
  setOrthogonalBasis(OrthogonalBasis(factory), 1);
}

/* Parameters constructor */
ChristoffelDistribution::ChristoffelDistribution(const OrthogonalBasis & basis,
    const UnsignedInteger size)
  : DistributionImplementation()
  , orthogonalBasis_(basis)
  , size_(size)
{
  setName("ChristoffelDistribution");
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive space dimension, here size=" << size;
  update();
}

/* Internal constructor from a prebuilt finite basis: the caller has already
   built the functions once, so no basis function is built twice */
ChristoffelDistribution::ChristoffelDistribution(const OrthogonalBasis & basis,
    const Basis & finiteBasis,
    const UnsignedInteger size)
  : DistributionImplementation()
  , orthogonalBasis_(basis)
  , size_(size)
{
  setName("ChristoffelDistribution");
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive space dimension, here size=" << size;
  if (finiteBasis.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected a finite basis of size=" << size << ", got size=" << finiteBasis.getSize();
  measure_ = orthogonalBasis_.getMeasure();
  if (!measure_.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the reference measure must be continuous.";
  if (measure_.isDiscrete()) throw InvalidArgumentException(HERE) << "Error: the reference measure must not be discrete.";
  const UnsignedInteger dimension = measure_.getDimension();
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    const Function phi(finiteBasis[i]);
    if (phi.getInputDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: basis function " << i << " has input dimension=" << phi.getInputDimension() << ", expected dimension=" << dimension;
    if (phi.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: basis function " << i << " must be scalar valued.";
  }
  basis_ = finiteBasis;
  finalizeFiniteBasis();
}

/* Virtual constructor */
ChristoffelDistribution * ChristoffelDistribution::clone() const
{
  return new ChristoffelDistribution(*this);
}

/* Comparison operator: value-based basis equality plus measure, size and basis
   values on range probes. The basis equality compares the measure, the
   enumerate function and the factory description; the probes catch factories
   that share a description but build different functions. */
Bool ChristoffelDistribution::operator ==(const ChristoffelDistribution & other) const
{
  if (this == &other) return true;
  if (size_ != other.size_) return false;
  if (getDimension() != other.getDimension()) return false;
  if (!(measure_ == other.measure_)) return false;
  if (!(orthogonalBasis_ == other.orthogonalBasis_)) return false;
  const Interval range(getRange());
  Sample probes(0, getDimension());
  probes.add(range.getLowerBound());
  probes.add(range.getUpperBound());
  probes.add((range.getLowerBound() + range.getUpperBound()) * 0.5);
  for (UnsignedInteger p = 0; p < probes.getSize(); ++p)
  {
    const Point probe(probes[p]);
    for (UnsignedInteger i = 0; i < size_; ++i)
      if (basis_[i](probe)[0] != other.basis_[i](probe)[0]) return false;
  }
  return true;
}

Bool ChristoffelDistribution::equals(const DistributionImplementation & other) const
{
  const ChristoffelDistribution * p_other = dynamic_cast<const ChristoffelDistribution *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String ChristoffelDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << ChristoffelDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " size=" << size_
      << " measure=" << measure_;
  return oss;
}

String ChristoffelDistribution::__str__(const String & offset) const
{
  OSS oss;
  oss << getClassName() << "(size = " << size_ << ", measure = " << measure_.__str__(offset) << ")";
  return oss;
}

/* Orthogonal basis and size accessor */
void ChristoffelDistribution::setOrthogonalBasis(const OrthogonalBasis & basis,
    const UnsignedInteger size)
{
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive space dimension, here size=" << size;
  // validate the reference measure before mutating: update() would throw midway, leaving a half-updated object
  const Distribution measure(basis.getMeasure());
  if (!measure.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the reference measure must be continuous.";
  if (measure.isDiscrete()) throw InvalidArgumentException(HERE) << "Error: the reference measure must not be discrete.";
  const OrthogonalBasis previousBasis(orthogonalBasis_);
  const UnsignedInteger previousSize = size_;
  orthogonalBasis_ = basis;
  size_ = size;
  try
  {
    update();
  }
  catch (...)
  {
    // restore the previous valid state (e.g. basis functions disagreeing with the measure dimension)
    orthogonalBasis_ = previousBasis;
    size_ = previousSize;
    update();
    throw;
  }
}

OrthogonalBasis ChristoffelDistribution::getOrthogonalBasis() const
{
  return orthogonalBasis_;
}

/* Size accessor */
UnsignedInteger ChristoffelDistribution::getSize() const
{
  return size_;
}

/* Reference measure accessor */
Distribution ChristoffelDistribution::getMeasure() const
{
  return measure_;
}

/* Finite basis accessor */
Basis ChristoffelDistribution::getBasis() const
{
  return basis_;
}

/* Per-point direct sum of the squared basis functions */
Scalar ChristoffelDistribution::computeChristoffelCore(const Point & point) const
{
  Scalar kn = 0.0;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    const Scalar value = basis_[i](point)[0];
    kn += value * value;
  }
  return kn;
}

/* Christoffel function on a point, direct sum and stable log in a single basis pass */
Scalar ChristoffelDistribution::computeChristoffelCore(const Point & point,
    Scalar & logCore) const
{
  Point values(size_);
  Scalar kn = 0.0;
  Scalar maxAbs = 0.0;
  UnsignedInteger maxIndex = 0;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    const Scalar value = basis_[i](point)[0];
    values[i] = value;
    kn += value * value;
    const Scalar absValue = std::abs(value);
    if (absValue > maxAbs)
    {
      maxAbs = absValue;
      maxIndex = i;
    }
  }
  if (!(maxAbs > 0.0))
  {
    logCore = SpecFunc::LowestScalar;
    return kn;
  }
  if (!std::isfinite(maxAbs))
  {
    logCore = std::numeric_limits<Scalar>::infinity();
    return kn;
  }
  Scalar reduced = 0.0;
  for (UnsignedInteger i = 0; i < size_; ++i)
    if (i != maxIndex)
    {
      const Scalar ratio = values[i] / maxAbs;
      reduced += ratio * ratio;
    }
  logCore = 2.0 * std::log(maxAbs) + std::log1p(reduced);
  return kn;
}

/* Christoffel function evaluation on a point */
Scalar ChristoffelDistribution::computeChristoffelFunction(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << getDimension() << ", got dimension=" << point.getDimension();
  // Direct core: a one-point sample would allocate and call the TBB policy for
  // no parallel benefit, as computeLogChristoffelFunction already avoids.
  return computeChristoffelCore(point);
}

struct ComputeChristoffelPolicy
{
  const Sample & input_;
  Sample & output_;
  const ChristoffelDistribution & distribution_;

  ComputeChristoffelPolicy(const Sample & input,
                           Sample & output,
                           const ChristoffelDistribution & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {
    // Nothing to do
  }

  inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_(i, 0) = distribution_.computeChristoffelCore(input_[i]);
  }
}; /* end struct ComputeChristoffelPolicy */

/* Christoffel function evaluation on a sample, parallel over points when large */
Sample ChristoffelDistribution::computeChristoffelFunction(const Sample & sample) const
{
  if (sample.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension=" << getDimension() << ", got dimension=" << sample.getDimension();
  const UnsignedInteger sampleSize = sample.getSize();
  Sample values(sampleSize, 1);
  const ComputeChristoffelPolicy policy(sample, values, *this);
  TBBImplementation::ParallelForIf(isParallel() && (sampleSize > 2048), 0, sampleSize, policy, 1024);
  return values;
}

/* Objective of the exact stability factor search: the natural logarithm of the Christoffel function.
   The maximizer is unchanged, but the logarithm keeps the objective tame in the tails where the raw function grows by orders of magnitude, as done in RatioOfUniforms */
class ChristoffelKnEvaluation: public EvaluationImplementation
{
public:
  ChristoffelKnEvaluation(const ChristoffelDistribution * p_distribution)
    : EvaluationImplementation()
    , p_distribution_(p_distribution)
  {
    // numerical range of the search: the stored bound values, which approximate an
    // unbounded support by extreme quantiles
    const Interval range(p_distribution_->getRange());
    lowerBound_ = range.getLowerBound();
    upperBound_ = range.getUpperBound();
  }

  ChristoffelKnEvaluation * clone() const override
  {
    return new ChristoffelKnEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return p_distribution_->getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator()(const Point & point) const override
  {
    if (point.getDimension() != lowerBound_.getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << lowerBound_.getDimension() << ", got dimension=" << point.getDimension();
    // Evaluate on the numerical range: a solver may probe outside it (COBYLA explores
    // infeasible points), where the polynomial products overflow to infinity and the
    // resulting ±inf objective stalls the progress tests of the solvers.
    Point clipped(point);
    for (UnsignedInteger i = 0; i < clipped.getDimension(); ++i) clipped[i] = std::clamp(clipped[i], lowerBound_[i], upperBound_[i]);
    const Scalar value(p_distribution_->computeLogChristoffelFunction(clipped));
    // whatever happens, the objective stays finite: an optimizer never terminates on ±inf
    if (std::isnan(value)) return {-SpecFunc::LogMaxScalar};
    return {std::min(std::max(value, -SpecFunc::LogMaxScalar), SpecFunc::LogMaxScalar)};
  }

private:
  const ChristoffelDistribution * p_distribution_;
  Point lowerBound_;
  Point upperBound_;
}; /* end class ChristoffelKnEvaluation */

/* Gradient of the log-objective.
   For a tensor product of orthogonal polynomials, the factor values and their
   derivatives come from a single forward pass of the three-term recurrence carried by
   the polynomials themselves (rows (a0, a1, a2): P_{k+1}(z) = (a0 z + a1) P_k(z) +
   a2 P_{k-1}(z) with z = a x + b, differentiated as
   P'_{k+1}(z) = a0 P_k(z) + (a0 z + a1) P'_k(z) + a2 P'_{k-1}(z), then d/dx = a d/dz).
   The recurrence evaluates exactly like OrthogonalUniVariatePolynomial::operator()
   (Clenshaw's algorithm), whereas the derivative built from the monomial coefficients
   loses all its digits in the tails of high-degree factors: for Hermite at degree 400
   and x=7 it returns 3.5e38 where the derivative is -5.9e5, which sent the optimizer
   into a scaling collapse. Other bases (functional, composed) fall back to centered
   differences of the objective itself, whose log-values are stable everywhere. */
class ChristoffelKnGradient: public GradientImplementation
{
public:
  ChristoffelKnGradient(const ChristoffelDistribution * p_distribution,
                        const Bool isPolynomialTensor,
                        const Collection<Collection<OrthogonalUniVariatePolynomial>> & tensorPolynomials,
                        const Collection<Indices> & multiIndices,
                        const Evaluation & objective)
    : GradientImplementation()
    , p_distribution_(p_distribution)
    , isPolynomialTensor_(isPolynomialTensor)
    , tensorPolynomials_(tensorPolynomials)
    , multiIndices_(multiIndices)
    , objective_(objective)
    , epsilon_(ResourceMap::GetAsScalar("CenteredFiniteDifferenceGradient-DefaultEpsilon"))
  {
    // Nothing to do
  }

  ChristoffelKnGradient * clone() const override
  {
    return new ChristoffelKnGradient(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return p_distribution_->getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Matrix gradient(const Point & point) const override
  {
    const UnsignedInteger dimension = point.getDimension();
    Matrix result(dimension, 1);
    if (dimension == 0) return result;
    if (!isPolynomialTensor_)
    {
      // Centered differences of the log-objective: its values stay tame wherever the
      // polynomial products overflow, so the direction is usable everywhere.
      const Scalar inverseEpsilon = 0.5 / epsilon_;
      for (UnsignedInteger i = 0; i < dimension; ++i)
      {
        Point x(point);
        x[i] += epsilon_;
        const Scalar plus(objective_(x)[0]);
        x[i] -= 2.0 * epsilon_;
        const Scalar minus(objective_(x)[0]);
        result(i, 0) = (plus - minus) * inverseEpsilon;
      }
      return result;
    }
    // One forward recurrence pass per coordinate: values and derivatives of every
    // univariate factor, for all degrees at once. Clip the point exactly like the
    // objective does: the optimizer may probe outside the numerical range, where
    // the raw recurrence can overflow and the clipped objective is constant.
    Point clipped(point);
    const Interval range(p_distribution_->getRange());
    const Point lowerBound(range.getLowerBound());
    const Point upperBound(range.getUpperBound());
    for (UnsignedInteger k = 0; k < dimension; ++k)
      clipped[k] = std::clamp(clipped[k], lowerBound[k], upperBound[k]);
    Collection<Point> values(dimension);
    Collection<Point> derivatives(dimension);
    for (UnsignedInteger k = 0; k < dimension; ++k)
    {
      const Collection<OrthogonalUniVariatePolynomial> & polynomials(tensorPolynomials_[k]);
      const UnsignedInteger maxDegree = polynomials.getSize() - 1;
      // the recurrence of the polynomial of maximal degree carries the rows 0..maxDegree-1 of the whole family
      const Sample rows(polynomials[maxDegree].getRecurrenceCoefficients());
      const Scalar a(polynomials[maxDegree].getA());
      const Scalar b(polynomials[maxDegree].getB());
      const Scalar z(a * clipped[k] + b);
      Point value(maxDegree + 1);
      Point derivative(maxDegree + 1);
      value[0] = 1.0;
      derivative[0] = 0.0;
      Scalar previous(0.0);
      Scalar current(1.0);
      Scalar previousDerivative(0.0);
      Scalar currentDerivative(0.0);
      for (UnsignedInteger degree = 0; degree < maxDegree; ++degree)
      {
        const Scalar a0(rows(degree, 0));
        const Scalar a1(rows(degree, 1));
        const Scalar a2(rows(degree, 2));
        const Scalar linear(a0 * z + a1);
        const Scalar next(linear * current + a2 * previous);
        const Scalar nextDerivative(a0 * current + linear * currentDerivative + a2 * previousDerivative);
        previous = current;
        current = next;
        previousDerivative = currentDerivative;
        currentDerivative = nextDerivative;
        value[degree + 1] = current;
        derivative[degree + 1] = a * currentDerivative;
      }
      values[k] = value;
      derivatives[k] = derivative;
    }
    // numerator_i = sum_j h_j d_i h_j over the basis functions, denominator = sum_j h_j^2,
    // gradient of the log = 2 numerator / denominator
    Point numerator(dimension, 0.0);
    Scalar denominator = 0.0;
    const UnsignedInteger size(p_distribution_->getSize());
    for (UnsignedInteger basisIndex = 0; basisIndex < size; ++basisIndex)
    {
      const Indices & index(multiIndices_[basisIndex]);
      Scalar h = 1.0;
      for (UnsignedInteger k = 0; k < dimension; ++k) h *= values[k][index[k]];
      denominator += h * h;
      for (UnsignedInteger i = 0; i < dimension; ++i)
      {
        Scalar partial(derivatives[i][index[i]]);
        for (UnsignedInteger k = 0; k < dimension; ++k)
          if (k != i) partial *= values[k][index[k]];
        numerator[i] += h * partial;
      }
    }
    // no usable direction: a null gradient lets the optimizer stop there
    if (!(denominator > 0.0) || !std::isfinite(denominator)) return result;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      const Scalar value(2.0 * numerator[i] / denominator);
      result(i, 0) = std::isfinite(value) ? value : 0.0;
    }
    return result;
  }

private:
  const ChristoffelDistribution * p_distribution_;
  const Bool isPolynomialTensor_;
  const Collection<Collection<OrthogonalUniVariatePolynomial>> tensorPolynomials_;
  const Collection<Indices> multiIndices_;
  const Evaluation objective_;
  const Scalar epsilon_;
}; /* end class ChristoffelKnGradient */

/* Candidate points for the stability factor search: a low-discrepancy sequence pushed through the inverse Rosenblatt transform of the measure.
   The candidates cover the support much more evenly than an i.i.d. sample of the same size, and the same points feed both the Monte-Carlo estimate and the multi-start seeds of the exact search. A direct sample of the measure is only a fallback for measures without a usable sequential conditional quantile function. */
Sample ChristoffelDistribution::generateKnCandidates(const UnsignedInteger size) const
{
  const UnsignedInteger dimension = getDimension();
  Sample candidates(0, dimension);
  try
  {
    SobolSequence sequence(dimension);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Point point(measure_.computeSequentialConditionalQuantile(sequence.generate()));
      // the extreme quantiles are infinite on an unbounded measure: skip those points
      Bool feasible = true;
      for (UnsignedInteger j = 0; j < dimension; ++j)
        if (!std::isfinite(point[j])) feasible = false;
      if (feasible) candidates.add(point);
    }
  }
  catch (const Exception &)
  {
    LOGDEBUG("No usable sequential conditional quantile function for the reference measure, using a natural sample instead.");
  }
  if (candidates.getSize() == 0) return measure_.getSample(size);
  return candidates;
}

/* Stability factor estimate */
Scalar ChristoffelDistribution::computeKn() const
{
  std::lock_guard<std::recursive_mutex> lock(*cacheMutex_);
  // Fail before the expensive candidate and optimizer searches
  const Scalar safety = CheckedKnSafetyFactor();
  const String cacheKey(KnCacheKey());
  if (!isAlreadyComputedKn_ || (knCacheKey_ != cacheKey))
  {
    const UnsignedInteger samplingSize = ResourceMap::GetAsUnsignedInteger("ChristoffelDistribution-KnSamplingSize");
    if (samplingSize == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive Kn sampling size.";
    const Sample candidates(generateKnCandidates(samplingSize));
    const Sample values(computeChristoffelFunction(candidates));
    Scalar knMax = 0.0;
    for (UnsignedInteger i = 0; i < values.getSize(); ++i) knMax = std::max(knMax, values(i, 0));
    if (!std::isfinite(knMax)) throw InternalException(HERE) << "Error: the Christoffel function overflows on the stability factor candidates; cannot build a finite rejection envelope.";
    // Optional exact refinement: bounded maximization of the log-Christoffel function over the numerical range, multi-started from the best candidates as in RatioOfUniforms::initialize(). The multi-start is a coverage argument only: a local optimum may be missed, hence the safety factor below.
    if (ResourceMap::GetAsBool("ChristoffelDistribution-ExactKn"))
    {
      try
      {
        const UnsignedInteger maximumMultiStart = ResourceMap::GetAsUnsignedInteger("ChristoffelDistribution-KnMaximumMultiStart");
        const Interval range(getRange());
        // The solvers only enforce the components flagged finite, and a measure with an
        // unbounded support keeps infinite flags on bounds storing numerical (extreme
        // quantile) values: ±7.65 for Normal, 38.9 for Gamma(3, 1). Rebuild the box from
        // the stored values so both solvers search exactly the numerical range the
        // envelope must bound.
        const Interval searchBox(range.getLowerBound(), range.getUpperBound());
        // rank the candidates by decreasing Christoffel value, best first
        std::vector<std::pair<Scalar, UnsignedInteger> > ranked(values.getSize());
        for (UnsignedInteger i = 0; i < values.getSize(); ++i) ranked[i] = std::make_pair(values(i, 0), i);
        const UnsignedInteger rankNumber = std::min(maximumMultiStart, static_cast<UnsignedInteger>(ranked.size()));
        std::partial_sort(ranked.begin(), ranked.begin() + rankNumber, ranked.end(), std::greater<std::pair<Scalar, UnsignedInteger> >());
        // the optimizer works inside the numerical range: seed it there only
        Sample startingPoints(0, getDimension());
        for (UnsignedInteger k = 0; k < rankNumber && startingPoints.getSize() < maximumMultiStart; ++k)
        {
          const Point candidate(candidates[ranked[k].second]);
          if (std::isfinite(ranked[k].first) && searchBox.contains(candidate)) startingPoints.add(candidate);
        }
        if (startingPoints.getSize() > 0)
        {
          const Evaluation evaluation(new ChristoffelKnEvaluation(this));
          const Function objective(evaluation,
                                   new ChristoffelKnGradient(this, usePolynomialTensor_, tensorPolynomials_, multiIndices_, evaluation),
                                   new CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar("CenteredFiniteDifferenceHessian-DefaultEpsilon"), evaluation));
          OptimizationProblem problem(objective);
          problem.setMinimization(false);
          problem.setBounds(searchBox);
          OptimizationAlgorithm algorithm(OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("ChristoffelDistribution-OptimizationAlgorithm")));
          algorithm.setProblem(problem);
          // exhausting the evaluation budget still yields a usable point: keep it instead of discarding the whole refinement, as the estimation factories do
          algorithm.setCheckStatus(false);
          MultiStart multistart(algorithm, startingPoints);
          multistart.run();
          LOGDEBUG(OSS() << "Exact stability factor search used " << multistart.getResult().getCallsNumber() << " evaluations");
          // evaluate at the optimum clipped back to the numerical range: the optimizer may report a point marginally outside the bounds, and the envelope must bound the Christoffel function on the range
          Point optimalPoint(multistart.getResult().getOptimalPoint());
          if (optimalPoint.getDimension() == getDimension())
          {
            const Point lowerBound(range.getLowerBound());
            const Point upperBound(range.getUpperBound());
            for (UnsignedInteger j = 0; j < optimalPoint.getDimension(); ++j)
              optimalPoint[j] = std::clamp(optimalPoint[j], lowerBound[j], upperBound[j]);
            const Scalar knOptimal = computeChristoffelFunction(optimalPoint);
            if (std::isfinite(knOptimal))
              knMax = std::max(knMax, knOptimal);
            else
            {
              // The raw value may overflow while its logarithm is still finite:
              // use the log value when it can be exponentiated, otherwise keep
              // the finite Monte-Carlo estimate instead of silently returning
              // an infinite envelope.
              const Scalar logOptimal = computeLogChristoffelFunction(optimalPoint);
              if (std::isfinite(logOptimal))
              {
                const Scalar expOptimal = std::exp(logOptimal);
                if (std::isfinite(expOptimal)) knMax = std::max(knMax, expOptimal);
              }
            }
          }
        }
      }
      catch (const Exception & ex)
      {
        // keep the Monte-Carlo estimate: the samplers refresh the envelope on the fly anyway
        LOGDEBUG(OSS() << "Could not compute the exact stability factor, keeping the Monte-Carlo estimate: " << ex.what());
      }
    }
    const Scalar estimate = knMax * safety;
    if (!(estimate > 0.0) || !std::isfinite(estimate))
      throw InternalException(HERE) << "Error: the Christoffel stability factor estimate is not positive and finite, estimate=" << estimate;
    knEstimate_ = estimate;
    knCacheKey_ = cacheKey;
    isAlreadyComputedKn_ = true;
  }
  return knEstimate_;
}

/* PDF evaluation */
Scalar ChristoffelDistribution::computePDF(const Point & point) const
{
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Error: cannot evaluate the PDF of an uninitialized distribution.";
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  const Scalar referencePDF = measure_.computePDF(point);
  if (referencePDF == 0.0) return 0.0;
  // direct core and stable log in a single basis pass: no second evaluation
  // when the raw core overflows in the tails of high-degree factors
  Scalar logCore = 0.0;
  const Scalar core = computeChristoffelCore(point, logCore);
  const Scalar pdf = referencePDF * core / size_;
  if ((pdf > 0.0) && std::isfinite(pdf)) return pdf;
  // Either the raw core or the product with the reference density overflowed
  // (or underflowed): use the log value already computed in the same pass.
  if (!std::isfinite(logCore)) return std::exp(logCore);
  return std::exp(std::log(referencePDF) + logCore - std::log(1.0 * size_));
}

/* Log-PDF evaluation */
Scalar ChristoffelDistribution::computeLogPDF(const Point & point) const
{
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Error: cannot evaluate the log-PDF of an uninitialized distribution.";
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  const Scalar referenceLogPDF = measure_.computeLogPDF(point);
  const Scalar logKn = computeLogChristoffelCore(point);
  if (!(logKn > SpecFunc::LowestScalar)) return SpecFunc::LowestScalar;
  return referenceLogPDF + logKn - std::log(1.0 * size_);
}

/* Stable log of the Christoffel function on a point */
Scalar ChristoffelDistribution::computeLogChristoffelFunction(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << getDimension() << ", got dimension=" << point.getDimension();
  // direct core call: the sample wrapper would build a one-point sample for nothing
  return computeLogChristoffelCore(point);
}

struct ComputeLogChristoffelPolicy
{
  const Sample & input_;
  Sample & output_;
  const ChristoffelDistribution & distribution_;

  ComputeLogChristoffelPolicy(const Sample & input,
                              Sample & output,
                              const ChristoffelDistribution & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {
    // Nothing to do
  }

  inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_(i, 0) = distribution_.computeLogChristoffelCore(input_[i]);
  }
}; /* end struct ComputeLogChristoffelPolicy */

/* Stable log of the Christoffel function on a sample, parallel over points when large */
Sample ChristoffelDistribution::computeLogChristoffelFunction(const Sample & sample) const
{
  if (sample.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension=" << getDimension() << ", got dimension=" << sample.getDimension();
  const UnsignedInteger sampleSize = sample.getSize();
  Sample values(sampleSize, 1);
  const ComputeLogChristoffelPolicy policy(sample, values, *this);
  TBBImplementation::ParallelForIf(isParallel() && (sampleSize > 2048), 0, sampleSize, policy, 1024);
  return values;
}

/* Per-point max factoring with log1p, callers check the dimension */
Scalar ChristoffelDistribution::computeLogChristoffelCore(const Point & point) const
{
  Point values(size_);
  Scalar maxAbs = 0.0;
  UnsignedInteger maxIndex = 0;
  for (UnsignedInteger j = 0; j < size_; ++j)
  {
    const Scalar value = basis_[j](point)[0];
    values[j] = value;
    const Scalar absValue = std::abs(value);
    if (absValue > maxAbs)
    {
      maxAbs = absValue;
      maxIndex = j;
    }
  }
  if (!(maxAbs > 0.0)) return SpecFunc::LowestScalar;
  // An overflowed factor gives an infinite log-value; the exact search clips
  // it to LogMaxScalar, and the PDF log-value saturates honestly.
  if (!std::isfinite(maxAbs)) return std::numeric_limits<Scalar>::infinity();
  Scalar reduced = 0.0;
  for (UnsignedInteger j = 0; j < size_; ++j)
    if (j != maxIndex)
    {
      const Scalar ratio = values[j] / maxAbs;
      reduced += ratio * ratio;
    }
  return 2.0 * std::log(maxAbs) + std::log1p(reduced);
}

/* Conditional PDF of Xj | X0..Xj-1, closed slice form in the tensor case */
Scalar ChristoffelDistribution::computeConditionalPDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger j = y.getDimension();
  if (j >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (!isTensorProduct_ || !measure_.hasIndependentCopula())
    return DistributionImplementation::computeConditionalPDF(x, y);
  // Partial products from the conditioning values, normalizer by orthonormality
  Point partial(size_, 1.0);
  for (UnsignedInteger k = 0; k < j; ++k) updatePartialProducts(partial, k, y[k]);
  return computeConditionalPDFFromPartial(x, j, partial);
}

/* Conditional PDF from partial products already accumulated for dimensions 0..j-1 */
Scalar ChristoffelDistribution::computeConditionalPDFFromPartial(const Scalar x,
    const UnsignedInteger j,
    const Point & partialProducts) const
{
  Scalar normalizer = 0.0;
  for (UnsignedInteger basisIndex = 0; basisIndex < size_; ++basisIndex) normalizer += partialProducts[basisIndex];
  if (!(normalizer > 0.0) || !std::isfinite(normalizer)) return 0.0;
  const Distribution marginalJ(measure_.getMarginal(j));
  const Scalar numerator = marginalJ.computePDF(x) * computePartialChristoffel(partialProducts, j, x);
  return numerator / normalizer;
}

/* Conditional CDF of Xj | X0..Xj-1, slice quadrature in the tensor case */
Scalar ChristoffelDistribution::computeConditionalCDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger j = y.getDimension();
  if (j >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (!isTensorProduct_ || !measure_.hasIndependentCopula())
    return DistributionImplementation::computeConditionalCDF(x, y);
  Point partial(size_, 1.0);
  for (UnsignedInteger k = 0; k < j; ++k) updatePartialProducts(partial, k, y[k]);
  return computeConditionalCDFFromPartial(x, j, partial);
}

/* Conditional CDF from partial products already accumulated for dimensions 0..j-1 */
Scalar ChristoffelDistribution::computeConditionalCDFFromPartial(const Scalar x,
    const UnsignedInteger j,
    const Point & partialProducts) const
{
  const Distribution marginalJ(measure_.getMarginal(j));
  const Interval rangeJ(marginalJ.getRange());
  const Scalar lower = rangeJ.getLowerBound()[0];
  const Scalar upper = rangeJ.getUpperBound()[0];
  if (x <= lower) return 0.0;
  if (x >= upper) return 1.0;
  Scalar normalizer = 0.0;
  for (UnsignedInteger basisIndex = 0; basisIndex < size_; ++basisIndex) normalizer += partialProducts[basisIndex];
  if (!(normalizer > 0.0) || !std::isfinite(normalizer)) return 0.0;
  const Function slice(ChristoffelTensorSliceEvaluation(*this, partialProducts, j));
  Scalar error = -1.0;
  Point ai;
  Point bi;
  Sample fi;
  Point ei;
  const Scalar integral = GaussKronrod().integrate(slice, lower, x, error, ai, bi, fi, ei)[0];
  return SpecFunc::Clip01(integral / normalizer);
}

/* Sequential conditional PDF fanning out to the scalar version */
Point ChristoffelDistribution::computeSequentialConditionalPDF(const Point & x) const
{
  if (x.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << getDimension() << ", got dimension=" << x.getDimension();
  if (!isTensorProduct_ || !measure_.hasIndependentCopula())
    return DistributionImplementation::computeSequentialConditionalPDF(x);
  Point result(getDimension());
  Point partial(size_, 1.0);
  for (UnsignedInteger j = 0; j < getDimension(); ++j)
  {
    result[j] = computeConditionalPDFFromPartial(x[j], j, partial);
    updatePartialProducts(partial, j, x[j]);
  }
  return result;
}

/* Sequential conditional CDF fanning out to the scalar version */
Point ChristoffelDistribution::computeSequentialConditionalCDF(const Point & x) const
{
  if (x.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << getDimension() << ", got dimension=" << x.getDimension();
  if (!isTensorProduct_ || !measure_.hasIndependentCopula())
    return DistributionImplementation::computeSequentialConditionalCDF(x);
  Point result(getDimension());
  Point partial(size_, 1.0);
  for (UnsignedInteger j = 0; j < getDimension(); ++j)
  {
    result[j] = computeConditionalCDFFromPartial(x[j], j, partial);
    updatePartialProducts(partial, j, x[j]);
  }
  return result;
}

/* Univariate factor of a basis function along dimension k at x */
Scalar ChristoffelDistribution::computeTensorFactor(const UnsignedInteger basisIndex,
    const UnsignedInteger k,
    const Scalar x) const
{
  const UnsignedInteger degree = multiIndices_[basisIndex][k];
  if (usePolynomialTensor_) return tensorPolynomials_[k][degree](x);
  return tensorFunctions_[k][degree](x);
}

/* Partial Christoffel sum over t in dimension j given partial products */
Scalar ChristoffelDistribution::computePartialChristoffel(const Point & partialProducts,
    const UnsignedInteger j,
    const Scalar t) const
{
  Scalar total = 0.0;
  for (UnsignedInteger basisIndex = 0; basisIndex < size_; ++basisIndex)
  {
    const Scalar factor = computeTensorFactor(basisIndex, j, t);
    total += partialProducts[basisIndex] * factor * factor;
  }
  return total;
}

/* Update the tensor partial products with the factor of dimension j */
void ChristoffelDistribution::updatePartialProducts(Point & partialProducts,
    const UnsignedInteger j,
    const Scalar x) const
{
  for (UnsignedInteger basisIndex = 0; basisIndex < size_; ++basisIndex)
  {
    const Scalar factor = computeTensorFactor(basisIndex, j, x);
    partialProducts[basisIndex] *= factor * factor;
  }
}

/* One draw by sequential 1D conditional sampling (tensor independent case) */
Point ChristoffelDistribution::drawSequentialTensor() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger gridSize = ResourceMap::GetAsUnsignedInteger("ChristoffelDistribution-SliceGridSize");
  if (gridSize == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive slice grid size.";
  const Scalar safety = CheckedKnSafetyFactor();
  Point point(dimension);
  Point partial(size_, 1.0);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    const Distribution marginalJ(measure_.getMarginal(j));
    const Interval rangeJ(marginalJ.getRange());
    const Scalar lower = rangeJ.getLowerBound()[0];
    const Scalar upper = rangeJ.getUpperBound()[0];
    // Grid abscissae shared by the envelope scan. The proposal is marginalJ,
    // so the rejection ratio is S_j / max S_j: the marginal density cancels
    // and must not appear in the envelope or in the acceptance test.
    Sample grid(gridSize + 1, 1);
    for (UnsignedInteger g = 0; g <= gridSize; ++g) grid(g, 0) = lower + (upper - lower) * g / gridSize;
    Point sliceTotals(gridSize + 1, 0.0);
    for (UnsignedInteger basisIndex = 0; basisIndex < size_; ++basisIndex)
    {
      const UnsignedInteger degree = multiIndices_[basisIndex][j];
      // batch the univariate factor evaluation over the grid (elementwise loop, same values as scalar calls)
      Sample factorValues;
      // NOTE: OrthogonalUniVariatePolynomial::operator()(Scalar) hides the base Sample overload: qualify it explicitly
      if (usePolynomialTensor_) factorValues = tensorPolynomials_[j][degree].UniVariateFunctionImplementation::operator()(grid);
      else factorValues = tensorFunctions_[j][degree](grid);
      for (UnsignedInteger g = 0; g <= gridSize; ++g)
      {
        const Scalar factor = factorValues(g, 0);
        sliceTotals[g] += partial[basisIndex] * factor * factor;
      }
    }
    Scalar envelope = 0.0;
    for (UnsignedInteger g = 0; g <= gridSize; ++g)
    {
      const Scalar density = sliceTotals[g];
      if (!std::isfinite(density))
        throw InternalException(HERE) << "Error: non-finite tensor slice density along dimension " << j << ".";
      if (density > envelope) envelope = density;
    }
    envelope *= safety;
    if (!(envelope > 0.0) || !std::isfinite(envelope))
      throw InternalException(HERE) << "Error: null or non-finite slice envelope along dimension " << j << ".";
    // Adaptive rejection with restart on envelope exceedance
    while (true)
    {
      const Scalar proposal = (marginalJ.getRealization())[0];
      if (!std::isfinite(proposal))
        throw InternalException(HERE) << "Error: non-finite proposal along dimension " << j << ".";
      const Scalar density = computePartialChristoffel(partial, j, proposal);
      if (!std::isfinite(density) || !(density >= 0.0))
        throw InternalException(HERE) << "Error: non-finite conditional slice density along dimension " << j << ".";
      if (density > envelope)
      {
        envelope = density * safety;
        if (!std::isfinite(envelope))
          throw InternalException(HERE) << "Error: non-finite slice envelope after an exceedance along dimension " << j << ".";
        continue;
      }
      if (RandomGenerator::Generate() * envelope <= density)
      {
        point[j] = proposal;
        break;
      }
    }
    updatePartialProducts(partial, j, point[j]);
  }
  return point;
}

/* One draw by rejection from the reference with the Kn envelope */
Point ChristoffelDistribution::drawByRejection() const
{
  const Scalar safety = CheckedKnSafetyFactor();
  Scalar envelope = computeKn();
  if (!(envelope > 0.0) || !std::isfinite(envelope))
    throw InternalException(HERE) << "Error: null or non-finite Christoffel envelope.";
  while (true)
  {
    const Point proposal(measure_.getRealization());
    const Scalar kn = computeChristoffelCore(proposal);
    if (!std::isfinite(kn) || !(kn >= 0.0))
      throw InternalException(HERE) << "Error: non-finite Christoffel value during rejection sampling.";
    if (kn > envelope)
    {
      envelope = kn * safety;
      if (!std::isfinite(envelope))
        throw InternalException(HERE) << "Error: non-finite Christoffel envelope after an exceedance.";
      {
        std::lock_guard<std::recursive_mutex> lock(*cacheMutex_);
        knEstimate_ = std::max(knEstimate_, envelope);
      }
      continue;
    }
    if (RandomGenerator::Generate() * envelope <= kn) return proposal;
  }
}

/* Sampler dispatch: choose between the internal ratio-of-uniforms sampler and
  rejection from the reference by their measured acceptance rates. Rejection
  accepts with exactly size_/kn for the envelope the rejection loop actually
  uses, and the ratio-of-uniforms sampler reports the rate its setup measured,
  so the comparison is between the rates the two branches really have. The
  dimension key only guards the setup attempt and is re-read here, so lowering
  it after construction takes effect immediately. */
Bool ChristoffelDistribution::preferRatioOfUniforms() const
{
  if (getDimension() > ResourceMap::GetAsUnsignedInteger("ChristoffelDistribution-RatioUniformMaxDimension")) return false;
  if (!hasRatioUniformsSetup_) return false;
  const Scalar rejectionAcceptance = static_cast<Scalar>(size_) / computeKn();
  const Scalar ratioUniformsAcceptance = sampler_.getAcceptanceRatio();
  return ratioUniformsAcceptance > rejectionAcceptance;
}

/* Realization: tensor sequential, then the branch with the larger acceptance */
Point ChristoffelDistribution::getRealization() const
{
  if (size_ == 0) return DistributionImplementation::getRealization();
  if (isTensorProduct_ && measure_.hasIndependentCopula()) return drawSequentialTensor();
  if (preferRatioOfUniforms()) return sampler_.getRealization();
  return drawByRejection();
}

/* Sample: batched ratio-of-uniforms when selected, sequential loop otherwise */
Sample ChristoffelDistribution::getSample(const UnsignedInteger size) const
{
  if (size_ != 0 && !isTensorProduct_ && preferRatioOfUniforms())
    return sampler_.getSample(size);
  return DistributionImplementation::getSample(size);
}

/* Parameters accessors: no parametric representation */
ChristoffelDistribution::PointWithDescriptionCollection ChristoffelDistribution::getParametersCollection() const
{
  throw NotYetImplementedException(HERE) << "In ChristoffelDistribution::getParametersCollection() const";
}

void ChristoffelDistribution::setParametersCollection(const PointCollection &)
{
  throw NotYetImplementedException(HERE) << "In ChristoffelDistribution::setParametersCollection(const PointCollection & parametersCollection)";
}

/* Numerical range: the reference range */
void ChristoffelDistribution::computeRange()
{
  setRange(measure_.getRange());
}

/* Rebuild the derived members */
void ChristoffelDistribution::update()
{
  measure_ = orthogonalBasis_.getMeasure();
  if (!measure_.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the reference measure must be continuous.";
  if (measure_.isDiscrete()) throw InvalidArgumentException(HERE) << "Error: the reference measure must not be discrete.";
  const UnsignedInteger dimension = measure_.getDimension();
  Basis::FunctionCollection coll(size_);
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    const Function phi(orthogonalBasis_.build(i));
    if (phi.getInputDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: basis function " << i << " has input dimension=" << phi.getInputDimension() << ", expected dimension=" << dimension;
    if (phi.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: basis function " << i << " must be scalar valued.";
    coll[i] = phi;
  }
  basis_ = Basis(coll);
  finalizeFiniteBasis();
}

/* Derive the range, tensor caches, parallel flag and sampler from basis_ and measure_ */
void ChristoffelDistribution::finalizeFiniteBasis()
{
  const UnsignedInteger dimension = measure_.getDimension();
  // The parallel paths (computeChristoffelFunction, computeLogChristoffelFunction
  // and the PDF/mean/covariance overloads they feed) evaluate the basis through
  // the Function objects of basis_, so parallelism requires the reference measure
  // and all of them to be parallel, as in PosteriorDistribution. The cached
  // univariate factors of detectTensorProduct() only serve computeTensorFactor,
  // which runs in the sequential conditional and drawing paths, never under
  // ParallelForIf, so they do not constrain this flag.
  Bool parallel = measure_.getImplementation()->isParallel();
  for (UnsignedInteger i = 0; i < size_; ++i)
    parallel = parallel && basis_[i].getImplementation()->isParallel();
  setParallel(parallel);
  setDimension(dimension);
  setDescription(measure_.getDescription());
  computeRange();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyComputedKn_ = false;
  knCacheKey_.clear();
  detectTensorProduct();
  // Internal RatioOfUniforms sampler, see https://en.wikipedia.org/wiki/Ratio_of_uniforms
  // Set up eagerly for non-tensor references within the dimension guard: the
  // acceptance comparison of preferRatioOfUniforms() runs at the first draw,
  // once the stability factor is known. Skipped for tensor independent
  // references (sequential path) and above the dimension cap (rejection path),
  // where its multi-start optimization is pure overhead. A setup failure (no
  // feasible starting point, invalid key value) leaves the sampler unset and
  // sampling falls back to rejection instead of propagating.
  sampler_ = RatioOfUniforms();
  hasRatioUniformsSetup_ = false;
  if (!(isTensorProduct_ && measure_.hasIndependentCopula()) && getDimension() <= ResourceMap::GetAsUnsignedInteger("ChristoffelDistribution-RatioUniformMaxDimension"))
  {
    try
    {
      sampler_.setOptimizationAlgorithm(OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("ChristoffelDistribution-OptimizationAlgorithm")));
      sampler_.setCandidateNumber(ResourceMap::GetAsUnsignedInteger("ChristoffelDistribution-RatioUniformCandidateNumber"));
      sampler_.setLogUnscaledPDFAndRange(getLogPDF(), getRange(), true);
      hasRatioUniformsSetup_ = true;
    }
    catch (const Exception & exc)
    {
      sampler_ = RatioOfUniforms();
      LOGDEBUG(OSS() << "ChristoffelDistribution: ratio-of-uniforms setup failed (" << exc.what() << "), sampling will use rejection");
    }
  }
}

/* Detect a tensor-product factory and cache univariate evaluators */
void ChristoffelDistribution::detectTensorProduct()
{
  isTensorProduct_ = false;
  usePolynomialTensor_ = false;
  tensorPolynomials_ = Collection<Collection<OrthogonalUniVariatePolynomial>>();
  tensorFunctions_ = Collection<Collection<UniVariateFunction>>();
  multiIndices_ = Collection<Indices>();
  const UnsignedInteger dimension = getDimension();
  const OrthogonalProductPolynomialFactory * p_polynomialFactory = dynamic_cast<const OrthogonalProductPolynomialFactory *>(orthogonalBasis_.getImplementation().get());
  if (p_polynomialFactory != nullptr)
  {
    const OrthogonalProductPolynomialFactory::PolynomialFamilyCollection families(p_polynomialFactory->getPolynomialFamilyCollection());
    if (families.getSize() == dimension)
    {
      enumerateFunction_ = p_polynomialFactory->getEnumerateFunction();
      // enumerate each multi-index once: it is also what the tensor-factor and gradient loops read
      multiIndices_ = Collection<Indices>(size_);
      for (UnsignedInteger basisIndex = 0; basisIndex < size_; ++basisIndex) multiIndices_[basisIndex] = enumerateFunction_(basisIndex);
      Collection<Collection<OrthogonalUniVariatePolynomial>> cache(dimension);
      for (UnsignedInteger k = 0; k < dimension; ++k)
      {
        UnsignedInteger maxDegree = 0;
        for (UnsignedInteger basisIndex = 0; basisIndex < size_; ++basisIndex) maxDegree = std::max(maxDegree, multiIndices_[basisIndex][k]);
        Collection<OrthogonalUniVariatePolynomial> perDegree(maxDegree + 1);
        for (UnsignedInteger degree = 0; degree <= maxDegree; ++degree) perDegree[degree] = families[k].build(degree);
        cache[k] = perDegree;
      }
      tensorPolynomials_ = cache;
      usePolynomialTensor_ = true;
      isTensorProduct_ = true;
    }
    return;
  }
  const OrthogonalProductFunctionFactory * p_functionFactory = dynamic_cast<const OrthogonalProductFunctionFactory *>(orthogonalBasis_.getImplementation().get());
  if (p_functionFactory != nullptr)
  {
    const OrthogonalProductFunctionFactory::FunctionFamilyCollection families(p_functionFactory->getFunctionFamilyCollection());
    if (families.getSize() == dimension)
    {
      enumerateFunction_ = p_functionFactory->getEnumerateFunction();
      multiIndices_ = Collection<Indices>(size_);
      for (UnsignedInteger basisIndex = 0; basisIndex < size_; ++basisIndex) multiIndices_[basisIndex] = enumerateFunction_(basisIndex);
      Collection<Collection<UniVariateFunction>> cache(dimension);
      for (UnsignedInteger k = 0; k < dimension; ++k)
      {
        const UniVariateFunctionFamily family(*families[k].getImplementation());
        UnsignedInteger maxDegree = 0;
        for (UnsignedInteger basisIndex = 0; basisIndex < size_; ++basisIndex) maxDegree = std::max(maxDegree, multiIndices_[basisIndex][k]);
        Collection<UniVariateFunction> perDegree(maxDegree + 1);
        for (UnsignedInteger degree = 0; degree <= maxDegree; ++degree) perDegree[degree] = family.build(degree);
        cache[k] = perDegree;
      }
      tensorFunctions_ = cache;
      usePolynomialTensor_ = false;
      isTensorProduct_ = true;
    }
  }
}

/* Method save() stores the object through the StorageManager */
void ChristoffelDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("orthogonalBasis_", orthogonalBasis_);
  adv.saveAttribute("size_", size_);
}

/* Method load() reloads the object from the StorageManager */
void ChristoffelDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("orthogonalBasis_", orthogonalBasis_);
  adv.loadAttribute("size_", size_);
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Error: cannot reload a distribution with null space dimension.";
  update();
}

END_NAMESPACE_OPENTURNS
