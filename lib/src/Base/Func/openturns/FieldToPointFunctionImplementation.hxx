//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for field to point function implementations
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
#ifndef OPENTURNS_FIELDTOPOINTFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_FIELDTOPOINTFUNCTIONIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Description.hxx"
#include "openturns/Field.hxx"
#include "openturns/ProcessSample.hxx"
#include "openturns/AtomicFunctions.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FieldToPointFunctionImplementation
 *
 * The class that implements a function mapping a field to a point.
 */
class OT_API FieldToPointFunctionImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */
  typedef Pointer<FieldToPointFunctionImplementation> Implementation;

  /** Default constructor */
  FieldToPointFunctionImplementation();

  /** Parameter constructor */
  FieldToPointFunctionImplementation(const Mesh & inputMesh,
                                     const UnsignedInteger inputDimension,
                                     const UnsignedInteger outputDimension);

  /** Virtual constructor */
  virtual FieldToPointFunctionImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const FieldToPointFunctionImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  virtual Point operator() (const Sample & inFld) const;
  virtual Sample operator() (const ProcessSample & inPS) const;

  /** Accessor for input mesh */
  virtual Mesh getInputMesh() const;

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
  /** Input mesh */
  Mesh inputMesh_;

  /** Dimension of the input variables */
  UnsignedInteger inputDimension_;

  /** Description of the output variables */
  UnsignedInteger outputDimension_;

  /** Description of the input variables */
  Description inputDescription_;

  /** Description of the output variables */
  Description outputDescription_;

  /** Counter for the number of calls */
  mutable AtomicInt callsNumber_;

}; /* class FieldToPointFunctionImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDTOPOINTFUNCTIONIMPLEMENTATION_HXX */
