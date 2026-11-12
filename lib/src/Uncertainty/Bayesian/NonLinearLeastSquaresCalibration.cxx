//                                               -*- C++ -*-
/**
 *  @brief Default NonLinearLeastSquaresCalibration
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
#include "openturns/NonLinearLeastSquaresCalibration.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"
#include "openturns/NormalFactory.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/BootstrapExperiment.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/LeastSquaresProblem.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/LinearLeastSquaresCalibration.hxx"

#include <algorithm>
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NonLinearLeastSquaresCalibration)

static const Factory<NonLinearLeastSquaresCalibration> Factory_NonLinearLeastSquaresCalibration;

/* Default constructor */
NonLinearLeastSquaresCalibration::NonLinearLeastSquaresCalibration()
  : CalibrationAlgorithmImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
NonLinearLeastSquaresCalibration::NonLinearLeastSquaresCalibration(const Function & model,
    const Sample & inputObservations,
    const Sample & outputObservations,
    const Point & startingPoint)
  : CalibrationAlgorithmImplementation(model, inputObservations, outputObservations, Normal(startingPoint, CovarianceMatrix((IdentityMatrix(startingPoint.getDimension()) * std::sqrt(SpecFunc::MaxScalar)).getImplementation())))
  , algorithm_()
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("NonLinearLeastSquaresCalibration-BootstrapSize"))
{
  // Check the input
  const UnsignedInteger inputDimension = inputObservations_.getDimension();
  if (model.getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of input dimension=" << inputDimension << ", got input dimension=" << model.getInputDimension();
  const UnsignedInteger outputDimension = outputObservations.getDimension();
  if (model.getOutputDimension() != outputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of output dimension=" << outputDimension << ", got output dimension=" << model.getOutputDimension();
  const UnsignedInteger size = outputObservations.getSize();
  if (inputObservations.getDimension() && inputObservations.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected an input sample of size=" << size << ", got size=" << inputObservations.getSize();

  // Now the automatic selection of the algorithm
  algorithm_ = OptimizationAlgorithm::Build(LeastSquaresProblem());
  parameterPrior_.setDescription(model.getParameterDescription());
}

namespace NonLinearLeastSquaresCalibrationFunctions
{
class CalibrationModelEvaluation: public EvaluationImplementation
{
public:
  CalibrationModelEvaluation(const Function & model,
                             const Sample & inputObservations,
                             const Sample & outputObservations)
    : EvaluationImplementation()
    , model_(model)
    , inputObservations_(inputObservations)
    , outputObservations_(outputObservations)
  {
    // Check if the given input observations are compatible with the model
    if (inputObservations.getDimension() != model.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected input observations of dimension=" << model.getInputDimension() << ", got dimension=" << inputObservations.getDimension();
    // Check if the given output observations are compatible with the model
    if (outputObservations.getDimension() != model.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: expected output observations of dimension=" << model.getOutputDimension() << ", got dimension=" << outputObservations.getDimension();
  }

  CalibrationModelEvaluation * clone() const override
  {
    return new CalibrationModelEvaluation(*this);
  }

  Point operator() (const Point & point) const override
  {
    Function localModel(model_);
    localModel.setParameter(point);
    return localModel(inputObservations_).getImplementation()->getData() - outputObservations_.getImplementation()->getData();
  }

  UnsignedInteger getInputDimension() const override
  {
    return model_.getParameterDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return model_.getOutputDimension() * inputObservations_.getSize();
  }

  Description getInputDescription() const override
  {
    return model_.getParameterDescription();
  }

  Description getOutputDescription() const override
  {
    const Description atomicOutputDescription(model_.getOutputDescription());
    Description outputDescription(0);
    for (UnsignedInteger i = 0; i < inputObservations_.getSize(); ++i)
      for (UnsignedInteger j = 0; j < model_.getOutputDimension(); ++j)
        outputDescription.add(String(OSS() << atomicOutputDescription[j] << "_" << i));
    return outputDescription;
  }

  Description getDescription() const override
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  String __repr__() const override
  {
    OSS oss;
    oss << "CalibrationModelEvaluation(" << model_.__str__() << ", " << inputObservations_ << ")";
    return oss;
  }

  String __str__(const String & ) const override
  {
    OSS oss;
    oss << "CalibrationModelEvaluation(" << model_.__str__() << ", " << inputObservations_ << ")";
    return oss;
  }

  Function getModel() const
  {
    return model_;
  }

  Sample getInputObservations() const
  {
    return inputObservations_;
  }

  Sample getOutputObservations() const
  {
    return outputObservations_;
  }

private:
  const Function model_;
  const Sample inputObservations_;
  const Sample outputObservations_;
}; // class CalibrationModelEvaluation

class CalibrationModelGradient: public GradientImplementation
{
public:
  CalibrationModelGradient(const CalibrationModelEvaluation & evaluation)
    : GradientImplementation()
    , evaluation_(evaluation)
  {
    // Nothing to do
  }

  CalibrationModelGradient * clone() const override
  {
    return new CalibrationModelGradient(*this);
  }

  Matrix gradient(const Point & point) const override
  {
    Function parametrizedModel(evaluation_.getModel());
    parametrizedModel.setParameter(point);
    const Sample inputObservations(evaluation_.getInputObservations());
    const UnsignedInteger size = inputObservations.getSize();
    MatrixImplementation gradientObservations(getInputDimension(), getOutputDimension());
    const UnsignedInteger skip = parametrizedModel.getParameterDimension() * parametrizedModel.getOutputDimension();
    UnsignedInteger shift = 0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Matrix parameterGradient(parametrizedModel.parameterGradient(inputObservations[i]));
      std::copy(parameterGradient.getImplementation()->begin(), parameterGradient.getImplementation()->end(), gradientObservations.begin() + shift);
      shift += skip;
    }
    return gradientObservations;
  }

  UnsignedInteger getInputDimension() const override
  {
    return evaluation_.getInputDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return evaluation_.getOutputDimension();
  }

  String __repr__() const override
  {
    OSS oss;
    oss << "CalibrationModelGradient(" << evaluation_ << ")";
    return oss;
  }

  String __str__(const String & ) const override
  {
    OSS oss;
    oss << "CalibrationModelGradient(" << evaluation_ << ")";
    return oss;
  }

private:
  const CalibrationModelEvaluation evaluation_;

}; // class CalibrationModelGradient
}

/* Build the associated residual function */
Function NonLinearLeastSquaresCalibration::BuildResidualFunction(const Function & model,
    const Sample & inputObservations,
    const Sample & outputObservations)
{
  // Build the residual function this way to benefit from the automatic Hessian
  const NonLinearLeastSquaresCalibrationFunctions::CalibrationModelEvaluation residualEvaluation(model, inputObservations, outputObservations);
  return Function(residualEvaluation, NonLinearLeastSquaresCalibrationFunctions::CalibrationModelGradient(residualEvaluation), CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), residualEvaluation));
}

/* Performs the actual computation. Must be overloaded by the actual calibration algorithm */
void NonLinearLeastSquaresCalibration::run()
{
  // Compute the posterior MAP
  // Non-zero size to tell the algorithm to return the residual at the optimal parameter
  Sample residual(1, 0);
  const Point thetaStar(run(inputObservations_, outputObservations_, getStartingPoint(), residual));
  // Recover the residuals
  const Normal error(NormalFactory().buildAsNormal(residual));
  // Compute the posterior distribution
  Distribution parameterPosterior;
  if (bootstrapSize_ > 0)
  {
    // Compute the covariance using Bootstrap
    Sample joinedData(inputObservations_);
    joinedData.stack(outputObservations_);
    const BootstrapExperiment bootstrap(joinedData);
    Sample thetaSample(bootstrapSize_, thetaStar.getDimension());
    Indices inputIndices(inputObservations_.getDimension());
    inputIndices.fill();
    Indices outputIndices(outputObservations_.getDimension());
    outputIndices.fill(inputIndices.getSize());
    Sample empty;
    for (UnsignedInteger i = 0; i < bootstrapSize_; ++i)
    {
      const Sample joinedSample(bootstrap.generate());
      thetaSample[i] = run(joinedSample.getMarginal(inputIndices), joinedSample.getMarginal(outputIndices), thetaStar, empty);
    }
    parameterPosterior = KernelSmoothing().build(thetaSample);
  }
  else
  {
    // Prepare the inverse Gram
    // Compute the linearization
    Function parametrizedModel(model_);
    parametrizedModel.setParameter(thetaStar);
    // Flatten everything related to the model evaluations over the input observations
    const UnsignedInteger parameterDimension = getParameterPrior().getDimension();
    const UnsignedInteger outputDimension = getOutputObservations().getDimension();
    const UnsignedInteger size = getOutputObservations().getSize();
    Matrix gradientObservations = MatrixImplementation(parameterDimension, size * outputDimension);
    UnsignedInteger shift = 0;
    UnsignedInteger skip = parameterDimension * outputDimension;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Matrix parameterGradient(parametrizedModel.parameterGradient(getInputObservations()[i]));
      std::copy(parameterGradient.getImplementation()->begin(), parameterGradient.getImplementation()->end(), gradientObservations.getImplementation()->begin() + shift);
      shift += skip;
    }
    const Matrix jacobian(gradientObservations.transpose());
    const String methodName(ResourceMap::GetAsString("LinearLeastSquaresCalibration-Method"));
    LeastSquaresMethod method(LeastSquaresMethod::Build(methodName, jacobian));
    // Full Hessian for the Laplace path, full Gram inverse otherwise.
    // The bounded case below restricts them to the free directions.
    Matrix laplaceHessian;
    CovarianceMatrix gramInverseFull;
    if (useLaplaceApproximation_)
    {
      // Laplace approximation: the posterior covariance uses the full Hessian
      // H = J^T J + sum_k r_k Hessian(r_k) at the MAP instead of the
      // Gauss-Newton Gram matrix (see issue #3273)
      const Function residualFunction(BuildResidualFunction(model_, inputObservations_, outputObservations_));
      const Point residuals(residual.getImplementation()->getData());
      const SymmetricTensor residualHessian(residualFunction.hessian(thetaStar));
      laplaceHessian = Matrix(gradientObservations * gradientObservations.transpose());
      const UnsignedInteger residualSize = residuals.getSize();
      for (UnsignedInteger k = 0; k < residualSize; ++k)
        for (UnsignedInteger i = 0; i < parameterDimension; ++i)
          for (UnsignedInteger j = 0; j <= i; ++j)
            laplaceHessian(i, j) += residuals[k] * residualHessian(i, j, k);
      // Symmetrize explicitly before inversion
      for (UnsignedInteger i = 0; i < parameterDimension; ++i)
        for (UnsignedInteger j = 0; j < i; ++j)
          laplaceHessian(j, i) = laplaceHessian(i, j);
      try
      {
        const Matrix laplaceInverse(laplaceHessian.solveLinearSystem(IdentityMatrix(parameterDimension)));
        gramInverseFull = CovarianceMatrix(*laplaceInverse.getImplementation());
      }
      catch (const Exception &)
      {
        throw InvalidArgumentException(HERE) << "In NonLinearLeastSquaresCalibration::run, cannot compute the Laplace approximation: the Hessian of the residuals at the optimum is not positive definite.";
      }
    }
    else
    {
      // Call solve only to insure that the decomposition (QR, Cholesky, SVD...) are up to date.
      (void) method.solve(Point(jacobian.getNbRows()));
      gramInverseFull = method.getGramInverse();
    }
    Distribution tmp;
    if (bounds_.getDimension() == 0)
    {
      Indices freeIndices(parameterDimension);
      freeIndices.fill();
      parameterPosterior = LinearLeastSquaresCalibration::ComputePosteriorAndErrorDistribution(thetaStar, residual.getImplementation()->getData(), gramInverseFull, freeIndices, parameterDimension, 0, tmp);
    }
    else
    {
      // Variables at an active bound are considered fixed: the posterior
      // covariance is computed on the free block only, and the residual
      // degrees of freedom exclude the active directions. This is the
      // usual active-set approximation, which treats the active bounds as
      // equality constraints and applies the unconstrained Gaussian
      // approximation to the remaining free parameters. See
      // S. Huet, A. Bouvier, M.-A. Poursat and E. Jolivet, Statistical Tools for
      // Nonlinear Regression: A Practical Guide with S-PLUS and R Examples,
      // 2nd ed., Springer, New York, 2004, for the Gaussian approximation, and
      // P. E. Gill, W. Murray and M. H. Wright, Practical Optimization,
      // Academic Press, London, 1981, Section 5.2, for the active-set treatment
      // of bound constraints.
      // Absolute tolerance used to detect a parameter lying on one of its bounds.
      const Scalar tolerance = ResourceMap::GetAsScalar("LinearLeastSquaresCalibration-ActiveBoundTolerance");
      if (!(tolerance > 0.0) || !std::isfinite(tolerance))
        throw InvalidArgumentException(HERE) << "Error: the active bound tolerance must be finite and strictly positive, got " << tolerance;
      const Point lowerBound(bounds_.getLowerBound());
      const Point upperBound(bounds_.getUpperBound());
      const Interval::BoolCollection finiteLowerBound(bounds_.getFiniteLowerBound());
      const Interval::BoolCollection finiteUpperBound(bounds_.getFiniteUpperBound());
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
      const UnsignedInteger freeDimension = freeIndices.getSize();
      // The covariance is zero in the directions fixed at a bound
      CovarianceMatrix gramInverse(parameterDimension, Point(parameterDimension));
      if (freeDimension > 0)
      {
        if (useLaplaceApproximation_)
        {
          Matrix freeHessian(freeDimension, freeDimension);
          for (UnsignedInteger i = 0; i < freeDimension; ++i)
            for (UnsignedInteger j = 0; j < freeDimension; ++j)
              freeHessian(i, j) = laplaceHessian(freeIndices[i], freeIndices[j]);
          try
          {
            const Matrix freeInverse(freeHessian.solveLinearSystem(IdentityMatrix(freeDimension)));
            const CovarianceMatrix freeGramInverse(*freeInverse.getImplementation());
            for (UnsignedInteger i = 0; i < freeDimension; ++i)
              for (UnsignedInteger j = 0; j < freeDimension; ++j)
              {
                const Scalar value = freeGramInverse(i, j);
                if (!std::isfinite(value))
                  throw InvalidArgumentException(HERE) << "Error: the calibration problem is not identifiable in the free parameter directions";
                gramInverse(freeIndices[i], freeIndices[j]) = value;
              }
          }
          catch (const InvalidArgumentException &)
          {
            throw;
          }
          catch (const Exception &)
          {
            throw InvalidArgumentException(HERE) << "In NonLinearLeastSquaresCalibration::run, cannot compute the Laplace approximation: the Hessian of the residuals at the optimum is not positive definite in the free parameter directions.";
          }
        }
        else
        {
          Matrix freeJacobian(jacobian.getNbRows(), freeDimension);
          for (UnsignedInteger j = 0; j < freeDimension; ++j)
            for (UnsignedInteger i = 0; i < jacobian.getNbRows(); ++i)
              freeJacobian(i, j) = jacobian(i, freeIndices[j]);
          // A rank-deficient free block gives a small positive singular value whose
          // inverse can still be finite: check the numerical rank before relying on
          // the factorization of the free block.
          const Point singularValues(freeJacobian.computeSingularValues());
          const UnsignedInteger singularValuesSize = singularValues.getSize();
          const Scalar largestSingularValue = (singularValuesSize > 0) ? singularValues[0] : 0.0;
          const Scalar rankTolerance = std::max(freeJacobian.getNbRows(), freeDimension) * SpecFunc::Precision * largestSingularValue;
          if ((singularValuesSize < freeDimension) || !(largestSingularValue > 0.0) || (singularValues[singularValuesSize - 1] <= rankTolerance))
            throw InvalidArgumentException(HERE) << "Error: the calibration problem is not identifiable in the free parameter directions";
          LeastSquaresMethod freeMethod(LeastSquaresMethod::Build(methodName, freeJacobian));
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
        }
      }
      parameterPosterior = LinearLeastSquaresCalibration::ComputePosteriorAndErrorDistribution(thetaStar, residual.getImplementation()->getData(), gramInverse, freeIndices, freeDimension, 0, tmp);
    }
  }
  parameterPosterior.setDescription(parameterPrior_.getDescription());
  const Function residualFunction(BuildResidualFunction(model_, inputObservations_, outputObservations_));
  result_ = CalibrationResult(parameterPrior_, parameterPosterior, thetaStar, error, inputObservations_, outputObservations_, residualFunction, false);
  // A bootstrapped posterior is the MAP distribution, not a posterior (see #2889)
  result_.setIsParameterDistributionPosterior(bootstrapSize_ == 0);
}

/* Perform a unique estimation */
Point NonLinearLeastSquaresCalibration::run(const Sample & inputObservations,
    const Sample & outputObservations,
    const Point & startingPoint,
    Sample & residual)
{
  const Function residualFunction(BuildResidualFunction(model_, inputObservations, outputObservations));
  LeastSquaresProblem problem(residualFunction);
  if (bounds_.getDimension() != 0)
  {
    if (bounds_.getDimension() != residualFunction.getInputDimension())
      throw InvalidArgumentException(HERE) << "Error: expected bounds of dimension=" << residualFunction.getInputDimension() << ", got dimension=" << bounds_.getDimension();
    problem.setBounds(bounds_);
  }
  algorithm_.setProblem(problem);
  try
  {
    // If the solver is single start, we can use its setStartingPoint method
    algorithm_.setStartingPoint(startingPoint);
  }
  catch (const NotDefinedException &) // setStartingPoint is not defined for the solver
  {
    LOGWARN(OSS() << "startingPoint=" << startingPoint << " is ignored because algorithm "
            << algorithm_.getImplementation()->getClassName() << " has no setStartingPoint method.");
  }
  algorithm_.run();
  const Point optimalPoint(algorithm_.getResult().getOptimalPoint());
  if (!optimalPoint.getDimension())
    throw InvalidArgumentException(HERE) << "optimization in NonLinearLeastSquaresCalibration did not yield a feasible point";
  // If asked for the residual values
  if (residual.getSize() > 0)
  {
    residual = Sample(outputObservations.getSize(), outputObservations.getDimension());
    residual.getImplementation()->setData(residualFunction(optimalPoint));
  }
  return optimalPoint;
}

/* StartingPoint accessor */
Point NonLinearLeastSquaresCalibration::getStartingPoint() const
{
  // The startingPoint is stored in the prior distribution, which is a Normal distribution
  return getParameterPrior().getMean();
}

/* Algorithm accessor */
OptimizationAlgorithm NonLinearLeastSquaresCalibration::getOptimizationAlgorithm() const
{
  return algorithm_;
}

void NonLinearLeastSquaresCalibration::setOptimizationAlgorithm(const OptimizationAlgorithm & algorithm)
{
  // Validate the bounds carried by the given algorithm before replacing
  // algorithm_, so a rejecting dimension leaves *this unchanged
  if (algorithm.getProblem().hasBounds())
  {
    const Interval bounds(algorithm.getProblem().getBounds());
    setBounds(bounds);
    algorithm_ = algorithm;
  }
  else
    algorithm_ = algorithm;
  // Otherwise keep the current bounds. Call setBounds() afterwards to
  // enforce specific bounds.
}

/* Bounds accessor */
Interval NonLinearLeastSquaresCalibration::getBounds() const
{
  return bounds_;
}

void NonLinearLeastSquaresCalibration::setBounds(const Interval & bounds)
{
  // An empty interval means no bounds; otherwise the dimension must match
  // the calibration parameter dimension when it is already known.
  const UnsignedInteger parameterDimension = parameterPrior_.getDimension();
  if ((bounds.getDimension() != 0) && (parameterDimension != 0) && (bounds.getDimension() != parameterDimension)) throw InvalidArgumentException(HERE) << "Error: expected bounds of dimension=" << parameterDimension << ", got dimension=" << bounds.getDimension();
  bounds_ = bounds;
}

/* Algorithm accessor */
UnsignedInteger NonLinearLeastSquaresCalibration::getBootstrapSize() const
{
  return bootstrapSize_;
}

void NonLinearLeastSquaresCalibration::setBootstrapSize(const UnsignedInteger bootstrapSize)
{
  bootstrapSize_ = bootstrapSize;
}

/* Laplace approximation accessor */
Bool NonLinearLeastSquaresCalibration::getUseLaplaceApproximation() const
{
  return useLaplaceApproximation_;
}

void NonLinearLeastSquaresCalibration::setUseLaplaceApproximation(const Bool useLaplaceApproximation)
{
  useLaplaceApproximation_ = useLaplaceApproximation;
}

/* String converter */
String NonLinearLeastSquaresCalibration::__repr__() const
{
  return OSS() << "class=" << NonLinearLeastSquaresCalibration::GetClassName()
         << " name=" << getName();
}


NonLinearLeastSquaresCalibration * NonLinearLeastSquaresCalibration::clone() const
{
  return new NonLinearLeastSquaresCalibration(*this);
}


/* Method save() stores the object through the StorageManager */
void NonLinearLeastSquaresCalibration::save(Advocate & adv) const
{
  CalibrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("algorithm_", algorithm_);
  adv.saveAttribute("bounds_", bounds_);
  adv.saveAttribute("bootstrapSize_", bootstrapSize_);
  adv.saveAttribute("useLaplaceApproximation_", useLaplaceApproximation_);
}

/* Method load() reloads the object from the StorageManager */
void NonLinearLeastSquaresCalibration::load(Advocate & adv)
{
  CalibrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("algorithm_", algorithm_);
  if (adv.hasAttribute("bounds_"))
    adv.loadAttribute("bounds_", bounds_);
  adv.loadAttribute("bootstrapSize_", bootstrapSize_);
  if (adv.hasAttribute("useLaplaceApproximation_")) adv.loadAttribute("useLaplaceApproximation_", useLaplaceApproximation_);
  else useLaplaceApproximation_ = false;
}


END_NAMESPACE_OPENTURNS
