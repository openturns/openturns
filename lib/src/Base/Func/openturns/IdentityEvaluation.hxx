//                                               -*- C++ -*-
/**
 * @brief Class for identity evaluation
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_IDENTITYEVALUATION_HXX
#define OPENTURNS_IDENTITYEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IdentityEvaluation
 */
class OT_API IdentityEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Parameter constructor */
  explicit IdentityEvaluation(const UnsignedInteger dimension = 1);

  /** Virtual constructor */
  IdentityEvaluation * clone() const override;

  /** Comparison operator */
  using EvaluationImplementation::operator ==;
  Bool operator ==(const IdentityEvaluation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  Point operator() (const Point & inP) const override;

  /** Operator () */
  Sample operator() (const Sample & inS) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Linearity accessors */
  Bool isLinear() const override;
  Bool isLinearlyDependent(const UnsignedInteger index) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:
  /** The dimension of the input/output */
  UnsignedInteger dimension_;
}; /* class IdentityEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_IDENTITYEVALUATION_HXX */
