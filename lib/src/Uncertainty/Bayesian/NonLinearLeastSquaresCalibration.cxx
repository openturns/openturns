//                                               -*- C++ -*-
/**
 *  @brief Default NonLinearLeastSquaresCalibration
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LowDiscrepancyExperiment.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/MultiStart.hxx"
#include "openturns/TNC.hxx"
#include "openturns/LeastSquaresProblem.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/LinearLeastSquaresCalibration.hxx"

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
  : CalibrationAlgorithmImplementation(model, inputObservations, outputObservations, Normal(startingPoint, CovarianceMatrix((IdentityMatrix(startingPoint.getDimension()) * SpecFunc::MaxScalar).getImplementation())))
  , algorithm_()
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("NonLinearLeastSquaresCalibration-BootstrapSize"))
{
  // Check the input
  const UnsignedInteger inputDimension = inputObservations_.getDimension();
  if (model.getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of input dimension=" << inputDimension << ", got input dimension=" << model.getInputDimension();
  const UnsignedInteger outputDimension = outputObservations.getDimension();
  if (model.getOutputDimension() != outputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of output dimension=" << outputDimension << ", got output dimension=" << model.getOutputDimension();
  const UnsignedInteger size = outputObservations.getSize();
  if (outputObservations.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected an output sample of size=" << size << ", got size=" << outputObservations.getSize();

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

  CalibrationModelEvaluation * clone() const
  {
    return new CalibrationModelEvaluation(*this);
  }

  Point operator() (const Point & point) const
  {
    Function localModel(model_);
    localModel.setParameter(point);
    return localModel(inputObservations_).getImplementation()->getData() - outputObservations_.getImplementation()->getData();
  }

  UnsignedInteger getInputDimension() const
  {
    return model_.getParameterDimension();
  }

  UnsignedInteger getOutputDimension() const
  {
    return model_.getOutputDimension() * inputObservations_.getSize();
  }

  Description getInputDescription() const
  {
    return model_.getParameterDescription();
  }

  Description getOutputDescription() const
  {
    const Description atomicOutputDescription(model_.getOutputDescription());
    Description outputDescription(0);
    for (UnsignedInteger i = 0; i < inputObservations_.getSize(); ++i)
      for (UnsignedInteger j = 0; j < model_.getOutputDimension(); ++j)
        outputDescription.add(String(OSS() << atomicOutputDescription[j] << "_" << i));
    return outputDescription;
  }

  Description getDescription() const
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  String __repr__() const
  {
    OSS oss;
    oss << "CalibrationModelEvaluation(" << model_.__str__() << ", " << inputObservations_ << ")";
    return oss;
  }

  String __str__(const String & ) const
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

  CalibrationModelGradient * clone() const
  {
    return new CalibrationModelGradient(*this);
  }

  Matrix gradient(const Point & point) const
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

  UnsignedInteger getInputDimension() const
  {
    return evaluation_.getInputDimension();
  }

  UnsignedInteger getOutputDimension() const
  {
    return evaluation_.getOutputDimension();
  }

  Description getInputDescription() const
  {
    return evaluation_.getInputDescription();
  }

  Description getOutputDescription() const
  {
    return evaluation_.getOutputDescription();
  }

  Description getDescription() const
  {
    return evaluation_.getDescription();
  }

  String __repr__() const
  {
    OSS oss;
    oss << "CalibrationModelGradient(" << evaluation_ << ")";
    return oss;
  }

  String __str__(const String & ) const
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
    LinearLeastSquaresCalibration linearAlgo(model_, inputObservations_, outputObservations_, thetaStar);
    linearAlgo.run();
    parameterPosterior = linearAlgo.getResult().getParameterPosterior();
  }
  parameterPosterior.setDescription(parameterPrior_.getDescription());
  const Function residualFunction(BuildResidualFunction(model_, inputObservations_, outputObservations_));
  result_ = CalibrationResult(parameterPrior_, parameterPosterior, thetaStar, error, inputObservations_, outputObservations_, residualFunction, false);
}

/* Perform a unique estimation */
Point NonLinearLeastSquaresCalibration::run(const Sample & inputObservations,
    const Sample & outputObservations,
    const Point & startingPoint,
    Sample & residual)
{
  const Function residualFunction(BuildResidualFunction(model_, inputObservations, outputObservations));
  LeastSquaresProblem problem(residualFunction);
  algorithm_.setVerbose(true);
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
  Point optimalPoint(algorithm_.getResult().getOptimalPoint());
  // If asked for the residual values
  if (residual.getSize() > 0)
  {
    residual = Sample(outputObservations.getSize(), outputObservations.getDimension());
    residual.getImplementation()->setData(residualFunction(optimalPoint));
  }
  return optimalPoint;
}

/* Candidate accessor */
Point NonLinearLeastSquaresCalibration::getCandidate() const
{
  LOGWARN(OSS() << "getCandidate is deprecated");
  return getStartingPoint();
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
  algorithm_ = algorithm;
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
  adv.saveAttribute("bootstrapSize_", bootstrapSize_);
}

/* Method load() reloads the object from the StorageManager */
void NonLinearLeastSquaresCalibration::load(Advocate & adv)
{
  CalibrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("algorithm_", algorithm_);
  adv.loadAttribute("bootstrapSize_", bootstrapSize_);
}


END_NAMESPACE_OPENTURNS
