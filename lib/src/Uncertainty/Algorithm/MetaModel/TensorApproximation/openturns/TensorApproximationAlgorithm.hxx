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
#include "openturns/FunctionalChaosResult.hxx"
#include "openturns/OrthogonalProductFunctionFactory.hxx"

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
                               const Indices & nk);

  /** Virtual constructor */
  virtual TensorApproximationAlgorithm * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Computes the functional chaos */
  void run();

  /** Result accessor */
//   TensorApproximationResult getResult() const;

  /** Sample accessors */
  NumericalSample getInputSample() const;
  NumericalSample getOutputSample() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

  friend class Factory<TensorApproximationAlgorithm>;

  /** Default constructor */
  TensorApproximationAlgorithm();

private:

  /** Marginal computation */
  void runMarginal(const UnsignedInteger marginalIndex);

  void rankOneApproximation(RankOneTensor & rank1Tensor, const UnsignedInteger marginalIndex);

  /** The isoprobabilistic transformation maps the distribution into the orthogonal measure */
  NumericalMathFunction transformation_;

  /** The inverse isoprobabilistic transformation */
  NumericalMathFunction inverseTransformation_;

  /** The composed model */
  NumericalMathFunction composedModel_;

  /** Result of the projection */
  FunctionalChaosResult result_;

  NumericalSample inputSample_;
  NumericalSample outputSample_;

  NumericalSample transformedInputSample_;

  OrthogonalProductFunctionFactory basisFactory_;

  CanonicalTensor tensor_;
} ; /* class TensorApproximationAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TENSORAPPROXIMATIONALGORITHM_HXX */
