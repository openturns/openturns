//                                               -*- C++ -*-
/**
 * @brief Canonical tensor gradient
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_CANONICALTENSORGRADIENT_HXX
#define OPENTURNS_CANONICALTENSORGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/CanonicalTensorEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CanonicalTensorGradient
 *
 * The class that implement the gradient of a canonical tensor.
 */
class OT_API CanonicalTensorGradient
  : public GradientImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  CanonicalTensorGradient();

  /** Default constructor */
  explicit CanonicalTensorGradient(const CanonicalTensorEvaluation & evaluation);

  /** Virtual constructor */
  CanonicalTensorGradient * clone() const override;

  /** Comparison operator */
  Bool operator ==(const CanonicalTensorGradient & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  using GradientImplementation::gradient;
  Matrix gradient(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  CanonicalTensorEvaluation evaluation_;


}; /* class CanonicalTensorGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CANONICALTENSORGRADIENT_HXX */
