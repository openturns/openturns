//                                               -*- C++ -*-
/**
 *  @brief Default ThreeDVAR
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/ThreeDVAR.hxx"
#include "openturns/BLUE.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Dirac.hxx"
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
#include "openturns/CMinpack.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ThreeDVAR)

static const Factory<ThreeDVAR> Factory_ThreeDVAR;

/* Default constructor */
ThreeDVAR::ThreeDVAR()
  : CalibrationAlgorithmImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
ThreeDVAR::ThreeDVAR(const Function & model,
                     const Sample & inputObservations,
                     const Sample & outputObservations,
                     const Point & candidate,
                     const CovarianceMatrix & parameterCovariance,
                     const CovarianceMatrix & errorCovariance)
  : CalibrationAlgorithmImplementation(outputObservations, Normal(candidate, parameterCovariance))
  , model_(model)
  , inputObservations_(inputObservations)
  , algorithm_()
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("ThreeDVAR-BootstrapSize"))
  , errorCovariance_(errorCovariance)
{
  // Check the input
  const UnsignedInteger parameterDimension = candidate.getDimension();
  if (model.getParameterDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of parameter dimension=" << parameterDimension << ", got parameter dimension=" << model.getParameterDimension();
  if (parameterCovariance.getDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a parameter covariance of dimension=" << parameterDimension << ", got dimension=" << parameterCovariance.getDimension();
  const UnsignedInteger inputDimension = inputObservations.getDimension();
  if (model.getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of input dimension=" << inputDimension << ", got input dimension=" << model.getInputDimension();
  const UnsignedInteger outputDimension = outputObservations.getDimension();
  if (model.getOutputDimension() != outputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of output dimension=" << outputDimension << ", got output dimension=" << model.getOutputDimension();
  if (errorCovariance.getDimension() != outputDimension) throw InvalidArgumentException(HERE) << "Error: expected an error covariance of dimension=" << outputDimension << ", got dimension=" << errorCovariance.getDimension();
  const UnsignedInteger size = inputObservations.getSize();
  if (outputObservations.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected an output sample of size=" << size << ", got size=" << outputObservations.getSize();
  // Now the automatic selection of the algorithm
#ifdef OPENTURNS_HAVE_CMINPACK
  algorithm_ = CMinpack();
#else
  algorithm_ = MultiStart(TNC(), LowDiscrepancyExperiment(SobolSequence(), Normal(candidate, CovarianceMatrix(candidate.getDimension())), ResourceMap::GetAsUnsignedInteger("NonLinearLeastSquaresCalibration-MultiStartSize")).generate());
#endif
}

namespace ThreeDVARFunctions
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
    {
      // Check if the given input observations are compatible with the model
      if (inputObservations.getDimension() != model.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected input observations of dimension=" << model.getInputDimension() << ", got dimension=" << inputObservations.getDimension();
      // Check if the given parameter Cholesky compatible with the model
      if (parameterInverseCholesky.getDimension() != model.getParameterDimension()) throw InvalidArgumentException(HERE) << "Error: expected parameter inverse Cholesky of dimension=" << model.getParameterDimension() << ", got dimension=" << parameterInverseCholesky.getDimension();
      // Check if the given output observations are compatible with the model
      if (outputObservations.getDimension() != model.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: expected output observations of dimension=" << model.getOutputDimension() << ", got dimension=" << outputObservations.getDimension();
      // Check if the given error Cholesky is compatible with the model
      if (errorInverseCholesky.getDimension() != model.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: expected error inverse Cholesky of dimension=" << model.getOutputDimension() << ", got dimension=" << errorInverseCholesky.getDimension();
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
      Point result(errorInverseCholesky_.getImplementation()->triangularProd(MatrixImplementation(localModel.getOutputDimension(), inputObservations_.getSize(), Collection<Scalar>(residualModel))));
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
    
  private:
    const Function model_;
    const Sample inputObservations_;
    const Sample outputObservations_;
    const Point candidate_;
    const TriangularMatrix parameterInverseCholesky_;
    const TriangularMatrix errorInverseCholesky_;
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
      MatrixImplementation gradientObservations(parameterDimension, size * outputDimension + parameterDimension);
      UnsignedInteger shift = 0;
      const TriangularMatrix parameterInverseCholesky(evaluation_.getParameterInverseCholesky().transpose());
      const TriangularMatrix errorInverseCholesky(evaluation_.getErrorInverseCholesky().transpose());
      const UnsignedInteger skip = parameterDimension * outputDimension;
      for (UnsignedInteger i = 0; i < size; ++i)
	{
	  const Matrix parameterGradient(parametrizedModel.parameterGradient(inputObservations[i]));
	  const Matrix scaledParameterGradient(parameterGradient * errorInverseCholesky);
	  std::copy(scaledParameterGradient.getImplementation()->begin(), scaledParameterGradient.getImplementation()->end(), gradientObservations.begin() + shift);
	  shift += skip;
	}
      for (UnsignedInteger j = 0; j < parameterDimension; ++j)
	for (UnsignedInteger i = 0; i <= j; ++i)
	  gradientObservations(i, size * outputDimension + j) = parameterInverseCholesky(i, j);
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
void ThreeDVAR::run()
{
  // Compute the posterior MAP
  // Error distribution
  const Normal error(Point(errorCovariance_.getDimension()), errorCovariance_);
  const TriangularMatrix parameterInverseCholesky(getParameterPrior().getInverseCholesky());
  const TriangularMatrix errorInverseCholesky(error.getInverseCholesky());
  const Point thetaStar(run(inputObservations_, outputObservations_, getCandidate(), parameterInverseCholesky, errorInverseCholesky));
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
      BLUE blueAlgo(model_, inputObservations_, outputObservations_, thetaStar, getParameterPrior().getCovariance(), error.getCovariance());
      blueAlgo.run();
      parameterPosterior = blueAlgo.getResult().getParameterPosterior();
    }
  result_ = CalibrationResult(parameterPrior_, parameterPosterior, thetaStar, error);
}

/* Perform a unique estimation */
Point ThreeDVAR::run(const Sample & inputObservations,
                     const Sample & outputObservations,
                     const Point & candidate,
		     const TriangularMatrix & parameterInverseCholesky,
		     const TriangularMatrix & errorInverseCholesky)
{
  // Build the residual function this way to benefit from the automatic Hessian
  const ThreeDVARFunctions::CalibrationModelEvaluation residualEvaluation(model_, inputObservations, outputObservations, candidate, parameterInverseCholesky, errorInverseCholesky);
  
  // Build the function in two steps, in order to benefit from the automatic Hessian
  MemoizeFunction residualFunction(Function(residualEvaluation, ThreeDVARFunctions::CalibrationModelGradient(residualEvaluation), CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), residualEvaluation)));
  LeastSquaresProblem problem(residualFunction);
  algorithm_.setVerbose(true);
  algorithm_.setProblem(problem);
  algorithm_.setStartingPoint(getCandidate());
  algorithm_.run();
  const Point thetaStar(algorithm_.getResult().getOptimalPoint());
  return thetaStar;
}

/* Candidate accessor */
Point ThreeDVAR::getCandidate() const
{
  // The candidate is stored in the prior distribution, which is a Normal distribution
  return getParameterPrior().getMean();
}

/* Parameter covariance accessor */
CovarianceMatrix ThreeDVAR::getParameterCovariance() const
{
  // The parameter covariance is stored in the prior distribution, which is a Normal distribution
  return getParameterPrior().getCovariance();
}

/* Error covariance accessor */
CovarianceMatrix ThreeDVAR::getErrorCovariance() const
{
  return errorCovariance_;
}

/* Algorithm accessor */
OptimizationAlgorithm ThreeDVAR::getAlgorithm() const
{
  return algorithm_;
}

void ThreeDVAR::setAlgorithm(const OptimizationAlgorithm & algorithm)
{
  algorithm_ = algorithm;
}

/* Algorithm accessor */
UnsignedInteger ThreeDVAR::getBootstrapSize() const
{
  return bootstrapSize_;
}

void ThreeDVAR::setBootstrapSize(const UnsignedInteger bootstrapSize)
{
  bootstrapSize_ = bootstrapSize;
}

/* String converter */
String ThreeDVAR::__repr__() const
{
  return OSS() << "class=" << ThreeDVAR::GetClassName()
         << " name=" << getName();
}


ThreeDVAR * ThreeDVAR::clone() const
{
  return new ThreeDVAR(*this);
}


/* Method save() stores the object through the StorageManager */
void ThreeDVAR::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("model_", model_);
  adv.saveAttribute("inputObservations_", inputObservations_);
  adv.saveAttribute("algorithm_", algorithm_);
  adv.saveAttribute("bootstrapSize_", bootstrapSize_);
}

/* Method load() reloads the object from the StorageManager */
void ThreeDVAR::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("model_", model_);
  adv.loadAttribute("inputObservations_", inputObservations_);
  adv.loadAttribute("algorithm_", algorithm_);
  adv.loadAttribute("bootstrapSize_", bootstrapSize_);
}


END_NAMESPACE_OPENTURNS
