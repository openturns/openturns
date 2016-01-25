//                                               -*- C++ -*-
/**
 *  @brief Result of a Meta-model algorithm
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
#ifndef OPENTURNS_METAMODELRESULT_HXX
#define OPENTURNS_METAMODELRESULT_HXX

#include "PersistentObject.hxx"
#include "NumericalMathFunction.hxx"
#include "NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class MetaModelResult
 * Implementation of SimulationResult
 */
class OT_API MetaModelResult
  : public PersistentObject
{

  CLASSNAME;
public:


  /** Default constructor */
  MetaModelResult();

  /** Standard constructor */
  MetaModelResult(const NumericalMathFunction & model,
                  const NumericalMathFunction & metaModel,
                  const NumericalPoint & residuals,
                  const NumericalPoint & relativeErrors);

  /** Virtual constructor */
  virtual MetaModelResult * clone() const;

  /** Model accessor */
  virtual void setModel(const NumericalMathFunction & model);
  virtual NumericalMathFunction getModel() const;

  /** MetaModel accessor */
  virtual void setMetaModel(const NumericalMathFunction & metaModel);
  virtual NumericalMathFunction getMetaModel() const;

  /** Marginal residuals accessor */
  virtual void setResiduals(const NumericalPoint & residuals);
  virtual NumericalPoint getResiduals() const;

  /** Marginal relative errors accessor */
  virtual void setRelativeErrors(const NumericalPoint & relativeErrors);
  virtual NumericalPoint getRelativeErrors() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  // The initial model
  NumericalMathFunction model_;

  // The corresponding meta-model
  NumericalMathFunction metaModel_;

  // The marginal residuals
  NumericalPoint residuals_;

  // The marginal relative errors
  NumericalPoint relativeErrors_;

}; // class MetaModelResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_METAMODELRESULT_HXX */
