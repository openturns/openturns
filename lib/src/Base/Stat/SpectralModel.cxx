//                                               -*- C++ -*-
/**
 *  @brief Interface class for the concept of spectral model for
 *         stationary processes
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SpectralModel.hxx"
#include "openturns/CauchyModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SpectralModel)

//   static const Factory<SpectralModel> Factory_SpectralModel;

/* Constructor with parameters */
SpectralModel::SpectralModel()
  : TypedInterfaceObject<SpectralModelImplementation>(new CauchyModel())
{
  // Nothing to do
}

/* Parameters constructor */
SpectralModel::SpectralModel(const SpectralModelImplementation & implementation)
  : TypedInterfaceObject<SpectralModelImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
SpectralModel::SpectralModel(const Implementation & p_implementation)
  : TypedInterfaceObject<SpectralModelImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
SpectralModel::SpectralModel(SpectralModelImplementation * p_implementation)
  : TypedInterfaceObject<SpectralModelImplementation>(p_implementation)
{
  // Nothing to do
}


/* Dimension accessor */
UnsignedInteger SpectralModel::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();
}

UnsignedInteger SpectralModel::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

HermitianMatrix SpectralModel::operator() (const Scalar frequency) const
{
  return getImplementation()->operator()(frequency);
}

Complex SpectralModel::computeStandardRepresentative(const Scalar frequency) const
{
  return getImplementation()->computeStandardRepresentative(frequency);
}

/* Drawing method */
Graph SpectralModel::draw(const UnsignedInteger rowIndex,
                          const UnsignedInteger columnIndex,
                          const Scalar minimumFrequency,
                          const Scalar maximumFrequency,
                          const UnsignedInteger frequencyNumber,
                          const Bool module) const
{
  return getImplementation()->draw(rowIndex, columnIndex, minimumFrequency, maximumFrequency, frequencyNumber, module);
}

/** Amplitude accessor */
Point SpectralModel::getAmplitude() const
{
  return getImplementation()->getAmplitude();
}

void SpectralModel::setAmplitude(const Point & amplitude)
{
  copyOnWrite();
  return getImplementation()->setAmplitude(amplitude);
}

/** Scale accessor */
Point SpectralModel::getScale() const
{
  return getImplementation()->getScale();
}

void SpectralModel::setScale(const Point & scale)
{
  copyOnWrite();
  return getImplementation()->setScale(scale);
}

/** Spatial correlation accessor */
CorrelationMatrix SpectralModel::getOutputCorrelation() const
{
  return getImplementation()->getOutputCorrelation();
}

/* String converter */
String SpectralModel::__repr__() const
{
  return getImplementation()->__repr__();
}

/* String converter */
String SpectralModel::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
