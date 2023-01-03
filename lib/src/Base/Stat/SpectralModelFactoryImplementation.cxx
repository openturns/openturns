//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpectralModelFactoryImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SpectralModelFactoryImplementation)
static const Factory<SpectralModelFactoryImplementation> Factory_SpectralModelFactoryImplementation;

/* Default constructor */
SpectralModelFactoryImplementation::SpectralModelFactoryImplementation()
  : PersistentObject()
  , fftAlgorithm_()
{
  // Nothing to do
}

/* Virtual constructor */
SpectralModelFactoryImplementation * SpectralModelFactoryImplementation::clone() const
{
  return new SpectralModelFactoryImplementation(*this);
}

/* FFT algorithm accessor */
FFT SpectralModelFactoryImplementation::getFFTAlgorithm() const
{
  return fftAlgorithm_;
}

/* FFT algorithm accessor */
void SpectralModelFactoryImplementation::setFFTAlgorithm(const FFT & fft)
{
  fftAlgorithm_ = fft;
}


/* String converter */
String SpectralModelFactoryImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SpectralModelFactoryImplementation::GetClassName();
  return oss;
}

/* String converter */
String SpectralModelFactoryImplementation::__str__(const String & ) const
{
  return OSS(false) << this->getClassName();
}

/* Here is the interface that all derived class must implement */

SpectralModel SpectralModelFactoryImplementation::build(const ProcessSample & ) const
{
  throw NotYetImplementedException(HERE) << "In SpectralModelFactoryImplementation::build(const ProcessSample & sample) const";
}

SpectralModel SpectralModelFactoryImplementation::build(const Field & ) const
{
  throw NotYetImplementedException(HERE) << "In SpectralModelFactoryImplementation::build(const Field & timeSeries) const";
}

/* Method save() stores the object through the StorageManager */
void SpectralModelFactoryImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "fftAlgorithm_", fftAlgorithm_);
}

/* Method load() reloads the object from the StorageManager */
void SpectralModelFactoryImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "fftAlgorithm_", fftAlgorithm_);
}
END_NAMESPACE_OPENTURNS
