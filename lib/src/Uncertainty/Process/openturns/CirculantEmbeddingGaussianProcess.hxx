//                                               -*- C++ -*-
/**
 *  @brief A class which implements the CirculantEmbeddingGaussianProcess process
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CIRCULANTEMBEDDINGGAUSSIANPROCESS_HXX
#define OPENTURNS_CIRCULANTEMBEDDINGGAUSSIANPROCESS_HXX

#include "openturns/ProcessImplementation.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/FFT.hxx"
#include "openturns/Interval.hxx"
#include "openturns/SquareComplexMatrix.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CirculantEmbeddingGaussianProcess
 *
 * The implementation of the Gaussian process class
 * using circulant embedding (Davies-Harte algorithm)
 */
class OT_API CirculantEmbeddingGaussianProcess
  : public ProcessImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  CirculantEmbeddingGaussianProcess();

  /** Standard constructor */
  CirculantEmbeddingGaussianProcess(const CovarianceModel & covarianceModel,
                                    const Interval & interval,
                                    const Indices & discretization);

  /** Virtual constructor */
  CirculantEmbeddingGaussianProcess * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter  - pretty print */
  String __str__(const String & offset = "") const override;

  /** Mesh accessor */
  void setMesh(const Mesh & mesh) override;
  void setTimeGrid(const RegularGrid & timeGrid) override;

  /** Circular size accessor */
  void setCircularSize(const Indices & circularSize);
  Indices getCircularSize() const;

  /** Realization accessor */
  Field getRealization() const override;

  /** Covariance model accessor */
  CovarianceModel getCovarianceModel() const override;

  /** FFT algorithm accessors */
  FFT getFFTAlgorithm() const;
  void setFFTAlgorithm(const FFT & fft);

  /** Interval accessor */
  Interval getInterval() const;

  /** Discretization accessor */
  Indices getDiscretization() const;

  /** Check if the process is stationary */
  Bool isStationary() const override;

  /** Check if the process is Normal */
  Bool isNormal() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Initialization of the process */
  void initialize() const;

  /** Initialize for arbitrary dimension using transform */
  void initializeND() const;

  /** Get realization for arbitrary dimension using transform */
  void getRealizationND(Field & field) const;

  /** Compute flat index from multi-index */
  UnsignedInteger flatIndex(const Indices & index,
                            const Indices & strides) const;

  /** Compute strides for row-major layout */
  Indices computeStrides(const Indices & dimensions) const;

  /** Covariance model */
  CovarianceModel covarianceModel_;

  /** Eigenvalues for scalar output special case */
  mutable Point eigenvalues_;

  /** Cholesky factors of spectral density matrices */
  mutable PersistentCollection<SquareComplexMatrix> cholFactors_;

  /** Size of the circulant matrix per dimension (power of two) */
  mutable Indices circularSize_;

  /** Flag to manage process initialization */
  mutable Bool isInitialized_ = false;

  /** FFT Algorithm */
  FFT fftAlgorithm_;

  /** Spatial domain interval */
  Interval interval_;

  /** Number of cells per dimension */
  Indices discretization_;

  /** Spatial dimension */
  UnsignedInteger dimension_ = 1;

}; /* class CirculantEmbeddingGaussianProcess */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CIRCULANTEMBEDDINGGAUSSIANPROCESS_HXX */
