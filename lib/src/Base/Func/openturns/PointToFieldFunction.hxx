//                                               -*- C++ -*-
/**
 *  @brief The class that implements functions mapping points to fields
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_POINTTOFIELDFUNCTION_HXX
#define OPENTURNS_POINTTOFIELDFUNCTION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/PointToFieldFunctionImplementation.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PointToFieldFunction
 * @brief Simulates a dynamical function
 *
 * The class that simulates a dynamical function, it means
 * a function acting on a set of vectors indexed by the time.
 * This class is just an interface to actual implementation
 * objects that can be hot-replaced during computation.
 * Each implementation object refers to a specific evaluation
 * @see PointToFieldFunctionImplementation
 */
class OT_API PointToFieldFunction
  : public TypedInterfaceObject<PointToFieldFunctionImplementation>
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */

  /** Default constructor */
  PointToFieldFunction();

  /** Parameter constructor */
  PointToFieldFunction(const UnsignedInteger inputDimension,
                       const Mesh & outputMesh,
                       const UnsignedInteger outputDimension);

  /** Constructor from PointToFieldFunctionImplementation */
  PointToFieldFunction(const PointToFieldFunctionImplementation & implementation);

  /** Constructor from implementation */
  PointToFieldFunction(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  PointToFieldFunction(PointToFieldFunctionImplementation * p_implementation);
#endif

  /** Comparison operator */
  Bool operator ==(const PointToFieldFunction & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  Sample operator() (const Point & inP) const;
  ProcessSample operator() (const Sample & inS) const;

  /** Get the i-th marginal function */
  PointToFieldFunction getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  PointToFieldFunction getMarginal(const Indices & indices) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for the output mesh */
  Mesh getOutputMesh() const;

  /** Input description accessor, i.e. the names of the input parameters */
  void setInputDescription(const Description & inputDescription);
  Description getInputDescription() const;

  /** Output description accessor, i.e. the names of the Output parameters */
  void setOutputDescription(const Description & outputDescription);
  Description getOutputDescription() const;

  /** Number of calls to the evaluation */
  UnsignedInteger getCallsNumber() const;

}; /* class PointToFieldFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINTTOFIELDFUNCTION_HXX */
