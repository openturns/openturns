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
#include <array>
#include <atomic>
#include <cmath>
#include <iomanip>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include "openturns/PushForwardDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Brent.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/ConstantFunction.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/IntervalMesher.hxx"
#include "openturns/IteratedQuadrature.hxx"
#include "openturns/LeastSquaresEquationsSolver.hxx"
#include "openturns/LevelSetContinuation.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Sample.hxx"
#include "openturns/SimplicialCubature.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SymbolicFunction.hxx"
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
                  const Scalar jacobian)
    : EvaluationImplementation()
    , origin_(origin)
    , kernel_(kernel)
    , antecedent_(antecedent)
    , jacobian_(jacobian)
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
    // Outside the support of the antecedent the fibre carries no mass.
    if (!(antecedentPDF > 0.0))
      return Point(1, 0.0);
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
        << " jacobian=" << jacobian_;
    return oss;
  }

  String __str__(const String & ) const override
  {
    OSS oss(false);
    oss << "CoAreaIntegrand(origin=" << origin_
        << ", kernel=" << kernel_
        << ", jacobian=" << jacobian_ << ")";
    return oss;
  }

private:
  Point origin_;
  Matrix kernel_;
  Distribution antecedent_;
  Scalar jacobian_;
}; // CoAreaIntegrand

/**
 * Integrand of the co-area formula on a level surface f(x) = level of a
 * scalar function f: R^3 -> R. The surface mesh supplies the Hausdorff
 * element, so the integrand only has to divide the antecedent density by the
 * norm of the gradient. When the function does not provide an analytical
 * gradient, a centered finite difference fallback is used. A vanishing
 * gradient is a critical point of the map: the co-area factor is singular
 * there, and the shared flag lets the caller return an infinite density.
 */
class NonlinearCoAreaSurfaceIntegrand: public EvaluationImplementation
{
public:
  NonlinearCoAreaSurfaceIntegrand(const Distribution & antecedent,
                                  const Function & function,
                                  const std::shared_ptr<std::atomic<Bool>> & singular)
    : EvaluationImplementation()
    , antecedent_(antecedent)
    , function_(function)
    , singular_(singular)
  {
    // Nothing to do
  }

  NonlinearCoAreaSurfaceIntegrand * clone() const override
  {
    return new NonlinearCoAreaSurfaceIntegrand(*this);
  }

  Point operator() (const Point & point) const override
  {
    const Scalar antecedentPDF = antecedent_.computePDF(point);
    // Outside the support of the antecedent the level surface carries no mass.
    if (!(antecedentPDF > 0.0))
      return Point(1, 0.0);
    const Scalar gradientNorm = computeGradientNorm(point);
    if (!std::isfinite(gradientNorm) || !(gradientNorm > 0.0))
    {
      singular_->store(true);
      return Point(1, 0.0);
    }
    return Point(1, antecedentPDF / gradientNorm);
  }

  UnsignedInteger getInputDimension() const override
  {
    return function_.getInputDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  String __repr__() const override
  {
    OSS oss(true);
    oss << "class=NonlinearCoAreaSurfaceIntegrand"
        << " antecedent=" << antecedent_.__repr__()
        << " function=" << function_.__repr__();
    return oss;
  }

  String __str__(const String & ) const override
  {
    OSS oss(false);
    oss << "NonlinearCoAreaSurfaceIntegrand(antecedent=" << antecedent_
        << ", function=" << function_ << ")";
    return oss;
  }

private:
  Scalar computeGradientNorm(const Point & point) const
  {
    // Prefer the analytical gradient of the function when it is available.
    try
    {
      const Matrix gradientMatrix(function_.gradient(point));
      if ((gradientMatrix.getNbRows() == point.getSize()) && (gradientMatrix.getNbColumns() == 1))
      {
        Scalar squaredNorm = 0.0;
        for (UnsignedInteger i = 0; i < point.getSize(); ++i)
          squaredNorm += gradientMatrix(i, 0) * gradientMatrix(i, 0);
        return std::sqrt(squaredNorm);
      }
    }
    catch (const Exception &)
    {
      // Fall through to the finite difference approximation
    }
    // Centered finite differences for a function without an analytical
    // gradient. The step is relative to the magnitude of the evaluation point
    // so that it remains meaningful for coordinates far from the origin.
    const Scalar step = std::sqrt(SpecFunc::ScalarEpsilon) * std::max(1.0, point.norm());
    Point plusPoint(point);
    Point minusPoint(point);
    Scalar squaredNorm = 0.0;
    for (UnsignedInteger i = 0; i < point.getSize(); ++i)
    {
      plusPoint[i] += step;
      minusPoint[i] -= step;
      Scalar plusValue = 0.0;
      Scalar minusValue = 0.0;
      try
      {
        plusValue = function_(plusPoint)[0];
        minusValue = function_(minusPoint)[0];
      }
      catch (const Exception &)
      {
        return 0.0;
      }
      const Scalar derivative = (plusValue - minusValue) / (2.0 * step);
      squaredNorm += derivative * derivative;
      plusPoint[i] = point[i];
      minusPoint[i] = point[i];
    }
    return std::sqrt(squaredNorm);
  }

  Distribution antecedent_;
  Function function_;
  std::shared_ptr<std::atomic<Bool>> singular_;
}; // NonlinearCoAreaSurfaceIntegrand

/**
 * Integrand of the co-area formula along a curve f(x) = point for a map
 * f: R^n -> R^{n-1}. The segment supplies the Hausdorff element, so the
 * integrand is the antecedent density multiplied by the segment length and
 * divided by the (n - 1)-dimensional Jacobian of f. The latter is the norm of
 * the generalized cross product of the n - 1 gradient vectors, i.e. the norm
 * of the vector of signed (n - 1) x (n - 1) minors. A rank deficient Jacobian
 * is a critical point of the map: the co-area factor is singular there, and
 * the shared flag lets the caller return an infinite density. When the
 * function does not provide an analytical gradient, a centered finite
 * difference fallback is used.
 */
class NonlinearCoAreaCurveIntegrand: public EvaluationImplementation
{
public:
  NonlinearCoAreaCurveIntegrand(const Point & origin,
                                const Point & direction,
                                const Distribution & antecedent,
                                const Function & function,
                                const std::shared_ptr<std::atomic<Bool>> & singular)
    : EvaluationImplementation()
    , origin_(origin)
    , direction_(direction)
    , length_(direction.norm())
    , antecedent_(antecedent)
    , function_(function)
    , singular_(singular)
  {
    // Nothing to do
  }

  NonlinearCoAreaCurveIntegrand * clone() const override
  {
    return new NonlinearCoAreaCurveIntegrand(*this);
  }

  Point operator() (const Point & parameter) const override
  {
    const Point point(origin_ + parameter[0] * direction_);
    const Scalar antecedentPDF = antecedent_.computePDF(point);
    // Outside the support of the antecedent the curve carries no mass.
    if (!(antecedentPDF > 0.0))
      return Point(1, 0.0);
    const Scalar jacobian = computeJacobian(point);
    if (!std::isfinite(jacobian) || !(jacobian > 0.0))
    {
      singular_->store(true);
      return Point(1, 0.0);
    }
    return Point(1, antecedentPDF * length_ / jacobian);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  String __repr__() const override
  {
    OSS oss(true);
    oss << "class=NonlinearCoAreaCurveIntegrand"
        << " origin=" << origin_.__repr__()
        << " direction=" << direction_.__repr__()
        << " antecedent=" << antecedent_.__repr__()
        << " function=" << function_.__repr__();
    return oss;
  }

  String __str__(const String & ) const override
  {
    OSS oss(false);
    oss << "NonlinearCoAreaCurveIntegrand(origin=" << origin_
        << ", direction=" << direction_
        << ", antecedent=" << antecedent_
        << ", function=" << function_ << ")";
    return oss;
  }

private:
  Scalar computeJacobian(const Point & point) const
  {
    const UnsignedInteger inputDimension = point.getSize();
    const UnsignedInteger outputDimension = function_.getOutputDimension();
    if ((inputDimension < 2) || (outputDimension + 1 != inputDimension))
      return 0.0;
    Matrix gradientMatrix;
    Bool hasGradient = false;
    // Prefer the analytical gradient of the function when it is available.
    try
    {
      gradientMatrix = function_.gradient(point);
      hasGradient = (gradientMatrix.getNbRows() == inputDimension)
                    && (gradientMatrix.getNbColumns() == outputDimension);
    }
    catch (const Exception &)
    {
      hasGradient = false;
    }
    if (!hasGradient)
    {
      // Centered finite differences for a function without an analytical
      // gradient. The step is relative to the magnitude of the evaluation point
      // so that it remains meaningful for coordinates far from the origin.
      const Scalar step = std::sqrt(SpecFunc::ScalarEpsilon) * std::max(1.0, point.norm());
      gradientMatrix = Matrix(inputDimension, outputDimension);
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
          return 0.0;
        }
        for (UnsignedInteger j = 0; j < outputDimension; ++j)
          gradientMatrix(i, j) = (plusValue[j] - minusValue[j]) / (2.0 * step);
        plusPoint[i] = point[i];
        minusPoint[i] = point[i];
      }
    }
    // The gradient matrix returned by the function has the input dimension as
    // rows and the output dimension as columns. Its generalized cross product
    // gives both the co-area factor and the tangent of the fibre.
    return LevelSetContinuation::computeCoAreaFactor(gradientMatrix);
  }

  Point origin_;
  Point direction_;
  Scalar length_;
  Distribution antecedent_;
  Function function_;
  std::shared_ptr<std::atomic<Bool>> singular_;
}; // NonlinearCoAreaCurveIntegrand

/**
 * A half space alpha . t <= beta of the parameter space of a fibre of
 * dimension d >= 2
 */
struct CoAreaConstraint
{
  Point normal;
  Scalar bound;
};

typedef Collection<CoAreaConstraint> CoAreaConstraintCollection;

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
                            const Scalar constraintTolerance,
                            CoAreaConstraintCollection & pruned)
{
  pruned.clear();
  for (UnsignedInteger i = 0; i < constraints.getSize(); ++i)
  {
    const Point & normal = constraints[i].normal;
    const Scalar norm = normal.norm();
    if (norm <= constraintTolerance)
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
    for (UnsignedInteger j = 0; j < pruned.getSize(); ++j)
    {
      if ((unitNormal - pruned[j].normal).norm() <= constraintTolerance)
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
      pruned.add(constraint);
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
                               const Scalar constraintTolerance,
                               CoAreaConstraintCollection & eliminated)
{
  // alpha . t <= beta with alpha[level] > 0 bounds t_level from above, and
  // alpha[level] < 0 bounds it from below
  CoAreaConstraintCollection lowerBounds(0);
  CoAreaConstraintCollection upperBounds(0);
  CoAreaConstraintCollection independent(0);
  for (UnsignedInteger i = 0; i < constraints.getSize(); ++i)
  {
    const Scalar coefficient = constraints[i].normal[level];
    if (coefficient > constraintTolerance)
      upperBounds.add(constraints[i]);
    else if (coefficient < -constraintTolerance)
      lowerBounds.add(constraints[i]);
    else
      independent.add(constraints[i]);
  }
  // Each upper bound and each lower bound combine into a constraint on the
  // other coordinates by eliminating t_level
  for (UnsignedInteger i = 0; i < upperBounds.getSize(); ++i)
  {
    const Point & upperNormal = upperBounds[i].normal;
    for (UnsignedInteger j = 0; j < lowerBounds.getSize(); ++j)
    {
      const Point & lowerNormal = lowerBounds[j].normal;
      Point normal(upperNormal / upperNormal[level] - lowerNormal / lowerNormal[level]);
      normal[level] = 0.0;
      CoAreaConstraint constraint;
      constraint.normal = normal;
      constraint.bound = upperBounds[i].bound / upperNormal[level] - lowerBounds[j].bound / lowerNormal[level];
      independent.add(constraint);
    }
  }
  return pruneCoAreaConstraints(independent, feasibilityTolerance, constraintTolerance, eliminated);
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
                         const Scalar constraintTolerance,
                         Scalar & lower,
                         Scalar & upper)
{
  lower = -SpecFunc::MaxScalar;
  upper = SpecFunc::MaxScalar;
  for (UnsignedInteger i = 0; i < system.getSize(); ++i)
  {
    const CoAreaConstraint & constraint = system[i];
    Scalar value = constraint.bound;
    for (UnsignedInteger j = 0; j < level; ++j)
      value -= constraint.normal[j] * prefix[j];
    const Scalar coefficient = constraint.normal[level];
    if (std::abs(coefficient) <= constraintTolerance)
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
 * Check whether the affine fibre origin + kernel * t meets the box delimited
 * by the finite bounds. The kernel columns are orthonormal, but only
 * feasibility is needed here: the Fourier-Motzkin elimination is run to the
 * end, and the zero-normal constraints left at each step are checked by
 * pruneCoAreaConstraints. Coordinates with an infinite bound do not constrain
 * the fibre.
 */
Bool checkCoAreaSupport(const Matrix & kernel,
                        const Point & origin,
                        const Point & lowerBound,
                        const Point & upperBound,
                        const Interval::BoolCollection & finiteLowerBound,
                        const Interval::BoolCollection & finiteUpperBound,
                        const Scalar feasibilityTolerance,
                        const Scalar constraintTolerance)
{
  const UnsignedInteger inputDimension = origin.getSize();
  const UnsignedInteger fibreDimension = kernel.getNbColumns();
  CoAreaConstraintCollection constraints(0);
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    Point normal(fibreDimension);
    for (UnsignedInteger j = 0; j < fibreDimension; ++j)
      normal[j] = kernel(i, j);
    if (finiteUpperBound[i])
    {
      CoAreaConstraint constraint;
      constraint.normal = normal;
      constraint.bound = upperBound[i] - origin[i];
      constraints.add(constraint);
    }
    if (finiteLowerBound[i])
    {
      CoAreaConstraint constraint;
      constraint.normal = -normal;
      constraint.bound = origin[i] - lowerBound[i];
      constraints.add(constraint);
    }
  }
  CoAreaConstraintCollection current(0);
  if (!pruneCoAreaConstraints(constraints, feasibilityTolerance, constraintTolerance, current))
    return false;
  for (UnsignedInteger level = fibreDimension; level > 0; --level)
  {
    CoAreaConstraintCollection eliminated(0);
    if (!eliminateCoAreaConstraint(current, level - 1, feasibilityTolerance, constraintTolerance, eliminated))
      return false;
    current = eliminated;
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
                      const Scalar feasibilityTolerance,
                      const Scalar constraintTolerance)
    : EvaluationImplementation()
    , system_(system)
    , level_(level)
    , upper_(upper)
    , feasibilityTolerance_(feasibilityTolerance)
    , constraintTolerance_(constraintTolerance)
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
    if (!computeCoAreaBounds(system_, level_, prefix, feasibilityTolerance_, constraintTolerance_, lower, upper))
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
  Scalar constraintTolerance_;
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
  // A nonlinear co-area map is accepted for sampling, range and moments: these
  // operations only need evaluations of the function or integrals over the
  // input space. The co-area density itself needs an implicit fibre and is
  // rejected in computePDF() when the function does not declare its affinity.
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
  const Scalar rangeMargin = ResourceMap::GetAsScalar("PushForwardDistribution-RangeMargin");
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    // Add a margin to each marginal span to avoid rejecting points lying on
    // the edge of the sampled support
    const Scalar halfSpan = rangeMargin * (upperBound[i] - lowerBound[i]);
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
  const Scalar upperQuantile = 1.0 - quantileEpsilon_;
  Point lowerBound(antecedentRange.getLowerBound());
  Point upperBound(antecedentRange.getUpperBound());
  const Point mean(antecedent_.getMean());
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (!finiteLowerBound[i] || !finiteUpperBound[i])
    {
      const Distribution marginal(antecedent_.getMarginal(i));
      if (!finiteLowerBound[i]) lowerBound[i] = marginal.computeQuantile(quantileEpsilon_)[0];
      if (!finiteUpperBound[i]) upperBound[i] = marginal.computeQuantile(upperQuantile)[0];
    }
    if (!(lowerBound[i] < upperBound[i]))
    {
      // A continuous distribution with a non-degenerate support normally
      // gives a positive width. If a numerical quantile still collapses the
      // interval, fall back to a finite box around the mean whose half width
      // is controlled by the ResourceMap, so that the search remains bounded.
      const Scalar halfWidth = ResourceMap::GetAsScalar("PushForwardDistribution-DegenerateSearchIntervalHalfWidth");
      lowerBound[i] = mean[i] - halfWidth;
      upperBound[i] = mean[i] + halfWidth;
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
  // computePDF() routes only affine functions here: a nonlinear co-area map
  // is rejected before this point. The Jacobian matrix is therefore constant
  // and is evaluated once, at the mean of the antecedent.
  const Point mean(antecedent_.getMean());
  const Matrix gradientMatrix(function_.gradient(mean));
  if ((gradientMatrix.getNbRows() != inputDimension) || (gradientMatrix.getNbColumns() != outputDimension))
    throw InternalException(HERE) << "Error: the given function has no actual gradient. Consider using finite differences.";
  const Interval searchInterval(searchInterval_);
  const Point lowerBound(searchInterval.getLowerBound());
  const Point upperBound(searchInterval.getUpperBound());
  const Point meanImage(function_(mean));
  // Full SVD of the n x p Jacobian matrix J of the function: its p singular
  // values give the p-dimensional Jacobian |J_p f| = prod sigma_i, i.e. the
  // factor relating the p-dimensional Hausdorff measure on the image to the
  // n-dimensional Lebesgue measure of the antecedent, and the last n - p
  // columns of the full left factor span the kernel of J, i.e. the directions
  // along which the function is constant, which are precisely the tangent
  // directions of the fibre.
  // Matrix::computeSVD is used directly rather than SVDMethod: the latter is
  // a least-squares solver based on an economic SVD, it throws on a zero
  // singular value and exposes neither the full left factor needed for the
  // kernel nor the residual needed to test membership of the image. The
  // co-area formula needs all of these
  Matrix leftFactor;
  Matrix rightFactorTransposed;
  const Point singularValues(gradientMatrix.computeSVD(leftFactor, rightFactorTransposed, true));
  // A singular value below this threshold is numerically zero, so that the
  // corresponding direction of the output space is not reached at all
  const Scalar rankTolerance = inputDimension * SpecFunc::ScalarEpsilon * singularValues[0];
  UnsignedInteger rank = 0;
  Scalar jacobian = 1.0;
  for (UnsignedInteger index = 0; index < outputDimension; ++index)
  {
    if (singularValues[index] > rankTolerance)
    {
      jacobian *= singularValues[index];
      ++rank;
    }
  }
  const Bool rankDeficient = rank < outputDimension;
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
  // deficient. A point that is even slightly off the image has an empty fibre
  // and a zero density, while a point exactly on it carries a singular
  // density. The threshold is the CDF epsilon of the distribution, which is
  // the natural relative tolerance for a measure zero test
  if (std::sqrt(imageResidual) > cdfEpsilon_ * std::max(1.0, target.norm()))
    // The point is not in the image of the function: its fibre is empty
    return 0.0;
  const Point origin(mean + delta);
  if (rankDeficient)
  {
    // The image of the function is contained in a set of zero measure of the
    // output space: the density is singular wherever the fibre carries mass.
    // The algebraic image membership above is not enough: the fibre must also
    // meet the support of the antecedent. Check the affine subspace against
    // the finite bounds of that support before returning the singular density
    Matrix kernel(inputDimension, inputDimension - rank);
    UnsignedInteger kernelIndex = 0;
    for (UnsignedInteger index = 0; index < outputDimension; ++index)
      if (singularValues[index] <= rankTolerance)
      {
        for (UnsignedInteger i = 0; i < inputDimension; ++i)
          kernel(i, kernelIndex) = leftFactor(i, index);
        ++kernelIndex;
      }
    for (UnsignedInteger index = outputDimension; index < inputDimension; ++index)
    {
      for (UnsignedInteger i = 0; i < inputDimension; ++i)
        kernel(i, kernelIndex) = leftFactor(i, index);
      ++kernelIndex;
    }
    const Interval antecedentRange(antecedent_.getRange());
    const Scalar constraintTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-CoAreaConstraintTolerance");
    const Scalar feasibilityTolerance = constraintTolerance * std::max(1.0, (upperBound - lowerBound).norm());
    if (!PushForwardDistributionFunctions::checkCoAreaSupport(kernel, origin,
                                                               antecedentRange.getLowerBound(),
                                                               antecedentRange.getUpperBound(),
                                                               antecedentRange.getFiniteLowerBound(),
                                                               antecedentRange.getFiniteUpperBound(),
                                                               feasibilityTolerance,
                                                               constraintTolerance))
      // The fibre does not meet the support of the antecedent: its density is
      // zero, even though the point belongs to the algebraic image
      return 0.0;
    return SpecFunc::Infinity;
  }
  // Orthonormal basis of the kernel of J: it is made of the last n - p
  // columns of the full left factor of the SVD. Being orthonormal, the
  // Hausdorff element along the fibre parameterised as origin + t * direction
  // for d = 1, resp. origin + kernel * t for d >= 2, is exactly dt
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
    // miss the bulk of the mass. The threshold is a numerical zero on the
    // density at the end point, not a requested relative accuracy: a large
    // end point multiplied by this density still estimates the neglected tail
    const Scalar negligibleIntegrand = ResourceMap::GetAsScalar("PushForwardDistribution-CoAreaTailEpsilon") * densityScale;
    if (densityScale > 0.0)
    {
      const UnsignedInteger maximumExtensions = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-CoAreaMaximumExtensions");
      for (UnsignedInteger extension = 0; (extension < maximumExtensions) && (antecedent_.computePDF(origin + tMax * direction) > negligibleIntegrand * jacobian); ++extension)
        tMax += tMax - tMin;
      for (UnsignedInteger extension = 0; (extension < maximumExtensions) && (antecedent_.computePDF(origin + tMin * direction) > negligibleIntegrand * jacobian); ++extension)
        tMin -= tMax - tMin;
    }
    const Matrix kernel(inputDimension, 1, direction);
    const PushForwardDistributionFunctions::CoAreaIntegrand integrand(origin, kernel, antecedent_, jacobian);
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
  // The normals of the constraints are built from the rows of an orthonormal
  // basis of the kernel of the Jacobian and combined by the Fourier-Motzkin
  // elimination below: their entries are of order one and their rounding error
  // is a small multiple of the machine precision, amplified by the elimination.
  // Two unit normals closer than the constraint tolerance are numerically the
  // same direction, and a coefficient below it is numerically zero. It is far
  // above the machine precision to absorb the amplification, but small enough
  // not to merge distinct facets of the projected box.
  const Scalar constraintTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-CoAreaConstraintTolerance");
  // The parameter t has the same scale as the point x because the kernel is
  // orthonormal, so the diagonal of the search box bounds the extent of the
  // fibre and gives the scale of a meaningful feasibility tolerance
  const Scalar feasibilityTolerance = constraintTolerance * std::max(1.0, (upperBound - lowerBound).norm());
  PushForwardDistributionFunctions::CoAreaConstraintCollection constraints(0);
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    Point normal(fibreDimension);
    for (UnsignedInteger j = 0; j < fibreDimension; ++j)
      normal[j] = kernel(i, j);
    PushForwardDistributionFunctions::CoAreaConstraint constraint;
    constraint.normal = normal;
    constraint.bound = upperBound[i] - origin[i];
    constraints.add(constraint);
    // The same face seen from the other side: -normal . t <= origin[i] - lower[i]
    constraint.normal = -normal;
    constraint.bound = origin[i] - lowerBound[i];
    constraints.add(constraint);
  }
  // systems[k] describes the exact projection of the polytope onto
  // (t_0, ..., t_k), computed once by eliminating the coordinates from the
  // last one to the first. The nested interval description of the fibre is
  // then exact: at a feasible prefix, the extent of t_k is the set of values
  // satisfying systems[k], and the kinks of these piecewise affine bounds are
  // left to the adaptive quadrature rule
  PushForwardDistributionFunctions::CoAreaConstraintCollection current(0);
  if (!PushForwardDistributionFunctions::pruneCoAreaConstraints(constraints, feasibilityTolerance, constraintTolerance, current))
    // The fibre does not meet the search box: its density is zero
    return 0.0;
  Collection<PushForwardDistributionFunctions::CoAreaConstraintCollection> systems(fibreDimension + 1);
  systems[fibreDimension] = current;
  for (UnsignedInteger level = fibreDimension; level > 0; --level)
  {
    const UnsignedInteger coordinate = level - 1;
    systems[coordinate] = current;
    PushForwardDistributionFunctions::CoAreaConstraintCollection eliminated(0);
    if (!PushForwardDistributionFunctions::eliminateCoAreaConstraint(current, coordinate, feasibilityTolerance, constraintTolerance, eliminated))
      // The projection is empty: the fibre does not meet the search box
      return 0.0;
    current = eliminated;
  }
  Scalar tMin = 0.0;
  Scalar tMax = 0.0;
  if (!PushForwardDistributionFunctions::computeCoAreaBounds(systems[0], 0, Point(0), feasibilityTolerance, constraintTolerance, tMin, tMax))
    return 0.0;
  if (!(tMin < tMax)) return 0.0;
  // Magnitude of the integrand at the point of the fibre closest to the mean:
  // it scales the error requested from the adaptive rule, as in the one
  // dimensional case
  const Scalar densityScale = antecedent_.computePDF(origin) / jacobian;
  // The nested bounds are piecewise affine functions with kinks along the
  // projected facets of the polytope. Adaptive quadrature converges only
  // algebraically across a kink: for a fibre of dimension three the measured
  // relative error stalls above 1e-9, and the nested error of the inner
  // integrals prevents any gain from a tighter request. For the sum of four
  // uniform variables, a request of 1e-7 reaches 5.5e-9 in 8.5 s whereas a
  // request of 1e-8 reaches 7.4e-9 in 57 s. Floor the request at the
  // PushForwardDistribution-CoAreaErrorFloor value for d >= 3. The one and
  // two dimensional fibres reached 3e-16 on the same integrand at a request of
  // 1e-14, so they are not floored.
  Scalar requestedError = ResourceMap::GetAsScalar("PushForwardDistribution-CoAreaMaximumError");
  if (fibreDimension > 2)
    requestedError = std::max(requestedError, ResourceMap::GetAsScalar("PushForwardDistribution-CoAreaErrorFloor"));
  // The floor on densityScale only avoids a zero absolute error request when
  // the density vanishes at the origin; it is a numerical safeguard, not an
  // accuracy target.
  const GaussKronrod algorithm(ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-CoAreaMaximumSubIntervals"),
                               requestedError * std::max(densityScale, SpecFunc::ScalarEpsilon),
                               GaussKronrodRule(GaussKronrodRule::G7K15));
  const PushForwardDistributionFunctions::CoAreaIntegrand integrand(origin, kernel, antecedent_, jacobian);
  IteratedQuadrature::FunctionCollection lowerBounds(fibreDimension - 1);
  IteratedQuadrature::FunctionCollection upperBounds(fibreDimension - 1);
  for (UnsignedInteger level = 1; level < fibreDimension; ++level)
  {
    lowerBounds[level - 1] = Function(new PushForwardDistributionFunctions::CoAreaBoundFunction(systems[level], level, false, feasibilityTolerance, constraintTolerance));
    upperBounds[level - 1] = Function(new PushForwardDistributionFunctions::CoAreaBoundFunction(systems[level], level, true, feasibilityTolerance, constraintTolerance));
  }
  return IteratedQuadrature(algorithm).integrate(Function(integrand), tMin, tMax, lowerBounds, upperBounds)[0];
}

/* Compute the PDF in the scalar nonlinear co-area case f: R^3 -> R. The
   tetrahedral mesh of the search box supplies the topology of the level
   surface: a tetrahedron is crossed when its vertices are not all on the same
   side of the requested level. The intersection of the level with a crossed
   tetrahedron is a convex polygon whose vertices lie on the actual level set:
   each crossed edge is solved with a scalar Brent solve on the actual
   function. The polygon is triangulated by an angular sort in a local tangent
   plane, and the antecedent density divided by the norm of the gradient of f
   is integrated over the resulting surface mesh with SimplicialCubature.
   This is the two-dimensional analogue of the scalar R^2 path, restricted to
   a regular level surface, and it keeps the affine co-area path dedicated to
   the exact affine case. */
Scalar PushForwardDistribution::computePDFByNonlinearCoAreaSurface(const Point & point) const
{
  const Scalar level = point[0];
  const UnsignedInteger discretization = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-NonlinearCoAreaSurfaceDiscretization");
  const Scalar levelTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-NonlinearCoAreaLevelTolerance");
  // A uniform tetrahedral mesh of the search box is enough for a first
  // deterministic implementation. The level crossings are polished on the
  // actual function, so the geometric error is quadratic in the mesh size,
  // and the resource key lets the caller trade cost for accuracy without
  // changing the algorithm.
  const Mesh mesh(IntervalMesher(Indices(3, discretization)).build(searchInterval_));
  const Sample vertexValues(function_(mesh.getVertices()));
  const Brent edgeSolver;
  const Scalar pointTolerance = levelTolerance * std::max(1.0, (searchInterval_.getUpperBound() - searchInterval_.getLowerBound()).norm());
  // The surface mesh is stored as a flat list of triangles. Each triangle has
  // three new vertices, padded with a repeated vertex to make the ambient
  // dimension three and the intrinsic dimension two, as expected by Mesh and
  // SimplicialCubature.
  Sample surfaceVertices(0, 3);
  const auto addTriangle = [&](const Point & first, const Point & second, const Point & third)
  {
    const Point firstEdge(second - first);
    const Point secondEdge(third - first);
    Point normal(3);
    normal[0] = firstEdge[1] * secondEdge[2] - firstEdge[2] * secondEdge[1];
    normal[1] = firstEdge[2] * secondEdge[0] - firstEdge[0] * secondEdge[2];
    normal[2] = firstEdge[0] * secondEdge[1] - firstEdge[1] * secondEdge[0];
    if (!(normal.norm() > 0.0))
      return;
    surfaceVertices.add(first);
    surfaceVertices.add(second);
    surfaceVertices.add(third);
  };
  // When the level set coincides exactly with a tetrahedron face, the two
  // adjacent tetrahedra see the same face. Count it once; this only happens
  // on a set of level values of measure zero, but it keeps the result stable
  // for round levels such as the mean of a symmetric distribution.
  std::set<std::array<UnsignedInteger, 3>> countedFaces;
  for (UnsignedInteger simplexIndex = 0; simplexIndex < mesh.getSimplicesNumber(); ++simplexIndex)
  {
    const Indices simplex(mesh.getSimplex(simplexIndex));
    if (simplex.getSize() != 4)
      continue;
    Point x[4];
    Scalar value[4];
    SignedInteger sign[4];
    for (UnsignedInteger vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
    {
      x[vertexIndex] = mesh.getVertex(simplex[vertexIndex]);
      value[vertexIndex] = vertexValues(simplex[vertexIndex], 0);
      // The tolerance is relative to the level and to the local values, so
      // that a vertex is classified as lying on the level set consistently
      // with the floating point representation of f.
      const Scalar scale = std::max(1.0, std::max(std::abs(level), std::abs(value[vertexIndex])));
      const Scalar tolerance = levelTolerance * scale;
      sign[vertexIndex] = (value[vertexIndex] < level - tolerance) ? -1 : ((value[vertexIndex] > level + tolerance) ? 1 : 0);
    }
    const UnsignedInteger zeroCount = (sign[0] == 0) + (sign[1] == 0) + (sign[2] == 0) + (sign[3] == 0);
    // A whole tetrahedron at the requested level is a three dimensional part
    // of the fibre: the two dimensional co-area density is singular there.
    if (zeroCount == 4)
      return SpecFunc::Infinity;
    const auto addCrossing = [&](const UnsignedInteger firstVertex, const UnsignedInteger secondVertex) -> Point
    {
      // The two vertices are on opposite sides of the level. Solve
      // f(x) = level on the edge instead of using the P1 interpolation only:
      // the root is bracketed, and this keeps the surface vertices on the
      // actual level set even when the function is strongly curved.
      const Point edgeDirection(x[secondVertex] - x[firstVertex]);
      const LinearFunction edge(Point(1, 0.0), x[firstVertex], Matrix(3, 1, edgeDirection));
      const Function edgeFunction(ComposedFunction(function_, edge));
      Scalar parameter = (level - value[firstVertex]) / (value[secondVertex] - value[firstVertex]);
      try
      {
        parameter = edgeSolver.solve(edgeFunction, level, 0.0, 1.0, value[firstVertex], value[secondVertex]);
      }
      catch (const Exception &)
      {
        // Keep the linear estimate when the scalar solver cannot improve it.
      }
      if (!(parameter >= 0.0) || !(parameter <= 1.0))
        parameter = (level - value[firstVertex]) / (value[secondVertex] - value[firstVertex]);
      return x[firstVertex] + parameter * edgeDirection;
    };
    std::vector<Point> points;
    const auto addPoint = [&](const Point & pointToAdd)
    {
      // Consecutive crossings can coincide when the level passes through a
      // mesh vertex. Drop such duplicates, which would only add degenerate
      // triangles to the surface mesh.
      for (UnsignedInteger i = 0; i < points.size(); ++i)
        if ((points[i] - pointToAdd).norm() <= pointTolerance)
          return;
      points.push_back(pointToAdd);
    };
    if (zeroCount == 3)
    {
      Indices zeroVertices(0);
      for (UnsignedInteger vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
        if (sign[vertexIndex] == 0)
          zeroVertices.add(vertexIndex);
      std::array<UnsignedInteger, 3> face = {simplex[zeroVertices[0]], simplex[zeroVertices[1]], simplex[zeroVertices[2]]};
      std::sort(face.begin(), face.end());
      if (countedFaces.insert(face).second)
      {
        // The face is already a part of the level surface. Add its vertices
        // in the order of the tetrahedron to preserve a consistent polygon.
        for (UnsignedInteger vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
          if (sign[vertexIndex] == 0)
            addPoint(x[vertexIndex]);
      }
    }
    else if (zeroCount == 2)
    {
      Indices zeroVertices(0);
      Indices otherVertices(0);
      for (UnsignedInteger vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
        if (sign[vertexIndex] == 0)
          zeroVertices.add(vertexIndex);
        else
          otherVertices.add(vertexIndex);
      // If the two other vertices are on the same side, the level only
      // touches the tetrahedron along an edge, whose area is zero.
      if (sign[otherVertices[0]] * sign[otherVertices[1]] < 0)
      {
        addPoint(x[zeroVertices[0]]);
        addPoint(x[zeroVertices[1]]);
        addPoint(addCrossing(otherVertices[0], otherVertices[1]));
      }
    }
    else if (zeroCount == 1)
    {
      UnsignedInteger zeroVertex = 4;
      Indices otherVertices(0);
      for (UnsignedInteger vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
        if (sign[vertexIndex] == 0)
          zeroVertex = vertexIndex;
        else
          otherVertices.add(vertexIndex);
      // If the three other vertices are on the same side, the level only
      // touches the tetrahedron at the vertex: its area is zero. Otherwise
      // the intersection is the vertex plus the crossings of the two edges
      // joining the opposite vertex to the two vertices on the other side.
      if (!((sign[otherVertices[0]] == sign[otherVertices[1]]) && (sign[otherVertices[1]] == sign[otherVertices[2]])))
      {
        addPoint(x[zeroVertex]);
        for (UnsignedInteger first = 0; first < 3; ++first)
          for (UnsignedInteger second = first + 1; second < 3; ++second)
            if (sign[otherVertices[first]] * sign[otherVertices[second]] < 0)
              addPoint(addCrossing(otherVertices[first], otherVertices[second]));
      }
    }
    else
    {
      // No vertex is on the level: the level either crosses three or four
      // edges, giving a triangle or a quadrilateral. Find every crossing.
      for (UnsignedInteger first = 0; first < 4; ++first)
        for (UnsignedInteger second = first + 1; second < 4; ++second)
          if (sign[first] * sign[second] < 0)
            addPoint(addCrossing(first, second));
    }
    if (points.size() < 3)
      continue;
    // For a quadrilateral, order the points around their centroid in a local
    // tangent plane before building the two triangles. A triangle does not
    // need an ordering, but the same projection is harmless.
    if (points.size() > 3)
    {
      Point normal(3);
      // Use the constant P1 gradient of f on the tetrahedron as the normal of
      // the local tangent plane. When the tetrahedron is degenerate, fall
      // back to the cross product of two of its edges.
      Matrix edgeMatrix(3, 3);
      Point valueDifference(3);
      for (UnsignedInteger row = 0; row < 3; ++row)
      {
        const Point edge(x[row + 1] - x[0]);
        for (UnsignedInteger column = 0; column < 3; ++column)
          edgeMatrix(row, column) = edge[column];
        valueDifference[row] = value[row + 1] - value[0];
      }
      try
      {
        const Point gradient(edgeMatrix.solveLinearSystem(valueDifference));
        normal = gradient;
      }
      catch (const Exception &)
      {
        // Fall through to the geometric normal below
      }
      if (!(normal.norm() > 0.0))
      {
        for (UnsignedInteger first = 0; (first < 4) && !(normal.norm() > 0.0); ++first)
          for (UnsignedInteger second = first + 1; (second < 4) && !(normal.norm() > 0.0); ++second)
            for (UnsignedInteger third = second + 1; (third < 4) && !(normal.norm() > 0.0); ++third)
            {
              const Point firstEdge(x[second] - x[first]);
              const Point secondEdge(x[third] - x[first]);
              normal[0] = firstEdge[1] * secondEdge[2] - firstEdge[2] * secondEdge[1];
              normal[1] = firstEdge[2] * secondEdge[0] - firstEdge[0] * secondEdge[2];
              normal[2] = firstEdge[0] * secondEdge[1] - firstEdge[1] * secondEdge[0];
            }
      }
      const Scalar normalNorm = normal.norm();
      if (!(normalNorm > 0.0))
        continue;
      normal /= normalNorm;
      // Build an orthonormal basis of the tangent plane. The coordinate axis
      // least aligned with the normal gives a stable first direction.
      UnsignedInteger axis = 0;
      for (UnsignedInteger coordinate = 1; coordinate < 3; ++coordinate)
        if (std::abs(normal[coordinate]) < std::abs(normal[axis]))
          axis = coordinate;
      Point axisVector(3, 0.0);
      axisVector[axis] = 1.0;
      Point tangent(3);
      tangent[0] = normal[1] * axisVector[2] - normal[2] * axisVector[1];
      tangent[1] = normal[2] * axisVector[0] - normal[0] * axisVector[2];
      tangent[2] = normal[0] * axisVector[1] - normal[1] * axisVector[0];
      const Scalar tangentNorm = tangent.norm();
      if (!(tangentNorm > 0.0))
        continue;
      tangent /= tangentNorm;
      Point binormal(3);
      binormal[0] = normal[1] * tangent[2] - normal[2] * tangent[1];
      binormal[1] = normal[2] * tangent[0] - normal[0] * tangent[2];
      binormal[2] = normal[0] * tangent[1] - normal[1] * tangent[0];
      Point centroid(3);
      for (UnsignedInteger i = 0; i < points.size(); ++i)
        centroid += points[i];
      centroid /= static_cast<Scalar>(points.size());
      std::vector<std::pair<Scalar, UnsignedInteger>> angles(points.size());
      for (UnsignedInteger i = 0; i < points.size(); ++i)
      {
        const Point offset(points[i] - centroid);
        angles[i] = std::make_pair(std::atan2(offset.dot(binormal), offset.dot(tangent)), i);
      }
      std::sort(angles.begin(), angles.end());
      std::vector<Point> orderedPoints(points.size());
      for (UnsignedInteger i = 0; i < points.size(); ++i)
        orderedPoints[i] = points[angles[i].second];
      points = orderedPoints;
    }
    // Triangulate the polygon as a fan from its first vertex.
    for (UnsignedInteger i = 1; i + 1 < points.size(); ++i)
      addTriangle(points[0], points[i], points[i + 1]);
  }
  if (surfaceVertices.getSize() == 0)
    return 0.0;
  // Pack the triangles as intrinsic two dimensional simplices embedded in R^3:
  // the last vertex index is repeated, which Mesh and SimplicialCubature both
  // understand as a lower intrinsic dimension.
  const UnsignedInteger triangleNumber = surfaceVertices.getSize() / 3;
  Indices flatSimplices(4 * triangleNumber);
  for (UnsignedInteger triangleIndex = 0; triangleIndex < triangleNumber; ++triangleIndex)
  {
    flatSimplices[4 * triangleIndex] = 3 * triangleIndex;
    flatSimplices[4 * triangleIndex + 1] = 3 * triangleIndex + 1;
    flatSimplices[4 * triangleIndex + 2] = 3 * triangleIndex + 2;
    flatSimplices[4 * triangleIndex + 3] = 3 * triangleIndex + 2;
  }
  const Mesh surfaceMesh(surfaceVertices, IndicesCollection(triangleNumber, 4, flatSimplices));
  const std::shared_ptr<std::atomic<Bool>> singular = std::make_shared<std::atomic<Bool>>(false);
  const Function integrand(new PushForwardDistributionFunctions::NonlinearCoAreaSurfaceIntegrand(antecedent_, function_, singular));
  const Point integral(SimplicialCubature().integrate(integrand, surfaceMesh));
  if (singular->load())
    return SpecFunc::Infinity;
  return integral[0];
}

/* Compute the PDF in the nonlinear co-area case f: R^n -> R^{n-1} with
   n >= 3. The n-dimensional mesh of the search box supplies the topology of
   the fibre: in each n-simplex, the n - 1 components of f are replaced by
   their P1 interpolations, and the intersection of the corresponding level
   hyperplanes with the simplex is a segment. The segment is obtained from the
   null space of the n x (n + 1) barycentric system, clipped to the simplex,
   and both ends are polished on the active facet by an (n - 1)-dimensional
   damped Newton solve of f(x) = point on the actual function. The antecedent
   density divided by the (n - 1)-dimensional Jacobian is then integrated
   along the segment. The same code handles R^3 -> R^2 and R^4 -> R^3; only
   the background mesh and the discretization key depend on the dimension. */
Scalar PushForwardDistribution::computePDFByNonlinearCoAreaCurve(const Point & point) const
{
  const UnsignedInteger inputDimension = function_.getInputDimension();
  const UnsignedInteger outputDimension = function_.getOutputDimension();
  if ((inputDimension < 3) || (outputDimension + 1 != inputDimension))
    throw InvalidArgumentException(HERE)
        << "Error: the nonlinear curve co-area path expects a map from R^n to R^(n-1) with n >= 3, here input dimension="
        << inputDimension << " and output dimension=" << outputDimension;
  const UnsignedInteger simplexSize = inputDimension + 1;
  const String discretizationKey = (inputDimension == 3)
                                   ? "PushForwardDistribution-NonlinearCoAreaSurfaceDiscretization"
                                   : "PushForwardDistribution-NonlinearCoAreaVolumeDiscretization";
  const UnsignedInteger discretization = ResourceMap::GetAsUnsignedInteger(discretizationKey);
  const Scalar levelTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-NonlinearCoAreaLevelTolerance");
  // The uniform mesh of the search box is the global seeder. The level
  // crossings are polished on the actual function, so the geometric error is
  // quadratic in the mesh size, and the resource key lets the caller trade
  // cost for accuracy without changing the algorithm.
  const Mesh mesh(IntervalMesher(Indices(inputDimension, discretization)).build(searchInterval_));
  const Sample vertexValues(function_(mesh.getVertices()));
  const GaussLegendre quadrature(1);
  const Scalar pointTolerance = levelTolerance * std::max(1.0, (searchInterval_.getUpperBound() - searchInterval_.getLowerBound()).norm());
  const Scalar barycentricTolerance = 1.0e-10;
  // Two adjacent simplices can produce the same segment when the fibre lies
  // on a shared facet or on a lower dimensional face of the mesh. Deduplicate
  // by the two polished endpoints rather than by the simplex face: a line in
  // R^4 can lie in a triangular face, which is neither a facet nor an edge.
  const Scalar segmentDedupTolerance = 1.0e-10 * std::max(1.0, (searchInterval_.getUpperBound() - searchInterval_.getLowerBound()).norm());
  const std::shared_ptr<std::atomic<Bool>> singular = std::make_shared<std::atomic<Bool>>(false);
  Scalar density = 0.0;
  Collection<LevelSetContinuation::Segment> continuationSegments;
  std::set<std::vector<SignedInteger>> countedSegments;
  for (UnsignedInteger simplexIndex = 0; simplexIndex < mesh.getSimplicesNumber(); ++simplexIndex)
  {
    const Indices simplex(mesh.getSimplex(simplexIndex));
    if (simplex.getSize() != simplexSize)
      continue;
    std::vector<Point> x(simplexSize);
    std::vector<Point> value(simplexSize);
    std::vector<Point> delta(simplexSize);
    std::vector<Bool> positive(outputDimension, false);
    std::vector<Bool> negative(outputDimension, false);
    std::vector<Bool> onLevel(outputDimension, false);
    Bool allOnLevel = true;
    for (UnsignedInteger vertexIndex = 0; vertexIndex < simplexSize; ++vertexIndex)
    {
      x[vertexIndex] = mesh.getVertex(simplex[vertexIndex]);
      value[vertexIndex] = Point(outputDimension);
      for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++outputIndex)
        value[vertexIndex][outputIndex] = vertexValues(simplex[vertexIndex], outputIndex);
      delta[vertexIndex] = value[vertexIndex] - point;
      if (!(delta[vertexIndex].norm() <= levelTolerance * std::max(1.0, point.norm())))
        allOnLevel = false;
      for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++outputIndex)
      {
        const Scalar tolerance = levelTolerance * std::max(1.0, std::abs(point[outputIndex]));
        if (delta[vertexIndex][outputIndex] > tolerance)
          positive[outputIndex] = true;
        else if (delta[vertexIndex][outputIndex] < -tolerance)
          negative[outputIndex] = true;
        else
          onLevel[outputIndex] = true;
      }
    }
    // A whole simplex maps to the requested point: the n-dimensional part of
    // the fibre makes the one dimensional co-area density singular.
    if (allOnLevel)
      return SpecFunc::Infinity;
    // Cheap rejection: a simplex whose vertices are all strictly on the same
    // side of one of the levels cannot contain the fibre.
    Bool crosses = true;
    for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++outputIndex)
      if (!(onLevel[outputIndex] || (positive[outputIndex] && negative[outputIndex])))
      {
        crosses = false;
        break;
      }
    if (!crosses)
      continue;
    // The P1 fibre is the set of barycentric coordinates lambda such that
    // sum_i lambda_i = 1 and sum_i lambda_i delta_i = 0. This is an
    // n x (n + 1) linear system; its null vector gives the direction of the
    // fibre and a particular solution is obtained by setting its largest
    // component to 0.
    Matrix system(inputDimension, simplexSize);
    for (UnsignedInteger column = 0; column < simplexSize; ++column)
    {
      for (UnsignedInteger row = 0; row < outputDimension; ++row)
        system(row, column) = delta[column][row];
      system(outputDimension, column) = 1.0;
    }
    Point nullVector(simplexSize);
    for (UnsignedInteger column = 0; column < simplexSize; ++column)
    {
      SquareMatrix minorMatrix(inputDimension);
      UnsignedInteger reducedColumn = 0;
      for (UnsignedInteger otherColumn = 0; otherColumn < simplexSize; ++otherColumn)
      {
        if (otherColumn == column)
          continue;
        for (UnsignedInteger row = 0; row < inputDimension; ++row)
          minorMatrix(row, reducedColumn) = system(row, otherColumn);
        ++reducedColumn;
      }
      const Scalar sign = (column % 2 == 0) ? 1.0 : -1.0;
      nullVector[column] = sign * minorMatrix.computeDeterminant();
    }
    const Scalar nullNorm = nullVector.norm();
    const Scalar nullTolerance = 1.0e-12 * nullNorm;
    if (!(nullNorm > 0.0))
      // The P1 level hyperplanes are parallel or coincident: no well defined
      // curve in this simplex. The true fibre may still be regular, but the
      // neighbouring simplices carry its segments.
      continue;
    UnsignedInteger freeColumn = 0;
    for (UnsignedInteger column = 1; column < simplexSize; ++column)
      if (std::abs(nullVector[column]) > std::abs(nullVector[freeColumn]))
        freeColumn = column;
    SquareMatrix reducedSystem(inputDimension);
    UnsignedInteger reducedColumn = 0;
    for (UnsignedInteger column = 0; column < simplexSize; ++column)
    {
      if (column == freeColumn)
        continue;
      for (UnsignedInteger row = 0; row < inputDimension; ++row)
        reducedSystem(row, reducedColumn) = system(row, column);
      ++reducedColumn;
    }
    Point rightHandSide(inputDimension, 0.0);
    rightHandSide[outputDimension] = 1.0;
    Point particularSolution(simplexSize, 0.0);
    try
    {
      const Point reducedSolution(reducedSystem.solveLinearSystem(rightHandSide));
      reducedColumn = 0;
      for (UnsignedInteger column = 0; column < simplexSize; ++column)
      {
        if (column == freeColumn)
          continue;
        particularSolution[column] = reducedSolution[reducedColumn];
        ++reducedColumn;
      }
    }
    catch (const Exception &)
    {
      continue;
    }
    // Intersect the barycentric line lambda(t) = particular + t * null with
    // the simplex lambda_i >= 0. The active constraints give the two facets
    // where the fibre enters and leaves the simplex.
    Scalar lowerParameter = -SpecFunc::MaxScalar;
    Scalar upperParameter = SpecFunc::MaxScalar;
    UnsignedInteger lowerFace = simplexSize;
    UnsignedInteger upperFace = simplexSize;
    Bool feasible = true;
    for (UnsignedInteger vertexIndex = 0; vertexIndex < simplexSize; ++vertexIndex)
    {
      const Scalar directionComponent = nullVector[vertexIndex];
      if (std::abs(directionComponent) <= nullTolerance)
      {
        if (particularSolution[vertexIndex] < -barycentricTolerance)
        {
          feasible = false;
          break;
        }
        continue;
      }
      const Scalar bound = -particularSolution[vertexIndex] / directionComponent;
      if (directionComponent > 0.0)
      {
        if (bound > lowerParameter)
        {
          lowerParameter = bound;
          lowerFace = vertexIndex;
        }
      }
      else if (bound < upperParameter)
      {
        upperParameter = bound;
        upperFace = vertexIndex;
      }
    }
    if (!feasible || (lowerFace == simplexSize) || (upperFace == simplexSize))
      continue;
    if (upperParameter < lowerParameter - barycentricTolerance)
      continue;
    if (upperParameter - lowerParameter <= barycentricTolerance)
      continue;
    Point lowerBarycentric(simplexSize);
    Point upperBarycentric(simplexSize);
    for (UnsignedInteger vertexIndex = 0; vertexIndex < simplexSize; ++vertexIndex)
    {
      lowerBarycentric[vertexIndex] = particularSolution[vertexIndex] + lowerParameter * nullVector[vertexIndex];
      upperBarycentric[vertexIndex] = particularSolution[vertexIndex] + upperParameter * nullVector[vertexIndex];
    }
    const auto pointFromBarycentric = [&](const Point & barycentric) -> Point
    {
      Point barycentricPoint(inputDimension, 0.0);
      Scalar barycentricSum = 0.0;
      for (UnsignedInteger vertexIndex = 0; vertexIndex < simplexSize; ++vertexIndex)
      {
        barycentricPoint += barycentric[vertexIndex] * x[vertexIndex];
        barycentricSum += barycentric[vertexIndex];
      }
      if (barycentricSum != 0.0)
        barycentricPoint /= barycentricSum;
      return barycentricPoint;
    };
    // Polish a P1 endpoint on the corresponding facet of the simplex by
    // solving the n - 1 actual equations f(x) = point in the n - 1
    // barycentric coordinates of that facet.
    const auto refineOnFace = [&](const Point & linearPoint,
                                  const UnsignedInteger faceIndex,
                                  const Point & barycentric) -> Point
    {
      Indices faceVertices(0);
      for (UnsignedInteger vertexIndex = 0; vertexIndex < simplexSize; ++vertexIndex)
        if (vertexIndex != faceIndex)
          faceVertices.add(vertexIndex);
      if (faceVertices.getSize() != inputDimension)
        return linearPoint;
      const UnsignedInteger firstVertex = faceVertices[0];
      Matrix faceMatrix(inputDimension, inputDimension - 1);
      for (UnsignedInteger row = 0; row < inputDimension; ++row)
        for (UnsignedInteger column = 1; column < inputDimension; ++column)
          faceMatrix(row, column - 1) = x[faceVertices[column]][row] - x[firstVertex][row];
      Point startingPoint(inputDimension - 1);
      Scalar parameterSum = 0.0;
      for (UnsignedInteger column = 1; column < inputDimension; ++column)
      {
        Scalar parameter = barycentric[faceVertices[column]];
        parameter = std::max(0.0, std::min(1.0 - parameterSum, parameter));
        startingPoint[column - 1] = parameter;
        parameterSum += parameter;
      }
      try
      {
        const LinearFunction faceFunction(Point(inputDimension - 1, 0.0), x[firstVertex], faceMatrix);
        const Function faceResidual(ComposedFunction(function_, faceFunction) - ConstantFunction(outputDimension, point));
        const Point root(dampedNewton(faceResidual, startingPoint));
        if (root.getSize() == inputDimension - 1)
        {
          const Scalar simplexTolerance = 1.0e-8;
          Scalar rootSum = 0.0;
          Bool inside = true;
          for (UnsignedInteger column = 0; column < inputDimension - 1; ++column)
          {
            if (root[column] < -simplexTolerance)
            {
              inside = false;
              break;
            }
            rootSum += root[column];
          }
          if (inside && (rootSum <= 1.0 + simplexTolerance))
          {
            Point parameters(inputDimension - 1);
            Scalar remaining = 1.0;
            for (UnsignedInteger column = 0; column < inputDimension - 1; ++column)
            {
              const Scalar parameter = std::max(0.0, std::min(remaining, root[column]));
              parameters[column] = parameter;
              remaining -= parameter;
            }
            return faceFunction(parameters);
          }
        }
      }
      catch (const Exception &)
      {
        // Keep the P1 endpoint when the face solve is not available.
      }
      return linearPoint;
    };
    const Point firstPoint(refineOnFace(pointFromBarycentric(lowerBarycentric), lowerFace, lowerBarycentric));
    const Point secondPoint(refineOnFace(pointFromBarycentric(upperBarycentric), upperFace, upperBarycentric));
    const Point direction(secondPoint - firstPoint);
    const Scalar length = direction.norm();
    if (!(length > pointTolerance))
      continue;
    const auto segmentKey = [&](const Point & first, const Point & second)
    {
      std::vector<SignedInteger> firstKey(inputDimension);
      std::vector<SignedInteger> secondKey(inputDimension);
      for (UnsignedInteger i = 0; i < inputDimension; ++i)
      {
        firstKey[i] = static_cast<SignedInteger>(std::llround(first[i] / segmentDedupTolerance));
        secondKey[i] = static_cast<SignedInteger>(std::llround(second[i] / segmentDedupTolerance));
      }
      if (secondKey < firstKey)
        std::swap(firstKey, secondKey);
      std::vector<SignedInteger> key(firstKey);
      key.insert(key.end(), secondKey.begin(), secondKey.end());
      return key;
    };
    if (!countedSegments.insert(segmentKey(firstPoint, secondPoint)).second)
      continue;
    const Function integrand(new PushForwardDistributionFunctions::NonlinearCoAreaCurveIntegrand(firstPoint, direction, antecedent_, function_, singular));
    density += quadrature.integrate(integrand, Interval(0.0, 1.0))[0];
    LevelSetContinuation::Segment continuationSegment;
    continuationSegment.first = firstPoint;
    continuationSegment.second = secondPoint;
    continuationSegments.add(continuationSegment);
  }
  if (singular->load())
    return SpecFunc::Infinity;
  if (ResourceMap::GetAsBool("PushForwardDistribution-NonlinearCoAreaContinuation")
      && !continuationSegments.isEmpty())
  {
    const Point width(searchInterval_.getUpperBound() - searchInterval_.getLowerBound());
    Scalar meshStep = 0.0;
    for (UnsignedInteger i = 0; i < width.getSize(); ++i)
      meshStep = std::max(meshStep, width[i] / discretization);
    const LevelSetContinuation continuation(function_, antecedent_.getPDF(), searchInterval_, point);
    const LevelSetContinuation::Result result = continuation.computeDensity(
          continuationSegments, meshStep, 1.0e-4 * meshStep);
    if (result.status == LevelSetContinuation::SINGULAR)
      return SpecFunc::Infinity;
    if ((result.status == LevelSetContinuation::SUCCESS)
        && ((result.density > 0.0) || (density == 0.0)))
      return result.density;
  }
  return density;
}

/* Compute the PDF in the scalar nonlinear co-area case f: R^2 -> R. The
   triangulation of the search box supplies the topology of the level set: a
   triangle is crossed when its vertices are not all on the same side of the
   requested level. The intersection of the level with each crossed mesh edge
   is found with a scalar Brent solve on the actual function, so that the
   segment ends lie on the true level set. The co-area factor is the norm of
   the gradient of f at the segment midpoint, and the antecedent density is
   integrated along the segment. The path is deterministic, handles several
   connected components and closed curves, and keeps the affine co-area path
   dedicated to the exact affine case. */
Scalar PushForwardDistribution::computePDFByNonlinearCoArea(const Point & point) const
{
  const UnsignedInteger inputDimension = function_.getInputDimension();
  const UnsignedInteger outputDimension = function_.getOutputDimension();
  if ((outputDimension >= 2) && (outputDimension + 1 == inputDimension))
    return computePDFByNonlinearCoAreaCurve(point);
  if (outputDimension != 1)
    throw NotYetImplementedException(HERE) << "Error: the nonlinear co-area density is only implemented for a scalar function on R^2 or R^3, or for a map from R^n to R^(n-1) with n >= 3, here input dimension=" << inputDimension << " and output dimension=" << outputDimension;
  if (inputDimension == 3)
    return computePDFByNonlinearCoAreaSurface(point);
  if (inputDimension != 2)
    throw NotYetImplementedException(HERE) << "Error: the nonlinear co-area density is only implemented for a scalar function on R^2 or R^3, here input dimension=" << inputDimension << " and output dimension=" << outputDimension;
  const Scalar level = point[0];
  const UnsignedInteger discretization = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-NonlinearCoAreaDiscretization");
  const Scalar levelTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-NonlinearCoAreaLevelTolerance");
  const Bool continuationEnabled = ResourceMap::GetAsBool("PushForwardDistribution-NonlinearCoAreaContinuation");
  // The background mesh only has to provide one seed per connected component
  // of the fibre: the continuation then traces the actual level set, so its
  // accuracy is governed by the requested integration resolution rather than
  // by the seeding mesh. Build a coarser mesh first to seed the continuation,
  // and keep the requested resolution for the mesh fallback used when the
  // continuation fails.
  const UnsignedInteger seedDiscretization = continuationEnabled ? std::max<UnsignedInteger>(1, discretization / 4) : discretization;
  const Point width(searchInterval_.getUpperBound() - searchInterval_.getLowerBound());
  Scalar seedMeshStep = 0.0;
  Scalar fineMeshStep = 0.0;
  for (UnsignedInteger i = 0; i < width.getSize(); ++i)
  {
    seedMeshStep = std::max(seedMeshStep, width[i] / seedDiscretization);
    fineMeshStep = std::max(fineMeshStep, width[i] / discretization);
  }
  const Function antecedentPDF(antecedent_.getPDF());
  const GaussLegendre quadrature(1);
  const Brent edgeSolver;
  for (UnsignedInteger pass = 0; ; ++pass)
  {
    const Bool useFallback = (pass == 0) && (seedDiscretization < discretization);
    const UnsignedInteger meshDiscretization = (pass == 0) ? seedDiscretization : discretization;
    const Scalar meshStep = (pass == 0) ? seedMeshStep : fineMeshStep;
    // A uniform triangulation of the search box is enough for a first
    // deterministic implementation. The level crossings are polished on the
    // actual function, so the geometric error is quadratic in the mesh size,
    // and the resource key lets the caller trade cost for accuracy without
    // changing the algorithm.
    const Mesh mesh(IntervalMesher(Indices(2, meshDiscretization)).build(searchInterval_));
    const Sample vertexValues(function_(mesh.getVertices()));
    Scalar density = 0.0;
    Collection<LevelSetContinuation::Segment> continuationSegments;
    // When the level set coincides exactly with a mesh edge, the two adjacent
    // triangles see the same edge. Count it once; this only happens on a set of
    // level values of measure zero, but it keeps the result stable for round
    // levels such as the mean of a symmetric distribution.
    std::set<std::pair<UnsignedInteger, UnsignedInteger>> countedEdges;
    for (UnsignedInteger simplexIndex = 0; simplexIndex < mesh.getSimplicesNumber(); ++simplexIndex)
    {
      const Indices simplex(mesh.getSimplex(simplexIndex));
      if (simplex.getSize() != 3)
        continue;
      Point x[3];
      Scalar value[3];
      SignedInteger sign[3];
      for (UnsignedInteger vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
      {
        x[vertexIndex] = mesh.getVertex(simplex[vertexIndex]);
        value[vertexIndex] = vertexValues(simplex[vertexIndex], 0);
        // The tolerance is relative to the level and to the local values, so
        // that a vertex is classified as lying on the level set consistently
        // with the floating point representation of f.
        const Scalar scale = std::max(1.0, std::max(std::abs(level), std::abs(value[vertexIndex])));
        const Scalar tolerance = levelTolerance * scale;
        sign[vertexIndex] = (value[vertexIndex] < level - tolerance) ? -1 : ((value[vertexIndex] > level + tolerance) ? 1 : 0);
      }
      const UnsignedInteger zeroCount = (sign[0] == 0) + (sign[1] == 0) + (sign[2] == 0);
      // A whole triangle at the requested level is a two dimensional part of
      // the fibre: the one dimensional co-area density is singular there.
      if (zeroCount == 3)
        return SpecFunc::Infinity;
      Point firstPoint(2);
      Point secondPoint(2);
      UnsignedInteger pointCount = 0;
      const auto addPoint = [&](const Point & pointToAdd)
      {
        if (pointCount == 0)
          firstPoint = pointToAdd;
        else if (pointCount == 1)
          secondPoint = pointToAdd;
        ++pointCount;
      };
      const auto addCrossing = [&](const UnsignedInteger firstVertex, const UnsignedInteger secondVertex)
      {
        // The two vertices are on opposite sides of the level. Solve
        // f(x) = level on the edge instead of using the P1 interpolation only:
        // the root is bracketed, and this keeps the segment ends on the actual
        // level set even when the function is strongly curved.
        const Point edgeDirection(x[secondVertex] - x[firstVertex]);
        const LinearFunction edge(Point(1, 0.0), x[firstVertex], Matrix(2, 1, edgeDirection));
        const Function edgeFunction(ComposedFunction(function_, edge));
        Scalar parameter = (level - value[firstVertex]) / (value[secondVertex] - value[firstVertex]);
        try
        {
          parameter = edgeSolver.solve(edgeFunction, level, 0.0, 1.0, value[firstVertex], value[secondVertex]);
        }
        catch (const Exception &)
        {
          // Keep the linear estimate when the scalar solver cannot improve it.
        }
        // Brent may return a value marginally outside the bracket because of
        // its termination criterion; fall back to the linear estimate in this
        // unlikely case.
        if (!(parameter >= 0.0) || !(parameter <= 1.0))
          parameter = (level - value[firstVertex]) / (value[secondVertex] - value[firstVertex]);
        addPoint(x[firstVertex] + parameter * edgeDirection);
      };
      if (zeroCount == 2)
      {
        UnsignedInteger firstZero = 3;
        UnsignedInteger secondZero = 3;
        for (UnsignedInteger vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
          if (sign[vertexIndex] == 0)
          {
            if (firstZero == 3)
              firstZero = vertexIndex;
            else
              secondZero = vertexIndex;
          }
        const UnsignedInteger firstGlobal = simplex[firstZero];
        const UnsignedInteger secondGlobal = simplex[secondZero];
        const std::pair<UnsignedInteger, UnsignedInteger> edge(std::min(firstGlobal, secondGlobal), std::max(firstGlobal, secondGlobal));
        if (countedEdges.insert(edge).second)
        {
          addPoint(x[firstZero]);
          addPoint(x[secondZero]);
        }
      }
      else if (zeroCount == 1)
      {
        UnsignedInteger zeroVertex = 3;
        for (UnsignedInteger vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
          if (sign[vertexIndex] == 0)
            zeroVertex = vertexIndex;
        const UnsignedInteger firstOther = (zeroVertex + 1) % 3;
        const UnsignedInteger secondOther = (zeroVertex + 2) % 3;
        // If the two other vertices are on the same side, the level set only
        // touches the triangle at the vertex: its length is zero.
        if (sign[firstOther] * sign[secondOther] < 0)
        {
          addPoint(x[zeroVertex]);
          addCrossing(firstOther, secondOther);
        }
      }
      else
      {
        // No vertex is on the level: either the level crosses two edges or it
        // does not enter the triangle.
        if (sign[0] * sign[1] < 0)
          addCrossing(0, 1);
        if (sign[1] * sign[2] < 0)
          addCrossing(1, 2);
        if (sign[2] * sign[0] < 0)
          addCrossing(2, 0);
      }
      if (pointCount != 2)
        continue;
      const Point direction(secondPoint - firstPoint);
      const Scalar length = direction.norm();
      if (!(length > 0.0))
        continue;
      // Co-area factor: norm of the gradient of the actual function at the
      // midpoint of the segment. When the function does not provide a gradient,
      // fall back to the constant gradient of its P1 interpolation on the
      // triangle, which solves grad f . (x_i - x_0) = f_i - f_0.
      Point midpoint(2);
      for (UnsignedInteger coordinate = 0; coordinate < 2; ++coordinate)
        midpoint[coordinate] = 0.5 * (firstPoint[coordinate] + secondPoint[coordinate]);
      Scalar gradientNorm = 0.0;
      try
      {
        const Matrix gradientMatrix(function_.gradient(midpoint));
        if ((gradientMatrix.getNbRows() == 2) && (gradientMatrix.getNbColumns() == 1))
          gradientNorm = std::sqrt(gradientMatrix(0, 0) * gradientMatrix(0, 0) + gradientMatrix(1, 0) * gradientMatrix(1, 0));
      }
      catch (const Exception &)
      {
        gradientNorm = 0.0;
      }
      if (!(gradientNorm > 0.0))
      {
        const Point firstEdge(x[1] - x[0]);
        const Point secondEdge(x[2] - x[0]);
        const Scalar determinant = firstEdge[0] * secondEdge[1] - firstEdge[1] * secondEdge[0];
        const Scalar determinantTolerance = SpecFunc::ScalarEpsilon * firstEdge.norm() * secondEdge.norm();
        if (std::abs(determinant) <= determinantTolerance)
          continue;
        const Scalar gradientX = ((value[1] - value[0]) * secondEdge[1] - (value[2] - value[0]) * firstEdge[1]) / determinant;
        const Scalar gradientY = ((value[2] - value[0]) * firstEdge[0] - (value[1] - value[0]) * secondEdge[0]) / determinant;
        gradientNorm = std::sqrt(gradientX * gradientX + gradientY * gradientY);
      }
      if (!(gradientNorm > 0.0))
        continue;
      // Parameterisation of the segment: t in [0, 1] maps to firstPoint +
      // t * direction. The antecedent density is integrated along the segment
      // with a fixed Gauss-Legendre rule; the rule is accurate for the smooth
      // densities considered here, while the mesh controls the geometric error.
      const LinearFunction segment(Point(1, 0.0), firstPoint, Matrix(2, 1, direction));
      const Function integrand(ComposedFunction(antecedentPDF, segment));
      const Scalar segmentIntegral = quadrature.integrate(integrand, Interval(0.0, 1.0))[0];
      density += segmentIntegral * length / gradientNorm;
      LevelSetContinuation::Segment continuationSegment;
      continuationSegment.first = firstPoint;
      continuationSegment.second = secondPoint;
      continuationSegments.add(continuationSegment);
    }
    if (continuationEnabled && !continuationSegments.isEmpty())
    {
      const LevelSetContinuation continuation(function_, antecedentPDF, searchInterval_, point);
      const LevelSetContinuation::Result result = continuation.computeDensity(
            continuationSegments, fineMeshStep, 1.0e-4 * meshStep);
      if (result.status == LevelSetContinuation::SINGULAR)
        return SpecFunc::Infinity;
      if ((result.status == LevelSetContinuation::SUCCESS)
          && ((result.density > 0.0) || (!useFallback && (density == 0.0))))
        return result.density;
    }
    // The coarse seeding failed: rebuild the mesh at the requested resolution
    // and use the same continuation and mesh fallback as before.
    if (useFallback)
      continue;
    return density;
  }
}

/* Solve r(u) = 0 by damped Newton with step control from the given starting point */
Point PushForwardDistribution::dampedNewton(const Function & residualFunction,
    const Point & startingPoint) const
{
  const Scalar residualTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-SolverResidualTolerance");
  const UnsignedInteger maximumIterations = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-NewtonMaximumIterations");
  const Scalar stepReduction = ResourceMap::GetAsScalar("PushForwardDistribution-NewtonStepReduction");
  const UnsignedInteger dimension = residualFunction.getInputDimension();
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
  const UnsignedInteger maximumBisections = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-MaximumBisections");
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
          for (UnsignedInteger bisection = 0; bisection < maximumBisections; ++bisection)
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
  // point is a continuum and its contribution is integrated along it. An
  // affine function uses the dedicated exact SVD path. A scalar nonlinear
  // map on R^2 or R^3 has a one or two dimensional fibre, extracted from a
  // triangulation or tetrahedralisation of the search box. A map from R^n to
  // R^(n-1) with n >= 3 has a one dimensional fibre, the intersection of its
  // n - 1 level hypersurfaces. Other nonlinear fibres require a general
  // implicit-manifold integration or equality-intersection meshing and are
  // outside this dispatch.
  if (outputDimension < inputDimension)
  {
    if (!function_.isLinear())
    {
      if (((outputDimension == 1) && ((inputDimension == 2) || (inputDimension == 3)))
          || ((outputDimension >= 2) && (outputDimension + 1 == inputDimension)))
        return computePDFByNonlinearCoArea(point);
      throw NotYetImplementedException(HERE) << "Error: the PDF in the co-area case (output dimension < input dimension) is only implemented for an affine function, for a nonlinear scalar function on R^2 or R^3, or for a nonlinear map from R^n to R^(n-1) with n >= 3; this nonlinear co-area map can be sampled and its moments computed, but its density requires an implicit fibre integration";
    }
    return computePDFByCoArea(point);
  }
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
  // An affine function has an exact image mean. A nonlinear function is
  // integrated deterministically over the finite search box built from the
  // support of the antecedent or from its extreme quantiles.  The cost of the
  // iterated quadrature grows exponentially with the input dimension, so fall
  // back to a Monte-Carlo estimate above the configured dimension.
  if (function_.isLinear())
    mean_ = function_(antecedent_.getMean());
  else if (function_.getInputDimension() <= ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-MomentsMaximumDimension"))
    mean_ = IteratedQuadrature().integrate(Function(antecedent_.getPDF() * function_), searchInterval_);
  else
  {
    const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-SampleSize");
    const Sample sample(function_(antecedent_.getSample(sampleSize)));
    mean_ = sample.computeMean();
  }
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void PushForwardDistribution::computeCovariance() const
{
  const UnsignedInteger outputDimension = function_.getOutputDimension();
  // The centered moments need the mean, which is exact for an affine
  // function, deterministic in low dimension and estimated otherwise
  (void) getMean();
  if (function_.isLinear())
  {
    // For f(x) = A x + c, Cov(Y) = A Cov(X) A^T. Function::gradient returns
    // the transposed Jacobian, hence the transpose below
    const Matrix gradient(function_.gradient(antecedent_.getMean()));
    if ((gradient.getNbRows() != function_.getInputDimension()) || (gradient.getNbColumns() != outputDimension))
      throw InternalException(HERE) << "Error: the given function has no actual gradient. Consider using finite differences.";
    const Matrix covarianceMatrix(gradient.transpose() * antecedent_.getCovariance() * gradient);
    covariance_ = CovarianceMatrix(outputDimension);
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      for (UnsignedInteger i = 0; i <= j; ++i)
        covariance_(i, j) = covarianceMatrix(i, j);
  }
  else if (function_.getInputDimension() <= ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-MomentsMaximumDimension"))
  {
    // Integrate the centered second order products against the antecedent
    // density over the finite search box, deterministically
    const Description imageVariables(Description::BuildDefault(outputDimension, "y"));
    Description formulas((outputDimension * (outputDimension + 1)) / 2);
    Description meanAsStrings(outputDimension);
    UnsignedInteger index = 0;
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
    {
      meanAsStrings[j] = (OSS() << std::setprecision(17) << mean_[j]);
      for (UnsignedInteger i = 0; i <= j; ++i)
      {
        formulas[index] = "(" + imageVariables[i] + "-(" + meanAsStrings[i] + "))*(" + imageVariables[j] + "-(" + meanAsStrings[j] + "))";
        ++index;
      }
    }
    const Function momentsFunction(ComposedFunction(SymbolicFunction(imageVariables, formulas), function_));
    const Function integrand(antecedent_.getPDF() * momentsFunction);
    const Point covarianceAsPoint(IteratedQuadrature().integrate(integrand, searchInterval_));
    covariance_ = CovarianceMatrix(outputDimension);
    index = 0;
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
    {
      for (UnsignedInteger i = 0; i <= j; ++i)
      {
        covariance_(i, j) = covarianceAsPoint[index];
        ++index;
      }
    }
  }
  else
  {
    const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-SampleSize");
    const Sample sample(function_(antecedent_.getSample(sampleSize)));
    covariance_ = sample.computeCovariance();
  }
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
