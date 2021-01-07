//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
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
#include "openturns/SpectralModelFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SpectralModelFactory)

/* Default constructor */
SpectralModelFactory::SpectralModelFactory()
  : TypedInterfaceObject<SpectralModelFactoryImplementation>(new SpectralModelFactoryImplementation())
{
  // Nothing to do
}

/* Parameters constructor */
SpectralModelFactory::SpectralModelFactory(const SpectralModelFactoryImplementation & implementation)
  : TypedInterfaceObject<SpectralModelFactoryImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
SpectralModelFactory::SpectralModelFactory(const Implementation & p_implementation)
  : TypedInterfaceObject<SpectralModelFactoryImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
SpectralModelFactory::SpectralModelFactory(SpectralModelFactoryImplementation * p_implementation)
  : TypedInterfaceObject<SpectralModelFactoryImplementation>(p_implementation)
{
  // Nothing to do
}

/* FFT algorithm accessor */
FFT SpectralModelFactory::getFFTAlgorithm() const
{
  return getImplementation()->getFFTAlgorithm();
}

/* FFT algorithm accessor */
void SpectralModelFactory::setFFTAlgorithm(FFT & fft)
{
  copyOnWrite();
  getImplementation()->setFFTAlgorithm(fft);
}

/* String converter */
String SpectralModelFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SpectralModelFactory::GetClassName()
      << " implementation = " << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String SpectralModelFactory::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

SpectralModel SpectralModelFactory::build(const ProcessSample & sample) const
{
  return getImplementation()->build(sample);
}

SpectralModel SpectralModelFactory::build(const Field & timeSeries) const
{
  return getImplementation()->build(timeSeries);
}

END_NAMESPACE_OPENTURNS
