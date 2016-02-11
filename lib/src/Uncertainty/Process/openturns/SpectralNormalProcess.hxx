//                                               -*- C++ -*-
/**
 *  @brief A class which implements the SpectralNormalProcess process
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
 */
#ifndef OPENTURNS_SPECTRALNORMALPROCESS_HXX
#define OPENTURNS_SPECTRALNORMALPROCESS_HXX

#include "ProcessImplementation.hxx"
#include "SpectralModel.hxx"
#include "SecondOrderModel.hxx"
#include "TriangularComplexMatrix.hxx"
#include "PersistentCollection.hxx"
#include "FFT.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SpectralNormalProcess
 *
 * The implementation of the Normal process class
 * in spectral domain
 */
class OT_API SpectralNormalProcess
  : public ProcessImplementation
{
  CLASSNAME;

public:

  /** Some typedefs to ease reading */
  typedef PersistentCollection<TriangularComplexMatrix>    TriangularComplexMatrixPersistentCollection;
  typedef PersistentCollection<NumericalComplex>           PersistentNumericalComplexCollection;
  typedef Collection<NumericalComplex>                     NumericalComplexCollection;

  /** Default constructor */
  SpectralNormalProcess();

  /** Standard constructor  - The timeGrid imposes the frequencies values*/
  SpectralNormalProcess(const SecondOrderModel & model,
                        const RegularGrid & grid);

  /** Standard constructor with spectralModel - The timeGrid imposes the frequencies values*/
  SpectralNormalProcess(const SpectralModel & spectralModel,
                        const RegularGrid & grid);

  /** Specific constructor  - The frequency parameters impose the time grid */
  SpectralNormalProcess(const SecondOrderModel & model,
                        const NumericalScalar maximalFrequency,
                        const UnsignedInteger nFrequency);

  /** Specific constructor  - The frequency parameters impose the time grid */
  SpectralNormalProcess(const SpectralModel & spectralModel,
                        const NumericalScalar maximalFrequency,
                        const UnsignedInteger nFrequency);

  /** Virtual constructor */
  virtual SpectralNormalProcess * clone() const;

  /** String converter */
  String __repr__() const;

  /** String converter  - pretty print */
  String __str__(const String & offset = "") const;

  /** Realization accessor */
  Field getRealization() const;

  /** Frequency grid accessor, covering both the negative and the positive axes */
  RegularGrid getFrequencyGrid() const;

  /** Maximal frequency accessor */
  NumericalScalar getMaximalFrequency() const;

  /** Number of frequency steps accessor */
  UnsignedInteger getNFrequency() const;

  /** Frequency step accessor */
  NumericalScalar getFrequencyStep() const;

  /** Spectral model accessor */
  SpectralModel getSpectralModel() const;

  /** Set TimeGrid */
  void setTimeGrid(const RegularGrid & timeGrid);

  /** FFT algorithm accessors */
  FFT getFFTAlgorithm() const;
  void setFFTAlgorithm(const FFT & fft);

  /** Check if the process is stationary */
  Bool isStationary() const;

  /** Check if the process is Normal */
  Bool isNormal() const;

  /** Adapt a time grid in order to have a power of two time stamps. Both the starting point and the end point are preserved. */
  static RegularGrid AdaptGrid(const RegularGrid & grid);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Initialization of the process */
  void initialize() const;

  /** Compute the time grid associated to the frequency grid */
  void computeTimeGrid();

  /** Set the alpha vector */
  void computeAlpha();

  /** Get the Cholesky factor of the kth DSP matrix from cache or computed on the fly */
  TriangularComplexMatrix getCholeskyFactor(const UnsignedInteger k) const;

  /** Compute the Cholesky factor of the kth DSP matrix */
  TriangularComplexMatrix computeCholeskyFactor(const UnsignedInteger k) const;

private:

  /** Spectral model associated to the process */
  SpectralModel spectralModel_;

  /** Maximal frequency */
  NumericalScalar maximalFrequency_;

  /** Number of frequencies on the positive axis */
  UnsignedInteger nFrequency_;

  /** Frequency step */
  NumericalScalar frequencyStep_;

  /** Cholesky factor  */
  mutable TriangularComplexMatrixPersistentCollection choleskyFactorsCache_;

  /** Cache size */
  mutable UnsignedInteger cacheSize_;

  /** alpha vector - scale factor */
  PersistentNumericalComplexCollection alpha_;

  /** FFT Algorithm */
  FFT fftAlgorithm_;

}; /* class SpectralNormalProcess */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECTRALNORMALPROCESS_HXX */
