//                                               -*- C++ -*-
/**
 *  @brief Default LinearLeastSquaresCalibration
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
#include "openturns/LinearLeastSquaresCalibration.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/QuadraticProblem.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

#include <algorithm>
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LinearLeastSquaresCalibration)

static const Factory<LinearLeastSquaresCalibration> Factory_LinearLeastSquaresCalibration;

/* Default constructor */
LinearLeastSquaresCalibration::LinearLeastSquaresCalibration()
  : CalibrationAlgorithmImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
LinearLeastSquaresCalibration::LinearLeastSquaresCalibration(const Function & model,
    const Sample & inputObservations,
    const Sample & outputObservations,
    const Point & startingPoint,
    const String & methodName)
  : CalibrationAlgorithmImplementation(model, inputObservations, outputObservations, Normal(startingPoint, CovarianceMatrix((IdentityMatrix(startingPoint.getDimension()) * std::sqrt(SpecFunc::MaxScalar)).getImplementation())))
  , modelObservations_(0, 0)
  , gradientObservations_(0, 0)
  , methodName_(methodName)
{
  // Check the input
  const UnsignedInteger parameterDimension = startingPoint.getDimension();
  if (model.getParameterDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of parameter dimension=" << parameterDimension << ", got parameter dimension=" << model.getParameterDimension();
  const UnsignedInteger inputDimension = inputObservations_.getDimension();
  if (model.getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of input dimension=" << inputDimension << ", got input dimension=" << model.getInputDimension();
  const UnsignedInteger outputDimension = outputObservations.getDimension();
  if (model.getOutputDimension() != outputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of output dimension=" << outputDimension << ", got output dimension=" << model.getOutputDimension();
  const UnsignedInteger size = outputObservations.getSize();
  if (inputObservations.getDimension() && inputObservations.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected an input sample of size=" << size << ", got size=" << inputObservations.getSize();
}

/* Parameter constructor */
LinearLeastSquaresCalibration::LinearLeastSquaresCalibration(const Sample & modelObservations,
    const Matrix & gradientObservations,
    const Sample & outputObservations,
    const Point & startingPoint,
    const String & methodName)
  : CalibrationAlgorithmImplementation(Function(), Sample(), outputObservations, Normal(startingPoint, CovarianceMatrix((IdentityMatrix(startingPoint.getDimension()) * std::sqrt(SpecFunc::MaxScalar)).getImplementation())))
  , modelObservations_(modelObservations)
  , gradientObservations_(gradientObservations)
  , methodName_(methodName)
{
  // Check the input
  const UnsignedInteger parameterDimension = startingPoint.getDimension();
  const UnsignedInteger outputDimension = outputObservations.getDimension();
  const UnsignedInteger size = outputObservations.getSize();
  if (gradientObservations.getNbColumns() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a gradient parameter of columns number=" << parameterDimension << ", got columns number=" << gradientObservations.getNbColumns();
  if (gradientObservations.getNbRows() != size * outputDimension) throw InvalidArgumentException(HERE) << "Error: expected a gradient parameter of rows number=" << size * outputDimension << ", got rows number=" << gradientObservations.getNbRows();
  if (gradientObservations.getNbColumns() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected an observations gradient of column dimension=" << parameterDimension << ", got column dimension=" << gradientObservations.getNbColumns();
  if (modelObservations.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected a model observation sample of size=" << size << ", got size=" << modelObservations.getSize();
  if (modelObservations.getDimension() != outputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model observation sample of dimension=" << outputDimension << ", got dimension=" << modelObservations.getDimension();
}

/* Performs the actual computation. Must be overloaded by the actual calibration algorithm */
void LinearLeastSquaresCalibration::run()
{
  const UnsignedInteger parameterDimension = getParameterPrior().getDimension();
  if (getModel().getEvaluation().getImplementation()->isActualImplementation())
  {
    // Compute the linearization
    Function parametrizedModel(getModel());
    parametrizedModel.setParameter(getParameterPrior().getMean());
    // Flatten everything related to the model evaluations over the input observations
    const UnsignedInteger outputDimension = getOutputObservations().getDimension();
    const UnsignedInteger size = getOutputObservations().getSize();
    modelObservations_ = parametrizedModel(getInputObservations());
    gradientObservations_ = MatrixImplementation(parameterDimension, size * outputDimension);
    UnsignedInteger shift = 0;
    UnsignedInteger skip = parameterDimension * outputDimension;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Matrix parameterGradient(parametrizedModel.parameterGradient(getInputObservations()[i]));
      std::copy(parameterGradient.getImplementation()->begin(), parameterGradient.getImplementation()->end(), gradientObservations_.getImplementation()->begin() + shift);
      shift += skip;
    }
    gradientObservations_ = gradientObservations_.transpose();
    parameterPrior_.setDescription(getModel().getParameterDescription());
  }

  const Point deltaY(modelObservations_.getImplementation()->getData() - outputObservations_.getImplementation()->getData());
  Distribution error;
  Point thetaStar;
  Point r;
  CovarianceMatrix gramInverse;
  UnsignedInteger degreesOfFreedom = 0;
  Indices freeIndices;
  if (bounds_.getDimension() > 0)
  {
    // Minimize ||J.theta-(J.theta0-deltaY)||^2 subject to the parameter bounds.
    // The constant term of the QP is ||J.theta0-deltaY||^2, so its optimal
    // value is the residual sum of squares.
    const Point target(gradientObservations_ * getStartingPoint() - deltaY);
    const QuadraticProblem problem(QuadraticProblem::FromLeastSquares(gradientObservations_, target, bounds_));
    OptimizationAlgorithm solver(OptimizationAlgorithm::Build(problem));
    // Build only sets the problem: the selected algorithm has no starting point
    // of its own, and solvers such as Cobyla require one. Start from the
    // calibration starting point projected onto the bounds, which is feasible.
    Point solverStart(getStartingPoint());
    const Point lowerBound(bounds_.getLowerBound());
    const Point upperBound(bounds_.getUpperBound());
    const Interval::BoolCollection finiteLowerBound(bounds_.getFiniteLowerBound());
    const Interval::BoolCollection finiteUpperBound(bounds_.getFiniteUpperBound());
    for (UnsignedInteger i = 0; i < parameterDimension; ++i)
    {
      if (finiteLowerBound[i] && (solverStart[i] < lowerBound[i]))
        solverStart[i] = lowerBound[i];
      if (finiteUpperBound[i] && (solverStart[i] > upperBound[i]))
        solverStart[i] = upperBound[i];
    }
    solver.setStartingPoint(solverStart);
    solver.run();
    const OptimizationResult result(solver.getResult());
    if (result.getStatus() != OptimizationResult::SUCCESS)
      throw InternalException(HERE) << "Error: the bounded quadratic program solver failed: " << result.getStatusMessage();
    thetaStar = result.getOptimalPoint();
    r = gradientObservations_ * thetaStar - target;

    // Variables at an active bound are considered fixed: the posterior
    // covariance is computed on the free block J_F^T J_F only, and the
    // residual degrees of freedom exclude the active directions. This is the
    // usual active-set approximation, which treats the active bounds as
    // equality constraints and applies the unconstrained Gaussian
    // approximation to the remaining free parameters. See
    // S. Huet, A. Bouvier, M.-A. Poursat and E. Jolivet, Statistical Tools for
    // Nonlinear Regression: A Practical Guide with S-PLUS and R Examples,
    // 2nd ed., Springer, New York, 2004, for the Gaussian approximation, and
    // P. E. Gill, W. Murray and M. H. Wright, Practical Optimization,
    // Academic Press, London, 1981, Section 5.2, for the active-set treatment
    // of bound constraints.
    freeIndices = ComputeFreeParameterIndices(thetaStar, bounds_);
    degreesOfFreedom = freeIndices.getSize();
    gramInverse = ComputeFreeGramInverse(gradientObservations_, freeIndices, methodName_);
  }
  else
  {
    LeastSquaresMethod method(LeastSquaresMethod::Build(methodName_, gradientObservations_));
    const Point deltaTheta(method.solve(deltaY));
    for (UnsignedInteger i = 0; i < deltaTheta.getDimension(); ++ i)
      if (!std::isfinite(deltaTheta[i])) throw InvalidArgumentException(HERE) << "The calibration problem is not identifiable";

    thetaStar = getStartingPoint() - deltaTheta;
    r = deltaY - gradientObservations_ * deltaTheta;
    gramInverse = method.getGramInverse();
    degreesOfFreedom = thetaStar.getDimension();
    freeIndices = Indices(degreesOfFreedom);
    for (UnsignedInteger i = 0; i < degreesOfFreedom; ++i)
      freeIndices[i] = i;
  }
  Distribution parameterPosterior(ComputePosteriorAndErrorDistribution(thetaStar, r, gramInverse, freeIndices, degreesOfFreedom, outputObservations_.getDimension(), error));
  parameterPosterior.setDescription(parameterPrior_.getDescription());
  const LinearFunction residualFunction(getStartingPoint(), deltaY, gradientObservations_);
  result_ = CalibrationResult(parameterPrior_, parameterPosterior, thetaStar, error, inputObservations_, outputObservations_, residualFunction, false);
}

Distribution LinearLeastSquaresCalibration::ComputePosteriorAndErrorDistribution(const Point & thetaStar,
    const Point & r,
    const SquareMatrix & gramInverse,
    const Indices & freeIndices,
    const UnsignedInteger degreesOfFreedom,
    const UnsignedInteger outputDimension,
    Distribution & error)
{
  if (!(r.getDimension() > degreesOfFreedom))
    throw InvalidArgumentException(HERE) << "Error: cannot estimate residual variance because residual dimension="
                                         << r.getDimension() << " is not greater than the number of free parameters=" << degreesOfFreedom;
  const Scalar varianceError = r.normSquare() / (r.getDimension() - degreesOfFreedom);
  CovarianceMatrix covarianceThetaStar;
  const Scalar epsilon = ResourceMap::GetAsScalar("LinearLeastSquaresCalibration-Regularization");
  covarianceThetaStar = CovarianceMatrix((gramInverse * varianceError).getImplementation());
  // A non-finite trace means a rank-deficient Jacobian block: the resulting
  // Gaussian posterior would be meaningless, so fail instead of returning it.
  const Scalar covarianceTrace = covarianceThetaStar.computeTrace();
  if (!std::isfinite(covarianceTrace))
    throw InvalidArgumentException(HERE) << "Error: the covariance of the posterior distribution is not finite. The problem may be not identifiable.";
  if (epsilon > 0.0)
  {
    // A zero covariance, obtained when every parameter is active at a bound,
    // has no eigenvalue to shift: skip the regularization in that case.
    if (covarianceTrace > 0.0)
    {
      const Scalar shift = epsilon * covarianceThetaStar.computeLargestEigenValueModule();
      // Parameters fixed at a bound must keep a zero variance: regularize the
      // free directions only, as the active directions are excluded from the
      // posterior covariance and from the residual degrees of freedom.
      for (UnsignedInteger k = 0; k < freeIndices.getSize(); ++k)
        covarianceThetaStar(freeIndices[k], freeIndices[k]) += shift;
    }
  }
  Normal parameterPosterior;
  try
  {
    parameterPosterior = Normal(thetaStar, covarianceThetaStar);
  }
  catch (...)
  {
    throw InternalException(HERE) << "Error: the covariance of the posterior distribution is not definite positive. The problem may be not identifiable. Try to increase the \"LinearLeastSquaresCalibration-Regularization\" key in ResourceMap";
  }
  if (outputDimension > 0)
  {
    try
    {
      error = Normal(Point(outputDimension), (IdentityMatrix(outputDimension) * varianceError).getImplementation());
    }
    catch (...)
    {
      error = Normal(Point(outputDimension), (IdentityMatrix(outputDimension) * SpecFunc::MaxScalar).getImplementation());
    }
  } // outputDimension > 0
  return parameterPosterior;
}

/* Model observations accessor */
Sample LinearLeastSquaresCalibration::getModelObservations() const
{
  return modelObservations_;
}


/* Model gradient wrt the parameter accessor */
Matrix LinearLeastSquaresCalibration::getGradientObservations() const
{
  return gradientObservations_;
}

/* StartingPoint accessor */
Point LinearLeastSquaresCalibration::getStartingPoint() const
{
  // The startingPoint is stored in the prior distribution, which is a flat Normal distribution
  return getParameterPrior().getMean();
}

/* Bounds accessor */
Interval LinearLeastSquaresCalibration::getBounds() const
{
  return bounds_;
}

void LinearLeastSquaresCalibration::setBounds(const Interval & bounds)
{
  // An empty interval means no bounds; otherwise the dimension must match
  // the calibration parameter dimension when it is already known.
  const UnsignedInteger parameterDimension = parameterPrior_.getDimension();
  if ((bounds.getDimension() != 0) && (parameterDimension != 0) && (bounds.getDimension() != parameterDimension))
    throw InvalidArgumentException(HERE) << "Error: expected bounds of dimension=" << parameterDimension << ", got dimension=" << bounds.getDimension();
  bounds_ = bounds;
}

/* Least squares method name accessor */
String LinearLeastSquaresCalibration::getMethodName() const
{
  return methodName_;
}

/* String converter */
String LinearLeastSquaresCalibration::__repr__() const
{
  return OSS() << "class=" << LinearLeastSquaresCalibration::GetClassName()
         << " name=" << getName();
}


LinearLeastSquaresCalibration * LinearLeastSquaresCalibration::clone() const
{
  return new LinearLeastSquaresCalibration(*this);
}


/* Method save() stores the object through the StorageManager */
void LinearLeastSquaresCalibration::save(Advocate & adv) const
{
  CalibrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("modelObservations_", modelObservations_);
  adv.saveAttribute("gradientObservations_", gradientObservations_);
  adv.saveAttribute("bounds_", bounds_);
  adv.saveAttribute("methodName_", methodName_);
}

/* Method load() reloads the object from the StorageManager */
void LinearLeastSquaresCalibration::load(Advocate & adv)
{
  CalibrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("modelObservations_", modelObservations_);
  adv.loadAttribute("gradientObservations_", gradientObservations_);
  if (adv.hasAttribute("bounds_"))
    adv.loadAttribute("bounds_", bounds_);
  adv.loadAttribute("methodName_", methodName_);
}


END_NAMESPACE_OPENTURNS
