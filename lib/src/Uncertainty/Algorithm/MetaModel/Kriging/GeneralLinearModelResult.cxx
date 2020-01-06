//                                               -*- C++ -*-
/**
 *  @brief The result of a linear model estimation
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/GeneralLinearModelResult.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/GaussianProcess.hxx"
#include "openturns/WhiteNoise.hxx"
#include "openturns/Normal.hxx"
#include "openturns/DatabaseFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralLinearModelResult)
static const Factory<GeneralLinearModelResult> Factory_GeneralLinearModelResult;

/* Default constructor */
GeneralLinearModelResult::GeneralLinearModelResult()
  : MetaModelResult()
{
  // Nothing to do
}

/* Constructor with parameters & Cholesky factor */
GeneralLinearModelResult::GeneralLinearModelResult(const Sample & inputSample,
    const Sample & outputSample,
    const Function & metaModel,
    const Point & residuals,
    const Point & relativeErrors,
    const BasisCollection & basis,
    const PointCollection & trendCoefficients,
    const CovarianceModel & covarianceModel,
    const Scalar optimalLogLikelihood)
  : MetaModelResult(DatabaseFunction(inputSample, outputSample), metaModel, residuals, relativeErrors)
  , inputData_(inputSample)
  , inputTransformedData_(inputSample)
  , inputTransformation_()
  , hasTransformation_(false)
  , basis_(basis)
  , beta_(trendCoefficients)
  , covarianceModel_(covarianceModel)
  , optimalLogLikelihood_(optimalLogLikelihood)
  , hasCholeskyFactor_(false)
  , covarianceCholeskyFactor_()
  , covarianceHMatrix_()
{
  const UnsignedInteger size = inputSample.getSize();
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In GeneralLinearModelResult::GeneralLinearModelResult, input & output sample have different size. input sample size = " << size << ", output sample size = " << outputSample.getSize();
}


/* Constructor with parameters & Cholesky factor */
GeneralLinearModelResult::GeneralLinearModelResult(const Sample & inputSample,
    const Sample & outputSample,
    const Function & metaModel,
    const Point & residuals,
    const Point & relativeErrors,
    const BasisCollection & basis,
    const PointCollection & trendCoefficients,
    const CovarianceModel & covarianceModel,
    const Scalar optimalLogLikelihood,
    const TriangularMatrix & covarianceCholeskyFactor,
    const HMatrix & covarianceHMatrix)
  : MetaModelResult(DatabaseFunction(inputSample, outputSample), metaModel, residuals, relativeErrors)
  , inputData_(inputSample)
  , inputTransformedData_(inputSample)
  , inputTransformation_()
  , hasTransformation_(false)
  , basis_(basis)
  , beta_(trendCoefficients)
  , covarianceModel_(covarianceModel)
  , optimalLogLikelihood_(optimalLogLikelihood)
  , hasCholeskyFactor_(true)
  , covarianceCholeskyFactor_(covarianceCholeskyFactor)
  , covarianceHMatrix_(covarianceHMatrix)
{
  const UnsignedInteger size = inputSample.getSize();
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In GeneralLinearModelResult::GeneralLinearModelResult, input & output sample have different size. input sample size = " << size << ", output sample size = " << outputSample.getSize();
  const UnsignedInteger outputDimension = outputSample.getDimension();
  if (covarianceCholeskyFactor_.getDimension() != 0 && covarianceCholeskyFactor_.getDimension() != size * outputDimension)
    throw InvalidArgumentException(HERE) << "In GeneralLinearModelResult::GeneralLinearModelResult, Cholesky factor has unexpected dimensions. Its dimension should be " << size * outputDimension << ". Here dimension = " << covarianceCholeskyFactor_.getDimension();
  if (covarianceHMatrix_.getNbRows() != 0)
  {
    if (covarianceHMatrix_.getNbRows() != covarianceHMatrix_.getNbColumns())
      throw InvalidArgumentException(HERE) << "In GeneralLinearModelResult::GeneralLinearModelResult, HMAT Cholesky factor is not square. Its dimension is " << covarianceHMatrix_.getNbRows() << "x" << covarianceHMatrix_.getNbColumns();
    if (covarianceHMatrix_.getNbRows() != size * outputDimension)
      throw InvalidArgumentException(HERE) << "In GeneralLinearModelResult::GeneralLinearModelResult, HMAT Cholesky factor has unexpected dimensions. Its dimension should be " << size * outputDimension << ". Here dimension = " << covarianceHMatrix_.getNbRows();
  }
}


/* Virtual constructor */
GeneralLinearModelResult * GeneralLinearModelResult::clone() const
{
  return new GeneralLinearModelResult(*this);
}


/* String converter */
String GeneralLinearModelResult::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << ", covariance models=" << covarianceModel_
         << ", basis=" << basis_
         << ", trend coefficients=" << beta_;
}

String GeneralLinearModelResult::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "("
      << "covariance models=" << covarianceModel_.__str__(offset)
      << ", basis=" << basis_.__str__(offset)
      << ", trend coefficients=" << beta_.__str__(offset) << ")";
  return oss;
}

/* Basis accessor */
GeneralLinearModelResult::BasisCollection GeneralLinearModelResult::getBasisCollection() const
{
  return basis_;
}

/* Trend coefficients accessor */
GeneralLinearModelResult::PointCollection GeneralLinearModelResult::getTrendCoefficients() const
{
  return beta_;
}

/* Covariance models accessor */
CovarianceModel GeneralLinearModelResult::getCovarianceModel() const
{
  return covarianceModel_;
}

Function GeneralLinearModelResult::getTransformation() const
{
  return inputTransformation_;
}

void GeneralLinearModelResult::setTransformation(const Function & transformation)
{
  if (transformation.getInputDimension() != inputData_.getDimension())
    throw InvalidArgumentException(HERE) << "In KrigingResult::setTransformation, incompatible function dimension. Function should have input dimension = " << inputData_.getDimension() << ". Here, function's input dimension = " << transformation.getInputDimension();
  inputTransformation_ = transformation;
  // Map inputData using the transformation
  inputTransformedData_ = transformation(inputData_);
  hasTransformation_ = true;
}

/* Optimal log-likelihood accessor */
Scalar GeneralLinearModelResult::getOptimalLogLikelihood() const
{
  return optimalLogLikelihood_;
}

/* process accessor */
Process GeneralLinearModelResult::getNoise() const
{
  // Define noise process
  if (covarianceModel_.getClassName() == "DiracCovarianceModel")
  {
    // Here it is assumed that the covariance model parameters are the
    // marginal amplitude.
    const Point sigma(covarianceModel_.getParameter());
    const CorrelationMatrix R(covarianceModel_.getOutputCorrelation());
    const Normal dist(Point(sigma.getSize(), 0.0), sigma, R);
    WhiteNoise noise(dist);
    return noise;
  }
  // Other covariance models
  const GaussianProcess noise(covarianceModel_, Mesh(inputTransformedData_));
  return noise;
}

/* Method that returns the covariance factor - lapack */
TriangularMatrix GeneralLinearModelResult::getCholeskyFactor() const
{
  return covarianceCholeskyFactor_;
}

/* Method that returns the covariance factor - hmat */
HMatrix GeneralLinearModelResult::getHMatCholeskyFactor() const
{
  return covarianceHMatrix_;
}

// Return input sample transformed
Sample GeneralLinearModelResult::getInputTransformedSample() const
{
  return inputTransformedData_;
}

/* Method save() stores the object through the StorageManager */
void GeneralLinearModelResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute( "inputData_", inputData_ );
  adv.saveAttribute( "inputTransformedData_", inputTransformedData_ );
  adv.saveAttribute( "inputTransformation_", inputTransformation_ );
  adv.saveAttribute( "hasTransformation_", hasTransformation_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "covarianceModel_", covarianceModel_ );
  adv.saveAttribute( "optimalLogLikelihood_", optimalLogLikelihood_ );
  adv.saveAttribute( "hasCholeskyFactor_", hasCholeskyFactor_);
  adv.saveAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_);
}


/* Method load() reloads the object from the StorageManager */
void GeneralLinearModelResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute( "inputData_", inputData_ );
  adv.loadAttribute( "inputTransformedData_", inputTransformedData_ );
  adv.loadAttribute( "inputTransformation_", inputTransformation_ );
  adv.loadAttribute( "hasTransformation_", hasTransformation_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "beta_", beta_ );
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
  adv.loadAttribute( "optimalLogLikelihood_", optimalLogLikelihood_ );
  adv.loadAttribute( "hasCholeskyFactor_", hasCholeskyFactor_);
  adv.loadAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_);
}



END_NAMESPACE_OPENTURNS
