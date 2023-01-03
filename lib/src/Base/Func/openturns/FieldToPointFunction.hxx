//                                               -*- C++ -*-
/**
 *  @brief The class that implements functions mapping fields into points
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FIELDTOPOINTFUNCTION_HXX
#define OPENTURNS_FIELDTOPOINTFUNCTION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/FieldToPointFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FieldToPointFunction
 * @brief Simulates a dynamical function
 *
 * The class that simulates a function mapping a field into a point.
 * Each implementation object refers to a specific evaluation
 * @see FieldToPointFunctionImplementation
 */
class OT_API FieldToPointFunction
  : public TypedInterfaceObject<FieldToPointFunctionImplementation>
{
  CLASSNAME
public:

  /** Default constructor */
  FieldToPointFunction();

  /** Parameter constructor */
  FieldToPointFunction(const Mesh & inputMesh,
                       const UnsignedInteger inputDimension,
                       const UnsignedInteger outputDimension);

  /** Constructor from FieldToPointFunctionImplementation */
  FieldToPointFunction(const FieldToPointFunctionImplementation & implementation);

  /** Constructor from implementation */
  FieldToPointFunction(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  FieldToPointFunction(FieldToPointFunctionImplementation * p_implementation);
#endif

  /** Comparison operator */
  Bool operator ==(const FieldToPointFunction & other) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Operator () */
  Point operator() (const Sample & inFld) const;
  Sample operator() (const ProcessSample & inPS) const;

  /** Get the i-th marginal function */
  FieldToPointFunction getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  FieldToPointFunction getMarginal(const Indices & indices) const;

  /** Accessor for input mesh */
  Mesh getInputMesh() const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Input description Accessor, i.e. the names of the input parameters */
  Description getInputDescription() const;
  void setInputDescription(const Description & inputDescription);

  /** Output description Accessor, i.e. the names of the Output parameters */
  Description getOutputDescription() const;
  void setOutputDescription(const Description & outputDescription);

  /** Number of calls to the evaluation */
  UnsignedInteger getCallsNumber() const;

}; /* class FieldToPointFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDTOPOINTFUNCTION_HXX */
