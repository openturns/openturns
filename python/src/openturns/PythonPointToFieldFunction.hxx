//                                               -*- C++ -*-
/**
 * @brief PythonPointToFieldFunction declaration
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

#ifndef OPENTURNS_PYTHONPOINTTOFIELDFUNCTION_HXX
#define OPENTURNS_PYTHONPOINTTOFIELDFUNCTION_HXX

#include <Python.h>
#include "openturns/PointToFieldFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonPointToFieldFunction
 *
 * This class binds a Python function to an OpenTURNS' PointToFieldFunction
 */
class PythonPointToFieldFunction
  : public PointToFieldFunctionImplementation
{
  CLASSNAME
public:

  /** Constructor from Python object*/
  PythonPointToFieldFunction(PyObject * pyCallable);

  /** Copy constructor */
  PythonPointToFieldFunction(const PythonPointToFieldFunction & other);

  /** Virtual constructor */
  virtual PythonPointToFieldFunction * clone() const;

  /** Copy assignment operator */
  PythonPointToFieldFunction & operator=(const PythonPointToFieldFunction & rhs);

  /** Destructor */
  virtual ~PythonPointToFieldFunction();

  /** Comparison operator */
  Bool operator ==(const PythonPointToFieldFunction & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset) const;


  /* Here is the interface that all derived class must implement */

  /** Operator () */
  using PointToFieldFunctionImplementation::operator();
  virtual Sample operator() (const Point & inP) const;

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

  friend class Factory<PythonPointToFieldFunction>;

  /** Default constructor */
  PythonPointToFieldFunction();

  /** The underlying Python callable object */
  PyObject * pyObj_;

}; /* class PythonPointToFieldFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONPOINTTOFIELDFUNCTION_HXX */

