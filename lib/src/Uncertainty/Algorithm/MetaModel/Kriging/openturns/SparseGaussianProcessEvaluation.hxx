//                                               -*- C++ -*-
/**
 *  @brief The class building sparse gaussian process evaluation
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
#ifndef OPENTURNS_SPARSEGAUSSIANPROCESSEVALUATION_HXX
#define OPENTURNS_SPARSEGAUSSIANPROCESSEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/Sample.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/SparseGaussianProcessFitterResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SparseGaussianProcessEvaluation
 *
 * This class permits prediction on a sparse gaussian process
 */
class OT_API SparseGaussianProcessEvaluation
  : public EvaluationImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  SparseGaussianProcessEvaluation();

  /** Constructor with parameters */
  SparseGaussianProcessEvaluation(const CovarianceModel & covarianceModel,
                                  const Sample & inducingPoints,
                                  const TriangularMatrix & whiteningFactor,
                                  const Point & posteriorMean,
                                  const CovarianceMatrix & posteriorCovariance,
                                  const HMatrix & whiteningFactorHMatrix = HMatrix(),
                                  const SparseGaussianProcessFitterResult::LinearAlgebra linearAlgebraMethod = SparseGaussianProcessFitterResult::LAPACK);

  /** Virtual constructor */
  SparseGaussianProcessEvaluation * clone() const override;

  /** Comparison operator */
  using EvaluationImplementation::operator ==;
  Bool operator ==(const SparseGaussianProcessEvaluation & other) const;
protected:
  Bool equals(const EvaluationImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  Point operator()(const Point & inP) const override;
  Sample operator()(const Sample & inS) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Conditional variance accessor */
  Scalar getConditionalVariance(const Point & inP) const;
  Point getConditionalVariance(const Sample & inS) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /// Whitened cross-covariance a(x) = Luu^{-1} k(Z, x)
  Point computeWhitenedCovariance(const Point & inP) const;

  /// Covariance model
  CovarianceModel covarianceModel_;

  /// Inducing points
  Sample inducingPoints_;

  /// Whitening factor (Cholesky factor of the inducing points covariance)
  TriangularMatrix whiteningFactor_;

  /// Whitening factor in HMAT form (Cholesky factor of the inducing points covariance)
  HMatrix whiteningFactorHMatrix_;

  /// Linear algebra method
  SparseGaussianProcessFitterResult::LinearAlgebra linearAlgebraMethod_ = SparseGaussianProcessFitterResult::LAPACK;

  /// Whitened variational posterior mean
  Point posteriorMean_;

  /// Whitened variational posterior covariance
  CovarianceMatrix posteriorCovariance_;

}; /* class SparseGaussianProcessEvaluation */



END_NAMESPACE_OPENTURNS

#endif
