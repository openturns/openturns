//                                               -*- C++ -*-
/**
 *  @brief The result of a chaos expansion
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
#ifndef OPENTURNS_FUNCTIONALCHAOSRESULT_HXX
#define OPENTURNS_FUNCTIONALCHAOSRESULT_HXX

#include "openturns/MetaModelResult.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/OrthogonalBasis.hxx"


BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FunctionalChaosResult
 *
 * The result of a chaos expansion
 */

class OT_API FunctionalChaosResult
  : public MetaModelResult
{
  CLASSNAME;

public:

  typedef Collection<NumericalMathFunction>           NumericalMathFunctionCollection;
  typedef PersistentCollection<NumericalMathFunction> NumericalMathFunctionPersistentCollection;

  // friend class Factory<FunctionalChaosResult>;

  /** Default constructor */
  FunctionalChaosResult();

  /** Parameter constructor */
  FunctionalChaosResult(const NumericalMathFunction & model,
                        const Distribution & distribution,
                        const NumericalMathFunction & transformation,
                        const NumericalMathFunction & inverseTransformation,
                        const NumericalMathFunction & composedModel,
                        const OrthogonalBasis & orthogonalBasis,
                        const Indices & I,
                        const NumericalSample & alpha_k,
                        const NumericalMathFunctionCollection & Psi_k,
                        const NumericalPoint & residuals,
                        const NumericalPoint & relativeErrors);

  /** Virtual constructor */
  virtual FunctionalChaosResult * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Distribution accessor */
  virtual Distribution getDistribution() const;

  /** IsoProbabilisticTransformation accessor */
  virtual NumericalMathFunction getTransformation() const;

  /** InverseIsoProbabilisticTransformation accessor */
  virtual NumericalMathFunction getInverseTransformation() const;

  /** Composed model accessor */
  virtual NumericalMathFunction getComposedModel() const;

  /** Orthogonal basis accessor */
  virtual OrthogonalBasis getOrthogonalBasis() const;

  /** Indices accessor */
  virtual Indices getIndices() const;

  /** Coefficients accessor */
  virtual NumericalSample getCoefficients() const;

  /** Reduced basis accessor */
  virtual NumericalMathFunctionCollection getReducedBasis() const;

  /** Composed meta model accessor */
  virtual NumericalMathFunction getComposedMetaModel() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

private:
  /** The input vector distribution */
  Distribution distribution_;

  /** The isoprobabilistic transformation maps the distribution into the orthogonal measure */
  NumericalMathFunction transformation_;

  /** The inverse isoprobabilistic transformation */
  NumericalMathFunction inverseTransformation_;

  /** The composed model */
  NumericalMathFunction composedModel_;

  /** The orthogonal basis */
  OrthogonalBasis orthogonalBasis_;

  /** The collection of indices */
  Indices I_;

  /** The collection of Alpha_k coefficients */
  NumericalSample alpha_k_;

  /** The collection of vectors upon which we project the (composed) model */
  NumericalMathFunctionPersistentCollection Psi_k_;

  /** Composed meta model */
  NumericalMathFunction composedMetaModel_;

} ; /* class FunctionalChaosResult */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FUNCTIONALCHAOSRESULT_HXX */
