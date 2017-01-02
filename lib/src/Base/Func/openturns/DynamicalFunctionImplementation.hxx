//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math function implementations
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_DYNAMICALFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_DYNAMICALFUNCTIONIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Description.hxx"
#include "openturns/Field.hxx"
#include "openturns/ProcessSample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DynamicalFunctionImplementation
 *
 * The class that simulates a numerical math function,
 * its gradient and its hessian. This class is just an interface
 * to actual implementation objects that can be hot-replaced
 * during computation. Each implementation object refers to
 * the evaluation, the gradient or the hessian.
 */
class OT_API DynamicalFunctionImplementation
  : public PersistentObject
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<DynamicalFunctionImplementation> Implementation;

  /** Default constructor */
  explicit DynamicalFunctionImplementation(const UnsignedInteger spatialDimension = 1);

  /** Virtual constructor */
  virtual DynamicalFunctionImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const DynamicalFunctionImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  virtual NumericalPoint operator() (const NumericalScalar timeStamp,
                                     const NumericalPoint & inP) const;
  virtual NumericalPoint operator() (const NumericalPoint & location,
                                     const NumericalPoint & inP) const;
  virtual Field operator() (const Field & inFld) const;
  virtual ProcessSample operator() (const ProcessSample & inPS) const;

  /** Accessor for mesh dimension */
  virtual UnsignedInteger getSpatialDimension() const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Input description Accessor, i.e. the names of the input parameters */
  virtual void setInputDescription(const Description & inputDescription);
  virtual Description getInputDescription() const;

  /** Output description Accessor, i.e. the names of the Output parameters */
  virtual void setOutputDescription(const Description & outputDescription);
  virtual Description getOutputDescription() const;

  /** Accessor for the output mesh associated with the given input mesh */
  Mesh getOutputMesh(const Mesh & inputMesh) const;

  /** Get the i-th marginal function */
  virtual Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  virtual Implementation getMarginal(const Indices & indices) const;

  /** Number of calls to the dynamical function */
  virtual UnsignedInteger getCallsNumber() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  /** Expected dimension of the mesh underlying the field arguments */
  UnsignedInteger spatialDimension_;

  /** Description of the input variables */
  Description inputDescription_;

  /** Description of the output variables */
  Description outputDescription_;

  /** Counter for the number of calls */
  mutable UnsignedInteger callsNumber_;

}; /* class DynamicalFunctionImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DYNAMICALFUNCTIONIMPLEMENTATION_HXX */
