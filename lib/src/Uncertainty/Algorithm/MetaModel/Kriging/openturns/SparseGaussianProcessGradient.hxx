//                                               -*- C++ -*-
/**
 *  @brief The class building sparse gaussian process regression gradient
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
#ifndef OPENTURNS_SPARSEGAUSSIANPROCESSGRADIENT_HXX
#define OPENTURNS_SPARSEGAUSSIANPROCESSGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Sample.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/SparseGaussianProcessFitterResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SparseGaussianProcessGradient
 *
 * This class computes the gradient of the sparse gaussian process prediction
 */
class OT_API SparseGaussianProcessGradient
  : public GradientImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  SparseGaussianProcessGradient();

  /** Constructor with parameters */
  SparseGaussianProcessGradient(const CovarianceModel & covarianceModel,
                                const Sample & inducingPoints,
                                const TriangularMatrix & whiteningFactor,
                                const Point & posteriorMean,
                                const HMatrix & whiteningFactorHMatrix = HMatrix(),
                                const SparseGaussianProcessFitterResult::LinearAlgebra linearAlgebraMethod = SparseGaussianProcessFitterResult::LAPACK);

  /** Virtual constructor */
  SparseGaussianProcessGradient * clone() const override;

  /** Comparison operator */
  using GradientImplementation::operator ==;
  Bool operator ==(const SparseGaussianProcessGradient & other) const;
protected:
  Bool equals(const GradientImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Gradient method */
  Matrix gradient(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /// Covariance model
  CovarianceModel covarianceModel_;

  /// Inducing points
  Sample inducingPoints_;

  /// Whitening factor (Cholesky factor of the inducing points covariance)
  TriangularMatrix whiteningFactor_;

  /// Whitening factor in HMAT form
  HMatrix whiteningFactorHMatrix_;

  /// Linear algebra method
  SparseGaussianProcessFitterResult::LinearAlgebra linearAlgebraMethod_ = SparseGaussianProcessFitterResult::LAPACK;

  /// Whitened variational posterior mean
  Point posteriorMean_;

}; /* class SparseGaussianProcessGradient */



END_NAMESPACE_OPENTURNS

#endif
