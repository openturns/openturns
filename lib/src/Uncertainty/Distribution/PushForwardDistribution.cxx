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
#include "openturns/PushForwardDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ConstantFunction.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/IdentityFunction.hxx"
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
 * Integrand of the co-area formula: the density of the antecedent at the
 * point u(t) = origin + t * direction of the fibre f(u) = point, divided by
 * the p-dimensional Jacobian. The direction is a unit vector spanning the
 * kernel of the Jacobian matrix, so that the Hausdorff element along the
 * fibre is exactly dt and no metric factor is needed in the integrand.
 */
class CoAreaIntegrand: public EvaluationImplementation
{
public:
  CoAreaIntegrand(const Point & origin,
                  const Point & direction,
                  const Distribution & antecedent,
                  const Function & function,
                  const Point & point,
                  const Scalar jacobian,
                  const Scalar residualTolerance)
    : EvaluationImplementation()
    , origin_(origin)
    , direction_(direction)
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
    const Scalar t = parameter[0];
    const Point x(origin_ + t * direction_);
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
    return 1;
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
        << " direction=" << direction_.__repr__()
        << " point=" << point_.__repr__();
    return oss;
  }

  String __str__(const String & ) const override
  {
    OSS oss(false);
    oss << "CoAreaIntegrand(origin=" << origin_
        << ", direction=" << direction_
        << ", point=" << point_
        << ", jacobian=" << jacobian_ << ")";
    return oss;
  }

private:
  Point origin_;
  Point direction_;
  Distribution antecedent_;
  Function function_;
  Point point_;
  Scalar jacobian_;
  Scalar residualTolerance_;
}; // CoAreaIntegrand

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
  // computePDFByCoArea(), which is restricted to a one dimensional fibre and
  // to an affine function. The restriction is not enforced here: sampling, the
  // mean and the range stay available for a function of any codimension
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
  if (fibreDimension != 1)
    throw NotYetImplementedException(HERE) << "Error: the co-area formula is only implemented for a one dimensional fibre, here input dimension=" << inputDimension << " and output dimension=" << outputDimension;
  // The function is assumed affine, hence its Jacobian matrix is constant and
  // is evaluated once, at the mean of the antecedent. The assumption is
  // checked numerically in the integrand, at each quadrature node
  const Point mean(antecedent_.getMean());
  const Matrix gradientMatrix(function_.gradient(mean));
  if ((gradientMatrix.getNbRows() != inputDimension) || (gradientMatrix.getNbColumns() != outputDimension))
    throw InternalException(HERE) << "Error: the given function has no actual gradient. Consider using finite differences.";
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
  const Point target(point - function_(mean));
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
  // Orthonormal basis of the kernel of J, of dimension n - p = 1: it is the
  // last column of the full left factor of the SVD. Being a unit vector, the
  // Hausdorff element along the fibre parameterised as origin + t * direction
  // is exactly dt
  Point direction(inputDimension);
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
    direction[i] = leftFactor(i, inputDimension - 1);
  const Point origin(mean + delta);
  // The fibre is integrated over its intersection with the search box in the
  // support of the antecedent. Each coordinate of the box constrains t
  // independently, so their intersection is exactly the set of admissible t
  const Interval searchInterval(searchInterval_);
  const Point lowerBound(searchInterval.getLowerBound());
  const Point upperBound(searchInterval.getUpperBound());
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
  const Scalar residualTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-CoAreaResidualTolerance");
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
  const PushForwardDistributionFunctions::CoAreaIntegrand integrand(origin, direction, antecedent_, function_, point, jacobian, residualTolerance);
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
