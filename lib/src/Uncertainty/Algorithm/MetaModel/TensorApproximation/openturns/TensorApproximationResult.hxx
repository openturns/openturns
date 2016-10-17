//                                               -*- C++ -*-
/**
 *  @brief The result of a tensor approximation
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
#ifndef OPENTURNS_TENSORAPPROXIMATIONRESULT_HXX
#define OPENTURNS_TENSORAPPROXIMATIONRESULT_HXX

#include "openturns/MetaModelResult.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/OrthogonalBasis.hxx"
#ifndef SWIG
#include "openturns/CanonicalTensor.hxx"
#endif

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class TensorApproximationResult
 *
 * The result of a tensor approximation
 */

class OT_API TensorApproximationResult
  : public MetaModelResult
{
  CLASSNAME;

public:

  typedef Collection<NumericalMathFunction>           NumericalMathFunctionCollection;
  typedef PersistentCollection<NumericalMathFunction> NumericalMathFunctionPersistentCollection;

  /** Default constructor */
  TensorApproximationResult();
#ifndef SWIG
  /** Parameter constructor */
  TensorApproximationResult(const Distribution & distribution,
                            const NumericalMathFunction & transformation,
                            const NumericalMathFunction & inverseTransformation,
                            const NumericalMathFunction & composedModel,
                            const Collection<CanonicalTensor> & tensor,
                            const NumericalPoint & residuals,
                            const NumericalPoint & relativeErrors);
#endif
  /** Virtual constructor */
  virtual TensorApproximationResult * clone() const;

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

  PersistentCollection<CanonicalTensor> tensor_;

  /** Composed meta model */
  NumericalMathFunction composedMetaModel_;

} ; /* class TensorApproximationResult */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TENSORAPPROXIMATIONRESULT_HXX */
