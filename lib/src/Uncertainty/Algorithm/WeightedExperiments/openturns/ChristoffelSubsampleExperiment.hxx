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
 * of an orthogonal basis, with weights w = m / k_m. The space
 * dimension m is deduced from the target size n by
 * n = gamma * m * log(m).
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

  /** Gramian of a subset: mean over kept of w * phi * phi^T, via BLAS */
  static SymmetricMatrix ComputeGramian(const Matrix & features,
                                        const Point & weights,
                                        const Indices & kept);

  /** Uniform weights ? */
  Bool hasUniformWeights() const override;

  /** Random experiment ? */
  Bool isRandom() const override;

  /** Sample generation with weights */
  Sample generateWithWeights(Point & weightsOut) const override;

  /** Sample generation with weights from a pool prepended with existing points */
  Sample generateWithWeights(const Sample & poolSample, Point & weightsOut) const;
  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  Bool equals(const WeightedExperimentImplementation & other) const override;

  /** Target pool size from the oversampling factor */
  UnsignedInteger computePoolSize() const;

  /** Draw fresh pool points from the Christoffel law */
  Sample drawPool(const UnsignedInteger count) const;

  /** Density ratios of pool points, reference over Christoffel law */
  Point computePoolWeights(const Sample & pool) const;

  /** Thin a pool to size_, weigh and frame-check the design */
  Sample finalizeDesign(const Sample & pool,
                        const Point & poolWeights,
                        Point & weightsOut) const;

private:
  /** Rebuild the derived members from basis_ and size_ */
  void update();
  /** Forward barrier greedy selection: kept indices plus barrier weights */
  void barrierSelection(const Sample & pool,
                        const Point & poolWeights,
                        const Matrix & features,
                        Indices & keptOut,
                        Point & barrierWeightsOut) const;

  /** Greedy removal of pool points down to size_, keeping the largest min eigenvalue */
  Indices thinIndices(const Sample & pool,
                      const Point & poolWeights,
                      const Matrix & features) const;

  /** Finite basis evaluations on a pool, shared by the thinning step and the frame check */
  Matrix computePoolFeatures(const Sample & pool) const;

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
