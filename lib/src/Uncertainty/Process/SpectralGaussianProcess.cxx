//                                               -*- C++ -*-
/**
 *  @brief A class which implements the SpectralGaussianProcess process
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

#include "openturns/SpectralGaussianProcess.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection< TriangularComplexMatrix >)
static const Factory<PersistentCollection<TriangularComplexMatrix> > Factory_PersistentCollection_TriangularComplexMatrix;

CLASSNAMEINIT(SpectralGaussianProcess)

static const Factory<SpectralGaussianProcess> Factory_SpectralGaussianProcess;

SpectralGaussianProcess::SpectralGaussianProcess()
  : ProcessImplementation()
  , spectralModel_()
  , maximalFrequency_(0.0)
  , nFrequency_(0)
  , frequencyStep_(0.0)
  , choleskyFactorsCache_(0)
  , alpha_(0)
  , fftAlgorithm_()
{
  setOutputDimension(spectralModel_.getOutputDimension());
  setDescription(Description::BuildDefault(getOutputDimension(), "x"));
}

/* Standard constructor with spectralModel - The timeGrid imposes the frequencies values*/
SpectralGaussianProcess::SpectralGaussianProcess(const SpectralModel & spectralModel,
    const RegularGrid & timeGrid)
  : ProcessImplementation()
  , spectralModel_(spectralModel)
  , maximalFrequency_(0.0)
  , nFrequency_(0)
  , frequencyStep_(0.0)
  , choleskyFactorsCache_(0)
  , alpha_(0)
  , fftAlgorithm_()
{
  setTimeGrid(timeGrid);
  setOutputDimension(spectralModel.getOutputDimension());
  setDescription(Description::BuildDefault(getOutputDimension(), "x"));
}

/* Standard constructor with spectralModel - The timeGrid imposes the frequencies values*/
SpectralGaussianProcess::SpectralGaussianProcess(const SpectralModel & spectralModel,
    const Scalar maximalFrequency,
    const UnsignedInteger nFrequency)

  : ProcessImplementation()
  , spectralModel_(spectralModel)
  , maximalFrequency_(maximalFrequency)
  , nFrequency_(nFrequency)
  , frequencyStep_(0.0)
  , choleskyFactorsCache_(0)
  , alpha_(0)
  , fftAlgorithm_()
{
  if (!(maximalFrequency > 0.0)) throw InvalidArgumentException(HERE) << "Error: the maximal frequency must be positive, here maximalFrequency=" << maximalFrequency;
  if (nFrequency < 1) throw InvalidArgumentException(HERE) << "Error: the number of frequency points in the positive domain must be at least 1.";
  frequencyStep_ = maximalFrequency_ / nFrequency_;
  // Adapt the time grid to the frequency discretization
  computeTimeGrid();
  computeAlpha();
  setOutputDimension(spectralModel.getOutputDimension());
  setDescription(Description::BuildDefault(getOutputDimension(), "x"));
}

/* Virtual constructor */
SpectralGaussianProcess * SpectralGaussianProcess::clone() const
{
  return new SpectralGaussianProcess(*this);
}

/** Get the Cholesky factor of the kth DSP matrix from cache or computed on the fly */
TriangularComplexMatrix SpectralGaussianProcess::getCholeskyFactor(const UnsignedInteger k) const
{
  // The value is in the cache
  if (k < choleskyFactorsCache_.getSize()) return choleskyFactorsCache_[k];
  // Compute the needed factor
  TriangularComplexMatrix factor(computeCholeskyFactor(k));
  // There are still room to store the factor
  if (k < ResourceMap::GetAsUnsignedInteger("SpectralGaussianProcess-CholeskyCacheSize")) choleskyFactorsCache_.add(factor);
  else LOGWARN(OSS() << "Warning! The cache for cholesky factors is full. Expect a big performance penalty. Increase the cache size using the ResourceMap key \"SpectralGaussianProcess-CholeskyCacheSize\" if you have enough memory.");
  return factor;
}

/** Compute the needed Cholesky factor using regularization */
TriangularComplexMatrix SpectralGaussianProcess::computeCholeskyFactor(const UnsignedInteger k) const
{
  // Convert the index into a frequency
  // The index k corresponds to the kth positive discretization point in the frequency domain [-f_max, f_max] discretized using the center of the regular partition into 2N cells of the interval.
  const Scalar frequency = (k + 0.5) * frequencyStep_;
  // Compute the DSP matrix
  HermitianMatrix spectralDensityMatrix(spectralModel_(frequency));
  // Flag to tell if the regularization has to be increased
  Bool continuationCondition = true;
  // Scale control values
  Scalar cumulatedScaling = 0.0;
  const Scalar startingScaling = ResourceMap::GetAsScalar("SpectralGaussianProcess-StartingScaling");
  const Scalar maximalScaling = ResourceMap::GetAsScalar("SpectralGaussianProcess-MaximalScaling");
  Scalar scaling = startingScaling;
  TriangularComplexMatrix choleskyFactor;
  while (continuationCondition)
  {
    // Try to compute the Cholesky factor with the current regularization
    try
    {
      choleskyFactor = spectralDensityMatrix.computeCholesky();
      continuationCondition = false;
    }
    // If the factorization failed, increase the regularization
    catch (const InternalException &)
    {
      cumulatedScaling += scaling;
      for (UnsignedInteger  index = 0; index < getOutputDimension(); ++index) spectralDensityMatrix(index, index) += scaling;
      scaling *= 2.0;
    }
    // No reasonable regularization succeeded
    if (cumulatedScaling >= maximalScaling)
      throw InvalidArgumentException(HERE) << "Error; Could not compute the Cholesky factor"
                                           << " Scaling up to "  << cumulatedScaling << " was not enough";
  } // Loop over the continuation condition
  return choleskyFactor;
}

/* String converter */
String SpectralGaussianProcess::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SpectralGaussianProcess::GetClassName();
  oss << " timeGrid=" << getTimeGrid()
      << " spectralModel=" << spectralModel_
      << " maximal frequency=" << maximalFrequency_
      << " n frequency=" << nFrequency_
      << " cholesky cache=" << choleskyFactorsCache_;
  return oss;
}

String SpectralGaussianProcess::__str__(const String & ) const
{
  OSS oss(false);
  oss << " SpectralGaussianProcess=" << SpectralGaussianProcess::GetClassName()
      << " dimension=" << getOutputDimension()
      << " spectralModel=" << spectralModel_
      << " maximal frequency=" << maximalFrequency_
      << " n frequency=" << nFrequency_;
  return oss;
}

/* Get the grid of frequencies, covering both the negative and the positive axes */
RegularGrid SpectralGaussianProcess::getFrequencyGrid() const
{
  return RegularGrid(-maximalFrequency_ + 0.5 * frequencyStep_, frequencyStep_, 2 * nFrequency_);
}

/* Maximal frequency accessor */
Scalar SpectralGaussianProcess::getMaximalFrequency() const
{
  return maximalFrequency_;
}

/* Number of frequency steps accessor */
UnsignedInteger SpectralGaussianProcess::getNFrequency() const
{
  return nFrequency_;
}

/* Frequency steps accessor */
Scalar SpectralGaussianProcess::getFrequencyStep() const
{
  return frequencyStep_;
}

/* FFT algorithm accessors */
FFT SpectralGaussianProcess::getFFTAlgorithm() const
{
  return fftAlgorithm_;
}

void SpectralGaussianProcess::setFFTAlgorithm(const FFT & fft)
{
  fftAlgorithm_ = fft;
}

/** Spectral model accessor */
SpectralModel SpectralGaussianProcess::getSpectralModel() const
{
  return spectralModel_;
}

/* Set TimeGrid */
void SpectralGaussianProcess::setTimeGrid(const RegularGrid & tg)
{
  if (tg != RegularGrid(mesh_))
  {
    // The time grid must contains an even number of points
    const UnsignedInteger nT = tg.getN();
    if (nT % 2 != 0) throw InvalidArgumentException(HERE) << "Error: only time grids with an even (or better a power of two) number of points are allowed. You may use the AdaptGrid method.";
    // Fix the new timeGrid
    mesh_ = tg;
    // Compute the frequency characteristics
    maximalFrequency_ = 0.5 / tg.getStep();
    nFrequency_ = nT / 2;
    frequencyStep_ = maximalFrequency_ / nFrequency_;
    // We must fix also the alpha vector
    computeAlpha();
    // Reset the cache
    choleskyFactorsCache_ = TriangularComplexMatrixPersistentCollection(0);
  }
}

/* Compute the time grid associated to the frequency grid */
void SpectralGaussianProcess::computeTimeGrid()
{
  mesh_ = RegularGrid(0.0, 1.0 / (2.0 * maximalFrequency_), 2 * nFrequency_);
}

/* Set the alpha vector */
void SpectralGaussianProcess::computeAlpha()
{
  alpha_ = PersistentComplexCollection(2 * nFrequency_);
  // Convert the frequency into pulsation, take into account that there are 2*nFrequency points and that
  // a sqrt(2) factor is needed to switch from Box Muller transform to normal complex random variable
  const Scalar factor = 2.0 * nFrequency_ * sqrt(frequencyStep_);
  const Scalar beta = -M_PI * (1.0 - 1.0 / (2.0 * nFrequency_));
  for (UnsignedInteger index = 0; index < 2 * nFrequency_; ++index)
  {
    const Scalar theta = beta * index;
    alpha_[index] = factor * Complex(cos(theta), sin(theta));
  }
}

/* Realization accessor */
Field SpectralGaussianProcess::getRealization() const
{
  // Build the big collection of size dimension * number of frequencies
  const UnsignedInteger twoNF = 2 * nFrequency_;
  ComplexCollection arrayCollection(getOutputDimension() * twoNF);
  // Loop over the frequencies
  // Gaussian vector
  // Loop over half of the frequency range
  for (UnsignedInteger k = 0; k < nFrequency_; ++k)
  {
    const TriangularComplexMatrix choleskyFactor(getCholeskyFactor(k));
    // Use matrix/vector product to optimize the loop
    ComplexCollection left(getOutputDimension());
    ComplexCollection right(getOutputDimension());
    // Compute both the left and the right points using the current Cholesky factor R.
    // We use the relation S(-f)=conjugate(S(f)) from which R(-f)=conjugate(R(f))
    // and R(-f).z = conjugate(R(f).conjugate(z))
    // If z ~ N(0, 1) in C, then conjugate(z) ~ N(0, 1) in C, so there is no need to conjugate z
    // Complex gaussian realization
    for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
    {
      // Care! Getting a realization of a random gaussian should be done using two intermediate variables
      // Complex(DistFunc::rNormal(), DistFunc::rNormal()) is correct but the fill of the complex depends on the os and compiler
      const Scalar realLeft = DistFunc::rNormal();
      const Scalar imagLeft = DistFunc::rNormal();
      left[i] = Complex(realLeft, imagLeft);
      const Scalar realRight = DistFunc::rNormal();
      const Scalar imagRight = DistFunc::rNormal();
      right[i] = Complex(realRight, imagRight);
    }
    // Use an efficient matrix/vector product here
    ComplexCollection resultLeft(choleskyFactor * left);
    ComplexCollection resultRight(choleskyFactor * right);
    for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
    {
      arrayCollection[i * twoNF + nFrequency_ - 1 - k] = conj(resultLeft[i]);
      arrayCollection[i * twoNF + nFrequency_     + k] = resultRight[i];
    }
  } // Loop over the frequencies
  // From the big collection, build the inverse FFT by blocks
  Sample sampleValues(twoNF, getOutputDimension());
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
  {
    const ComplexCollection inverseFFTResult(fftAlgorithm_.inverseTransform(arrayCollection, i * twoNF, twoNF));
    for (UnsignedInteger k = 0; k < twoNF; ++k) sampleValues(k, i) = std::real(inverseFFTResult[k] * alpha_[k]);
  }
  sampleValues.setDescription(getDescription());
  return Field(mesh_, sampleValues);
}

/* Check if the process is stationary */
Bool SpectralGaussianProcess::isStationary() const
{
  return true;
}

/* Check if the process is Normal */
Bool SpectralGaussianProcess::isNormal() const
{
  return true;
}

/* Adapt a time grid in order to have a power of two time stamps. Both the starting point and the end point are preserved. */
RegularGrid SpectralGaussianProcess::AdaptGrid(const RegularGrid & grid)
{
  const Scalar start = grid.getStart();
  const Scalar end = grid.getEnd();
  UnsignedInteger powerOfTwo = SpecFunc::NextPowerOfTwo(grid.getN());
  return RegularGrid(start, (end - start) / powerOfTwo, powerOfTwo);
}

/* Method save() stores the object through the StorageManager */
void SpectralGaussianProcess::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute("spectralModel_", spectralModel_);
  adv.saveAttribute("maximalFrequency_", maximalFrequency_);
  adv.saveAttribute("nFrequency_", nFrequency_);
  adv.saveAttribute("choleskyFactorsCache_", choleskyFactorsCache_);
  adv.saveAttribute("alpha_", alpha_);
  adv.saveAttribute("fftAlgorithm_", fftAlgorithm_);
}

/* Method load() reloads the object from the StorageManager */
void SpectralGaussianProcess::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute("spectralModel_", spectralModel_);
  adv.loadAttribute("maximalFrequency_", maximalFrequency_);
  adv.loadAttribute("nFrequency_", nFrequency_);
  adv.loadAttribute("choleskyFactorsCache_", choleskyFactorsCache_);
  adv.loadAttribute("alpha_", alpha_);
  adv.loadAttribute("fftAlgorithm_", fftAlgorithm_);
}

END_NAMESPACE_OPENTURNS
