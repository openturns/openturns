//                                               -*- C++
/**
 *  @brief The PushForwardDistribution distribution
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
#include <vector>
#include "openturns/PushForwardDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ConstantFunction.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/IteratedQuadrature.hxx"
#include "openturns/LeastSquaresEquationsSolver.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Sample.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PushForwardDistribution)

static const Factory<PushForwardDistribution> Factory_PushForwardDistribution;

namespace PushForwardDistributionFunctions
{

/**
 * Integrand of the co-area formula for a fibre of any dimension: the density
 * of the antecedent at the point u(t) = origin + kernel * t of the fibre
 * f(u) = point, divided by the p-dimensional Jacobian. The columns of the
 * kernel form an orthonormal basis of the kernel of the Jacobian matrix (a
 * single column for a one dimensional fibre), so that the Hausdorff element
 * along the fibre is exactly the Lebesgue element dt and no metric factor is
 * needed in the integrand.
 */
class CoAreaIntegrand: public EvaluationImplementation
{
public:
  CoAreaIntegrand(const Point & origin,
                  const Matrix & kernel,
                  const Distribution & antecedent,
                  const Function & function,
                  const Point & point,
                  const Scalar jacobian,
                  const Scalar residualTolerance)
    : EvaluationImplementation()
    , origin_(origin)
    , kernel_(kernel)
    , antecedent_(antecedent)
    , function_(function)
    , point_(point)
    , jacobian_(jacobian)
    , residualTolerance_(residualTolerance)
  {
    // Nothing to do
  }

  CoAreaIntegrand * clone() const override
  {
    return new CoAreaIntegrand(*this);
  }

  Point operator() (const Point & parameter) const override
  {
    const Point x(origin_ + kernel_ * parameter);
    const Scalar antecedentPDF = antecedent_.computePDF(x);
    // Outside the support of the antecedent the fibre carries no mass. The
    // residual is not tested there: only the region carrying mass matters
    if (!(antecedentPDF > 0.0))
      return Point(1, 0.0);
    // The co-area formula is implemented for an affine function only, in
    // which case the image of the whole fibre is the point itself. A curved
    // level set betrays itself by a residual that no rounding error can
    // explain, and integrating along the straight fibre regardless would
    // return a density that is silently wrong
    const Point image(function_(x));
    if ((image - point_).norm() > residualTolerance_ * std::max(1.0, std::max(point_.norm(), image.norm())))
      throw NotYetImplementedException(HERE) << "Error: the co-area formula is only implemented for an affine function, here f(" << x << ")=" << image << " whereas the density is evaluated at " << point_ << ", so that the fibre is curved or the function is not affine";
    return Point(1, antecedentPDF / jacobian_);
  }

  UnsignedInteger getInputDimension() const override
  {
    return kernel_.getNbColumns();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  String __repr__() const override
  {
    OSS oss(true);
    oss << "class=CoAreaIntegrand"
        << " origin=" << origin_.__repr__()
        << " kernel=" << kernel_.__repr__()
        << " point=" << point_.__repr__();
    return oss;
  }

  String __str__(const String & ) const override
  {
    OSS oss(false);
    oss << "CoAreaIntegrand(origin=" << origin_
        << ", kernel=" << kernel_
        << ", point=" << point_
        << ", jacobian=" << jacobian_ << ")";
    return oss;
  }

private:
  Point origin_;
  Matrix kernel_;
  Distribution antecedent_;
  Function function_;
  Point point_;
  Scalar jacobian_;
  Scalar residualTolerance_;
}; // CoAreaIntegrand

/**
 * A half space alpha . t <= beta of the parameter space of a fibre of
 * dimension d >= 2
 */
struct CoAreaConstraint
{
  Point normal;
  Scalar bound;
};

typedef std::vector<CoAreaConstraint> CoAreaConstraintCollection;

/**
 * The normals of the constraints are built from the rows of an orthonormal
 * basis of the kernel of the Jacobian and combined by the Fourier-Motzkin
 * elimination below: their entries are of order one and their rounding error
 * is a small multiple of the machine precision, amplified by the elimination.
 * Two unit normals closer than this threshold are numerically the same
 * direction, and a coefficient below it is numerically zero. It is far above
 * the machine precision to absorb the amplification, but small enough not to
 * merge distinct facets of the projected box.
 */
const Scalar CoAreaConstraintTolerance = 1.0e-12;

/**
 * Drop the constraints with a numerically zero normal after checking that
 * they are satisfied, and among constraints with the same normalised
 * direction keep the tightest only. The constraints are stored in the
 * normalised form alpha . t <= beta with ||alpha|| = 1, which leaves them
 * unchanged and makes the comparison of parallel constraints meaningful.
 * Return false when a constraint with a zero normal is violated, i.e. when
 * the fibre does not meet the search box.
 */
Bool pruneCoAreaConstraints(const CoAreaConstraintCollection & constraints,
                            const Scalar feasibilityTolerance,
                            CoAreaConstraintCollection & pruned)
{
  pruned.clear();
  for (UnsignedInteger i = 0; i < constraints.size(); ++i)
  {
    const Point & normal = constraints[i].normal;
    const Scalar norm = normal.norm();
    if (norm <= CoAreaConstraintTolerance)
    {
      // The constraint no longer depends on t: it is either trivially
      // satisfied or it makes the whole fibre empty
      if (constraints[i].bound < -feasibilityTolerance)
        return false;
      continue;
    }
    const Point unitNormal(normal / norm);
    const Scalar unitBound = constraints[i].bound / norm;
    Bool merged = false;
    for (UnsignedInteger j = 0; j < pruned.size(); ++j)
    {
      if ((unitNormal - pruned[j].normal).norm() <= CoAreaConstraintTolerance)
      {
        // Same direction: only the tightest bound matters
        if (unitBound < pruned[j].bound)
          pruned[j].bound = unitBound;
        merged = true;
        break;
      }
    }
    if (!merged)
    {
      CoAreaConstraint constraint;
      constraint.normal = unitNormal;
      constraint.bound = unitBound;
      pruned.push_back(constraint);
    }
  }
  return true;
}

/**
 * Fourier-Motzkin elimination of the coordinate t_level from a system of
 * constraints: the result describes the exact projection of the set
 * {alpha . t <= beta} onto the other coordinates. Return false when the
 * projection is empty.
 */
Bool eliminateCoAreaConstraint(const CoAreaConstraintCollection & constraints,
                               const UnsignedInteger level,
                               const Scalar feasibilityTolerance,
                               CoAreaConstraintCollection & eliminated)
{
  // alpha . t <= beta with alpha[level] > 0 bounds t_level from above, and
  // alpha[level] < 0 bounds it from below
  CoAreaConstraintCollection lowerBounds(0);
  CoAreaConstraintCollection upperBounds(0);
  CoAreaConstraintCollection independent(0);
  for (UnsignedInteger i = 0; i < constraints.size(); ++i)
  {
    const Scalar coefficient = constraints[i].normal[level];
    if (coefficient > CoAreaConstraintTolerance)
      upperBounds.push_back(constraints[i]);
    else if (coefficient < -CoAreaConstraintTolerance)
      lowerBounds.push_back(constraints[i]);
    else
      independent.push_back(constraints[i]);
  }
  // Each upper bound and each lower bound combine into a constraint on the
  // other coordinates by eliminating t_level
  for (UnsignedInteger i = 0; i < upperBounds.size(); ++i)
  {
    const Point & upperNormal = upperBounds[i].normal;
    for (UnsignedInteger j = 0; j < lowerBounds.size(); ++j)
    {
      const Point & lowerNormal = lowerBounds[j].normal;
      Point normal(upperNormal / upperNormal[level] - lowerNormal / lowerNormal[level]);
      normal[level] = 0.0;
      CoAreaConstraint constraint;
      constraint.normal = normal;
      constraint.bound = upperBounds[i].bound / upperNormal[level] - lowerBounds[j].bound / lowerNormal[level];
      independent.push_back(constraint);
    }
  }
  return pruneCoAreaConstraints(independent, feasibilityTolerance, eliminated);
}

/**
 * Bounds of the coordinate t_level of the fibre when the coordinates
 * t_0, ..., t_{level-1} are fixed to the given prefix, computed from the
 * exact projection of the fibre intersected with the search box onto
 * (t_0, ..., t_level). Return false when the prefix does not belong to the
 * projection, i.e. when the section of the fibre is empty.
 */
Bool computeCoAreaBounds(const CoAreaConstraintCollection & system,
                         const UnsignedInteger level,
                         const Point & prefix,
                         const Scalar feasibilityTolerance,
                         Scalar & lower,
                         Scalar & upper)
{
  lower = -SpecFunc::MaxScalar;
  upper = SpecFunc::MaxScalar;
  for (UnsignedInteger i = 0; i < system.size(); ++i)
  {
    const CoAreaConstraint & constraint = system[i];
    Scalar value = constraint.bound;
    for (UnsignedInteger j = 0; j < level; ++j)
      value -= constraint.normal[j] * prefix[j];
    const Scalar coefficient = constraint.normal[level];
    if (std::abs(coefficient) <= CoAreaConstraintTolerance)
    {
      // The constraint only involves the prefix: a violation means that the
      // section of the fibre is empty
      if (value < -feasibilityTolerance)
        return false;
      continue;
    }
    const Scalar extent = value / coefficient;
    if (coefficient > 0.0)
      upper = std::min(upper, extent);
    else
      lower = std::max(lower, extent);
  }
  return true;
}

/**
 * Bound function of a nested integration level: it returns the lower or the
 * upper bound of t_level given the already integrated coordinates t_0, ...,
 * t_{level-1}. An empty section is returned as the zero length interval
 * [0, 0], in which the quadrature naturally finds no mass.
 */
class CoAreaBoundFunction: public EvaluationImplementation
{
public:
  CoAreaBoundFunction(const CoAreaConstraintCollection & system,
                      const UnsignedInteger level,
                      const Bool upper,
                      const Scalar feasibilityTolerance)
    : EvaluationImplementation()
    , system_(system)
    , level_(level)
    , upper_(upper)
    , feasibilityTolerance_(feasibilityTolerance)
  {
    // Nothing to do
  }

  CoAreaBoundFunction * clone() const override
  {
    return new CoAreaBoundFunction(*this);
  }

  Point operator() (const Point & prefix) const override
  {
    Scalar lower = 0.0;
    Scalar upper = 0.0;
    if (!computeCoAreaBounds(system_, level_, prefix, feasibilityTolerance_, lower, upper))
      return Point(1, 0.0);
    // The two bound functions are evaluated independently by the integration
    // algorithm: clamp the upper one to the lower one so that it never
    // describes a reversed interval, which the quadrature would integrate
    // backwards
    return Point(1, upper_ ? std::max(lower, upper) : lower);
  }

  UnsignedInteger getInputDimension() const override
  {
    return level_;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  String __repr__() const override
  {
    OSS oss(true);
    oss << "class=CoAreaBoundFunction"
        << " level=" << level_
        << " upper=" << upper_;
    return oss;
  }

  String __str__(const String & ) const override
  {
    OSS oss(false);
    oss << "CoAreaBoundFunction(level=" << level_
        << ", upper=" << upper_ << ")";
    return oss;
  }

private:
  CoAreaConstraintCollection system_;
  UnsignedInteger level_;
  Bool upper_;
  Scalar feasibilityTolerance_;
}; // CoAreaBoundFunction

} // namespace PushForwardDistributionFunctions

/* Default constructor */
PushForwardDistribution::PushForwardDistribution()
  : DistributionImplementation()
  , function_(IdentityFunction(1))
  , antecedent_(Uniform(0.0, 1.0))
  , solver_(LeastSquaresEquationsSolver(ResourceMap::GetAsScalar("Solver-DefaultAbsoluteError"),
                                        ResourceMap::GetAsScalar("Solver-DefaultRelativeError"),
                                        ResourceMap::GetAsScalar("PushForwardDistribution-SolverResidualTolerance"),
                                        ResourceMap::GetAsUnsignedInteger("Solver-DefaultMaximumFunctionEvaluation")))
{
  setFunctionAndAntecedent(function_, antecedent_);
}

/* Parameters constructor */
PushForwardDistribution::PushForwardDistribution(const Function & function,
    const Distribution & antecedent)
  : DistributionImplementation()
  , function_()
  , antecedent_()
  , solver_(LeastSquaresEquationsSolver(ResourceMap::GetAsScalar("Solver-DefaultAbsoluteError"),
                                        ResourceMap::GetAsScalar("Solver-DefaultRelativeError"),
                                        ResourceMap::GetAsScalar("PushForwardDistribution-SolverResidualTolerance"),
                                        ResourceMap::GetAsUnsignedInteger("Solver-DefaultMaximumFunctionEvaluation")))
{
  setFunctionAndAntecedent(function, antecedent);
}

/* Comparison operator */
Bool PushForwardDistribution::operator ==(const PushForwardDistribution & other) const
{
  if (this == &other) return true;
  return (function_ == other.function_) && (antecedent_ == other.antecedent_);
}

Bool PushForwardDistribution::equals(const DistributionImplementation & other) const
{
  const PushForwardDistribution* p_other = dynamic_cast<const PushForwardDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String PushForwardDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << PushForwardDistribution::GetClassName()
      << " name=" << getName()
      << " function=" << function_
      << " antecedent=" << antecedent_
      << " solver=" << solver_;
  return oss;
}

String PushForwardDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "=f(" << antecedent_.__str__() << ") with f=" << function_.getEvaluation().__str__();
  return oss;
}

/* Virtual constructor */
PushForwardDistribution * PushForwardDistribution::clone() const
{
  return new PushForwardDistribution(*this);
}

/* Set the function and antecedent with check */
void PushForwardDistribution::setFunctionAndAntecedent(const Function & function,
    const Distribution & antecedent)
{
  const UnsignedInteger inputDimension = function.getInputDimension();
  const UnsignedInteger outputDimension = function.getOutputDimension();
  if (inputDimension != antecedent.getDimension())
    throw InvalidArgumentException(HERE) << "Error: the function must have an input dimension equal to the dimension of the antecedent, here input dimension=" << inputDimension << " and dimension of the antecedent=" << antecedent.getDimension();
  if (!antecedent.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: the antecedent must be absolutely continuous, here isContinuous()=" << antecedent.isContinuous();
  // The case outputDimension < inputDimension (co-area formula) is handled by
  // computePDFByCoArea(), which is restricted to an affine function. The
  // restriction is not enforced here: sampling, the mean and the range stay
  // available for a function of any codimension
  function_ = function;
  antecedent_ = antecedent;
  // The search box only depends on the antecedent, cache it: rebuilding it at
  // each PDF evaluation would redo the marginal quantiles and the mean
  searchInterval_ = buildSearchInterval();
  setDimension(outputDimension);
  // We don't know if the function is thread-safe and it could be called in parallel in computePDF()
  setParallel(false);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

/* Function accessors */
void PushForwardDistribution::setFunction(const Function & function)
{
  setFunctionAndAntecedent(function, antecedent_);
}

Function PushForwardDistribution::getFunction() const
{
  return function_;
}

/* Antecedent accessor */
void PushForwardDistribution::setAntecedent(const Distribution & antecedent)
{
  setFunctionAndAntecedent(function_, antecedent);
}

Distribution PushForwardDistribution::getAntecedent() const
{
  return antecedent_;
}

/* Set the solver used to invert the function */
void PushForwardDistribution::setSolver(const Solver & solver)
{
  solver_ = solver;
}

Solver PushForwardDistribution::getSolver() const
{
  return solver_;
}

/* Get one realization of the distribution */
Point PushForwardDistribution::getRealization() const
{
  return function_(antecedent_.getRealization());
}

/* Compute the numerical range of the distribution given the parameters values */
void PushForwardDistribution::computeRange()
{
  const UnsignedInteger outputDimension = function_.getOutputDimension();
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-SampleSize");
  const Sample sample(function_(antecedent_.getSample(sampleSize)));
  const Point lowerBound(sample.getMin());
  const Point upperBound(sample.getMax());
  Point marginedLowerBound(outputDimension);
  Point marginedUpperBound(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    // Add a margin of 5% of each marginal span to avoid rejecting points
    // lying on the edge of the support
    const Scalar halfSpan = 0.025 * (upperBound[i] - lowerBound[i]);
    marginedLowerBound[i] = lowerBound[i] - halfSpan;
    marginedUpperBound[i] = upperBound[i] + halfSpan;
  }
  const Interval::BoolCollection finiteLowerBound(outputDimension, true);
  const Interval::BoolCollection finiteUpperBound(outputDimension, true);
  setRange(Interval(marginedLowerBound, marginedUpperBound, finiteLowerBound, finiteUpperBound));
}

/* Build the finite search box in the support of the antecedent */
Interval PushForwardDistribution::buildSearchInterval() const
{
  const UnsignedInteger dimension = antecedent_.getDimension();
  const Interval antecedentRange(antecedent_.getRange());
  const Interval::BoolCollection finiteLowerBound(antecedentRange.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(antecedentRange.getFiniteUpperBound());
  // When a bound is not finite, clip it using such a marginal quantile of
  // the antecedent that the probability mass outside the box is negligible
  const Scalar quantileEpsilon = ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
  const Scalar upperQuantile = 1.0 - quantileEpsilon;
  Point lowerBound(antecedentRange.getLowerBound());
  Point upperBound(antecedentRange.getUpperBound());
  const Point mean(antecedent_.getMean());
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (!finiteLowerBound[i] || !finiteUpperBound[i])
    {
      const Distribution marginal(antecedent_.getMarginal(i));
      if (!finiteLowerBound[i]) lowerBound[i] = marginal.computeQuantile(quantileEpsilon)[0];
      if (!finiteUpperBound[i]) upperBound[i] = marginal.computeQuantile(upperQuantile)[0];
    }
    if (!(lowerBound[i] < upperBound[i]))
    {
      // Degenerate search interval: fall back to a unit interval around the mean
      lowerBound[i] = mean[i] - 1.0;
      upperBound[i] = mean[i] + 1.0;
    }
  }
  return Interval(lowerBound, upperBound, Interval::BoolCollection(dimension, true), Interval::BoolCollection(dimension, true));
}

/* Compute the PDF in the co-area case, i.e. when the output dimension of the
   function is lower than its input dimension */
Scalar PushForwardDistribution::computePDFByCoArea(const Point & point) const
{
  const UnsignedInteger outputDimension = function_.getOutputDimension();
  const UnsignedInteger inputDimension = function_.getInputDimension();
  const UnsignedInteger fibreDimension = inputDimension - outputDimension;
  // The function is assumed affine, hence its Jacobian matrix is constant and
  // is evaluated once, at the mean of the antecedent. The assumption is
  // checked on the faces of the search box, which bound the region where the
  // fibre is integrated, and again in the integrand at each quadrature node
  const Point mean(antecedent_.getMean());
  const Scalar residualTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-CoAreaResidualTolerance");
  const Matrix gradientMatrix(function_.gradient(mean));
  if ((gradientMatrix.getNbRows() != inputDimension) || (gradientMatrix.getNbColumns() != outputDimension))
    throw InternalException(HERE) << "Error: the given function has no actual gradient. Consider using finite differences.";
  const Interval searchInterval(searchInterval_);
  const Point lowerBound(searchInterval.getLowerBound());
  const Point upperBound(searchInterval.getUpperBound());
  const Point meanImage(function_(mean));
  // The linearization at the mean is the only thing the co-area formula can
  // rely on, and the checks below are the necessary conditions it must satisfy
  // for the shortcuts of the density computation, which return a value without
  // integrating along the fibre, not to give a zero or an infinite density
  // that no integral backs
  const auto checkAffineModel = [&](const Point & x)
  {
    const Point image(function_(x));
    Point linearization(meanImage);
    for (UnsignedInteger index = 0; index < inputDimension; ++index)
      for (UnsignedInteger k = 0; k < outputDimension; ++k)
        linearization[k] += gradientMatrix(index, k) * (x[index] - mean[index]);
    if ((image - linearization).norm() > residualTolerance * std::max(1.0, std::max(image.norm(), linearization.norm())))
      throw NotYetImplementedException(HERE) << "Error: the co-area formula is only implemented for an affine function, here f(" << x << ")=" << image << " whereas its linearization at the mean of the antecedent gives " << linearization << ", so that the fibre is not a straight line";
  };
  // The faces of the search box, one coordinate at a time, bound the region
  // where the fibre is integrated
  for (UnsignedInteger index = 0; index < inputDimension; ++index)
  {
    for (UnsignedInteger side = 0; side < 2; ++side)
    {
      Point x(mean);
      x[index] = (side == 0 ? lowerBound[index] : upperBound[index]);
      checkAffineModel(x);
    }
  }
  // Moving all the coordinates together probes their interactions, such as
  // x0 * x1, which vanish on the faces above
  for (UnsignedInteger side = 0; side < 2; ++side)
  {
    Point x(mean);
    for (UnsignedInteger index = 0; index < inputDimension; ++index)
      x[index] = (side == 0 ? lowerBound[index] : upperBound[index]);
    checkAffineModel(x);
  }
  // Full SVD of the n x p Jacobian matrix J of the function: its p singular
  // values give the p-dimensional Jacobian |J_p f| = prod sigma_i, i.e. the
  // factor relating the p-dimensional Hausdorff measure on the image to the
  // n-dimensional Lebesgue measure of the antecedent, and the last n - p
  // columns of the full left factor span the kernel of J, i.e. the directions
  // along which the function is constant, which are precisely the tangent
  // directions of the fibre
  Matrix leftFactor;
  Matrix rightFactorTransposed;
  const Point singularValues(gradientMatrix.computeSVD(leftFactor, rightFactorTransposed, true));
  // A singular value below this threshold is numerically zero, so that the
  // corresponding direction of the output space is not reached at all
  const Scalar rankTolerance = inputDimension * SpecFunc::ScalarEpsilon * singularValues[0];
  Bool rankDeficient = false;
  Scalar jacobian = 1.0;
  for (UnsignedInteger index = 0; index < outputDimension; ++index)
  {
    if (singularValues[index] <= rankTolerance)
      rankDeficient = true;
    else
      jacobian *= singularValues[index];
  }
  // Solve J * delta = point - f(mean) in the least squares sense, keeping the
  // minimum norm solution, so that origin = mean + delta is the point of the
  // fibre closest to the mean of the antecedent. Writing the residual in the
  // directions of the output space that J cannot reach tells whether the
  // point belongs to the image of the function at all
  const Point target(point - meanImage);
  Point delta(inputDimension, 0.0);
  Scalar imageResidual = 0.0;
  for (UnsignedInteger index = 0; index < outputDimension; ++index)
  {
    Point leftVector(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      leftVector[i] = leftFactor(i, index);
    Scalar projection = 0.0;
    for (UnsignedInteger i = 0; i < outputDimension; ++i)
      projection += rightFactorTransposed(index, i) * target[i];
    // Minimum norm solution of J * delta = target
    if (singularValues[index] > rankTolerance)
      delta += (projection / singularValues[index]) * leftVector;
    else
      imageResidual += projection * projection;
  }
  // Membership of the image is a measure zero condition when J is rank
  // deficient, hence the strict, machine precision threshold: a point that is
  // even slightly off the image has an empty fibre and a zero density, while
  // a point exactly on it carries a singular density
  if (std::sqrt(imageResidual) > SpecFunc::ScalarEpsilon * std::max(1.0, target.norm()))
    // The point is not in the image of the function: its fibre is empty
    return 0.0;
  if (rankDeficient)
    // The image of the function is contained in a set of zero measure of the
    // output space: the density is singular wherever it does not vanish
    return SpecFunc::Infinity;
  // Orthonormal basis of the kernel of J: it is made of the last n - p
  // columns of the full left factor of the SVD. Being orthonormal, the
  // Hausdorff element along the fibre parameterised as origin + t * direction
  // for d = 1, resp. origin + kernel * t for d >= 2, is exactly dt
  const Point origin(mean + delta);
  if (fibreDimension == 1)
  {
    // The fibre is a straight line directed by the single unit vector of the
    // kernel
    Point direction(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      direction[i] = leftFactor(i, inputDimension - 1);
    // The fibre is integrated over its intersection with the search box in the
    // support of the antecedent. Each coordinate of the box constrains t
    // independently, so their intersection is exactly the set of admissible t
    const Scalar directionTolerance = inputDimension * SpecFunc::ScalarEpsilon;
    Scalar tMin = -SpecFunc::MaxScalar;
    Scalar tMax = SpecFunc::MaxScalar;
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      if (std::abs(direction[i]) <= directionTolerance)
      {
        // The fibre does not move along this axis: it meets the search box only
        // if its coordinate already lies inside it
        if ((origin[i] < lowerBound[i]) || (origin[i] > upperBound[i]))
          return 0.0;
        continue;
      }
      Scalar first = (lowerBound[i] - origin[i]) / direction[i];
      Scalar second = (upperBound[i] - origin[i]) / direction[i];
      if (first > second) std::swap(first, second);
      tMin = std::max(tMin, first);
      tMax = std::min(tMax, second);
    }
    if (!(tMin < tMax)) return 0.0;
    // Magnitude of the integrand at the fibre point: it scales both the error
    // requested from GaussKronrod and the significance test of the ends of the
    // integration interval
    const Scalar densityScale = antecedent_.computePDF(origin) / jacobian;
    // The intersection with the search box clips the fibre. The clipping is
    // exact for an antecedent with a bounded support, as the integrand then
    // vanishes beyond the box, but for an unbounded antecedent it neglects a
    // relative mass of the order of Distribution-DefaultQuantileEpsilon. Each
    // end is therefore pushed outward while the integrand is still significant
    // there. A heavy tail stops the extension immediately: its box already
    // extends far into the region where its density is negligible, which is
    // precisely the situation where a wider interval would make the quadrature
    // miss the bulk of the mass
    const Scalar negligibleIntegrand = SpecFunc::ScalarEpsilon * densityScale;
    if (densityScale > 0.0)
    {
      const UnsignedInteger maximumExtensions = 16;
      for (UnsignedInteger extension = 0; (extension < maximumExtensions) && (antecedent_.computePDF(origin + tMax * direction) > negligibleIntegrand * jacobian); ++extension)
        tMax += tMax - tMin;
      for (UnsignedInteger extension = 0; (extension < maximumExtensions) && (antecedent_.computePDF(origin + tMin * direction) > negligibleIntegrand * jacobian); ++extension)
        tMin -= tMax - tMin;
    }
    const Matrix kernel(inputDimension, 1, direction);
    const PushForwardDistributionFunctions::CoAreaIntegrand integrand(origin, kernel, antecedent_, function_, point, jacobian, residualTolerance);
    // The error requested from GaussKronrod is an absolute error, whereas the
    // accuracy of a density is naturally relative: scaling it by the magnitude
    // of the integrand at the fibre point requests a number of significant
    // digits instead, which is what matters in the tails of the antecedent
    const GaussKronrod integrator(ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-CoAreaMaximumSubIntervals"),
                                  ResourceMap::GetAsScalar("PushForwardDistribution-CoAreaMaximumError") * std::max(densityScale, SpecFunc::ScalarEpsilon),
                                  GaussKronrodRule(GaussKronrodRule::G7K15));
    Scalar error = 0.0;
    // GaussKronrod reports by itself, at the info level, when it cannot reach
    // the requested error
    return integrator.integrate(Function(integrand), Interval(tMin, tMax), error)[0];
  }
  // The fibre has dimension d >= 2: it is the affine subspace origin +
  // kernel * t, t in R^d, whose intersection with the search box in the
  // support of the antecedent is the convex polytope defined by the 2 n
  // constraints lower <= origin + kernel * t <= upper
  Matrix kernel(inputDimension, fibreDimension);
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
    for (UnsignedInteger j = 0; j < fibreDimension; ++j)
      kernel(i, j) = leftFactor(i, outputDimension + j);
  // The parameter t has the same scale as the point x because the kernel is
  // orthonormal, so the diagonal of the search box bounds the extent of the
  // fibre and gives the scale of a meaningful feasibility tolerance
  const Scalar feasibilityTolerance = PushForwardDistributionFunctions::CoAreaConstraintTolerance * std::max(1.0, (upperBound - lowerBound).norm());
  PushForwardDistributionFunctions::CoAreaConstraintCollection constraints(0);
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    Point normal(fibreDimension);
    for (UnsignedInteger j = 0; j < fibreDimension; ++j)
      normal[j] = kernel(i, j);
    PushForwardDistributionFunctions::CoAreaConstraint constraint;
    constraint.normal = normal;
    constraint.bound = upperBound[i] - origin[i];
    constraints.push_back(constraint);
    // The same face seen from the other side: -normal . t <= origin[i] - lower[i]
    constraint.normal = -normal;
    constraint.bound = origin[i] - lowerBound[i];
    constraints.push_back(constraint);
  }
  // systems[k] describes the exact projection of the polytope onto
  // (t_0, ..., t_k), computed once by eliminating the coordinates from the
  // last one to the first. The nested interval description of the fibre is
  // then exact: at a feasible prefix, the extent of t_k is the set of values
  // satisfying systems[k], and the kinks of these piecewise affine bounds are
  // left to the adaptive quadrature rule
  PushForwardDistributionFunctions::CoAreaConstraintCollection current(0);
  if (!PushForwardDistributionFunctions::pruneCoAreaConstraints(constraints, feasibilityTolerance, current))
    // The fibre does not meet the search box: its density is zero
    return 0.0;
  std::vector<PushForwardDistributionFunctions::CoAreaConstraintCollection> systems(fibreDimension + 1);
  systems[fibreDimension] = current;
  for (UnsignedInteger level = fibreDimension; level > 0; --level)
  {
    const UnsignedInteger coordinate = level - 1;
    systems[coordinate] = current;
    PushForwardDistributionFunctions::CoAreaConstraintCollection eliminated(0);
    if (!PushForwardDistributionFunctions::eliminateCoAreaConstraint(current, coordinate, feasibilityTolerance, eliminated))
      // The projection is empty: the fibre does not meet the search box
      return 0.0;
    current = eliminated;
  }
  Scalar tMin = 0.0;
  Scalar tMax = 0.0;
  if (!PushForwardDistributionFunctions::computeCoAreaBounds(systems[0], 0, Point(0), feasibilityTolerance, tMin, tMax))
    return 0.0;
  if (!(tMin < tMax)) return 0.0;
  // Magnitude of the integrand at the point of the fibre closest to the mean:
  // it scales the error requested from the adaptive rule, as in the one
  // dimensional case
  const Scalar densityScale = antecedent_.computePDF(origin) / jacobian;
  // The nested bounds are piecewise affine functions with kinks along the
  // projected facets of the polytope. Adaptive quadrature converges only
  // algebraically across a kink: for a fibre of dimension three the measured
  // relative accuracy stalls around 1e-8, and the nested error of the inner
  // integrals prevents any gain from a tighter request. For the sum of four
  // uniform variables, a request of 1e-7 reaches 5.5e-9 in 8.5 s whereas a
  // request of 1e-8 reaches 7.4e-9 in 57 s. Floor the request at the
  // PushForwardDistribution-CoAreaErrorFloor value for d >= 3, the one and two
  // dimensional fibres being integrated to the machine precision, as measured
  // on the same integrand: 3e-16 at 1e-14.
  Scalar requestedError = ResourceMap::GetAsScalar("PushForwardDistribution-CoAreaMaximumError");
  if (fibreDimension > 2)
    requestedError = std::max(requestedError, ResourceMap::GetAsScalar("PushForwardDistribution-CoAreaErrorFloor"));
  const GaussKronrod algorithm(ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-CoAreaMaximumSubIntervals"),
                               requestedError * std::max(densityScale, SpecFunc::ScalarEpsilon),
                               GaussKronrodRule(GaussKronrodRule::G7K15));
  const PushForwardDistributionFunctions::CoAreaIntegrand integrand(origin, kernel, antecedent_, function_, point, jacobian, residualTolerance);
  IteratedQuadrature::FunctionCollection lowerBounds(fibreDimension - 1);
  IteratedQuadrature::FunctionCollection upperBounds(fibreDimension - 1);
  for (UnsignedInteger level = 1; level < fibreDimension; ++level)
  {
    lowerBounds[level - 1] = Function(new PushForwardDistributionFunctions::CoAreaBoundFunction(systems[level], level, false, feasibilityTolerance));
    upperBounds[level - 1] = Function(new PushForwardDistributionFunctions::CoAreaBoundFunction(systems[level], level, true, feasibilityTolerance));
  }
  return IteratedQuadrature(algorithm).integrate(Function(integrand), tMin, tMax, lowerBounds, upperBounds)[0];
}

/* Solve r(u) = 0 by damped Newton with step control from the given starting point */
Point PushForwardDistribution::dampedNewton(const Function & residualFunction,
    const Point & startingPoint) const
{
  const Scalar residualTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-SolverResidualTolerance");
  const UnsignedInteger maximumIterations = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-NewtonMaximumIterations");
  const Scalar stepReduction = ResourceMap::GetAsScalar("PushForwardDistribution-NewtonStepReduction");
  const UnsignedInteger dimension = antecedent_.getDimension();
  Point point(startingPoint);
  for (UnsignedInteger iteration = 0; iteration < maximumIterations; ++iteration)
  {
    // Residual at the current point
    Point residual;
    Scalar residualNorm = -1.0;
    try
    {
      residual = residualFunction(point);
      residualNorm = residual.norm();
    }
    catch (const Exception &)
    {
      // The residual does not exist at this point
      return Point();
    }
    if (residualNorm <= residualTolerance)
    {
      // Polish the root by a few pure Newton steps: as the Jacobian of the
      // residual is computed analytically, the residual converges up to the
      // machine precision and the roots found from different starting
      // points coincide, so that they can be distinguished from the other
      // distinct roots
      for (UnsignedInteger polish = 0; polish < maximumIterations; ++polish)
      {
        const Point currentPoint(point);
        Matrix jacobianMatrix;
        try
        {
          jacobianMatrix = residualFunction.gradient(currentPoint);
        }
        catch (const Exception &)
        {
          break;
        }
        Point correction;
        try
        {
          const SquareMatrix jacobian(*jacobianMatrix.getImplementation());
          correction = jacobian.transpose().solveLinearSystem(residualFunction(currentPoint));
        }
        catch (const Exception &)
        {
          break;
        }
        const Point polishedPoint(Point(currentPoint - correction));
        Scalar polishedResidualNorm = residualNorm;
        try
        {
          polishedResidualNorm = residualFunction(polishedPoint).norm();
        }
        catch (const Exception &)
        {
          break;
        }
        if (!(polishedResidualNorm < residualNorm))
          break;
        point = polishedPoint;
        residualNorm = polishedResidualNorm;
      }
      return point;
    }
    // Jacobian of the residual
    Matrix gradientMatrix;
    try
    {
      gradientMatrix = residualFunction.gradient(point);
    }
    catch (const Exception &)
    {
      // The residual has no computable Jacobian at this point
      return Point();
    }
    if ((gradientMatrix.getNbRows() != dimension) || (gradientMatrix.getNbColumns() != dimension))
      throw InternalException(HERE) << "Error: the given function has no actual gradient. Consider using finite differences.";
    // Solve J * correction = residual. As the gradient matrix returned by
    // the function is the transpose of the Jacobian J of the residual, the
    // Jacobian is the transpose of the gradient matrix.
    Point correction;
    try
    {
      const SquareMatrix jacobian(*gradientMatrix.getImplementation());
      correction = jacobian.transpose().solveLinearSystem(residual);
    }
    catch (const Exception &)
    {
      // The Jacobian is not invertible at this point
      return Point();
    }
    // Step control: shrink the step until the residual norm decreases
    Scalar step = 1.0;
    Point nextPoint(point);
    Scalar nextResidualNorm = residualNorm;
    for (UnsignedInteger backtracking = 0; backtracking < maximumIterations; ++backtracking)
    {
      nextPoint = Point(point - step * correction);
      try
      {
        nextResidualNorm = residualFunction(nextPoint).norm();
      }
      catch (const Exception &)
      {
        // The residual does not exist at the trial point: keep reducing the step
        nextResidualNorm = residualNorm;
      }
      if (std::isfinite(nextResidualNorm) && (nextResidualNorm < residualNorm))
        break;
      if (step * correction.norm() <= SpecFunc::ScalarEpsilon * std::max(1.0, point.norm()))
        // No step can reduce the residual norm from here: no preimage reached
        return Point();
      step *= stepReduction;
    }
    if (nextResidualNorm >= residualNorm)
      // The residual norm could not be decreased: no preimage reached
      return Point();
    point = nextPoint;
  }
  return Point();
}

/* Find all the preimages of the given point by a damped Newton solver with step control */
DistributionImplementation::PointCollection PushForwardDistribution::findPreimages(const Function & residualFunction) const
{
  PointCollection preimages(0);
  const UnsignedInteger dimension = antecedent_.getDimension();
  const Interval searchInterval(searchInterval_);
  const Point lowerBound(searchInterval.getLowerBound());
  const Point upperBound(searchInterval.getUpperBound());
  const Scalar residualTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-SolverResidualTolerance");
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-PreimageSearchSampleSize");
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << "Error: the resource map key PushForwardDistribution-PreimageSearchSampleSize must be nonzero.";
  // Tolerance below which two preimages are considered to be the same one
  const Scalar dedupTolerance = std::sqrt(SpecFunc::ScalarEpsilon) * std::max(1.0, std::max(lowerBound.norm(), upperBound.norm()));
  const auto isValidRoot = [&](const Point & candidate)
  {
    try
    {
      return residualFunction(candidate).norm() <= residualTolerance;
    }
    catch (const Exception &)
    {
      return false;
    }
  };
  const auto addRoot = [&](const Point & candidate)
  {
    if (!isValidRoot(candidate))
      return;
    for (UnsignedInteger i = 0; i < preimages.getSize(); ++i)
      if ((preimages[i] - candidate).norm() <= dedupTolerance)
        return;
    preimages.add(candidate);
  };
  // Refine a candidate by damped Newton, so that all the preimages are
  // accurate up to the machine precision and can be deduplicated
  const auto addRefinedRoot = [&](const Point & startingPoint)
  {
    const Point refined(dampedNewton(residualFunction, startingPoint));
    if (refined.getSize() > 0)
      addRoot(refined);
    else
      addRoot(startingPoint);
  };
  if (dimension == 1)
  {
    const Scalar a = lowerBound[0];
    const Scalar b = upperBound[0];
    if (a >= b)
      return preimages;
    // Evaluate the residual on a regular grid over the search interval
    Point gridPoints(sampleSize + 1);
    Point residualValues(sampleSize + 1);
    for (UnsignedInteger i = 0; i <= sampleSize; ++i)
    {
      const Scalar u = (i * b + (sampleSize - i) * a) / sampleSize;
      gridPoints[i] = u;
      try
      {
        residualValues[i] = residualFunction(Point(1, u))[0];
      }
      catch (const Exception &)
      {
        residualValues[i] = SpecFunc::ScalarEpsilon;
      }
    }
    // Detect the preimages with an odd multiplicity through the sign
    // changes of the residual and refine them by damped Newton starting
    // from the middle of the sign change interval, with a bisection
    // fallback which guarantees the convergence
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      const Scalar rA = residualValues[i];
      const Scalar rB = residualValues[i + 1];
      if (!std::isfinite(rA) || !std::isfinite(rB))
        continue;
      if (std::abs(rA) <= residualTolerance)
        addRefinedRoot(Point(1, gridPoints[i]));
      if (std::abs(rB) <= residualTolerance)
        addRefinedRoot(Point(1, gridPoints[i + 1]));
      if ((rA < 0.0) != (rB < 0.0))
      {
        // Refine the preimage by damped Newton from the middle of the
        // sign change interval, with a bisection fallback which
        // guarantees the convergence given the opposite signs of the
        // residual at the interval ends
        Point root(dampedNewton(residualFunction, Point(1, 0.5 * (gridPoints[i] + gridPoints[i + 1]))));
        if (root.getSize() == 0)
        {
          Scalar uLow = gridPoints[i];
          Scalar uHigh = gridPoints[i + 1];
          Scalar rLow = rA;
          Scalar rootValue = 0.5 * (uLow + uHigh);
          for (UnsignedInteger bisection = 0; bisection <= 64; ++bisection)
          {
            rootValue = 0.5 * (uLow + uHigh);
            Scalar rMiddle = SpecFunc::ScalarEpsilon;
            try
            {
              rMiddle = residualFunction(Point(1, rootValue))[0];
            }
            catch (const Exception &)
            {
              break;
            }
            if (std::abs(rMiddle) <= residualTolerance)
              break;
            if ((rMiddle < 0.0) == (rLow < 0.0))
            {
              uLow = rootValue;
              rLow = rMiddle;
            }
            else uHigh = rootValue;
          }
          root = Point(1, rootValue);
        }
        addRoot(root);
      }
    }
    // Detect the preimages with an even multiplicity through the local
    // minima of the absolute value of the residual
    for (UnsignedInteger i = 1; i < sampleSize; ++i)
    {
      const Scalar rA = residualValues[i];
      if (!std::isfinite(rA) || !std::isfinite(residualValues[i - 1]) || !std::isfinite(residualValues[i + 1]))
        continue;
      if (std::abs(rA) < std::abs(residualValues[i - 1]) && std::abs(rA) < std::abs(residualValues[i + 1]))
      {
        const Point root(dampedNewton(residualFunction, Point(1, gridPoints[i])));
        if (root.getSize() > 0)
          addRoot(root);
      }
    }
  }
  else
  {
    // Multi-start damped Newton from a low-discrepancy sequence over the
    // search interval
    const Sample startingPoints(SobolSequence(dimension).generate(sampleSize));
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      Point startingPoint(dimension);
      for (UnsignedInteger j = 0; j < dimension; ++j)
        startingPoint[j] = lowerBound[j] + (upperBound[j] - lowerBound[j]) * startingPoints(i, j);
      const Point root(dampedNewton(residualFunction, startingPoint));
      if (root.getSize() > 0)
        addRoot(root);
    }
  }
  // Whatever the dimension, also try the mean of the antecedent as a
  // starting point, which is the natural preimage for injective functions
  const Point mean(antecedent_.getMean());
  if (searchInterval.contains(mean))
  {
    const Point root(dampedNewton(residualFunction, mean));
    if (root.getSize() > 0)
      addRoot(root);
  }
  return preimages;
}

/* Compute the numerical PDF of the distribution at the given point */
Scalar PushForwardDistribution::computePDF(const Point & point) const
{
  const UnsignedInteger inputDimension = antecedent_.getDimension();
  const UnsignedInteger outputDimension = function_.getOutputDimension();
  if (point.getDimension() != outputDimension)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << outputDimension << ", here dimension=" << point.getDimension();
  // The co-area case has no discrete preimage structure: the fibre f(u) =
  // point is a continuum and its contribution is integrated along it
  if (outputDimension < inputDimension)
    return computePDFByCoArea(point);
  // The preimages of the point are the roots of the residual function
  // r(u) = f(u) - point
  const Function residualFunction(function_ - ConstantFunction(inputDimension, point));
  const Interval antecedentRange(antecedent_.getRange());
  // Search for the preimages of the point:
  // - in the square case (p == n), a damped Newton solver with step control
  //   finds all the preimages, being comprehensive in dimension one through
  //   a decomposition of the search interval and multi-start in higher
  //   dimensions, hence supporting non-injective functions;
  // - in the intrinsic case (p > n), all the preimages are searched for by
  //   multi-start least squares over the finite search box, deduplicated by
  //   distance up to the solver accuracy, hence supporting non-injective maps
  PointCollection preimages(0);
  if (outputDimension == inputDimension)
  {
    preimages = findPreimages(residualFunction);
  }
  else
  {
    // Intrinsic case: collect all the preimages by multi-start least
    // squares from a low-discrepancy sequence over the finite search box
    // plus the antecedent mean, deduplicated by distance. This extends the
    // previous closest-to-mean single branch, which underestimated the
    // density for non-injective maps.
    const Interval searchInterval(searchInterval_);
    const Point searchLowerBound(searchInterval.getLowerBound());
    const Point searchUpperBound(searchInterval.getUpperBound());
    const Scalar residualTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-SolverResidualTolerance");
    const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-PreimageSearchSampleSize");
    // The least squares solver is only accurate up to residualTolerance, so
    // two solves of the same branch may differ by order sqrt(residualTolerance):
    // deduplicate at that scale, unlike the square case whose Newton polish
    // reaches machine precision
    const Scalar dedupTolerance = std::sqrt(residualTolerance) * std::max(1.0, std::max(searchLowerBound.norm(), searchUpperBound.norm()));
    PointCollection startingPoints(0);
    if (inputDimension == 1)
    {
      const Scalar a = searchLowerBound[0];
      const Scalar b = searchUpperBound[0];
      for (UnsignedInteger i = 0; i <= sampleSize; ++i)
        startingPoints.add(Point(1, (i * b + (sampleSize - i) * a) / sampleSize));
    }
    else
    {
      const Sample sobolPoints(SobolSequence(inputDimension).generate(sampleSize));
      for (UnsignedInteger i = 0; i < sampleSize; ++i)
      {
        Point startingPoint(inputDimension);
        for (UnsignedInteger j = 0; j < inputDimension; ++j)
          startingPoint[j] = searchLowerBound[j] + (searchUpperBound[j] - searchLowerBound[j]) * sobolPoints(i, j);
        startingPoints.add(startingPoint);
      }
    }
    startingPoints.add(antecedent_.getMean());
    for (UnsignedInteger s = 0; s < startingPoints.getSize(); ++s)
    {
      Point preimage(0);
      try
      {
        preimage = solver_.solve(residualFunction, startingPoints[s], searchInterval);
      }
      catch (const InternalException &)
      {
        continue;
      }
      if (preimage.getSize() == 0)
        continue;
      if ((function_(preimage) - point).norm() > residualTolerance)
        continue;
      if (!antecedentRange.contains(preimage))
        continue;
      Bool duplicate = false;
      for (UnsignedInteger k = 0; k < preimages.getSize(); ++k)
        if ((preimages[k] - preimage).norm() <= dedupTolerance)
        {
          duplicate = true;
          break;
        }
      if (!duplicate)
        preimages.add(preimage);
    }
  }
  // Change of variables formula: the density at the point is the sum of the
  // contributions over all its preimages
  Scalar pdf = 0.0;
  for (UnsignedInteger k = 0; k < preimages.getSize(); ++k)
  {
    const Point preimage(preimages[k]);
    if (!antecedentRange.contains(preimage))
      continue;
    const Scalar numerator = antecedent_.computePDF(preimage);
    if (!(numerator > 0.0))
      continue;
    const Matrix gradientMatrix(function_.gradient(preimage));
    if ((gradientMatrix.getNbRows() != inputDimension) || (gradientMatrix.getNbColumns() != outputDimension))
      throw InternalException(HERE) << "Error: the given function has no actual gradient. Consider using finite differences.";
    // In the square case the denominator is |det(J)|, in the intrinsic case
    // it is sqrt(det(J^T J)) where J is the Jacobian matrix of the function.
    // The gradient matrix returned by the function is the transpose of J.
    Scalar denominator = 0.0;
    if (outputDimension == inputDimension)
    {
      const SquareMatrix jacobianSquareMatrix(*gradientMatrix.getImplementation());
      denominator = std::abs(jacobianSquareMatrix.computeDeterminant());
    }
    else
    {
      // sqrt(det(J^T J)) is the product of the singular values of J. Taking
      // the determinant of the Gram matrix J^T J instead would square the
      // conditioning of the computation and lose the sign of its square
      // root to roundoff, turning a finite density into +inf.
      denominator = 1.0;
      const Point singularValues(gradientMatrix.computeSingularValues());
      for (UnsignedInteger index = 0; index < inputDimension; ++index)
        denominator *= singularValues[index];
    }
    if (!std::isfinite(denominator) || !(denominator > 0.0))
    {
      // Critical value of the function where the Jacobian is singular: the
      // density diverges where the antecedent carries mass (e.g. Y=X^2 at 0)
      return SpecFunc::Infinity;
    }
    pdf += numerator / denominator;
  }
  return pdf;
}

/* Parameters value and description accessor */
PushForwardDistribution::PointWithDescriptionCollection PushForwardDistribution::getParametersCollection() const
{
  return antecedent_.getParametersCollection();
}

void PushForwardDistribution::setParametersCollection(const PointCollection & parametersCollection)
{
  antecedent_.setParametersCollection(parametersCollection);
  setFunctionAndAntecedent(function_, antecedent_);
}

/* Parameters value accessors */
void PushForwardDistribution::setParameter(const Point & parameter)
{
  antecedent_.setParameter(parameter);
  setFunctionAndAntecedent(function_, antecedent_);
}

Point PushForwardDistribution::getParameter() const
{
  return antecedent_.getParameter();
}

/* Parameters description accessor */
Description PushForwardDistribution::getParameterDescription() const
{
  return antecedent_.getParameterDescription();
}

/* Tell if the distribution is continuous */
Bool PushForwardDistribution::isContinuous() const
{
  return antecedent_.isContinuous();
}

/* Tell if the distribution is discrete */
Bool PushForwardDistribution::isDiscrete() const
{
  return antecedent_.isDiscrete();
}

/* Compute the mean of the distribution */
void PushForwardDistribution::computeMean() const
{
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-SampleSize");
  const Sample sample(function_(antecedent_.getSample(sampleSize)));
  mean_ = sample.computeMean();
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void PushForwardDistribution::computeCovariance() const
{
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-SampleSize");
  const Sample sample(function_(antecedent_.getSample(sampleSize)));
  covariance_ = sample.computeCovariance();
  isAlreadyComputedCovariance_ = true;
}

/* Method save() stores the object through the StorageManager */
void PushForwardDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("function_", function_);
  adv.saveAttribute("antecedent_", antecedent_);
  adv.saveAttribute("solver_", solver_);
}

/* Method load() reloads the object from the StorageManager */
void PushForwardDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("function_", function_);
  adv.loadAttribute("antecedent_", antecedent_);
  adv.loadAttribute("solver_", solver_);
}

END_NAMESPACE_OPENTURNS
