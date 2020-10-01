//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
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
#ifndef OPENTURNS_SPECTRALMODELFACTORYIMPLEMENTATION_HXX
#define OPENTURNS_SPECTRALMODELFACTORYIMPLEMENTATION_HXX

#include "openturns/SpectralModel.hxx"
#include "openturns/ProcessSample.hxx"
#include "openturns/Field.hxx"
#include "openturns/Point.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/FFT.hxx"

BEGIN_NAMESPACE_OPENTURNS

// Forward declaration
class SpectralModel;

/**
 * @class SpectralModelFactoryImplementation
 */
class OT_API SpectralModelFactoryImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  typedef Pointer<SpectralModelImplementation> Implementation;

  /** Default constructor */
  SpectralModelFactoryImplementation();

  /** Virtual constructor */
  SpectralModelFactoryImplementation * clone() const override;

  /** FFT algorithm accessor */
  FFT getFFTAlgorithm() const;

  /** FFT algorithm accessor */
  void setFFTAlgorithm(const FFT & fft);

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */
  /** Build a a spectral model based on a sample */
  virtual SpectralModel build(const ProcessSample & sample) const;

  /** Build a a spectral model based on a Field */
  virtual SpectralModel build(const Field & timeSerie) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** FFT algorithm */
  FFT fftAlgorithm_;

}; /* class SpectralModelFactoryImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECTRALMODELFACTORYIMPLEMENTATION_HXX */
