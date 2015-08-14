//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  Id:      $Id$
 */
#include "StationaryCovarianceModelFactory.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"
#include "Collection.hxx"
#include "NumericalPoint.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(StationaryCovarianceModelFactory);
static Factory<StationaryCovarianceModelFactory> RegisteredFactory("StationaryCovarianceModelFactory");

static Factory<PersistentCollection<NumericalComplex> > RegisteredFactory2("PersistentCollection<NumericalComplex>");

typedef Collection<CovarianceMatrix>  CovarianceMatrixCollection;

/* Default constructor */
StationaryCovarianceModelFactory::StationaryCovarianceModelFactory(const SpectralModelFactory & factory)
  : CovarianceModelFactoryImplementation()
{
  setSpectralModelFactory(factory);
}

/* Virtual constructor */
StationaryCovarianceModelFactory * StationaryCovarianceModelFactory::clone() const
{
  return new StationaryCovarianceModelFactory(*this);
}

/* String converter */
String StationaryCovarianceModelFactory::__repr__() const
{
  OSS oss;
  oss << "class=" << StationaryCovarianceModelFactory::GetClassName();
  return oss;
}

/* SpectralModelFactory accessors */
SpectralModelFactory StationaryCovarianceModelFactory::getSpectralModelFactory() const
{
  return spectralFactory_;
}

void StationaryCovarianceModelFactory::setSpectralModelFactory(const SpectralModelFactory & factory)
{
  spectralFactory_ = factory;
}

/* String converter */
String StationaryCovarianceModelFactory::__str__(const String & offset) const
{
  return __repr__();
}

UserDefinedStationaryCovarianceModel StationaryCovarianceModelFactory::buildAsUserDefinedStationaryCovarianceModel(const SpectralModel & mySpectralModel) const
{
  // We get the dimension of the model
  const UnsignedInteger dimension(mySpectralModel.getDimension());
  // From the spectral model, we want to evaluate the autocovariance function
  // We first get the frequency grid : the grid contains only positive frequencies
  const RegularGrid frequencyGrid(mySpectralModel.getFrequencyGrid());
  const UnsignedInteger N(frequencyGrid.getN());
  const NumericalScalar df(frequencyGrid.getStep());
  const NumericalScalar maximalFrequency(frequencyGrid.getValue(N - 1) + 0.5 * df);
  // We use the integrale of the spectral density through the frequencies, i.e.
  // \int_{\Omega_c} S(f) exp(i2\pi f h) df  ==> the algorithm works on frequencies
  // The used algorithm imposes both positive and negative ones
  // Some transformations are needed
  const UnsignedInteger size(2 * N);
  // Care!!! Check the expression of dt - The time grid should corresponds to the frequency grid
  const NumericalScalar dt(0.5 / maximalFrequency);

  // As we need Fourier transformations, we need to have a structure which enables to stock elements
  // For d = dimension, we need dimension * 0.5 * (dimension + 1) transformations
  const UnsignedInteger numberOfFFT(dimension * (dimension + 1) / 2);
  ComplexMatrix matrix(size, numberOfFFT);
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    UnsignedInteger columnIndex(0);
    // Computation is done for the current frequency value
    // The frequency is computed thanks to the formula (2k +1 -size) *0.5 * df with k=0,.,..,size-1
    const NumericalScalar currentFrequency((2.0 * k + 1 - size) * 0.5 * df);
    const HermitianMatrix spectralDensity(mySpectralModel(currentFrequency));
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      for (UnsignedInteger j = 0; j <= i; ++j)
      {
        const NumericalScalar theta((size - 1.0) * k * M_PI / size);
        const NumericalComplex alpha(cos(theta), -sin(theta));
        const NumericalComplex spectralValue(spectralDensity(i, j));
        const NumericalComplex phi_k(spectralValue * alpha);
        matrix(k, columnIndex) = phi_k;
        columnIndex += 1;
      }
    }
  }
  // At this level, we get elements to launch the fft
  // We first compute a temporal factor delta(m) = df * N * exp(-\pi * i * (2m + 1 - N) * (N-1) / 2N)
  // The formula of this last one may be found in the UseCaseGuide
  Collection<NumericalComplex> delta(size);
  for (UnsignedInteger m = 0; m < size; ++m)
  {
    const NumericalScalar theta((size - 1.0) / size * 0.5 * M_PI * (2.0 * m + 1.0 - size));
    const NumericalComplex alpha(cos(theta), -1.0 * sin(theta));
    delta[m] = df * size * alpha;
  }

  // We use the same FFT as the spectral factory
  FFT fftAlgorithm(spectralFactory_.getFFTAlgorithm());
  for (UnsignedInteger columnIndex = 0; columnIndex < numberOfFFT; ++columnIndex)
  {
    // FFT applications
    const Collection<NumericalComplex> marginal(fftAlgorithm.inverseTransform(*matrix.getImplementation(), columnIndex * size, size));
    // We save result in the same matrix
    for (UnsignedInteger rowIndex = 0; rowIndex < size; ++rowIndex)
    {
      matrix(rowIndex, columnIndex) = marginal[rowIndex] * delta[rowIndex];
    }
  }

  // We rewrite the elements in the adequate structure
  RegularGrid timeGrid(0.5 * dt, dt, N);
  CovarianceMatrixCollection collection(N);
  for (UnsignedInteger currentIndex = 0; currentIndex < N; ++currentIndex)
  {
    const UnsignedInteger index(currentIndex + N);
    CovarianceMatrix covariance(dimension);
    UnsignedInteger columnIndex(0);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      for (UnsignedInteger j = 0; j <= i; ++j)
      {
        covariance(i, j) = std::real(matrix(index, columnIndex));
        ++columnIndex;
      }
    }
    collection[currentIndex] = covariance;
  }
  return UserDefinedStationaryCovarianceModel(timeGrid, collection);
}

CovarianceModelImplementation::Implementation StationaryCovarianceModelFactory::build(const ProcessSample & sample) const
{
  return buildAsUserDefinedStationaryCovarianceModel(sample).clone();
}

UserDefinedStationaryCovarianceModel StationaryCovarianceModelFactory::buildAsUserDefinedStationaryCovarianceModel(const ProcessSample & sample) const
{
  return buildAsUserDefinedStationaryCovarianceModel(spectralFactory_.build(sample));
}

CovarianceModelImplementation::Implementation StationaryCovarianceModelFactory::build(const Field & timeSeries) const
{
  return buildAsUserDefinedStationaryCovarianceModel(timeSeries).clone();
}

UserDefinedStationaryCovarianceModel StationaryCovarianceModelFactory::buildAsUserDefinedStationaryCovarianceModel(const Field & timeSeries) const
{
  return buildAsUserDefinedStationaryCovarianceModel(spectralFactory_.build(timeSeries));
}

/* Method save() stores the object through the StorageManager */
void StationaryCovarianceModelFactory::save(Advocate & adv) const
{
  CovarianceModelFactoryImplementation::save(adv);
  adv.saveAttribute( "spectralFactory_", spectralFactory_);
}

/* Method load() reloads the object from the StorageManager */
void StationaryCovarianceModelFactory::load(Advocate & adv)
{
  CovarianceModelFactoryImplementation::load(adv);
  adv.loadAttribute( "spectralFactory_", spectralFactory_);
}

END_NAMESPACE_OPENTURNS
