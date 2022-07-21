//                                               -*- C++ -*-
/**
 *  @brief Tensor approximation algorithm
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
#ifndef OPENTURNS_TENSORAPPROXIMATIONALGORITHM_HXX
#define OPENTURNS_TENSORAPPROXIMATIONALGORITHM_HXX

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/CanonicalTensorEvaluation.hxx"
#include "openturns/TensorApproximationResult.hxx"
#include "openturns/OrthogonalProductFunctionFactory.hxx"
#include "openturns/ApproximationAlgorithmImplementationFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class TensorApproximationAlgorithm
 *
 * Tensor approximation algorithm
 */

class OT_API TensorApproximationAlgorithm
  : public MetaModelAlgorithm
{
  CLASSNAME

public:
  /** Default constructor */
  TensorApproximationAlgorithm();

  /** Constructor */
  TensorApproximationAlgorithm(const Sample & inputSample,
                               const Sample & outputSample,
                               const Distribution & distribution,
                               const OrthogonalProductFunctionFactory & functionFactory,
                               const Indices & nk,
                               const UnsignedInteger maxRank = 1);

  /** Virtual constructor */
  TensorApproximationAlgorithm * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Computes the functional chaos */
  void run() override;

  /** Result accessor */
  TensorApproximationResult getResult() const;

  /** Sample accessors */
  Sample getInputSample() const override;
  Sample getOutputSample() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Max ALS iteration accessor */
  void setMaximumAlternatingLeastSquaresIteration(const UnsignedInteger maximumAlternatingLeastSquaresIteration);
  UnsignedInteger getMaximumAlternatingLeastSquaresIteration() const;

  /** Radius error accessor */
  void setMaximumRadiusError(const Scalar maximumRadiusError);
  Scalar getMaximumRadiusError() const;

  /** Residual error accessor */
  void setMaximumResidualError(const Scalar maximumResidualError);
  Scalar getMaximumResidualError() const;

private:

  /** Marginal computation */
  void runMarginal(const UnsignedInteger marginalIndex,
                   Scalar & marginalResidual,
                   Scalar & marginalRelativeError);

  /** Greedy rank-1 algorithm */
  void greedyRankOne(const Sample & x,
                     const Sample & y,
                     CanonicalTensorEvaluation & tensor,
                     Scalar & marginalResidual,
                     Scalar & marginalRelativeError);

  /** Alternating least-squares algorithm to estimate a rank-1 tensor */
  void rankOne(const Sample & x,
               const Sample & y,
               CanonicalTensorEvaluation & tensor,
               const UnsignedInteger k,
               Scalar & marginalResidual,
               Scalar & marginalRelativeError);

  /** Rank-M algorithm */
  void rankM (const Sample & x,
              const Sample & y,
              CanonicalTensorEvaluation & tensor,
              Scalar & marginalResidual,
              Scalar & marginalRelativeError);

  void rankMComponent (const Sample & x,
                       const Sample & y,
                       CanonicalTensorEvaluation & tensor,
                       const UnsignedInteger j);

  /** The isoprobabilistic transformation maps the distribution into the orthogonal measure */
  Function transformation_;

  /** The inverse isoprobabilistic transformation */
  Function inverseTransformation_;

  /** The composed model */
  Function composedModel_;

  // samples
  Sample inputSample_;
  Sample outputSample_;

  UnsignedInteger maxRank_;

  Sample transformedInputSample_;

  // tensorized basis
  OrthogonalProductFunctionFactory basisFactory_;

  // tensor format
  Collection<CanonicalTensorEvaluation> tensor_;

  // maximum rank-1 iterations
  UnsignedInteger maximumAlternatingLeastSquaresIteration_;
  // error on the radius for rank-1
  Scalar maximumRadiusError_;
  // error on the residual for rank-1
  Scalar maximumResidualError_;

  mutable Collection<DesignProxy> proxy_;

  /** Result of the projection */
  TensorApproximationResult result_;

} ; /* class TensorApproximationAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TENSORAPPROXIMATIONALGORITHM_HXX */
