//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpectralModelFactoryImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SpectralModelFactoryImplementation);
static const Factory<SpectralModelFactoryImplementation> Factory_SpectralModelFactoryImplementation;

/* Default constructor */
SpectralModelFactoryImplementation::SpectralModelFactoryImplementation()
  : PersistentObject(),
    frequencyGrid_(),
    fftAlgorithm_()
{
  // Nothing to do
}

/* Virtual constructor */
SpectralModelFactoryImplementation * SpectralModelFactoryImplementation::clone() const
{
  return new SpectralModelFactoryImplementation(*this);
}


/* Frequency grid accessor */
RegularGrid SpectralModelFactoryImplementation::getFrequencyGrid() const
{
  return frequencyGrid_;
}

/* Frequency grid accessor */
void SpectralModelFactoryImplementation::setFrequencyGrid(const RegularGrid & grid)
{
  frequencyGrid_ = grid;
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
String SpectralModelFactoryImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << this->getClassName();
}

/* Here is the interface that all derived class must implement */

SpectralModelImplementation * SpectralModelFactoryImplementation::build(const ProcessSample & sample) const
{
  throw NotYetImplementedException(HERE) << "In SpectralModelFactoryImplementation::build(const ProcessSample & sample) const";
}

SpectralModelImplementation * SpectralModelFactoryImplementation::build(const Field & timeSeries) const
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
