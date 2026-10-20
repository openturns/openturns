//                                               -*- C++ -*-
/**
 *  @brief Christoffel subsample experiment with prescribed size
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
#ifndef OPENTURNS_CHRISTOFFELSUBSAMPLEEXPERIMENT_HXX
#define OPENTURNS_CHRISTOFFELSUBSAMPLEEXPERIMENT_HXX

#include "openturns/WeightedExperimentImplementation.hxx"
#include "openturns/OrthogonalBasis.hxx"
#include "openturns/ChristoffelDistribution.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SymmetricMatrix.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ChristoffelSubsampleExperiment
 *
 * Weighted experiment drawing size_ points from the Christoffel
 * distribution associated to the first spaceDimension_ functions
 * of an orthogonal basis, with importance weights w = m / k_m.
 * The space dimension m = n-k is the largest dimension supplied by
 * the basis: k=0 with an inexhaustible basis, and k=1,2,... is tried
 * until the basis supplies m functions. Fresh designs use the exact
 * projection DPP of size m, completed by independent Christoffel
 * points when n > m. Supplied points are either volume-sampled when
 * there are at least n of them, or used as the conditioning set of a
 * projection DPP when there are at most m of them.
 */
class OT_API ChristoffelSubsampleExperiment
  : public WeightedExperimentImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ChristoffelSubsampleExperiment();

  /** Parameters constructor */
  ChristoffelSubsampleExperiment(const OrthogonalBasis & basis,
                                 const UnsignedInteger size);

  /** Virtual constructor */
  ChristoffelSubsampleExperiment * clone() const override;

  /** Comparison operator */
  using WeightedExperimentImplementation::operator ==;
  Bool operator ==(const ChristoffelSubsampleExperiment & other) const;

  /** String converter */
  String __repr__() const override;

  /** Orthogonal basis accessor */
  void setOrthogonalBasis(const OrthogonalBasis & basis);
  OrthogonalBasis getOrthogonalBasis() const;

  /** Size accessor, ie the number of points of the experiment */
  void setSize(const UnsignedInteger size) override;
  UnsignedInteger getSize() const override;

  /** Space dimension accessor, ie the number of basis functions used */
  UnsignedInteger getSpaceDimension() const;

  /** Reference distribution accessor (read-only, embedded in the basis) */
  void setDistribution(const Distribution & distribution) override;

  /** Deduce the space dimension from the target size */
  static UnsignedInteger DeduceSpaceDimension(const UnsignedInteger size);

  /** Eigenvalues of the weighted design Gramian, the frame stability certificate */
  Point computeDesignEigenvalues(const Sample & sample, const Point & weights) const;

  /** Uniform weights ? */
  Bool hasUniformWeights() const override;

  /** Random experiment ? */
  Bool isRandom() const override;

  /** Sample generation with weights */
  Sample generateWithWeights(Point & weightsOut) const override;

  /** Sample generation with weights from supplied points */
  Sample generateWithWeights(const Sample & suppliedSample, Point & weightsOut) const;
  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  Bool equals(const WeightedExperimentImplementation & other) const override;

  /** Draw fresh Christoffel points */
  Sample drawChristoffelPoints(const UnsignedInteger count) const;

  /** Density ratios of design points, reference over Christoffel law */
  Point computeImportanceWeights(const Sample & sample) const;

private:
  /** Rebuild the derived members from basis_ and size_ */
  void update();

  /** Gramian of a subset of sqrt(w) * phi, via BLAS */
  static SymmetricMatrix ComputeGramian(const Matrix & features,
                                        const Point & designWeights,
                                        const Indices & kept);

  /** Exact volume sample of the given size from the rows of a feature matrix */
  static Indices ComputeVolumeSample(const Matrix & features,
                                     const UnsignedInteger size);

  /** Raw finite-basis features of a sample, one row per point */
  Matrix computeFeatures(const Sample & sample,
                         const Bool checkFinite = true) const;

  /** Exact projection determinantal point process sample, optionally
      conditioned on supplied points */
  Sample drawProjectionDPP(const UnsignedInteger size,
                           const Sample & fixed = Sample()) const;

  /** Warn when the weighted design misses the frame bounds */
  void checkFrame(const Sample & sample,
                  const Point & weights) const;

  /** Identity index set of given size */
  static Indices IdentityIndices(const UnsignedInteger size);

  /** The orthogonal basis defining the approximation space */
  OrthogonalBasis basis_;

  /** The number of basis functions used */
  UnsignedInteger spaceDimension_ = 0;

  /** The Christoffel distribution sampled by the experiment */
  ChristoffelDistribution christoffelDistribution_;

}; /* class ChristoffelSubsampleExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CHRISTOFFELSUBSAMPLEEXPERIMENT_HXX */
