//                                               -*- C++ -*-
/**
 *  @brief The class that implements dynamical functions
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_DYNAMICALFUNCTION_HXX
#define OPENTURNS_DYNAMICALFUNCTION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/DynamicalFunctionImplementation.hxx"
#include "openturns/NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DynamicalFunction
 * @brief Simulates a dynamical function
 *
 * The class that simulates a dynamical function, it means
 * a function acting on a set of vectors indexed by the time.
 * This class is just an interface to actual implementation
 * objects that can be hot-replaced during computation.
 * Each implementation object refers to a specific evaluation
 * @see DynamicalFunctionImplementation
 */
class OT_API DynamicalFunction
  : public TypedInterfaceObject<DynamicalFunctionImplementation>
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */

  /** Default constructor */
  DynamicalFunction(const UnsignedInteger spatialDimension = 1);

  /** Constructor from NumericalMathFunction */
  DynamicalFunction(const NumericalMathFunction & function,
                    const UnsignedInteger spatialDimension = 1);

  /** Constructor from DynamicalFunctionImplementation */
  DynamicalFunction(const DynamicalFunctionImplementation & implementation);

  /** Constructor from implementation */
  DynamicalFunction(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  DynamicalFunction(DynamicalFunctionImplementation * p_implementation);
#endif

  /** Comparison operator */
  Bool operator ==(const DynamicalFunction & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  NumericalPoint operator() (const NumericalScalar timeStamp,
                             const NumericalPoint & inP) const;
  NumericalPoint operator() (const NumericalPoint & location,
                             const NumericalPoint & inP) const;
  Field operator() (const Field & inFld) const;
  ProcessSample operator() (const ProcessSample & inPS) const;

  /** Get the i-th marginal function */
  DynamicalFunction getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  DynamicalFunction getMarginal(const Indices & indices) const;

  /** Accessor for mesh dimension */
  UnsignedInteger getSpatialDimension() const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for the output mesh associated with the given input mesh */
  Mesh getOutputMesh(const Mesh & inputMesh) const;

  /** Input description Accessor, i.e. the names of the input parameters */
  Description getInputDescription() const;

  /** Output description Accessor, i.e. the names of the Output parameters */
  Description getOutputDescription() const;

  /** Number of calls to the evaluation */
  UnsignedInteger getCallsNumber() const;

}; /* class DynamicalFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DYNAMICALFUNCTION_HXX */
