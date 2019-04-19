//                                               -*- C++ -*-
/**
 *  @brief Default BLUE
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
#include "openturns/BLUE.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(BLUE)

static const Factory<BLUE> Factory_BLUE;

/* Default constructor */
BLUE::BLUE()
  : CalibrationAlgorithmImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
BLUE::BLUE(const Function & model,
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
}

/* Parameter constructor */
BLUE::BLUE(const Sample & modelObservations,
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
void BLUE::run()
{
  const Point deltaY(outputObservations_.getImplementation()->getData() - modelObservations_.getImplementation()->getData());
  CovarianceMatrix B(getParameterCovariance());
  const IdentityMatrix IB(B.getDimension());
  const CovarianceMatrix invB(B.solveLinearSystem(IB).getImplementation());
  CovarianceMatrix R(deltaY.getSize());
  const UnsignedInteger dimension = errorCovariance_.getDimension();
  if (globalErrorCovariance_) R = errorCovariance_;
  else
    {
      if (dimension == 1) R = (R * errorCovariance_(0, 0)).getImplementation();
      else
	{
	  const UnsignedInteger size = outputObservations_.getSize();
	  for (UnsignedInteger i = 0; i < size; ++i)
	    for (UnsignedInteger j = 0; j < dimension; ++j)
	      for (UnsignedInteger k = 0; k < dimension; ++k)
		R(i * dimension + j, i * dimension + k) = errorCovariance_(j, k);
	}
    }
  const IdentityMatrix IR(R.getDimension());
  const CovarianceMatrix invR(R.solveLinearSystem(IR).getImplementation());
  const Matrix M(gradientObservations_);
  const Matrix MtinvR(M.transpose() * invR);
  const Matrix K((invB + MtinvR * M).solveLinearSystem(MtinvR));
  const Point thetaStar(getCandidate() + K * deltaY);
  const SquareMatrix L((IB - K * M).getImplementation());
  const CovarianceMatrix covarianceThetaStar((K * R * K.transpose() + L * B * L.transpose()).getImplementation());
  result_ = CalibrationResult(parameterPrior_, Normal(thetaStar, covarianceThetaStar), thetaStar, Normal(Point(errorCovariance_.getDimension()), errorCovariance_));
}

/* Model observations accessor */
Sample BLUE::getModelObservations() const
{
  return modelObservations_;
}


/* Model gradient wrt the parameter accessor */
Matrix BLUE::getGradientObservations() const
{
  return gradientObservations_;
}

/* Candidate accessor */
Point BLUE::getCandidate() const
{
  // The candidate is stored in the prior distribution, which is a Normal distribution
  return getParameterPrior().getMean();
}

/* Parameter covariance accessor */
CovarianceMatrix BLUE::getParameterCovariance() const
{
  // The parameter covariance is stored in the prior distribution, which is a Normal distribution
  return getParameterPrior().getCovariance();
}

/* Error covariance accessor */
CovarianceMatrix BLUE::getErrorCovariance() const
{
  return errorCovariance_;
}

/* Global error covariance accessor */
Bool BLUE::getGlobalErrorCovariance() const
{
  return globalErrorCovariance_;
}

/* Least squares method name accessor */
String BLUE::getMethodName() const
{
  return methodName_;
}

/* String converter */
String BLUE::__repr__() const
{
  return OSS() << "class=" << BLUE::GetClassName()
         << " name=" << getName();
}


BLUE * BLUE::clone() const
{
  return new BLUE(*this);
}


/* Method save() stores the object through the StorageManager */
void BLUE::save(Advocate & adv) const
{
  CalibrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("modelObservations_", modelObservations_);
  adv.saveAttribute("gradientObservations_", gradientObservations_);
  adv.saveAttribute("errorCovariance_", errorCovariance_);
  adv.saveAttribute("globalErrorCovariance_", globalErrorCovariance_);
  adv.saveAttribute("methodName_", methodName_);
}

/* Method load() reloads the object from the StorageManager */
void BLUE::load(Advocate & adv)
{
  CalibrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("modelObservations_", modelObservations_);
  adv.loadAttribute("gradientObservations_", gradientObservations_);
  adv.loadAttribute("errorCovariance_", errorCovariance_);
  adv.loadAttribute("globalErrorCovariance_", globalErrorCovariance_);
  adv.loadAttribute("methodName_", methodName_);
}


END_NAMESPACE_OPENTURNS
