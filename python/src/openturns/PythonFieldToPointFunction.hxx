//                                               -*- C++ -*-
/**
 * @brief PythonFieldToPointFunction declaration
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_PYTHONFIELDTOPOINTFUNCTION_HXX
#define OPENTURNS_PYTHONFIELDTOPOINTFUNCTION_HXX

#include <Python.h>
#include "openturns/FieldToPointFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonFieldToPointFunction
 *
 * This class binds a Python function to an OpenTURNS' FieldToPointFunction
 */
class PythonFieldToPointFunction
  : public FieldToPointFunctionImplementation
{
  CLASSNAME
public:

  /** Constructor from Python object*/
  PythonFieldToPointFunction(PyObject * pyCallable);

  /** Copy constructor */
  PythonFieldToPointFunction(const PythonFieldToPointFunction & other);

  /** Virtual constructor */
  PythonFieldToPointFunction * clone() const override;

  /** Copy assignment operator */
  PythonFieldToPointFunction & operator=(const PythonFieldToPointFunction & rhs);

  /** Destructor */
  virtual ~PythonFieldToPointFunction();

  /** Comparison operator */
  Bool operator ==(const PythonFieldToPointFunction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /* Here is the interface that all derived class must implement */

  /** Operator () */
  Point operator() (const Sample & inF) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method save() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  friend class Factory<PythonFieldToPointFunction>;

  /** Default constructor */
  PythonFieldToPointFunction();

  /** The underlying Python callable object */
  PyObject * pyObj_;

}; /* class PythonFieldToPointFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONFIELDTOPOINTFUNCTION_HXX */

