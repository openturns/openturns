//                                               -*- C++ -*-
/**
 * @brief PythonFieldFunctionImplementation declaration
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

#ifndef OPENTURNS_PYTHONFIELDFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_PYTHONFIELDFUNCTIONIMPLEMENTATION_HXX

#include <Python.h>
#include "openturns/FieldFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonFieldFunctionImplementation
 *
 * This class binds a Python function to an OpenTURNS' FieldFunction
 */
class PythonFieldFunctionImplementation
  : public FieldFunctionImplementation
{
  CLASSNAME;
public:

  /** Constructor from Python object*/
  PythonFieldFunctionImplementation(PyObject * pyCallable);

  /** Copy constructor */
  PythonFieldFunctionImplementation(const PythonFieldFunctionImplementation & other);

  /** Virtual constructor */
  virtual PythonFieldFunctionImplementation * clone() const;

  /** Destructor */
  virtual ~PythonFieldFunctionImplementation();

  /** Comparison operator */
  Bool operator ==(const PythonFieldFunctionImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset) const;


  /* Here is the interface that all derived class must implement */

  /** Operator () */
  virtual Field operator() (const Field & inF) const;

  /** Accessor for mesh dimension */
  virtual UnsignedInteger getSpatialDimension() const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method save() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:


private:

  friend class Factory<PythonFieldFunctionImplementation>;

  /** Default constructor */
  PythonFieldFunctionImplementation();

  /** The underlying Python callable object */
  PyObject * pyObj_;

}; /* class PythonFieldFunctionImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONFIELDFUNCTIONIMPLEMENTATION_HXX */

