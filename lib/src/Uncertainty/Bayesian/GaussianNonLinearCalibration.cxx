//                                               -*- C++ -*-
/**
 *  @brief Default GaussianNonLinearCalibration
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/GaussianNonLinearCalibration.hxx"
#include "openturns/GaussianLinearCalibration.hxx"
#include "openturns/NonLinearLeastSquaresCalibration.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"
#include "openturns/NormalFactory.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/MemoizeFunction.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/BootstrapExperiment.hxx"
#include "openturns/LowDiscrepancyExperiment.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/LeastSquaresProblem.hxx"
#include "openturns/MultiStart.hxx"
#include "openturns/TNC.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianNonLinearCalibration)

static const Factory<GaussianNonLinearCalibration> Factory_GaussianNonLinearCalibration;

/* Default constructor */
GaussianNonLinearCalibration::GaussianNonLinearCalibration()
  : CalibrationAlgorithmImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
GaussianNonLinearCalibration::GaussianNonLinearCalibration(const Function & model,
    const Sample & inputObservations,
    const Sample & outputObservations,
    const Point & candidate,
    const CovarianceMatrix & parameterCovariance,
    const CovarianceMatrix & errorCovariance)
  : CalibrationAlgorithmImplementation(model, inputObservations, outputObservations, Normal(candidate, parameterCovariance))
  , algorithm_()
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("GaussianNonLinearCalibration-BootstrapSize"))
  , errorCovariance_(errorCovariance)
  , globalErrorCovariance_(false)
{
  // Check the input
  const UnsignedInteger parameterDimension = candidate.getDimension();
  if (model.getParameterDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of parameter dimension=" << parameterDimension << ", got parameter dimension=" << model.getParameterDimension();
  if (parameterCovariance.getDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a parameter covariance of dimension=" << parameterDimension << ", got dimension=" << parameterCovariance.getDimension();
  const UnsignedInteger inputDimension = inputObservations.getDimension();
  if (model.getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of input dimension=" << inputDimension << ", got input dimension=" << model.getInputDimension();
  const UnsignedInteger outputDimension = outputObservations.getDimension();
  if (model.getOutputDimension() != outputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of output dimension=" << outputDimension << ", got output dimension=" << model.getOutputDimension();
  const UnsignedInteger size = inputObservations.getSize();
  if (outputObservations.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected an output sample of size=" << size << ", got size=" << outputObservations.getSize();
  globalErrorCovariance_ = errorCovariance.getDimension() != outputDimension;
  if (globalErrorCovariance_ && !(errorCovariance.getDimension() == outputDimension * size)) throw InvalidArgumentException(HERE) << "Error: expected an error covariance either of dimension=" << outputDimension << " or dimension=" << outputDimension * size << ", got dimension=" << errorCovariance.getDimension();
  // Now the automatic selection of the algorithm
  algorithm_ = OptimizationAlgorithm::Build(LeastSquaresProblem());
  parameterPrior_.setDescription(model.getParameterDescription());
}

namespace GaussianNonLinearFunctions
{
class CalibrationModelEvaluation: public EvaluationImplementation
{
public:
  CalibrationModelEvaluation(const Function & model,
                             const Sample & inputObservations,
                             const Sample & outputObservations,
                             const Point & candidate,
                             const TriangularMatrix & parameterInverseCholesky,
                             const TriangularMatrix & errorInverseCholesky)
    : EvaluationImplementation()
    , model_(model)
    , inputObservations_(inputObservations)
    , outputObservations_(outputObservations)
    , candidate_(candidate)
    , parameterInverseCholesky_(parameterInverseCholesky)
    , errorInverseCholesky_(errorInverseCholesky)
    , globalErrorInverseCholesky_(errorInverseCholesky.getDimension() != outputObservations_.getDimension())
  {
    // Check if the given input observations are compatible with the model
    if (inputObservations.getDimension() != model.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected input observations of dimension=" << model.getInputDimension() << ", got dimension=" << inputObservations.getDimension();
    // Check if the given parameter Cholesky compatible with the model
    if (parameterInverseCholesky.getDimension() != model.getParameterDimension()) throw InvalidArgumentException(HERE) << "Error: expected parameter inverse Cholesky of dimension=" << model.getParameterDimension() << ", got dimension=" << parameterInverseCholesky.getDimension();
    // Check if the given output observations are compatible with the model
    if (outputObservations.getDimension() != model.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: expected output observations of dimension=" << model.getOutputDimension() << ", got dimension=" << outputObservations.getDimension();
    // Check if the given error Cholesky is compatible with the model
    if (globalErrorInverseCholesky_ && (errorInverseCholesky.getDimension() != model.getOutputDimension() * outputObservations.getSize())) throw InvalidArgumentException(HERE) << "Error: expected error inverse Cholesky of dimension=" << model.getOutputDimension() << ", got dimension=" << errorInverseCholesky.getDimension();
  }

  CalibrationModelEvaluation * clone() const
  {
    return new CalibrationModelEvaluation(*this);
  }

  Point operator() (const Point & point) const
  {
    Function localModel(model_);
    localModel.setParameter(point);
    const Point residualModel(localModel(inputObservations_).getImplementation()->getData() - outputObservations_.getImplementation()->getData());
    Point result;
    if (globalErrorInverseCholesky_) result = errorInverseCholesky_ * residualModel;
    else result = errorInverseCholesky_.getImplementation()->triangularProd(MatrixImplementation(localModel.getOutputDimension(), inputObservations_.getSize(), Collection<Scalar>(residualModel)));
    result.add(parameterInverseCholesky_ * (point - candidate_));
    return result;
  }

  UnsignedInteger getInputDimension() const
  {
    return model_.getParameterDimension();
  }

  UnsignedInteger getOutputDimension() const
  {
    return model_.getOutputDimension() * inputObservations_.getSize() + getInputDimension();
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
    outputDescription.add(getInputDescription());
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

  Point getCandidate() const
  {
    return candidate_;
  }

  TriangularMatrix getParameterInverseCholesky() const
  {
    return parameterInverseCholesky_;
  }

  TriangularMatrix getErrorInverseCholesky() const
  {
    return errorInverseCholesky_;
  }

  Bool getGlobalErrorInverseCholesky() const
  {
    return globalErrorInverseCholesky_;
  }

private:
  const Function model_;
  const Sample inputObservations_;
  const Sample outputObservations_;
  const Point candidate_;
  const TriangularMatrix parameterInverseCholesky_;
  const TriangularMatrix errorInverseCholesky_;
  const Bool globalErrorInverseCholesky_;
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
    const UnsignedInteger parameterDimension = point.getDimension();
    const UnsignedInteger outputDimension = evaluation_.getModel().getOutputDimension();
    Function parametrizedModel(evaluation_.getModel());
    parametrizedModel.setParameter(point);
    const Sample inputObservations(evaluation_.getInputObservations());
    const UnsignedInteger size = inputObservations.getSize();
    const UnsignedInteger fullDimension = size * outputDimension;
    MatrixImplementation gradientObservations(parameterDimension, fullDimension + parameterDimension);
    const TriangularMatrix parameterInverseCholesky(evaluation_.getParameterInverseCholesky().transpose());
    const MatrixImplementation errorInverseCholesky(*evaluation_.getErrorInverseCholesky().getImplementation());
    if (evaluation_.getGlobalErrorInverseCholesky())
    {
      MatrixImplementation fullParameterGradient(parameterDimension, fullDimension);
      UnsignedInteger skip = parameterDimension * outputDimension;
      UnsignedInteger shift = 0;
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        const MatrixImplementation parameterGradient(*parametrizedModel.parameterGradient(inputObservations[i]).getImplementation());
        std::copy(parameterGradient.begin(), parameterGradient.end(), fullParameterGradient.begin() + shift);
        shift += skip;
      }
      fullParameterGradient = fullParameterGradient.genProd(errorInverseCholesky, false, true);
      std::copy(fullParameterGradient.begin(), fullParameterGradient.end(), gradientObservations.begin());
    } // evaluation_.getGlobalErrorInverseCholesky()
    else
    {
      const UnsignedInteger skip = parameterDimension * outputDimension;
      UnsignedInteger shift = 0;
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        const MatrixImplementation parameterGradient(*parametrizedModel.parameterGradient(inputObservations[i]).getImplementation());
        const MatrixImplementation scaledParameterGradient(parameterGradient.genProd(errorInverseCholesky, false, true));
        std::copy(scaledParameterGradient.begin(), scaledParameterGradient.end(), gradientObservations.begin() + shift);
        shift += skip;
      }
    } // !evaluation_.getGlobalErrorInverseCholesky()
    for (UnsignedInteger j = 0; j < parameterDimension; ++j)
      for (UnsignedInteger i = 0; i <= j; ++i)
        gradientObservations(i, fullDimension + j) = parameterInverseCholesky(i, j);
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

/* Performs the actual computation. Must be overloaded by the actual calibration algorithm */
void GaussianNonLinearCalibration::run()
{
  // Compute the posterior MAP
  // Error distribution
  // It is built in two steps to benefit from the Cholesky factorization of the
  // error covariance in the computation of thetaStar
  Normal error(Point(errorCovariance_.getDimension()), errorCovariance_);
  const TriangularMatrix parameterInverseCholesky(getParameterPrior().getInverseCholesky());
  const TriangularMatrix errorInverseCholesky(error.getInverseCholesky());
  const Point thetaStar(run(inputObservations_, outputObservations_, getCandidate(), parameterInverseCholesky, errorInverseCholesky));
  // Build the residual function this way to benefit from the automatic Hessian
  const MemoizeFunction residualFunction(NonLinearLeastSquaresCalibration::BuildResidualFunction(model_, inputObservations_, outputObservations_));
  const Point residuals(residualFunction(thetaStar));
  if (globalErrorCovariance_)
    error.setMean(residuals);
  else
  {
    SampleImplementation residualsAsSample(outputObservations_.getSize(), outputObservations_.getDimension());
    residualsAsSample.setData(residuals);
    error.setMean(residualsAsSample.computeMean());
  }
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
      thetaSample[i] = run(joinedSample.getMarginal(inputIndices), joinedSample.getMarginal(outputIndices), thetaStar, parameterInverseCholesky, errorInverseCholesky);
    }
    parameterPosterior = KernelSmoothing().build(thetaSample);
  }
  else
  {
    GaussianLinearCalibration algo(model_, inputObservations_, outputObservations_, thetaStar, getParameterPrior().getCovariance(), error.getCovariance());
    algo.run();
    parameterPosterior = algo.getResult().getParameterPosterior();
  }
  parameterPosterior.setDescription(parameterPrior_.getDescription());
  result_ = CalibrationResult(parameterPrior_, parameterPosterior, thetaStar, error, inputObservations_, outputObservations_, residualFunction);
  computeOutputAtPriorAndPosterior();
}

/* Perform a unique estimation */
Point GaussianNonLinearCalibration::run(const Sample & inputObservations,
                                        const Sample & outputObservations,
                                        const Point & candidate,
                                        const TriangularMatrix & parameterInverseCholesky,
                                        const TriangularMatrix & errorInverseCholesky)
{
  // Build the residual function this way to benefit from the automatic Hessian
  const GaussianNonLinearFunctions::CalibrationModelEvaluation residualEvaluation(model_, inputObservations, outputObservations, candidate, parameterInverseCholesky, errorInverseCholesky);
  MemoizeFunction residualFunction(Function(residualEvaluation, GaussianNonLinearFunctions::CalibrationModelGradient(residualEvaluation), CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), residualEvaluation)));
  LeastSquaresProblem problem(residualFunction);
  algorithm_.setVerbose(true);
  algorithm_.setProblem(problem);
  try
  {
    // If the solver is single start, we can use its setStartingPoint method
    algorithm_.setStartingPoint(getCandidate());
  }
  catch (NotDefinedException &) // setStartingPoint is not defined for the solver
  {
    LOGWARN(OSS() << "Candidate=" << getCandidate() << " is ignored because algorithm "
                  << algorithm_.getImplementation()->getClassName() << " has no setStartingPoint method.");
  }
  algorithm_.run();
  const Point thetaStar(algorithm_.getResult().getOptimalPoint());
  return thetaStar;
}

/* Candidate accessor */
Point GaussianNonLinearCalibration::getCandidate() const
{
  // The candidate is stored in the prior distribution, which is a Normal distribution
  return getParameterPrior().getMean();
}

/* Parameter covariance accessor */
CovarianceMatrix GaussianNonLinearCalibration::getParameterCovariance() const
{
  // The parameter covariance is stored in the prior distribution, which is a Normal distribution
  return getParameterPrior().getCovariance();
}

/* Error covariance accessor */
CovarianceMatrix GaussianNonLinearCalibration::getErrorCovariance() const
{
  return errorCovariance_;
}

/* Global error covariance accessor */
Bool GaussianNonLinearCalibration::getGlobalErrorCovariance() const
{
  return globalErrorCovariance_;
}

/* Algorithm accessor */
OptimizationAlgorithm GaussianNonLinearCalibration::getOptimizationAlgorithm() const
{
  return algorithm_;
}

void GaussianNonLinearCalibration::setOptimizationAlgorithm(const OptimizationAlgorithm & algorithm)
{
  algorithm_ = algorithm;
}

/* Algorithm accessor */
UnsignedInteger GaussianNonLinearCalibration::getBootstrapSize() const
{
  return bootstrapSize_;
}

void GaussianNonLinearCalibration::setBootstrapSize(const UnsignedInteger bootstrapSize)
{
  bootstrapSize_ = bootstrapSize;
}

/* String converter */
String GaussianNonLinearCalibration::__repr__() const
{
  return OSS() << "class=" << GaussianNonLinearCalibration::GetClassName()
         << " name=" << getName();
}


GaussianNonLinearCalibration * GaussianNonLinearCalibration::clone() const
{
  return new GaussianNonLinearCalibration(*this);
}


/* Method save() stores the object through the StorageManager */
void GaussianNonLinearCalibration::save(Advocate & adv) const
{
  CalibrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("algorithm_", algorithm_);
  adv.saveAttribute("bootstrapSize_", bootstrapSize_);
}

/* Method load() reloads the object from the StorageManager */
void GaussianNonLinearCalibration::load(Advocate & adv)
{
  CalibrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("algorithm_", algorithm_);
  adv.loadAttribute("bootstrapSize_", bootstrapSize_);
}


END_NAMESPACE_OPENTURNS
