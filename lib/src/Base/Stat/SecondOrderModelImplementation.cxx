//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/SecondOrderModelImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Field.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SecondOrderModelImplementation
 */

CLASSNAMEINIT(SecondOrderModelImplementation)

static const Factory<SecondOrderModelImplementation> Factory_SecondOrderModelImplementation;

/* Constructor without parameters */
SecondOrderModelImplementation::SecondOrderModelImplementation()
  : PersistentObject()
  , covarianceModel_()
  , spectralModel_()
{
  // Nothing to do
}

SecondOrderModelImplementation::SecondOrderModelImplementation(const CovarianceModel & covarianceModel,
    const SpectralModel & spectralModel)
  : PersistentObject(),
    covarianceModel_(),
    spectralModel_()
{
  setModels(covarianceModel, spectralModel);
}

/* Virtual constructor */
SecondOrderModelImplementation * SecondOrderModelImplementation::clone() const
{
  return new SecondOrderModelImplementation(*this);
}

/* Dimension accessor */
UnsignedInteger SecondOrderModelImplementation::getInputDimension() const
{
  return covarianceModel_.getInputDimension();
}

UnsignedInteger SecondOrderModelImplementation::getOutputDimension() const
{
  return covarianceModel_.getOutputDimension();
}

UnsignedInteger SecondOrderModelImplementation::getSpatialDimension() const
{
  LOGWARN(OSS() << "SecondOrder::getSpatialDimension is deprecated in favor of getInputDimension.");
  return covarianceModel_.getInputDimension();
}

UnsignedInteger SecondOrderModelImplementation::getDimension() const
{
  LOGWARN(OSS() << "SecondOrder::getDimension is deprecated in favor of getOutputDimension.");
  return covarianceModel_.getOutputDimension();
}

/* Computation of the covariance function */
CovarianceMatrix SecondOrderModelImplementation::computeCovariance(const Scalar s,
    const Scalar t) const
{
  return covarianceModel_(s, t);
}

/* Computation of the covariance function */
CovarianceMatrix SecondOrderModelImplementation::computeCovariance(const Point & s,
    const Point & t) const
{
  return covarianceModel_(s, t);
}

CovarianceMatrix SecondOrderModelImplementation::computeCovariance(const Scalar tau) const
{
  return covarianceModel_(tau);
}

CovarianceMatrix SecondOrderModelImplementation::computeCovariance(const Point & tau) const
{
  return covarianceModel_(tau);
}

/* Computation of the spectral density function */
HermitianMatrix SecondOrderModelImplementation::computeSpectralDensity(const Scalar frequency) const
{
  return spectralModel_(frequency);
}

/* Discretize the covariance function on a given TimeGrid */
CovarianceMatrix SecondOrderModelImplementation::discretize(const RegularGrid & timeGrid) const
{
  return covarianceModel_.discretize(timeGrid) ;
}

/* String converter */
String SecondOrderModelImplementation::__repr__() const
{
  OSS oss;
  oss << "class= " << SecondOrderModelImplementation::GetClassName();
  oss << " covariance model=" << covarianceModel_.__repr__()
      << " spectral model=" << spectralModel_.__repr__();
  return oss;
}

/* SpectralModel and Covariance model accessor */
SpectralModel SecondOrderModelImplementation::getSpectralModel() const
{
  return spectralModel_;
}

CovarianceModel SecondOrderModelImplementation::getCovarianceModel() const
{
  return covarianceModel_;
}

void SecondOrderModelImplementation::setModels(const CovarianceModel & covarianceModel,
    const SpectralModel & spectralModel)
{
  if (!covarianceModel.isStationary()) throw InvalidArgumentException(HERE) << "Error: the covariance model is not stationary.";
  if (covarianceModel.getOutputDimension() != spectralModel.getOutputDimension())
    throw InvalidDimensionException(HERE) << "Error: the spectral model and the covariance model have different dimensions"
                                          << " spectral dimension = " << spectralModel.getOutputDimension()
                                          << " covariance dimension = " << covarianceModel.getOutputDimension();
  if (covarianceModel.getInputDimension() != spectralModel.getInputDimension())
    throw InvalidDimensionException(HERE) << "Error: the spectral model and the covariance model have different spatial dimensions"
                                          << " spectral spatial dimension = " << spectralModel.getInputDimension()
                                          << " covariance spatial dimension = " << covarianceModel.getInputDimension();
  covarianceModel_ = covarianceModel;
  spectralModel_ = spectralModel;
}

/* String converter */
String SecondOrderModelImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class= " << SecondOrderModelImplementation::GetClassName();
  oss << covarianceModel_.__str__(offset) << spectralModel_.__str__(offset);
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SecondOrderModelImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);

  adv.saveAttribute( "spectralModel_", spectralModel_ );
  adv.saveAttribute( "covarianceModel_", covarianceModel_);
}

/* Method load() reloads the object from the StorageManager */
void SecondOrderModelImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);

  adv.loadAttribute( "spectralModel_", spectralModel_ );
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
}

END_NAMESPACE_OPENTURNS
