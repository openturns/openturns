//                                               -*- C++ -*-
/**
 *  @brief Default GaussianLinearCalibration
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
#include "openturns/GaussianLinearCalibration.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/Normal.hxx"
#include "openturns/LinearFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(GaussianLinearCalibration)

static const Factory<GaussianLinearCalibration> Factory_GaussianLinearCalibration;

/* Default constructor */
GaussianLinearCalibration::GaussianLinearCalibration()
  : CalibrationAlgorithmImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
GaussianLinearCalibration::GaussianLinearCalibration(const Function & model,
           const Sample & inputObservations,
           const Sample & outputObservations,
           const Point & candidate,
           const CovarianceMatrix & parameterCovariance,
           const CovarianceMatrix & errorCovariance,
           const String & methodName)
  : CalibrationAlgorithmImplementation(outputObservations, Normal(candidate, parameterCovariance))
  , modelObservations_(0, 0)
  , gradientObservations_(0, 0)
  , errorCovariance_(errorCovariance)
  , globalErrorCovariance_(false)
  , methodName_(methodName)
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
  // Compute the linearization
  Function parametrizedModel(model);
  parametrizedModel.setParameter(candidate);
  // Flatten everything related to the model evaluations over the input observations
  modelObservations_ = parametrizedModel(inputObservations);
  MatrixImplementation transposedGradientObservations(parameterDimension, size * outputDimension);
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Matrix parameterGradient(parametrizedModel.parameterGradient(inputObservations[i]));
    std::copy(parameterGradient.getImplementation()->begin(), parameterGradient.getImplementation()->end(), transposedGradientObservations.begin() + shift);
    shift += parameterDimension * outputDimension;
  }
  gradientObservations_ = transposedGradientObservations.transpose();
  parameterPrior_.setDescription(model.getParameterDescription());
}

/* Parameter constructor */
GaussianLinearCalibration::GaussianLinearCalibration(const Sample & modelObservations,
           const Matrix & gradientObservations,
           const Sample & outputObservations,
           const Point & candidate,
           const CovarianceMatrix & parameterCovariance,
           const CovarianceMatrix & errorCovariance,
           const String & methodName)
  : CalibrationAlgorithmImplementation(outputObservations, Normal(candidate, parameterCovariance))
  , modelObservations_(modelObservations)
  , gradientObservations_(gradientObservations)
  , errorCovariance_(errorCovariance)
  , globalErrorCovariance_(false)
  , methodName_(methodName)
{
  // Check the input
  // Check the input
  const UnsignedInteger parameterDimension = candidate.getDimension();
  if (parameterCovariance.getDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a parameter covariance of dimension=" << parameterDimension << ", got dimension=" << parameterCovariance.getDimension();
  const UnsignedInteger outputDimension = outputObservations.getDimension();
  const UnsignedInteger size = outputObservations.getSize();
  if (gradientObservations.getNbColumns() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a gradient parameter of columns number=" << parameterDimension << ", got columns number=" << gradientObservations.getNbColumns();
  if (gradientObservations.getNbRows() != size * outputDimension) throw InvalidArgumentException(HERE) << "Error: expected a gradient parameter of rows number=" << size * outputDimension << ", got rows number=" << gradientObservations.getNbRows();
  globalErrorCovariance_ = errorCovariance.getDimension() != outputDimension;
  if (globalErrorCovariance_ && !(errorCovariance.getDimension() == outputDimension * size)) throw InvalidArgumentException(HERE) << "Error: expected an error covariance either of dimension=" << outputDimension << " or dimension=" << outputDimension * size << ", got dimension=" << errorCovariance.getDimension();
}

/* Performs the actual computation. Must be overloaded by the actual calibration algorithm */
void GaussianLinearCalibration::run()
{
  // Compute the difference of output observations and output predictions
  const Point deltaY(modelObservations_.getImplementation()->getData() - outputObservations_.getImplementation()->getData());
  // Compute covariance matrix B of the parameter and its Cholesky decomposition
  CovarianceMatrix B(getParameterCovariance());
  const TriangularMatrix LB(B.computeCholesky());
  // Compute the covariance matrix R
  CovarianceMatrix R(deltaY.getSize());
  const UnsignedInteger dimension = errorCovariance_.getDimension();
  const UnsignedInteger size = outputObservations_.getSize();
  if (globalErrorCovariance_) R = errorCovariance_;
  else
    {
      if (dimension == 1) R = (R * errorCovariance_(0, 0)).getImplementation();
      else
	{
	  for (UnsignedInteger i = 0; i < size; ++i)
	    for (UnsignedInteger j = 0; j < dimension; ++j)
	      for (UnsignedInteger k = 0; k < dimension; ++k)
		R(i * dimension + j, i * dimension + k) = errorCovariance_(j, k);
	}
    }
  // Compute the Cholesky decomposition of R
  const TriangularMatrix LR(R.computeCholesky());
  // Compute inv(LB), the first part of the extended design matrix
  const IdentityMatrix ILB(LB.getDimension());
  const Matrix invLB(LB.solveLinearSystem(ILB).getImplementation());
  // Compute inv(LR)*J, the second part of the extended design matrix
  const CovarianceMatrix invLRJ(LR.solveLinearSystem(gradientObservations_).getImplementation());
  // Create the extended design matrix of the linear least squares problem
  const UnsignedInteger parameterDimension = B.getDimension();
  Matrix Abar(size+parameterDimension,parameterDimension);
  for (UnsignedInteger i = 0; i < parameterDimension; ++i)
    for (UnsignedInteger j = 0; j < parameterDimension; ++j)
      Abar(i,j) = invLB(i,j)
  for (UnsignedInteger i = parameterDimension; i < size + parameterDimension; ++i)
    for (UnsignedInteger j = 0; j < parameterDimension; ++j)
      Abar(i,j) = -invLRJ(i,j)
  // Compute inv(LR)*deltay, the right hand size of the extended residual
  const Point invLRz(LR.solveLinearSystem(deltaY))
  // Create the right hand side of the extended linear least squares system
  Point ybar(size+parameterDimension)
  for (UnsignedInteger i = parameterDimension; i < size + parameterDimension; ++i)
    ybar(i) = -invLRz(i)
  // Solve the linear least squares problem
  LeastSquaresMethod method(LeastSquaresMethod::Build(methodName_, Abar));
  const Point deltaTheta(method.solve(ybar));
  const Point thetaStar(getCandidate() + deltaTheta);
  covarianceThetaStar = CovarianceMatrix(method.getGramInverse().getImplementation());
  Normal parameterPosterior(thetaStar, covarianceThetaStar);
  parameterPosterior.setDescription(parameterPrior_.getDescription());
  const LinearFunction residualFunction(getCandidate(), deltaY, gradientObservations_);
  result_ = CalibrationResult(parameterPrior_, parameterPosterior, thetaStar, Normal(Point(errorCovariance_.getDimension()), errorCovariance_), outputObservations_, residualFunction);
}

/* Model observations accessor */
Sample GaussianLinearCalibration::getModelObservations() const
{
  return modelObservations_;
}


/* Model gradient wrt the parameter accessor */
Matrix GaussianLinearCalibration::getGradientObservations() const
{
  return gradientObservations_;
}

/* Candidate accessor */
Point GaussianLinearCalibration::getCandidate() const
{
  // The candidate is stored in the prior distribution, which is a Normal distribution
  return getParameterPrior().getMean();
}

/* Parameter covariance accessor */
CovarianceMatrix GaussianLinearCalibration::getParameterCovariance() const
{
  // The parameter covariance is stored in the prior distribution, which is a Normal distribution
  return getParameterPrior().getCovariance();
}

/* Error covariance accessor */
CovarianceMatrix GaussianLinearCalibration::getErrorCovariance() const
{
  return errorCovariance_;
}

/* Global error covariance accessor */
Bool GaussianLinearCalibration::getGlobalErrorCovariance() const
{
  return globalErrorCovariance_;
}

/* Least squares method name accessor */
String GaussianLinearCalibration::getMethodName() const
{
  return methodName_;
}

/* String converter */
String GaussianLinearCalibration::__repr__() const
{
  return OSS() << "class=" << GaussianLinearCalibration::GetClassName()
         << " name=" << getName();
}


GaussianLinearCalibration * GaussianLinearCalibration::clone() const
{
  return new GaussianLinearCalibration(*this);
}


/* Method save() stores the object through the StorageManager */
void GaussianLinearCalibration::save(Advocate & adv) const
{
  CalibrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("modelObservations_", modelObservations_);
  adv.saveAttribute("gradientObservations_", gradientObservations_);
  adv.saveAttribute("errorCovariance_", errorCovariance_);
  adv.saveAttribute("globalErrorCovariance_", globalErrorCovariance_);
  adv.saveAttribute("methodName_", methodName_);
}

/* Method load() reloads the object from the StorageManager */
void GaussianLinearCalibration::load(Advocate & adv)
{
  CalibrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("modelObservations_", modelObservations_);
  adv.loadAttribute("gradientObservations_", gradientObservations_);
  adv.loadAttribute("errorCovariance_", errorCovariance_);
  adv.loadAttribute("globalErrorCovariance_", globalErrorCovariance_);
  adv.loadAttribute("methodName_", methodName_);
}


END_NAMESPACE_OPENTURNS
