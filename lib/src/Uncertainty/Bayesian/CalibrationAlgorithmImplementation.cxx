//                                               -*- C++ -*-
/**
 *  @brief Default CalibrationAlgorithmImplementation
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
#include "openturns/CalibrationAlgorithmImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Normal.hxx"
#include "openturns/TruncatedDistribution.hxx"
#include "openturns/QuadraticProblem.hxx"
#include "openturns/SymmetricMatrix.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/OptimizationResult.hxx"
#include "openturns/Cobyla.hxx"

#include <algorithm>
#include <cmath>
#include <vector>

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(CalibrationAlgorithmImplementation)

static const Factory<CalibrationAlgorithmImplementation> Factory_CalibrationAlgorithmImplementation;

/* Default constructor */
CalibrationAlgorithmImplementation::CalibrationAlgorithmImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Parameter constructor */
CalibrationAlgorithmImplementation::CalibrationAlgorithmImplementation(const Function & model,
    const Sample & inputObservations,
    const Sample & outputObservations,
    const Distribution & parameterPrior)
  : PersistentObject()
  , model_(model)
  , inputObservations_(inputObservations)
  , outputObservations_(outputObservations)
  , parameterPrior_(parameterPrior)
  , result_()
{
  // Allow for a dim-0 sample but it should have the right size internally
  if (!inputObservations_.getDimension())
    inputObservations_ = Sample(outputObservations.getSize(), 0);
}

/* Performs the actual computation. Must be overloaded by the actual calibration algorithm */
void CalibrationAlgorithmImplementation::run()
{
  throw NotYetImplementedException(HERE) << "In CalibrationAlgorithmImplementation::run()";
}

/* Indices of the parameters that are not fixed at a bound: all the
 * directions when bounds is empty, the free ones otherwise. */
Indices CalibrationAlgorithmImplementation::ComputeFreeParameterIndices(const Point & thetaStar,
    const Interval & bounds)
{
  const UnsignedInteger parameterDimension = thetaStar.getDimension();
  if (bounds.getDimension() == 0)
  {
    Indices freeIndices(parameterDimension);
    freeIndices.iota();
    return freeIndices;
  }
  if (bounds.getDimension() != parameterDimension)
    throw InvalidArgumentException(HERE) << "Error: expected bounds of dimension=" << parameterDimension << ", got dimension=" << bounds.getDimension();
  // Absolute tolerance used to detect a parameter lying on one of its bounds.
  // It is deliberately larger than the default primal feasibility tolerance of
  // the quadratic solver (1e-7) so that a parameter reported slightly outside
  // its bound is still detected. It is not scaled by the bound magnitude:
  // scaling by a large bound would classify a parameter lying well inside the
  // feasible interval as fixed.
  const Scalar tolerance = ResourceMap::GetAsScalar("LinearLeastSquaresCalibration-ActiveBoundTolerance");
  if (!(tolerance > 0.0) || !std::isfinite(tolerance))
    throw InvalidArgumentException(HERE) << "Error: the active bound tolerance must be finite and strictly positive, got " << tolerance;
  const Point lowerBound(bounds.getLowerBound());
  const Point upperBound(bounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(bounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(bounds.getFiniteUpperBound());
  Indices freeIndices(0);
  for (UnsignedInteger i = 0; i < parameterDimension; ++i)
  {
    Bool active = false;
    if (finiteLowerBound[i] && (std::abs(thetaStar[i] - lowerBound[i]) <= tolerance))
      active = true;
    if (finiteUpperBound[i] && (std::abs(thetaStar[i] - upperBound[i]) <= tolerance))
      active = true;
    if (!active)
      freeIndices.add(i);
  }
  return freeIndices;
}

/* Inverse of the Gram matrix on the free directions of a design matrix,
 * padded with zeros in the directions fixed at a bound. */
CovarianceMatrix CalibrationAlgorithmImplementation::ComputeFreeGramInverse(const Matrix & design,
    const Indices & freeIndices,
    const String & methodName,
    const Bool checkRank)
{
  const UnsignedInteger parameterDimension = design.getNbColumns();
  const UnsignedInteger freeDimension = freeIndices.getSize();
  // The covariance is zero in the directions fixed at a bound: start from a
  // zero matrix instead of the identity matrix used by CovarianceMatrix(dim).
  CovarianceMatrix gramInverse(parameterDimension, Point(parameterDimension));
  if (freeDimension == 0)
    return gramInverse;
  Matrix freeDesign(design.getNbRows(), freeDimension);
  for (UnsignedInteger j = 0; j < freeDimension; ++j)
    for (UnsignedInteger i = 0; i < design.getNbRows(); ++i)
      freeDesign(i, j) = design(i, freeIndices[j]);
  // When requested, a rank-deficient free block gives a small positive
  // singular value whose inverse can still be finite: check the numerical rank
  // before relying on the factorization. The unbounded non-linear path keeps
  // the unconstrained behaviour and lets the caller detect a non-finite
  // covariance, so that regularized non-identifiable problems keep being
  // calibrated.
  if (checkRank)
  {
    const Point singularValues(freeDesign.computeSingularValues());
    const UnsignedInteger singularValuesSize = singularValues.getSize();
    const Scalar largestSingularValue = (singularValuesSize > 0) ? singularValues[0] : 0.0;
    const Scalar rankTolerance = std::max(freeDesign.getNbRows(), freeDimension) * SpecFunc::Precision * largestSingularValue;
    if ((singularValuesSize < freeDimension) || !(largestSingularValue > 0.0) || (singularValues[singularValuesSize - 1] <= rankTolerance))
      throw InvalidArgumentException(HERE) << "Error: the calibration problem is not identifiable in the free parameter directions";
  }
  LeastSquaresMethod freeMethod(LeastSquaresMethod::Build(methodName, freeDesign));
  // Initialize the decomposition before accessing the Gram inverse
  freeMethod.update(Indices(), freeMethod.getCurrentIndices(), Indices(), false);
  const CovarianceMatrix freeGramInverse(freeMethod.getGramInverse());
  for (UnsignedInteger i = 0; i < freeDimension; ++i)
    for (UnsignedInteger j = 0; j < freeDimension; ++j)
    {
      const Scalar value = freeGramInverse(i, j);
      if (!std::isfinite(value))
        throw InvalidArgumentException(HERE) << "Error: the calibration problem is not identifiable in the free parameter directions";
      gramInverse(freeIndices[i], freeIndices[j]) = value;
    }
  return gramInverse;
}

/* Inverse of a Hessian matrix on the free directions, padded with zeros in
 * the directions fixed at a bound. */
CovarianceMatrix CalibrationAlgorithmImplementation::ComputeFreeHessianInverse(const Matrix & hessian,
    const Indices & freeIndices)
{
  const UnsignedInteger parameterDimension = hessian.getNbRows();
  const UnsignedInteger freeDimension = freeIndices.getSize();
  CovarianceMatrix hessianInverse(parameterDimension, Point(parameterDimension));
  if (freeDimension == 0)
    return hessianInverse;
  Matrix freeHessian(freeDimension, freeDimension);
  for (UnsignedInteger i = 0; i < freeDimension; ++i)
    for (UnsignedInteger j = 0; j < freeDimension; ++j)
      freeHessian(i, j) = hessian(freeIndices[i], freeIndices[j]);
  // solveLinearSystem() is rank revealing: a singular or indefinite free
  // Hessian would give a finite least-squares pseudo-inverse, i.e. a spurious
  // tiny variance in an unidentified direction, without any error. Check that
  // the free Hessian is positive definite before inverting it.
  SymmetricMatrix symmetricHessian(freeDimension);
  for (UnsignedInteger i = 0; i < freeDimension; ++i)
    for (UnsignedInteger j = 0; j < freeDimension; ++j)
      symmetricHessian(i, j) = freeHessian(i, j);
  const Point eigenValues(symmetricHessian.computeEigenValues());
  const Scalar smallestEigenValue = *std::min_element(eigenValues.begin(), eigenValues.end());
  const Scalar largestEigenValue = *std::max_element(eigenValues.begin(), eigenValues.end());
  const Scalar eigenTolerance = freeDimension * SpecFunc::Precision * largestEigenValue;
  if (!(largestEigenValue > 0.0) || !(smallestEigenValue > eigenTolerance))
    throw InvalidArgumentException(HERE) << "Error: cannot compute the Laplace approximation: the Hessian of the residuals at the optimum is not positive definite in the free parameter directions.";
  try
  {
    const Matrix freeInverse(freeHessian.solveLinearSystem(IdentityMatrix(freeDimension)));
    const CovarianceMatrix freeCovariance(*freeInverse.getImplementation());
    for (UnsignedInteger i = 0; i < freeDimension; ++i)
      for (UnsignedInteger j = 0; j < freeDimension; ++j)
      {
        const Scalar value = freeCovariance(i, j);
        if (!std::isfinite(value))
          throw InvalidArgumentException(HERE) << "Error: the calibration problem is not identifiable in the free parameter directions";
        hessianInverse(freeIndices[i], freeIndices[j]) = value;
      }
  }
  catch (const InvalidArgumentException &)
  {
    throw;
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot compute the Laplace approximation: the Hessian of the residuals at the optimum is not positive definite in the free parameter directions.";
  }
  return hessianInverse;
}

/* Minimize ||design.increment - rightHandSide||^2 subject to the increment
 * lying in the parameter bounds shifted by -startingPoint, and return the
 * optimal increment. The problem is solved in the increment variable so that
 * a large starting point does not lose the right-hand side through
 * cancellation. The problem is solved with the first algorithm returned by
 * OptimizationAlgorithm::Build(), i.e. the HiGHS quadratic solver when
 * OpenTURNS is built with HiGHS support. When the design is so ill-conditioned
 * that forming its Gram matrix would lose an identified direction, the problem
 * is solved directly on the design with an active-set method instead. */
Point CalibrationAlgorithmImplementation::SolveBoundedLeastSquaresIncrement(const Matrix & design,
    const Point & rightHandSide,
    const Interval & bounds,
    const Point & startingPoint)
{
  const UnsignedInteger parameterDimension = startingPoint.getDimension();
  if (design.getNbColumns() != parameterDimension)
    throw InvalidArgumentException(HERE) << "Error: expected a design matrix with " << parameterDimension << " columns, got " << design.getNbColumns();
  if (rightHandSide.getDimension() != design.getNbRows())
    throw InvalidArgumentException(HERE) << "Error: expected a right-hand side of dimension=" << design.getNbRows() << ", got dimension=" << rightHandSide.getDimension();
  if (bounds.getDimension() != parameterDimension)
    throw InvalidArgumentException(HERE) << "Error: expected bounds of dimension=" << parameterDimension << ", got dimension=" << bounds.getDimension();
  const Point lowerBound(bounds.getLowerBound());
  const Point upperBound(bounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(bounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(bounds.getFiniteUpperBound());
  Point shiftedLowerBound(lowerBound);
  Point shiftedUpperBound(upperBound);
  for (UnsignedInteger i = 0; i < parameterDimension; ++i)
  {
    if (finiteLowerBound[i])
      shiftedLowerBound[i] -= startingPoint[i];
    if (finiteUpperBound[i])
      shiftedUpperBound[i] -= startingPoint[i];
  }
  const Interval shiftedBounds(shiftedLowerBound, shiftedUpperBound, finiteLowerBound, finiteUpperBound);
  // Forming the normal equations squares the condition number of the design: a
  // direction identified by a singular value smaller than sqrt(epsilon) times
  // the largest one vanishes from the Hessian, and no solver can recover it
  // from the quadratic problem alone. Solve that case on the design itself,
  // with subproblems that keep every singular value.
  const Point singularValues(design.computeSingularValues());
  Scalar smallestSingularValue = SpecFunc::MaxScalar;
  Scalar largestSingularValue = 0.0;
  for (UnsignedInteger i = 0; i < singularValues.getSize(); ++i)
  {
    smallestSingularValue = std::min(smallestSingularValue, singularValues[i]);
    largestSingularValue = std::max(largestSingularValue, singularValues[i]);
  }
  if ((largestSingularValue > 0.0) && (smallestSingularValue < std::sqrt(SpecFunc::Precision) * largestSingularValue))
    return SolveBoundedLeastSquaresByActiveSet(design, rightHandSide, shiftedBounds);
  QuadraticProblem problem(QuadraticProblem::FromLeastSquares(design, rightHandSide, shiftedBounds));
  // The constant term ||rightHandSide||^2 does not change the optimal point,
  // but it can be huge compared with the variable part and swamp it in the
  // solver's comparisons: a constant of 1e18 hides a variation of order one.
  // Drop it so that the solver ranks the candidate points correctly.
  problem.setConstant(0.0);
  OptimizationAlgorithm solver(OptimizationAlgorithm::Build(problem));
  // Cobyla uses getMaximumAbsoluteError() as its final trust-region radius.
  // Its default value (1e-5) is coarser than the active-bound tolerance
  // (1e-6 by default), so an active bound could be reported slightly inside
  // the feasible interval and then be classified as free.
  if (solver.getImplementation()->getClassName() == "Cobyla")
  {
    const Scalar tolerance = ResourceMap::GetAsScalar("LinearLeastSquaresCalibration-ActiveBoundTolerance");
    if (!(tolerance > 0.0) || !std::isfinite(tolerance))
      throw InvalidArgumentException(HERE) << "Error: the active bound tolerance must be finite and strictly positive, got " << tolerance;
    const Scalar cobylaTolerance = std::max(std::sqrt(SpecFunc::Precision), 0.1 * tolerance);
    if (cobylaTolerance < solver.getMaximumAbsoluteError())
      solver.setMaximumAbsoluteError(cobylaTolerance);
    // Cobyla starts from an initial change of the variables that only
    // shrinks: scale it to the size of the feasible increment box, otherwise a
    // large increment exhausts the call budget long before reaching the
    // solution.
    Cobyla * cobyla = dynamic_cast<Cobyla *>(solver.getImplementation().get());
    if (cobyla != nullptr)
    {
      Scalar rhoBeg = cobyla->getRhoBeg();
      for (UnsignedInteger i = 0; i < parameterDimension; ++i)
        if (finiteLowerBound[i] && finiteUpperBound[i])
          rhoBeg = std::max(rhoBeg, 0.1 * (shiftedUpperBound[i] - shiftedLowerBound[i]));
      cobyla->setRhoBeg(rhoBeg);
    }
  }
  // Build only sets the problem: the selected algorithm has no starting point
  // of its own, and solvers such as Cobyla require one. The zero increment is
  // feasible when the starting point lies in the bounds; otherwise project it
  // onto the shifted bounds.
  Point solverStart(parameterDimension);
  for (UnsignedInteger i = 0; i < parameterDimension; ++i)
  {
    if (finiteLowerBound[i] && (solverStart[i] < shiftedLowerBound[i]))
      solverStart[i] = shiftedLowerBound[i];
    if (finiteUpperBound[i] && (solverStart[i] > shiftedUpperBound[i]))
      solverStart[i] = shiftedUpperBound[i];
  }
  solver.setStartingPoint(solverStart);
  solver.run();
  const OptimizationResult result(solver.getResult());
  if (result.getStatus() != OptimizationResult::SUCCESS)
    throw InternalException(HERE) << "Error: the bounded quadratic program solver failed: " << result.getStatusMessage();
  return result.getOptimalPoint();
}

/* Minimize ||design.increment - rightHandSide||^2 over the increment box,
 * directly on the design. The normal equations cannot be used here: squaring
 * the condition number of the design would erase its smallest identified
 * direction, which is precisely why this method is called. Bounded-variable
 * least squares (Lawson & Hanson, Solving Least Squares Problems, 1974): each
 * iteration solves the least-squares problem restricted to the free variables
 * with an SVD, moves toward that solution until a bound is reached, and
 * releases the variable whose sign condition is the most violated, until the
 * optimality conditions hold for every variable. */
Point CalibrationAlgorithmImplementation::SolveBoundedLeastSquaresByActiveSet(const Matrix & design,
    const Point & rightHandSide,
    const Interval & bounds)
{
  const UnsignedInteger size = design.getNbRows();
  const UnsignedInteger parameterDimension = design.getNbColumns();
  Point lowerBound(bounds.getLowerBound());
  Point upperBound(bounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(bounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(bounds.getFiniteUpperBound());
  // An Interval keeps its stored endpoint on an unbounded side: replace it by
  // the largest representable value so that every comparison below ignores
  // that side, as the quadratic problem does with the same flags.
  for (UnsignedInteger j = 0; j < parameterDimension; ++j)
  {
    if (!finiteLowerBound[j])
      lowerBound[j] = -SpecFunc::MaxScalar;
    if (!finiteUpperBound[j])
      upperBound[j] = SpecFunc::MaxScalar;
  }
  // A parameter whose bounds are equal is fixed: releasing it would only make
  // the free solve return the same value again, and the active set would cycle
  // until the iteration budget is exhausted.
  std::vector<Bool> isFixed(parameterDimension, false);
  for (UnsignedInteger j = 0; j < parameterDimension; ++j)
    isFixed[j] = finiteLowerBound[j] && finiteUpperBound[j] && (lowerBound[j] == upperBound[j]);
  // Start from the zero increment projected onto the box: the bounds are
  // shifted by the starting point, which may itself lie outside them.
  Point solution(parameterDimension);
  for (UnsignedInteger j = 0; j < parameterDimension; ++j)
    solution[j] = std::min(std::max(solution[j], lowerBound[j]), upperBound[j]);
  std::vector<Bool> isFree(parameterDimension, true);
  for (UnsignedInteger j = 0; j < parameterDimension; ++j)
    isFree[j] = !isFixed[j];
  // Optimality is tested on the gradient of the objective. Normalizing each
  // component by the norm of its design column makes the test independent of
  // the parameter units.
  Point columnNorm(parameterDimension);
  for (UnsignedInteger j = 0; j < parameterDimension; ++j)
  {
    Scalar squaredNorm = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
      squaredNorm += design(i, j) * design(i, j);
    columnNorm[j] = std::sqrt(squaredNorm);
  }
  const Scalar optimalityTolerance = std::sqrt(SpecFunc::Precision);
  const UnsignedInteger maximumIteration = 10 * parameterDimension + 10;
  for (UnsignedInteger iteration = 0; iteration < maximumIteration; ++iteration)
  {
    Indices freeIndices;
    for (UnsignedInteger j = 0; j < parameterDimension; ++j)
      if (isFree[j])
        freeIndices.add(j);
    const UnsignedInteger freeDimension = freeIndices.getSize();
    // Least-squares solution restricted to the free variables, the variables
    // fixed at a bound being moved to the right-hand side.
    Point freeSolution;
    if (freeDimension > 0)
    {
      Matrix freeDesign(size, freeDimension);
      Point freeRightHandSide(rightHandSide);
      for (UnsignedInteger k = 0; k < freeDimension; ++k)
      {
        const UnsignedInteger column = freeIndices[k];
        for (UnsignedInteger i = 0; i < size; ++i)
          freeDesign(i, k) = design(i, column);
      }
      for (UnsignedInteger j = 0; j < parameterDimension; ++j)
        if (!isFree[j])
          for (UnsignedInteger i = 0; i < size; ++i)
            freeRightHandSide[i] -= design(i, j) * solution[j];
      LeastSquaresMethod method(LeastSquaresMethod::Build("SVD", freeDesign));
      freeSolution = method.solve(freeRightHandSide);
    }
    // Move toward the free solution, stopping at the first bound reached.
    Scalar step = 1.0;
    for (UnsignedInteger k = 0; k < freeDimension; ++k)
    {
      const UnsignedInteger column = freeIndices[k];
      if (freeSolution[k] == solution[column])
        continue;
      if ((freeSolution[k] < lowerBound[column]) || (freeSolution[k] > upperBound[column]))
      {
        const Scalar reached = freeSolution[k] < lowerBound[column]
          ? lowerBound[column] : upperBound[column];
        step = std::min(step, (reached - solution[column]) / (freeSolution[k] - solution[column]));
      }
    }
    for (UnsignedInteger k = 0; k < freeDimension; ++k)
    {
      const UnsignedInteger column = freeIndices[k];
      solution[column] += step * (freeSolution[k] - solution[column]);
      // Fix a coordinate only when the free solve wants to go beyond the bound
      // it reaches. A coordinate that merely starts on a bound and whose free
      // solution is interior must stay free, otherwise a step blocked by
      // another coordinate would freeze it at its current value.
      if ((solution[column] <= lowerBound[column]) && (freeSolution[k] <= lowerBound[column]))
      {
        solution[column] = lowerBound[column];
        isFree[column] = false;
      }
      else if ((solution[column] >= upperBound[column]) && (freeSolution[k] >= upperBound[column]))
      {
        solution[column] = upperBound[column];
        isFree[column] = false;
      }
    }
    if (step < 1.0)
      continue;
    // The solution is now optimal over the free variables. Check the sign
    // conditions on the variables fixed at a bound: the gradient may not point
    // below a lower bound nor above an upper bound.
    Point gradient(parameterDimension);
    Scalar squaredResidual = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      Scalar residual = -rightHandSide[i];
      for (UnsignedInteger j = 0; j < parameterDimension; ++j)
        residual += design(i, j) * solution[j];
      squaredResidual += residual * residual;
      for (UnsignedInteger j = 0; j < parameterDimension; ++j)
        gradient[j] += design(i, j) * residual;
    }
    const Scalar residualNorm = std::sqrt(squaredResidual);
    UnsignedInteger releasedIndex = parameterDimension;
    Scalar worstViolation = 0.0;
    for (UnsignedInteger j = 0; j < parameterDimension; ++j)
    {
      // A parameter with equal bounds cannot move: it is never a release
      // candidate, otherwise the active set would cycle.
      if (isFree[j] || isFixed[j])
        continue;
      const Scalar tolerance = optimalityTolerance * columnNorm[j] * residualNorm;
      Scalar violation = 0.0;
      if ((solution[j] <= lowerBound[j]) && (gradient[j] < -tolerance))
        violation = -gradient[j] - tolerance;
      else if ((solution[j] >= upperBound[j]) && (gradient[j] > tolerance))
        violation = gradient[j] - tolerance;
      if (violation > worstViolation)
      {
        worstViolation = violation;
        releasedIndex = j;
      }
    }
    if (releasedIndex == parameterDimension)
      return solution;
    isFree[releasedIndex] = true;
  }
  throw InternalException(HERE) << "Error: the bounded least-squares active-set solver did not converge in " << maximumIteration << " iterations.";
}

/* Truncated Gaussian posterior: Normal(mean, covariance) truncated to the
 * parameter bounds. */
Distribution CalibrationAlgorithmImplementation::ComputeTruncatedPosterior(const Point & mean,
    const CovarianceMatrix & covariance,
    const Interval & bounds)
{
  const Normal gaussian(mean, covariance);
  return TruncatedDistribution(gaussian, bounds);
}

/* String converter */
String CalibrationAlgorithmImplementation::__repr__() const
{
  return OSS() << "class=" << CalibrationAlgorithmImplementation::GetClassName()
         << " output observations=" << outputObservations_
         << " parameter prior=" << parameterPrior_
         << " result=" << result_;
}

/* Model accessor */
Function CalibrationAlgorithmImplementation::getModel() const
{
  return model_;
}

/* Input observations accessor */
Sample CalibrationAlgorithmImplementation::getInputObservations() const
{
  return inputObservations_;
}

/* Output observations accessor */
Sample CalibrationAlgorithmImplementation::getOutputObservations() const
{
  return outputObservations_;
}

/* Parameter prior accessor */
Distribution CalibrationAlgorithmImplementation::getParameterPrior() const
{
  return parameterPrior_;
}

/* Bounds accessor */
Interval CalibrationAlgorithmImplementation::getBounds() const
{
  return bounds_;
}

void CalibrationAlgorithmImplementation::setBounds(const Interval & bounds)
{
  // An empty interval means no bounds; otherwise the dimension must match
  // the calibration parameter dimension when it is already known.
  const UnsignedInteger parameterDimension = parameterPrior_.getDimension();
  if ((bounds.getDimension() != 0) && (parameterDimension != 0) && (bounds.getDimension() != parameterDimension))
    throw InvalidArgumentException(HERE) << "Error: expected bounds of dimension=" << parameterDimension << ", got dimension=" << bounds.getDimension();
  bounds_ = bounds;
}

/* Result accessor */
void CalibrationAlgorithmImplementation::setResult(const CalibrationResult & result)
{
  result_ = result;
}

CalibrationResult CalibrationAlgorithmImplementation::getResult() const
{
  return result_;
}

/* Virtual copy constructor */
CalibrationAlgorithmImplementation* CalibrationAlgorithmImplementation::clone() const
{
  return new CalibrationAlgorithmImplementation(*this);
}

/* Method save() stores the object through the StorageManager */
void CalibrationAlgorithmImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("model_", model_);
  adv.saveAttribute("inputObservations_", inputObservations_);
  adv.saveAttribute("outputObservations_", outputObservations_);
  adv.saveAttribute("parameterPrior_", parameterPrior_);
  adv.saveAttribute("bounds_", bounds_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void CalibrationAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("model_", model_);
  adv.loadAttribute("inputObservations_", inputObservations_);
  adv.loadAttribute("outputObservations_", outputObservations_);
  adv.loadAttribute("parameterPrior_", parameterPrior_);
  if (adv.hasAttribute("bounds_"))
    adv.loadAttribute("bounds_", bounds_);
  adv.loadAttribute("result_", result_);
}


END_NAMESPACE_OPENTURNS
