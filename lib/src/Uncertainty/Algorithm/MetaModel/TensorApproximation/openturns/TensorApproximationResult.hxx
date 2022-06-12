//                                               -*- C++ -*-
/**
 *  @brief The result of a tensor approximation
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
#ifndef OPENTURNS_TENSORAPPROXIMATIONRESULT_HXX
#define OPENTURNS_TENSORAPPROXIMATIONRESULT_HXX

#include "openturns/MetaModelResult.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/OrthogonalBasis.hxx"
#include "openturns/CanonicalTensorEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class TensorApproximationResult
 *
 * The result of a tensor approximation
 */

class OT_API TensorApproximationResult
  : public MetaModelResult
{
  CLASSNAME

public:

  typedef Collection<Function>           FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  /** Default constructor */
  TensorApproximationResult();

  /** Parameter constructor */
  TensorApproximationResult(const Distribution & distribution,
                            const Function & transformation,
                            const Function & inverseTransformation,
                            const Function & composedModel,
                            const Collection<CanonicalTensorEvaluation> & tensorCollection,
                            const Point & residuals,
                            const Point & relativeErrors);

  /** Virtual constructor */
  TensorApproximationResult * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Distribution accessor */
  virtual Distribution getDistribution() const;

  /** IsoProbabilisticTransformation accessor */
  virtual Function getTransformation() const;

  /** InverseIsoProbabilisticTransformation accessor */
  virtual Function getInverseTransformation() const;

  /** Composed model accessor */
  virtual Function getComposedModel() const;

  /** Composed meta model accessor */
  virtual Function getComposedMetaModel() const;

  CanonicalTensorEvaluation getTensor(const UnsignedInteger marginalIndex = 0) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:

private:
  /** The input vector distribution */
  Distribution distribution_;

  /** The isoprobabilistic transformation maps the distribution into the orthogonal measure */
  Function transformation_;

  /** The inverse isoprobabilistic transformation */
  Function inverseTransformation_;

  /** The composed model */
  Function composedModel_;

  PersistentCollection<CanonicalTensorEvaluation> tensorCollection_;

  /** Composed meta model */
  Function composedMetaModel_;

} ; /* class TensorApproximationResult */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TENSORAPPROXIMATIONRESULT_HXX */
