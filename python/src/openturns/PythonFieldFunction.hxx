//                                               -*- C++ -*-
/**
 * @brief PythonFieldFunction declaration
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

#ifndef OPENTURNS_PYTHONFIELDFUNCTION_HXX
#define OPENTURNS_PYTHONFIELDFUNCTION_HXX

#include <Python.h>
#include "openturns/FieldFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonFieldFunction
 *
 * This class binds a Python function to an OpenTURNS' FieldFunction
 */
class PythonFieldFunction
  : public FieldFunctionImplementation
{
  CLASSNAME
public:

  /** Constructor from Python object*/
  PythonFieldFunction(PyObject * pyCallable);

  /** Copy constructor */
  PythonFieldFunction(const PythonFieldFunction & other);

  /** Virtual constructor */
  virtual PythonFieldFunction * clone() const;

  /** Copy assignment operator */
  PythonFieldFunction & operator=(const PythonFieldFunction & rhs);

  /** Destructor */
  virtual ~PythonFieldFunction();

  /** Comparison operator */
  Bool operator ==(const PythonFieldFunction & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset) const;


  /* Here is the interface that all derived class must implement */

  /** Operator () */
  using FieldFunctionImplementation::operator();
  virtual Sample operator() (const Sample & inF) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Acts point-wise ? */
  virtual Bool isActingPointwise() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method save() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:


private:

  friend class Factory<PythonFieldFunction>;

  /** Default constructor */
  PythonFieldFunction();

  /** The underlying Python callable object */
  PyObject * pyObj_;

}; /* class PythonFieldFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONFIELDFUNCTION_HXX */

