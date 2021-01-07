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
#ifndef OPENTURNS_SPECTRALMODELFACTORY_HXX
#define OPENTURNS_SPECTRALMODELFACTORY_HXX

#include "openturns/SpectralModelFactoryImplementation.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class  SpectralModelFactory
 */
class OT_API  SpectralModelFactory
  : public TypedInterfaceObject<SpectralModelFactoryImplementation>
{
  CLASSNAME
public:

  typedef Pointer<SpectralModelFactoryImplementation> Implementation;

  /** Default constructor */
  SpectralModelFactory();

  /** Copy constructors */
  SpectralModelFactory(const SpectralModelFactoryImplementation & implementation);

  /** Constructor from implementation */
  SpectralModelFactory(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  SpectralModelFactory(SpectralModelFactoryImplementation * p_implementation);
#endif

  /** FFT algorithm accessor */
  FFT getFFTAlgorithm() const;

  /** FFT algorithm accessor */
  void setFFTAlgorithm(FFT & fft);

  /** String converter */
  String __repr__() const override;

  /** String converter */

  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */
  /** Build a a spectral model based on a sample */
  virtual SpectralModel build(const ProcessSample & sample) const;

  /** Build a a spectral model based on a Field */
  virtual SpectralModel build(const Field & timeSerie) const;


protected:

}; /* class  SpectralModelFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECTRALMODELFACTORY_HXX */
