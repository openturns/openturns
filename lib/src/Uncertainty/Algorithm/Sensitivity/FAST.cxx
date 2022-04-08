//                                               -*- C++ -*-
/**
 * @brief FAST implements the sensivity analysis method based on fourier decomposition
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

#include <cmath>

#include "openturns/FAST.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/FFT.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Constructor with model */
FAST::FAST(const Function & model,
           const Distribution & inputsDistribution,
           const UnsignedInteger samplingSize,
           const UnsignedInteger resamplingSize,
           const UnsignedInteger interferenceFactor)
  : model_(model)
  , inputsDistribution_(inputsDistribution)
  , samplingSize_(samplingSize)
  , blockSize_(1)
  , resamplingSize_(resamplingSize)
  , interferenceFactor_(interferenceFactor)
  , alreadyComputedIndices_(false)
{
  if (inputsDistribution_.getDimension() != model_.getInputDimension())
    throw InvalidArgumentException(HERE) << "Error: The distribution's dimension " << inputsDistribution_.getDimension()
                                         << " must be equal to the model's number of inputs " << model_.getInputDimension() << ".";
  if (resamplingSize_ < 1) throw InvalidArgumentException(HERE) << "Error: The number of resamplings must be greater or equal than 0, here Nr=" << resamplingSize_ << ".";

  if (interferenceFactor_ < 1) throw InvalidArgumentException(HERE) << "Error: The interference factor is necessarily greater than 0, here M=" << interferenceFactor_ << ".";

  if (samplingSize_ - 1 < 4 * interferenceFactor_ * interferenceFactor_ )
    throw InvalidArgumentException(HERE) << "Error: It is necessary that 4*M^2 <= N-1 to compute a valid set of frequencies.";

  if (!inputsDistribution.hasIndependentCopula())
    throw InvalidArgumentException(HERE) << "Error: Cannot use FAST method with distributions having a non-independent copula.";
}

/* Compute all the FAST indices */
void FAST::run() const
{
  // Model dimensions
  const UnsignedInteger nbIn = model_.getInputDimension();
  const UnsignedInteger nbOut = model_.getOutputDimension();

  // Allocate indices
  firstOrderIndice_ = Sample(nbOut, nbIn);
  totalOrderIndice_ = Sample(nbOut, nbIn);

  // this avoids to store huge input samples while allowing for multi-threading
  const UnsignedInteger maximumOuterSampling = static_cast<UnsignedInteger>(ceil(1.0 * samplingSize_ / blockSize_));
  const UnsignedInteger modulo = samplingSize_ % blockSize_;
  const UnsignedInteger lastBlockSize = modulo == 0 ? blockSize_ : modulo;

  // S-space discretization
  Point s(samplingSize_);
  for (UnsignedInteger i = 1; i < samplingSize_; ++ i) s[i] = 2. * M_PI * i / samplingSize_;

  // Set of frequencies definition
  Indices w_i_0(nbIn);
  UnsignedInteger omega = (samplingSize_ - 1) / (2 * interferenceFactor_);
  //  omega_{-i} = omega / (2 * interferenceFactor_):;
  UnsignedInteger max_w_l = (samplingSize_ - 1) / (4 * interferenceFactor_ * interferenceFactor_);

  w_i_0[0] = omega;
  if ( max_w_l >= nbIn - 1 )
  {
    const Scalar step = (max_w_l - 1.) / (nbIn - 2.);
    for (UnsignedInteger inp = 0; inp < nbIn - 1; ++ inp)
      w_i_0[inp + 1] = inp * step + 1;
    w_i_0[nbIn - 1] = max_w_l;
  }
  else
  {
    for (UnsignedInteger inp = 0; inp < nbIn - 1; ++ inp)
      w_i_0[inp + 1] = inp % max_w_l + 1;
  }

  // Initializations
  Sample D_i(nbOut, nbIn);
  Sample D_l(nbOut, nbIn);

  // For each input, compute first order and total order indices for each model's output
  for (UnsignedInteger inp = 0; inp < nbIn; ++ inp)
  {
    Point D(nbOut, 0.);

    // Frequencies assignment
    Indices w_i(w_i_0);
    w_i[inp] = omega;
    for (UnsignedInteger i = 0; i < inp; ++ i)
      w_i[i] = w_i_0[i + 1];

    // Loop of resampling
    for (UnsignedInteger t = 0; t < resamplingSize_; ++ t)
    {
      // Random phase-shift
      Point phi_i(nbIn);
      for (UnsignedInteger i = 0; i < nbIn; ++ i)
        phi_i[i] = 2. * M_PI * RandomGenerator::Generate();

      Point xi_s(nbIn);
      Sample output(0, nbOut);

      // for each block ...
      for (UnsignedInteger outerSampling = 0; outerSampling < maximumOuterSampling; ++ outerSampling)
      {
        // the last block can be smaller
        const UnsignedInteger effectiveBlockSize = outerSampling < (maximumOuterSampling - 1) ? blockSize_ : lastBlockSize;

        Sample inputBlock(effectiveBlockSize, nbIn);

        for (UnsignedInteger blockIndex = 0; blockIndex < effectiveBlockSize; ++ blockIndex)
        {
          // Search-curve x_i(s)=g_i(w_i,s) definition
          for (UnsignedInteger i = 0; i < nbIn; ++ i)
          {
            const Scalar ui_s = 0.5 + std::asin(std::sin(w_i[i] * s[outerSampling * blockSize_ + blockIndex] + phi_i[i])) / M_PI;
            inputBlock(blockIndex, i) = inputsDistribution_.getMarginal(i).computeQuantile(ui_s)[0];
          }
        }
        output.add(model_(inputBlock));
      }

      // For each model's output
      for (UnsignedInteger out = 0; out < nbOut; ++ out)
      {
        // Fourier transformation
        Point y(output.getMarginal(out).getImplementation()->getData());
        ComplexCollection coefficients(fftAlgorithm_.transform(y));

        // Total variance
        for (UnsignedInteger j = 0; j < (samplingSize_ - 1) / 2; ++ j)
          D[out] += std::norm(coefficients[j + 1]);

        // Partial variance of all factors except the factor of interest
        for (UnsignedInteger j = 0; j < omega / 2; ++ j)
          D_l(out, inp) += std::norm(coefficients[j + 1]);

        // Partial variance of the factor of interest
        for (UnsignedInteger j = 0; j < interferenceFactor_; ++ j)
          D_i(out, inp) += std::norm(coefficients[(j + 1) * omega]);
      }
    }
    // When all resamplings are realised: save the indices
    for ( UnsignedInteger out = 0; out < nbOut; ++ out )
    {
      firstOrderIndice_(out, inp) = D_i(out, inp) / D[out];
      totalOrderIndice_(out, inp) = 1. - D_l(out, inp) / D[out];
    }
  }
  alreadyComputedIndices_ = true;
}

/* First order indices accessor */
Point FAST::getFirstOrderIndices(const UnsignedInteger marginalIndex) const
{
  if (!alreadyComputedIndices_) run();

  if (marginalIndex >= firstOrderIndice_.getSize()) throw InvalidArgumentException(HERE) << "Output dimension is " << firstOrderIndice_.getSize();
  return firstOrderIndice_[marginalIndex];
}

/* Total order indices accessor */
Point FAST::getTotalOrderIndices(const UnsignedInteger marginalIndex) const
{
  if (!alreadyComputedIndices_) run();
  if (marginalIndex >= totalOrderIndice_.getSize()) throw InvalidArgumentException(HERE) << "Output dimension is " << totalOrderIndice_.getSize();
  return totalOrderIndice_[marginalIndex];
}


/* FFT algorithm accessor */
FFT FAST::getFFTAlgorithm() const
{
  return fftAlgorithm_;
}

void FAST::setFFTAlgorithm(const FFT & fft)
{
  fftAlgorithm_ = fft;
}

/* Block size accessor */
void FAST::setBlockSize(const UnsignedInteger blockSize)
{
  blockSize_ = blockSize;
}

UnsignedInteger FAST::getBlockSize() const
{
  return blockSize_;
}

END_NAMESPACE_OPENTURNS
