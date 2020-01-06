//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math function implementations
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
#ifndef OPENTURNS_FIELDFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_FIELDFUNCTIONIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Description.hxx"
#include "openturns/Field.hxx"
#include "openturns/ProcessSample.hxx"
#include "openturns/AtomicFunctions.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FieldFunctionImplementation
 *
 * The class that simulates a numerical math function,
 * its gradient and its hessian. This class is just an interface
 * to actual implementation objects that can be hot-replaced
 * during computation. Each implementation object refers to
 * the evaluation, the gradient or the hessian.
 */
class OT_API FieldFunctionImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */
  typedef Pointer<FieldFunctionImplementation> Implementation;

  /** Default constructor */
  FieldFunctionImplementation();

  /** Parameter constructor */
  FieldFunctionImplementation(const Mesh & inputMesh,
                              const UnsignedInteger inputDimension,
                              const Mesh & outputMesh,
                              const UnsignedInteger outputDimension);

  /** Virtual constructor */
  virtual FieldFunctionImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const FieldFunctionImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  virtual Sample operator() (const Sample & inFld) const;
  virtual ProcessSample operator() (const ProcessSample & inPS) const;

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

  /** Accessor for the output mesh */
  virtual void setInputMesh(const Mesh & inputMesh);
  virtual Mesh getInputMesh() const;

  /** Accessor for the output mesh */
  virtual void setOutputMesh(const Mesh & outputMesh);
  virtual Mesh getOutputMesh() const;

  /** Acts point-wise ? */
  virtual Bool isActingPointwise() const;

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
  /** Input/output meshes */
  Mesh inputMesh_;
  Mesh outputMesh_;

  /** Expected dimension of the values of the field arguments */
  UnsignedInteger inputDimension_;

  /** Expected dimension of the values of the field produced */
  UnsignedInteger outputDimension_;

  /** Description of the input variables */
  Description inputDescription_;

  /** Description of the output variables */
  Description outputDescription_;

  /** Counter for the number of calls */
  mutable AtomicInt callsNumber_;

}; /* class FieldFunctionImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDFUNCTIONIMPLEMENTATION_HXX */
