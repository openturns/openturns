//                                               -*- C++ -*-
/**
 *  @brief Tensor approximation algorithm
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
  CLASSNAME;

public:
  /** Constructor */
  TensorApproximationAlgorithm(const NumericalSample & inputSample,
                               const NumericalSample & outputSample,
                               const Distribution & distribution,
                               const OrthogonalProductFunctionFactory & functionFactory,
                               const Indices & nk,
                               const UnsignedInteger maxRank = 1);

  /** Virtual constructor */
  virtual TensorApproximationAlgorithm * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Computes the functional chaos */
  void run();

  /** Result accessor */
  TensorApproximationResult getResult() const;

  /** Sample accessors */
  NumericalSample getInputSample() const;
  NumericalSample getOutputSample() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  /** Max ALS iteration accessor */
  void setMaximumAlternatingLeastSquaresIteration(const UnsignedInteger maximumAlternatingLeastSquaresIteration);
  UnsignedInteger getMaximumAlternatingLeastSquaresIteration() const;

  /** Radius error accessor */
  void setMaximumRadiusError(const NumericalScalar maximumRadiusError);
  NumericalScalar getMaximumRadiusError() const;

  /** Residual error accessor */
  void setMaximumResidualError(const NumericalScalar maximumResidualError);
  NumericalScalar getMaximumResidualError() const;

protected:

  friend class Factory<TensorApproximationAlgorithm>;

  /** Default constructor */
  TensorApproximationAlgorithm();

private:

  /** Marginal computation */
  void runMarginal(const UnsignedInteger marginalIndex,
                   NumericalScalar & marginalResidual,
                   NumericalScalar & marginalRelativeError);

  /** Greedy rank-1 algorithm */
  void greedyRankOne(const NumericalSample & x,
                     const NumericalSample & y,
                     CanonicalTensorEvaluation & tensor,
                     NumericalScalar & marginalResidual,
                     NumericalScalar & marginalRelativeError);

  /** Alternating least-squares algorithm to estimate a rank-1 tensor */
  void rankOne(const NumericalSample & x,
               const NumericalSample & y,
               CanonicalTensorEvaluation & tensor,
               const UnsignedInteger k,
               NumericalScalar & marginalResidual,
               NumericalScalar & marginalRelativeError);

  /** Rank-M algorithm */
  void rankM (const NumericalSample & x,
              const NumericalSample & y,
              CanonicalTensorEvaluation & tensor,
              NumericalScalar & marginalResidual,
              NumericalScalar & marginalRelativeError);

  void rankMComponent (const NumericalSample & x,
              const NumericalSample & y,
              CanonicalTensorEvaluation & tensor,
              const UnsignedInteger j);

  /** The isoprobabilistic transformation maps the distribution into the orthogonal measure */
  Function transformation_;

  /** The inverse isoprobabilistic transformation */
  Function inverseTransformation_;

  /** The composed model */
  Function composedModel_;

  // samples
  NumericalSample inputSample_;
  NumericalSample outputSample_;

  UnsignedInteger maxRank_;

  NumericalSample transformedInputSample_;

  // tensorized basis
  OrthogonalProductFunctionFactory basisFactory_;

  // tensor format
  Collection<CanonicalTensorEvaluation> tensor_;

  // maximum rank-1 iterations
  UnsignedInteger maximumAlternatingLeastSquaresIteration_;
  // error on the radius for rank-1
  NumericalScalar maximumRadiusError_;
  // error on the residual for rank-1
  NumericalScalar maximumResidualError_;

  mutable Collection<DesignProxy> proxy_;

  /** Result of the projection */
  TensorApproximationResult result_;

} ; /* class TensorApproximationAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TENSORAPPROXIMATIONALGORITHM_HXX */
