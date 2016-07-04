//                                               -*- C++ -*-
/**
 *  @brief Tensor approximation algorithm
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/MetaModelResult.hxx"
#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/CanonicalTensor.hxx"
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
  typedef Collection<OrthogonalUniVariateFunctionFamily> FunctionFamilyCollection;
  typedef PersistentCollection<OrthogonalUniVariateFunctionFamily> FunctionFamilyPersistentCollection;

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

  /** ApproximationAlgorithm accessor */
  void setApproximationAlgorithmFactory(const ApproximationAlgorithmImplementationFactory & factory);

  /** Max ALS iteration accessor */
  void setMaximumAlternatingLeastSquaresIteration(const UnsignedInteger maximumAlternatingLeastSquaresIteration);
  UnsignedInteger getMaximumAlternatingLeastSquaresIteration() const;

  /** Radius error accessor */
  void setMaximumRadiusError(const NumericalScalar maximumRadiusError);
  NumericalScalar getMaximumRadiusError() const;

  /** Residual error accessor */
  void setMaximumResidual(const NumericalScalar maximumResidual);
  NumericalScalar getMaximumResidual() const;

  /** Selection flag */
  void setRankSelection(const Bool rankSelection);

protected:

  friend class Factory<TensorApproximationAlgorithm>;

  /** Default constructor */
  TensorApproximationAlgorithm();

private:

  /** Marginal computation */
  void runMarginal(const UnsignedInteger marginalIndex,
                   NumericalScalar & marginalResidual,
                   NumericalScalar & marginalRelativeError);

  /** Compute all coefficients at once */
  void rankK (const UnsignedInteger marginalIndex,
              NumericalScalar & marginalResidual,
              NumericalScalar & marginalRelativeError);

  /** Greedy rank-1 with selection of the optimal rank*/
  void greedyRankOneSelection(const UnsignedInteger marginalIndex,
                              NumericalScalar & marginalResidual,
                              NumericalScalar & marginalRelativeError);

  /** Greedy rank-1 algorithm */
  void greedyRankOne(const UnsignedInteger marginalIndex,
                     const NumericalSample & x,
                     const NumericalSample & y,
                     NumericalScalar & marginalResidual,
                     NumericalScalar & marginalRelativeError);

  /** Alternating least-squares algorithm to estimate a rank-1 tensor */
  RankOneTensor rankOne(const UnsignedInteger marginalIndex,
                        const CanonicalTensor & canonicalTensor,
                        const NumericalSample & x,
                        const NumericalSample & y,
                        NumericalScalar & marginalResidual,
                        NumericalScalar & marginalRelativeError);

  /** The isoprobabilistic transformation maps the distribution into the orthogonal measure */
  NumericalMathFunction transformation_;

  /** The inverse isoprobabilistic transformation */
  NumericalMathFunction inverseTransformation_;

  /** The composed model */
  NumericalMathFunction composedModel_;

  /** Result of the projection */
  TensorApproximationResult result_;

  // samples
  NumericalSample inputSample_;
  NumericalSample outputSample_;

  NumericalSample transformedInputSample_;

  // tensorized basis
  OrthogonalProductFunctionFactory basisFactory_;

  // tensor format
  Collection<CanonicalTensor> tensor_;

  // factory for rank-1 approximation: allows to select plain or sparse
  Pointer<ApproximationAlgorithmImplementationFactory> p_approximationAlgorithmImplementationFactory_;

  // maximum rank-1 iterations
  UnsignedInteger maximumAlternatingLeastSquaresIteration_;
  // error on the radius for rank-1
  NumericalScalar maximumRadiusError_;
  // error on the residual for rank-1
  NumericalScalar maximumResidual_;

  mutable Collection<DesignProxy> proxy_;

  // rank selection or not
  Bool rankSelection_;

} ; /* class TensorApproximationAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TENSORAPPROXIMATIONALGORITHM_HXX */
