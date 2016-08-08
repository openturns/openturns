//                                               -*- C++ -*-
/**
 *  @brief The result of a linear model estimation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/GeneralizedLinearModelResult.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/TemporalNormalProcess.hxx"
#include "openturns/WhiteNoise.hxx"
#include "openturns/Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedLinearModelResult);
static const Factory<GeneralizedLinearModelResult> Factory_GeneralizedLinearModelResult;

/* Default constructor */
GeneralizedLinearModelResult::GeneralizedLinearModelResult()
  : MetaModelResult()
{
  // Nothing to do
}

/* Constructor with parameters & Cholesky factor */
GeneralizedLinearModelResult::GeneralizedLinearModelResult(const NumericalSample & inputSample,
                                                           const NumericalSample & outputSample,
                                                           const NumericalMathFunction & metaModel,
                                                           const NumericalPoint & residuals,
                                                           const NumericalPoint & relativeErrors,
                                                           const BasisCollection & basis,
                                                           const NumericalPointCollection & trendCoefficients,
                                                           const CovarianceModel & covarianceModel)
  : MetaModelResult(NumericalMathFunction(inputSample, outputSample), metaModel, residuals, relativeErrors)
  , inputData_(inputSample)
  , inputTransformedData_(inputSample)
  , inputTransformation_()
  , hasTransformation_(false)
  , basis_(basis)
  , beta_(trendCoefficients)
  , covarianceModel_(covarianceModel)
  , hasCholeskyFactor_(false)
  , covarianceCholeskyFactor_()
  , covarianceHMatrix_()
{
  const UnsignedInteger size = inputSample.getSize();
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelResult::GeneralizedLinearModelResult, input & output sample have different size. input sample size = " << size << ", output sample size = " << outputSample.getSize();
}


/* Constructor with parameters & Cholesky factor */
GeneralizedLinearModelResult::GeneralizedLinearModelResult(const NumericalSample & inputSample,
                                                           const NumericalSample & outputSample,
                                                           const NumericalMathFunction & metaModel,
                                                           const NumericalPoint & residuals,
                                                           const NumericalPoint & relativeErrors,
                                                           const BasisCollection & basis,
                                                           const NumericalPointCollection & trendCoefficients,
                                                           const CovarianceModel & covarianceModel,
                                                           const TriangularMatrix & covarianceCholeskyFactor,
                                                           const HMatrix & covarianceHMatrix)
  : MetaModelResult(NumericalMathFunction(inputSample, outputSample), metaModel, residuals, relativeErrors)
  , inputData_(inputSample)
  , inputTransformedData_(inputSample)
  , inputTransformation_()
  , hasTransformation_(false)
  , basis_(basis)
  , beta_(trendCoefficients)
  , covarianceModel_(covarianceModel)
  , hasCholeskyFactor_(true)
  , covarianceCholeskyFactor_(covarianceCholeskyFactor)
  , covarianceHMatrix_(covarianceHMatrix)
{
  const UnsignedInteger size = inputSample.getSize();
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelResult::GeneralizedLinearModelResult, input & output sample have different size. input sample size = " << size << ", output sample size = " << outputSample.getSize();
  const UnsignedInteger outputDimension = outputSample.getDimension();
  if (covarianceCholeskyFactor_.getDimension() != 0 && covarianceCholeskyFactor_.getDimension() != size * outputDimension)
    throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelResult::GeneralizedLinearModelResult, Cholesky factor has unexpected dimensions. Its dimension should be " << size * outputDimension << ". Here dimension = " << covarianceCholeskyFactor_.getDimension();
  if (covarianceHMatrix_.getNbRows() != 0)
  {
    if (covarianceHMatrix_.getNbRows() != covarianceHMatrix_.getNbColumns())
      throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelResult::GeneralizedLinearModelResult, HMAT Cholesky factor is not square. Its dimension is " << covarianceHMatrix_.getNbRows() << "x" << covarianceHMatrix_.getNbColumns();
    if (covarianceHMatrix_.getNbRows() != size * outputDimension)
      throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelResult::GeneralizedLinearModelResult, HMAT Cholesky factor has unexpected dimensions. Its dimension should be " << size * outputDimension << ". Here dimension = " << covarianceHMatrix_.getNbRows();
  }
}


/* Virtual constructor */
GeneralizedLinearModelResult * GeneralizedLinearModelResult::clone() const
{
  return new GeneralizedLinearModelResult(*this);
}


/* String converter */
String GeneralizedLinearModelResult::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
                   << ", covariance models=" << covarianceModel_
                   << ", basis=" << basis_
                   << ", trend coefficients=" << beta_;
}

String GeneralizedLinearModelResult::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "("
      << "covariance models=" << covarianceModel_.__str__(offset)
      << ", basis=" << basis_.__str__(offset)
      << ", trend coefficients=" << beta_.__str__(offset) << ")";
  return oss;
}

/* Basis accessor */
GeneralizedLinearModelResult::BasisCollection GeneralizedLinearModelResult::getBasisCollection() const
{
  return basis_;
}

/* Trend coefficients accessor */
GeneralizedLinearModelResult::NumericalPointCollection GeneralizedLinearModelResult::getTrendCoefficients() const
{
  return beta_;
}

/* Covariance models accessor */
CovarianceModel GeneralizedLinearModelResult::getCovarianceModel() const
{
  return covarianceModel_;
}

NumericalMathFunction GeneralizedLinearModelResult::getTransformation() const
{
  return inputTransformation_;
}

void GeneralizedLinearModelResult::setTransformation(const NumericalMathFunction & transformation)
{
  if (transformation.getInputDimension() != inputData_.getDimension())
    throw InvalidArgumentException(HERE) << "In KrigingResult::setTransformation, incompatible function dimension. Function should have input dimension = " << inputData_.getDimension() << ". Here, function's input dimension = " << transformation.getInputDimension();
  inputTransformation_ = transformation;
  // Map inputData using the transformation
  inputTransformedData_ = transformation(inputData_);
  hasTransformation_ = true;
}

/** process accessor */
Process GeneralizedLinearModelResult::getNoise() const
{
  // Define noise process
  if (covarianceModel_.getClassName() == "DiracCovarianceModel")
  {
    //
    const NumericalPoint sigma = covarianceModel_.getParameter();
    const CorrelationMatrix R = covarianceModel_.getSpatialCorrelation();
    const Normal dist(NumericalPoint(sigma.getSize(), 0.0), sigma, R);
    WhiteNoise noise(dist);
    return noise;
  }
  // Other covariance models
  const TemporalNormalProcess noise(covarianceModel_, Mesh(inputTransformedData_));
  return noise;
}

/** Method that returns the covariance factor - lapack */
TriangularMatrix GeneralizedLinearModelResult::getCholeskyFactor() const
{
  return covarianceCholeskyFactor_;
}

/** Method that returns the covariance factor - hmat */
HMatrix GeneralizedLinearModelResult::getHMatCholeskyFactor() const
{
  return covarianceHMatrix_;
}

// Return input sample transformed
NumericalSample GeneralizedLinearModelResult::getInputTransformedSample() const
{
  return inputTransformedData_;
}

/* Method save() stores the object through the StorageManager */
void GeneralizedLinearModelResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute( "inputData_", inputData_ );
  adv.saveAttribute( "inputTransformedData_", inputTransformedData_ );
  adv.saveAttribute( "inputTransformation_", inputTransformation_ );
  adv.saveAttribute( "hasTransformation_", hasTransformation_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "covarianceModel_", covarianceModel_ );
  adv.saveAttribute( "hasCholeskyFactor_", hasCholeskyFactor_);
  adv.saveAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_);
}


/* Method load() reloads the object from the StorageManager */
void GeneralizedLinearModelResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute( "inputData_", inputData_ );
  adv.loadAttribute( "inputTransformedData_", inputTransformedData_ );
  adv.loadAttribute( "inputTransformation_", inputTransformation_ );
  adv.loadAttribute( "hasTransformation_", hasTransformation_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "beta_", beta_ );
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
  adv.loadAttribute( "hasCholeskyFactor_", hasCholeskyFactor_);
  adv.loadAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_);
}



END_NAMESPACE_OPENTURNS
