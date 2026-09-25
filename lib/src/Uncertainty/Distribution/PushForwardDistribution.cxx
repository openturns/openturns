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
#include "openturns/PushForwardDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ConstantFunction.hxx"
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
  if (outputDimension < inputDimension)
    throw NotYetImplementedException(HERE) << "Error: the case where the output dimension of the function is lower than the input dimension (co-area formula) is not yet implemented, here input dimension=" << inputDimension << " and output dimension=" << outputDimension;
  function_ = function;
  antecedent_ = antecedent;
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
  // When a bound is not finite, clip it using such a quantile of the
  // antecedent that the probability mass outside the box is negligible
  const Scalar quantileEpsilon = ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
  const Scalar upperQuantile = 1.0 - quantileEpsilon;
  Point lowerBound(dimension);
  Point upperBound(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (finiteLowerBound[i]) lowerBound[i] = antecedentRange.getLowerBound()[i];
    else lowerBound[i] = antecedent_.computeQuantile(quantileEpsilon)[i];
    if (finiteUpperBound[i]) upperBound[i] = antecedentRange.getUpperBound()[i];
    else upperBound[i] = antecedent_.computeQuantile(upperQuantile)[i];
    if (!(lowerBound[i] < upperBound[i]))
    {
      // Degenerate search interval: fall back to a unit interval around the mean
      const Scalar mean_i = antecedent_.getMean()[i];
      lowerBound[i] = mean_i - 1.0;
      upperBound[i] = mean_i + 1.0;
    }
  }
  return Interval(lowerBound, upperBound, Interval::BoolCollection(dimension, true), Interval::BoolCollection(dimension, true));
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
    catch (...)
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
        catch (...)
        {
          break;
        }
        Point correction;
        try
        {
          const SquareMatrix jacobian(*jacobianMatrix.getImplementation());
          correction = jacobian.transpose().solveLinearSystem(residualFunction(currentPoint));
        }
        catch (...)
        {
          break;
        }
        const Point polishedPoint(Point(currentPoint - correction));
        Scalar polishedResidualNorm = residualNorm;
        try
        {
          polishedResidualNorm = residualFunction(polishedPoint).norm();
        }
        catch (...)
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
    catch (...)
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
    catch (...)
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
      catch (...)
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
  const Interval searchInterval(buildSearchInterval());
  const Point lowerBound(searchInterval.getLowerBound());
  const Point upperBound(searchInterval.getUpperBound());
  const Scalar residualTolerance = ResourceMap::GetAsScalar("PushForwardDistribution-SolverResidualTolerance");
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-PreimageSearchSampleSize");
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << "Error: the resource map key PushForwardDistribution-PreimageSearchSampleSize must be nonzero.";
  // Tolerance below which two preimages are considered to be the same one
  const Scalar dedupTolerance = std::sqrt(SpecFunc::ScalarEpsilon) * std::max(1.0, std::max(lowerBound.norm(), upperBound.norm()));
  auto isValidRoot = [&](const Point & candidate)
  {
    try
    {
      return residualFunction(candidate).norm() <= residualTolerance;
    }
    catch (...)
    {
      return false;
    }
  };
  auto addRoot = [&](const Point & candidate)
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
  auto addRefinedRoot = [&](const Point & startingPoint)
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
      catch (...)
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
            catch (...)
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
  // The preimages of the point are the roots of the residual function
  // r(u) = f(u) - point
  const Function residualFunction(function_ - ConstantFunction(inputDimension, point));
  const Interval antecedentRange(antecedent_.getRange());
  // Search for the preimages of the point:
  // - in the square case (p == n), a damped Newton solver with step control
  //   finds all the preimages, being comprehensive in dimension one through
  //   a decomposition of the search interval and multi-start in higher
  //   dimensions, hence supporting non-injective functions;
  // - in the intrinsic case (p > n), only the preimage closest to the mean
  //   of the antecedent is searched for, through the least squares solver
  PointCollection preimages(0);
  if (outputDimension == inputDimension)
  {
    preimages = findPreimages(residualFunction);
  }
  else
  {
    const Interval::BoolCollection finiteLowerBound(antecedentRange.getFiniteLowerBound());
    const Interval::BoolCollection finiteUpperBound(antecedentRange.getFiniteUpperBound());
    Bool hasFiniteBounds = true;
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      hasFiniteBounds = hasFiniteBounds && finiteLowerBound[i] && finiteUpperBound[i];
    Point preimage(0);
    try
    {
      if (hasFiniteBounds)
        preimage = solver_.solve(residualFunction, antecedent_.getMean(), antecedentRange);
      else
        preimage = solver_.solve(residualFunction, antecedent_.getMean());
    }
    catch (const InternalException &)
    {
      // No preimage found within the tolerance: the point is not in the support
      return 0.0;
    }
    // Check that the solved point is an actual preimage
    const Scalar residualNorm = (function_(preimage) - point).norm();
    if (residualNorm > ResourceMap::GetAsScalar("PushForwardDistribution-SolverResidualTolerance"))
      return 0.0;
    if (!antecedentRange.contains(preimage))
      return 0.0;
    preimages.add(preimage);
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
    Scalar determinant = 0.0;
    if (outputDimension == inputDimension)
    {
      const SquareMatrix jacobianSquareMatrix(*gradientMatrix.getImplementation());
      determinant = jacobianSquareMatrix.computeDeterminant();
    }
    else
    {
      // Here J^T J = gradient * gradient^T
      SquareMatrix gramMatrix(inputDimension);
      for (UnsignedInteger i = 0; i < inputDimension; ++i)
        for (UnsignedInteger j = 0; j < inputDimension; ++j)
        {
          Scalar value = 0.0;
          for (UnsignedInteger kk = 0; kk < outputDimension; ++kk)
            value += gradientMatrix(i, kk) * gradientMatrix(j, kk);
          gramMatrix(i, j) = value;
        }
      determinant = gramMatrix.computeDeterminant();
    }
    const Scalar denominator = (outputDimension == inputDimension) ? std::abs(determinant) : std::sqrt(determinant);
    if (!std::isfinite(denominator) || !(denominator > 0.0))
      // Critical value of the function where the Jacobian is singular: the
      // density is singular, the contribution is not evaluated
      continue;
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