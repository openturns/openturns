//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for point to field function implementations
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_POINTTOFIELDFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_POINTTOFIELDFUNCTIONIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Description.hxx"
#include "openturns/Field.hxx"
#include "openturns/FieldSample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PointToFieldFunctionImplementation
 *
 * The class that implements functions mapping points to fields.
 */
class OT_API PointToFieldFunctionImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */
  typedef Pointer<PointToFieldFunctionImplementation> Implementation;

  /** Default constructor */
  PointToFieldFunctionImplementation();

  /** Parameter constructor */
  PointToFieldFunctionImplementation(const UnsignedInteger inputDimension,
                                     const UnsignedInteger outputDimension);

  /** Virtual constructor */
  virtual PointToFieldFunctionImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const PointToFieldFunctionImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  virtual Field operator() (const Point & inP) const;
  virtual FieldSample operator() (const Sample & inS) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Input description accessor, i.e. the names of the input parameters */
  virtual void setInputDescription(const Description & inputDescription);
  virtual Description getInputDescription() const;

  /** Output description accessor, i.e. the names of the Output parameters */
  virtual void setOutputDescription(const Description & outputDescription);
  virtual Description getOutputDescription() const;

  /** Accessor for the output mesh */
  Mesh getOutputMesh() const;

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

  /** Dimension of the input variables */
  UnsignedInteger inputDimension_;

  /** Dimension of the output variables */
  UnsignedInteger outputDimension_;

  /** Description of the input variables */
  Description inputDescription_;

  /** Description of the output variables */
  Description outputDescription_;

  /** Counter for the number of calls */
  mutable UnsignedInteger callsNumber_;

}; /* class PointToFieldFunctionImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINTTOFIELDFUNCTIONIMPLEMENTATION_HXX */
