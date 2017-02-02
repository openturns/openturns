//                                               -*- C++ -*-
/**
 * @brief PythonDynamicalFunctionImplementation declaration
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

#ifndef OPENTURNS_PYTHONDYNAMICALFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_PYTHONDYNAMICALFUNCTIONIMPLEMENTATION_HXX

#include <Python.h>
#include "openturns/DynamicalFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonDynamicalFunctionImplementation
 *
 * This class binds a Python function to an OpenTURNS' DynamicalFunction
 */
class PythonDynamicalFunctionImplementation
  : public DynamicalFunctionImplementation
{
  CLASSNAME;
public:

  /** Constructor from Python object*/
  PythonDynamicalFunctionImplementation(PyObject * pyCallable);

  /** Copy constructor */
  PythonDynamicalFunctionImplementation(const PythonDynamicalFunctionImplementation & other);

  /** Virtual constructor */
  virtual PythonDynamicalFunctionImplementation * clone() const;

  /** Destructor */
  virtual ~PythonDynamicalFunctionImplementation();

  /** Comparison operator */
  Bool operator ==(const PythonDynamicalFunctionImplementation & other) const;

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

  friend class Factory<PythonDynamicalFunctionImplementation>;

  /** Default constructor */
  PythonDynamicalFunctionImplementation();

  /** The underlying Python callable object */
  PyObject * pyObj_;

}; /* class PythonDynamicalFunctionImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONDYNAMICALFUNCTIONIMPLEMENTATION_HXX */

