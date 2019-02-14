//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/WelchFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Collection.hxx"
#include "openturns/HermitianMatrix.hxx"
#include "openturns/Point.hxx"
#include "openturns/Hamming.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WelchFactory)
static const Factory<WelchFactory> Factory_WelchFactory;

typedef Collection<Complex> ComplexCollection;
typedef Collection<HermitianMatrix>  HermitianMatrixCollection;

/* Default constructor */
WelchFactory::WelchFactory()
  : SpectralModelFactoryImplementation()
  , window_(Hamming())
  , blockNumber_(1)
  , overlap_(0.0)
{
  // Nothing to do
}

WelchFactory::WelchFactory(const FilteringWindows & window,
                           const UnsignedInteger blockNumber,
                           const Scalar overlap)
  : SpectralModelFactoryImplementation()
  , window_(window)
  , blockNumber_(0)
  , overlap_(0.0)
{
  setBlockNumber(blockNumber);
  setOverlap(overlap);
}

/* Virtual constructor */
WelchFactory * WelchFactory::clone() const
{
  return new WelchFactory(*this);
}

/* FilteringWindows accessor */
FilteringWindows WelchFactory::getFilteringWindows() const
{
  return window_;
}

void WelchFactory::setFilteringWindows(const FilteringWindows & window)
{
  window_ = window;
}

/* String converter */
String WelchFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << WelchFactory::GetClassName()
      << " window = " << window_
      << " blockNumber = " << blockNumber_
      << " overlap = " << overlap_;
  return oss;
}

/* String converter */
String WelchFactory::__str__(const String & ) const
{
  return __repr__();
}

/* Number of blockNumber accessor */
UnsignedInteger WelchFactory::getBlockNumber() const
{
  return blockNumber_;
}

void WelchFactory::setBlockNumber(const UnsignedInteger blockNumber)
{
  if (blockNumber < 1) throw InvalidArgumentException(HERE) << "Error: the number of blocks should be at least 1";
  blockNumber_ = blockNumber;
}

/* Overlap accessor */
Scalar WelchFactory::getOverlap() const
{
  return overlap_;
}

void WelchFactory::setOverlap(const Scalar overlap)
{
  if ((overlap < 0.0) || (overlap > 0.5)) throw InvalidArgumentException(HERE) << "Error: the overlap must be in [0, 0.5], here overlap=" << overlap;
  overlap_ = overlap;
}

SpectralModel WelchFactory::build(const ProcessSample & sample) const
{
  return buildAsUserDefinedSpectralModel(sample).clone();
}

SpectralModel WelchFactory::build(const Field & timeSeries) const
{
  return buildAsUserDefinedSpectralModel(timeSeries).clone();
}

UserDefinedSpectralModel WelchFactory::buildAsUserDefinedSpectralModel(const ProcessSample & sample) const
{
  const UnsignedInteger dimension = sample.getDimension();
  const UnsignedInteger sampleSize = sample.getSize();
  const RegularGrid timeGrid(sample.getTimeGrid());
  const UnsignedInteger N = timeGrid.getN();
  const Scalar timeStep = timeGrid.getStep();
  const Scalar T = timeGrid.getEnd() - timeGrid.getStart();
  // Preprocessing: the scaling factor, including the tappering window
  ComplexCollection alpha(N);
  const Scalar factor = timeStep / sqrt(sampleSize * T);
  for (UnsignedInteger m = 0; m < N; ++m)
  {
    // The window argument is normalized on [0, 1]
    const Scalar xiM = static_cast<Scalar>(m) / N;
    // Phase shift
    const Scalar theta = M_PI * (N - 1) * xiM;
    alpha[m] = factor * window_(xiM) * Complex(cos(theta), sin(theta));
  }
  // The DSP estimate will be done over a regular frequency grid containing only
  // nonnegative frequency values. It is then extended as a stepwise function of
  // the frequency on positive and negative values using the hermitian symmetry
  // If N is even, kMax = N / 2, else kMax = (N + 1) / 2.
  UnsignedInteger kMax = N / 2;
  const Scalar frequencyStep = 1.0 / T;
  Scalar frequencyMin = 0.5 * frequencyStep;
  // Adjust kMax and frequencyMin if N is odd
  if (N % 2 == 1)
  {
    ++kMax;
    frequencyMin = 0.0;
  }
  const RegularGrid frequencyGrid(frequencyMin, frequencyStep, kMax);
  HermitianMatrixCollection DSPCollection(kMax, HermitianMatrix(dimension));
  // Loop over the time series
  for (UnsignedInteger l = 0 ; l < sampleSize; ++l)
  {
    // The current time series values are stored into a ComplexMatrix in order to
    // have a two-indices access and an internal linear storage. The data are stored column-wise, i.e the column elements are contiguous in memory
    ComplexMatrix zHat(kMax, dimension);
    // Loop over the dimension
    // For each component, compute the FFT of the time series component
    for (UnsignedInteger p = 0; p < dimension; ++p)
    {
      // Loop over the time stamps
      ComplexCollection zP(N);
      for (UnsignedInteger m = 0; m < N; ++m) zP[m] = alpha[m] * sample[l](m, p); // The first component of the value of a time series at a given index is the time value
      // Perform the FFT direct transform of the tapered data
      const ComplexCollection zPHat(fftAlgorithm_.transform(zP));
      // Stores the result. Only the values associated with nonnegative frequency values are stored.
      for (UnsignedInteger k = 0; k < kMax; ++k) zHat(k, p) = zPHat[N - kMax + k];
    }
    // Now, we can estimate the spectral density over the reduced frequency grid
    for (UnsignedInteger k = 0; k < kMax; ++k)
    {
      // Perform the row-wise kronecker product
      for (UnsignedInteger p = 0; p < dimension; ++p)
        for (UnsignedInteger q = 0; q <= p; ++q)
          DSPCollection[k](p, q) += zHat(k, p) * std::conj(zHat(k, q));
    }
  } // Loop over the time series
  return UserDefinedSpectralModel(frequencyGrid, DSPCollection);
}

UserDefinedSpectralModel WelchFactory::buildAsUserDefinedSpectralModel(const Field & timeSeries) const
{
  // We split the time series into overlaping blockNumbers that are used as a ProcessSample
  const UnsignedInteger size = timeSeries.getSize();
  const UnsignedInteger dimension = timeSeries.getOutputDimension();
  // First, compute the block size
  const UnsignedInteger blockSize = static_cast<UnsignedInteger>(size / (1.0 + (blockNumber_ - 1.0) * (1.0 - overlap_)));
  // Then, compute the associated hop size, even if it is not exactly associated with the overlap value
  // No hop size if only one block
  const UnsignedInteger hopSize = blockNumber_ == 1 ? 0 : (size - blockSize) / (blockNumber_ - 1);
  const RegularGrid timeGrid(timeSeries.getTimeGrid());
  // Initialize the equivalent process sample with the correct time grid
  ProcessSample sample(blockNumber_, Field(RegularGrid(timeGrid.getStart(), timeGrid.getStep(), blockSize), dimension));
  const Sample values(timeSeries.getValues());
  for (UnsignedInteger blockIndex = 0; blockIndex < blockNumber_; ++blockIndex)
  {
    std::copy(&values(blockIndex * hopSize, 0),
              &values(blockIndex * hopSize + blockSize - 1, dimension - 1) + 1,
              &sample[blockIndex](0, 0));
  } // Loop on the blocks
  return buildAsUserDefinedSpectralModel(sample);
}

/* Method save() stores the object through the StorageManager */
void WelchFactory::save(Advocate & adv) const
{
  SpectralModelFactoryImplementation::save(adv);
  adv.saveAttribute( "window_", window_);
  adv.saveAttribute( "blockNumber_", blockNumber_);
  adv.saveAttribute( "overlap_", overlap_);
}

/* Method load() reloads the object from the StorageManager */
void WelchFactory::load(Advocate & adv)
{
  SpectralModelFactoryImplementation::load(adv);
  adv.loadAttribute( "window_", window_);
  adv.loadAttribute( "blockNumber_", blockNumber_);
  adv.loadAttribute( "overlap_", overlap_);
}
END_NAMESPACE_OPENTURNS
