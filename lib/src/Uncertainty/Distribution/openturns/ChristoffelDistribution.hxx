//                                               -*- C++ -*-
/**
 *  @brief The Christoffel distribution associated to an orthonormal basis
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
#ifndef OPENTURNS_CHRISTOFFELDISTRIBUTION_HXX
#define OPENTURNS_CHRISTOFFELDISTRIBUTION_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/OrthogonalBasis.hxx"
#include "openturns/Basis.hxx"
#include "openturns/RatioOfUniforms.hxx"
#include "openturns/EnumerateFunction.hxx"
#include "openturns/OrthogonalUniVariatePolynomial.hxx"
#include "openturns/UniVariateFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ChristoffelDistribution
 *
 * The Christoffel distribution associated to the first size_ functions
 * of an orthonormal basis. Its density with respect to the Lebesgue
 * measure is p(x) * k(x) / size_, where p is the density of the
 * reference measure embedded in the basis and k is the Christoffel
 * function (sum of the squared basis functions).
 */
class OT_API ChristoffelDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ChristoffelDistribution();

  /** Parameters constructor */
  ChristoffelDistribution(const OrthogonalBasis & basis,
                          const UnsignedInteger size);

  /** Virtual constructor */
  ChristoffelDistribution * clone() const override;

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const ChristoffelDistribution & other) const;

  /** Quadrature wrapper over tensor slices needs the partial sums */
  friend class ChristoffelTensorSliceEvaluation;

  /** TBB policies evaluate the per-point cores directly */
  friend struct ComputeChristoffelPolicy;
  friend struct ComputeLogChristoffelPolicy;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Orthogonal basis and size accessor */
  void setOrthogonalBasis(const OrthogonalBasis & basis,
                          const UnsignedInteger size);
  OrthogonalBasis getOrthogonalBasis() const;

  /** Size accessor, ie the dimension of the approximation space */
  UnsignedInteger getSize() const;

  /** Reference measure accessor */
  Distribution getMeasure() const;

  /** Finite basis accessor (first size_ functions) */
  Basis getBasis() const;

  /** Christoffel function evaluation */
  Scalar computeChristoffelFunction(const Point & point) const;
  Sample computeChristoffelFunction(const Sample & sample) const;

  /** Stable log of the Christoffel function, for extreme tails */
  Scalar computeLogChristoffelFunction(const Point & point) const;
  Sample computeLogChristoffelFunction(const Sample & sample) const;

  /** Stability factor estimate, ie sup of the Christoffel function */
  Scalar computeKn() const;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Compute the PDF of Xj | X0..Xj-1, fast slice form for independent references */
  using DistributionImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const override;

  /** Compute the CDF of Xj | X0..Xj-1, slice quadrature in the tensor case */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;

  /** Sequential conditionals fanning out to the scalar versions */
  using DistributionImplementation::computeSequentialConditionalPDF;
  Point computeSequentialConditionalPDF(const Point & x) const override;
  using DistributionImplementation::computeSequentialConditionalCDF;
  Point computeSequentialConditionalCDF(const Point & x) const override;

  /** Realization and sample, through the internal sampler */
  Point getRealization() const override;
  Sample getSample(const UnsignedInteger size) const override;

  /** Parameters accessors (no parametric representation) */
  PointWithDescriptionCollection getParametersCollection() const override;
  using DistributionImplementation::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Compute the numerical range, ie the reference range */
  void computeRange() override;

  /** Comparison to another distribution */
  Bool equals(const DistributionImplementation & other) const override;

private:
  /** Internal constructor from a prebuilt finite basis, used by ChristoffelSubsampleExperiment */
  ChristoffelDistribution(const OrthogonalBasis & basis,
                          const Basis & finiteBasis,
                          const UnsignedInteger size);
  friend class ChristoffelSubsampleExperiment;

  /** Rebuild the derived members from orthogonalBasis_ and size_ */
  void update();

  /** Derive the range, tensor caches, parallel flag and sampler from basis_ and measure_ */
  void finalizeFiniteBasis();

  /** Detect a tensor-product factory and cache univariate evaluators */
  void detectTensorProduct();

  /** Univariate factor of a basis function along dimension k at x */
  Scalar computeTensorFactor(const UnsignedInteger basisIndex,
                             const UnsignedInteger k,
                             const Scalar x) const;

  /** Partial Christoffel sum over t in dimension j given partial products */
  Scalar computePartialChristoffel(const Point & partialProducts,
                                   const UnsignedInteger j,
                                   const Scalar t) const;

  /** Per-point direct sum of the squared basis functions */
  Scalar computeChristoffelCore(const Point & point) const;

  /** Per-point direct sum and stable log in a single basis pass */
  Scalar computeChristoffelCore(const Point & point,
                                Scalar & logCore) const;

  /** Per-point stable log by max factoring with log1p */
  Scalar computeLogChristoffelCore(const Point & point) const;

  /** Update the tensor partial products with the factor of dimension j */
  void updatePartialProducts(Point & partialProducts,
                             const UnsignedInteger j,
                             const Scalar x) const;

  /** Conditional PDF from precomputed partial products */
  Scalar computeConditionalPDFFromPartial(const Scalar x,
                                          const UnsignedInteger j,
                                          const Point & partialProducts) const;

  /** Conditional CDF from precomputed partial products */
  Scalar computeConditionalCDFFromPartial(const Scalar x,
                                          const UnsignedInteger j,
                                          const Point & partialProducts) const;

  /** Candidate points for the stability factor search */
  Sample generateKnCandidates(const UnsignedInteger size) const;

  /** One draw by sequential 1D conditional sampling (tensor independent case) */
  Point drawSequentialTensor() const;

  /** One draw by rejection from the reference with the Kn envelope */
  Point drawByRejection() const;

  /** Choose between the internal ratio-of-uniforms sampler and rejection by
      their measured acceptance rates, once the stability factor is known */
  Bool preferRatioOfUniforms() const;

  /** The orthogonal basis factory */
  OrthogonalBasis orthogonalBasis_;

  /** The dimension of the approximation space */
  UnsignedInteger size_ = 0;

  /** The reference measure embedded in the basis */
  Distribution measure_;

  /** The first size_ functions of the basis */
  Basis basis_;

  /** Internal sampler built from the log-PDF and the range */
  RatioOfUniforms sampler_;

  /** Whether the sampler completed its setup: a default-constructed sampler
      reports initialized with a dummy one-dimensional placeholder */
  Bool hasRatioUniformsSetup_ = false;

  /** Cached stability factor estimate and the ResourceMap values it used */
  mutable Scalar knEstimate_ = 0.0;
  mutable Bool isAlreadyComputedKn_ = false;
  mutable String knCacheKey_;

  /** Tensor-product structure detected in the factory */
  Bool isTensorProduct_ = false;

  /** Enumerate function mapping flat indices to multi-indices */
  EnumerateFunction enumerateFunction_;

  /** Cached multi-index of each basis function, filled by detectTensorProduct() */
  Collection<Indices> multiIndices_;

  /** Cached univariate polynomial evaluators [dimension][degree] */
  Collection<Collection<OrthogonalUniVariatePolynomial>> tensorPolynomials_;

  /** Cached univariate function evaluators [dimension][degree] */
  Collection<Collection<UniVariateFunction>> tensorFunctions_;

  /** True when the polynomial (vs function) cache is active */
  Bool usePolynomialTensor_ = false;

}; /* class ChristoffelDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CHRISTOFFELDISTRIBUTION_HXX */
