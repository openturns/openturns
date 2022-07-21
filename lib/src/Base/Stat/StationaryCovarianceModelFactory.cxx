//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/StationaryCovarianceModelFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Point.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(StationaryCovarianceModelFactory)
static const Factory<StationaryCovarianceModelFactory> Factory_StationaryCovarianceModelFactory;

static const Factory<PersistentCollection<Complex> > Factory_PersistentCollection_Complex;

typedef Collection<SquareMatrix>  SquareMatrixCollection;

/* Default constructor */
StationaryCovarianceModelFactory::StationaryCovarianceModelFactory(const WelchFactory & factory)
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
WelchFactory StationaryCovarianceModelFactory::getSpectralModelFactory() const
{
  return spectralFactory_;
}

void StationaryCovarianceModelFactory::setSpectralModelFactory(const WelchFactory & factory)
{
  spectralFactory_ = factory;
}

/* String converter */
String StationaryCovarianceModelFactory::__str__(const String & ) const
{
  return __repr__();
}

/* Build a covariance model based on a user defined spectral model */
UserDefinedStationaryCovarianceModel StationaryCovarianceModelFactory::buildAsUserDefinedStationaryCovarianceModel(const UserDefinedSpectralModel & mySpectralModel) const
{
  return buildAsUserDefinedStationaryCovarianceModel(mySpectralModel, mySpectralModel.getFrequencyGrid());
}

/* Build a covariance model based on a spectral model and a frequency grid */
UserDefinedStationaryCovarianceModel StationaryCovarianceModelFactory::buildAsUserDefinedStationaryCovarianceModel(const SpectralModel & mySpectralModel,
    const RegularGrid & frequencyGrid) const
{
  // We get the dimension of the model
  const UnsignedInteger dimension = mySpectralModel.getOutputDimension();
  // From the spectral model, we want to evaluate the autocovariance function
  const UnsignedInteger N = frequencyGrid.getN();
  const Scalar df = frequencyGrid.getStep();
  const Scalar maximalFrequency = frequencyGrid.getValue(N - 1) + 0.5 * df;
  // We use the integrale of the spectral density through the frequencies, i.e.
  // \int_{\Omega_c} S(f) exp(i2\pi f h) df  ==> the algorithm works on frequencies
  // The used algorithm imposes both positive and negative ones
  // Some transformations are needed
  const UnsignedInteger size = 2 * N;
  // Care!!! Check the expression of dt - The time grid should corresponds to the frequency grid
  const Scalar dt = 0.5 / maximalFrequency;

  // As we need Fourier transformations, we need to have a structure which enables to stock elements
  // For d = dimension, we need dimension * 0.5 * (dimension + 1) transformations
  const UnsignedInteger numberOfFFT = dimension * (dimension + 1) / 2;
  ComplexMatrix matrix(size, numberOfFFT);
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    UnsignedInteger columnIndex = 0;
    // Computation is done for the current frequency value
    // The frequency is computed thanks to the formula (2k +1 -size) *0.5 * df with k=0,.,..,size-1
    const Scalar currentFrequency = (2.0 * k + 1 - size) * 0.5 * df;
    const HermitianMatrix spectralDensity(mySpectralModel(currentFrequency));
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      for (UnsignedInteger j = 0; j <= i; ++j)
      {
        const Scalar theta = (size - 1.0) * k * M_PI / size;
        const Complex alpha(cos(theta), -sin(theta));
        const Complex spectralValue(spectralDensity(i, j));
        const Complex phi_k(spectralValue * alpha);
        matrix(k, columnIndex) = phi_k;
        columnIndex += 1;
      }
    }
  }
  // At this level, we get elements to launch the fft
  // We first compute a temporal factor delta(m) = df * N * exp(-\pi * i * (2m + 1 - N) * (N-1) / 2N)
  // The formula of this last one may be found in the UseCaseGuide
  Collection<Complex> delta(size);
  for (UnsignedInteger m = 0; m < size; ++m)
  {
    const Scalar theta = (size - 1.0) / size * 0.5 * M_PI * (2.0 * m + 1.0 - size);
    const Complex alpha(cos(theta), -1.0 * sin(theta));
    delta[m] = df * size * alpha;
  }

  // We use the same FFT as the spectral factory
  FFT fftAlgorithm(spectralFactory_.getFFTAlgorithm());
  for (UnsignedInteger columnIndex = 0; columnIndex < numberOfFFT; ++columnIndex)
  {
    // FFT applications
    const Collection<Complex> marginal(fftAlgorithm.inverseTransform(*matrix.getImplementation(), columnIndex * size, size));
    // We save result in the same matrix
    for (UnsignedInteger rowIndex = 0; rowIndex < size; ++rowIndex)
    {
      matrix(rowIndex, columnIndex) = marginal[rowIndex] * delta[rowIndex];
    }
  }

  // We rewrite the elements in the adequate structure
  RegularGrid timeGrid(0.5 * dt, dt, N);
  SquareMatrixCollection collection(N);
  for (UnsignedInteger currentIndex = 0; currentIndex < N; ++currentIndex)
  {
    const UnsignedInteger index = currentIndex + N;
    CovarianceMatrix covariance(dimension);
    UnsignedInteger columnIndex = 0;
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

/* Build a covariance model based on a process sample */
CovarianceModel StationaryCovarianceModelFactory::build(const ProcessSample & sample) const
{
  return buildAsUserDefinedStationaryCovarianceModel(sample).clone();
}

/* Build a user defined covariance model based on a process sample */
UserDefinedStationaryCovarianceModel StationaryCovarianceModelFactory::buildAsUserDefinedStationaryCovarianceModel(const ProcessSample & sample) const
{
  return buildAsUserDefinedStationaryCovarianceModel(spectralFactory_.buildAsUserDefinedSpectralModel(sample));
}

/* Build a covariance model based on a field */
CovarianceModel StationaryCovarianceModelFactory::build(const Field & timeSeries) const
{
  return buildAsUserDefinedStationaryCovarianceModel(timeSeries).clone();
}

/* Build a user defined covariance model based on a field */
UserDefinedStationaryCovarianceModel StationaryCovarianceModelFactory::buildAsUserDefinedStationaryCovarianceModel(const Field & timeSeries) const
{
  return buildAsUserDefinedStationaryCovarianceModel(spectralFactory_.buildAsUserDefinedSpectralModel(timeSeries));
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
