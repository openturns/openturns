//                                               -*- C++ -*-
/**
 *  @brief The class that implements dynamical functions
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
#ifndef OPENTURNS_FIELDFUNCTION_HXX
#define OPENTURNS_FIELDFUNCTION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/FieldFunctionImplementation.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FieldFunction
 * @brief Simulates a dynamical function
 *
 * The class that simulates a dynamical function, it means
 * a function acting on a set of vectors indexed by the time.
 * This class is just an interface to actual implementation
 * objects that can be hot-replaced during computation.
 * Each implementation object refers to a specific evaluation
 * @see FieldFunctionImplementation
 */
class OT_API FieldFunction
  : public TypedInterfaceObject<FieldFunctionImplementation>
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */

  /** Default constructor */
  FieldFunction();

  /** Parameter constructor */
  FieldFunction(const Mesh & inputMesh,
                const UnsignedInteger inputDimension,
                const Mesh & outputMesh,
                const UnsignedInteger outputDimension);

  /** Constructor from FieldFunctionImplementation */
  FieldFunction(const FieldFunctionImplementation & implementation);

  /** Constructor from implementation */
  FieldFunction(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  FieldFunction(FieldFunctionImplementation * p_implementation);
#endif

  /** Comparison operator */
  Bool operator ==(const FieldFunction & other) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Operator () */
  Sample operator() (const Sample & inFld) const;
  ProcessSample operator() (const ProcessSample & inPS) const;

  /** Get the i-th marginal function */
  FieldFunction getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  FieldFunction getMarginal(const Indices & indices) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for the input mesh */
  void setInputMesh(const Mesh & inputMesh);
  Mesh getInputMesh() const;

  /** Accessor for the output mesh */
  void setOutputMesh(const Mesh & outputMesh);
  Mesh getOutputMesh() const;

  /** Acts point-wise ? */
  Bool isActingPointwise() const;

  /** Input description Accessor, i.e. the names of the input parameters */
  Description getInputDescription() const;

  /** Output description Accessor, i.e. the names of the Output parameters */
  Description getOutputDescription() const;

  /** Number of calls to the evaluation */
  UnsignedInteger getCallsNumber() const;

}; /* class FieldFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDFUNCTION_HXX */
