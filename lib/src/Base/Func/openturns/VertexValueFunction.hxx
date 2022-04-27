//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math function implementations
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
#ifndef OPENTURNS_VERTEXVALUEFUNCTION_HXX
#define OPENTURNS_VERTEXVALUEFUNCTION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/FieldFunctionImplementation.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class VertexValueFunction
 *
 * The class that simulates a dynamical function based on a
 * numerical math function that acts only on the temporal part
 * of a time series.
 */
class OT_API VertexValueFunction
  : public FieldFunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  VertexValueFunction();

  /** Parameter constructor */
  VertexValueFunction(const Function & function,
                      const Mesh & mesh);

  /** Parameter constructor */
  VertexValueFunction(const Evaluation & evaluation,
                      const Mesh & mesh);

  /** Parameter constructor */
  VertexValueFunction(const EvaluationImplementation & evaluation,
                      const Mesh & mesh);

  /** Virtual constructor */
  VertexValueFunction * clone() const override;

  /** Comparison operator */
  Bool operator ==(const VertexValueFunction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  using FieldFunctionImplementation::operator();
  Sample operator() (const Sample & inFld) const override;

  /** Get the i-th marginal function */
  Implementation getMarginal(const UnsignedInteger i) const override;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const override;

  /** Function accessor */
  Function getFunction() const;

  /** Acts point-wise ? */
  Bool isActingPointwise() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** The underlying function */
  Function function_;

}; /* class VertexValueFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_VERTEXVALUEFUNCTION_HXX */
