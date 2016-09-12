//                                               -*- C++ -*-
/**
 *  @brief Implementation for metamodel validation
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
#ifndef OPENTURNS_METAMODELVALIDATION_HXX
#define OPENTURNS_METAMODELVALIDATION_HXX

#include "openturns/NumericalPoint.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class MetaModelValidation
 *
 */

class OT_API MetaModelValidation
  : public PersistentObject
{
  CLASSNAME;
public:


  /** Default constructor */
  MetaModelValidation();

  /** Constructor with parameters */
  MetaModelValidation(const NumericalSample & inputSample,
                      const NumericalSample & outputSample,
                      const NumericalMathFunction & metaModel);

  /** Virtual constructor */
  virtual MetaModelValidation * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Sample accessors */
  virtual NumericalSample getInputSample() const;
  virtual NumericalSample getOutputSample() const;

  /** Compute predictivity factor */
  NumericalScalar computePredictivityFactor() const;

  /** Get residual sample */
  NumericalSample getResidualSample() const;

  /** Get residual distribution */
  Distribution getResidualDistribution(const Bool smooth = true) const;

  /** Draw model vs metamodel validation graph */
  Graph drawValidation() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  void initialize() const;

  /** The input sample */
  NumericalSample inputSample_;

  /** The output sample */
  NumericalSample outputSample_;

  /** The metamodel */
  NumericalMathFunction metaModel_;

  /** Boolean argument */
  mutable Bool isInitialized_;

  /** Residual sample */
  mutable NumericalSample residual_;

  /** Q2 arguments  */
  mutable NumericalScalar q2_;

private:


}; /* class MetaModelValidation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_METAMODELVALIDATION_HXX */
