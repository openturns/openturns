//                                               -*- C++ -*-
/**
 * @brief PythonHessian declaration
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

#ifndef OPENTURNS_PYTHONHESSIAN_HXX
#define OPENTURNS_PYTHONHESSIAN_HXX

#include <Python.h>
#include "openturns/HessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonHessian
 *
 * This class binds a Python function to a HessianImplementation
 */
class PythonHessian
  : public HessianImplementation
{
  CLASSNAME
public:


  /** Constructor from Python object*/
  PythonHessian(PyObject * pyCallable);

  /** Copy constructor */
  PythonHessian(const PythonHessian & other);

  /** Virtual constructor */
  virtual PythonHessian * clone() const;

  /** Copy assignment operator */
  PythonHessian & operator=(const PythonHessian & rhs);

  /** Destructor */
  virtual ~PythonHessian();

  /** Comparison operator */
  Bool operator ==(const PythonHessian & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset) const;


  /** Test for actual implementation */
  virtual Bool isActualImplementation() const;


  /* Here is the interface that all derived class must implement */

  /** Gradient method */
  virtual SymmetricTensor hessian(const Point & inP) const;

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

  friend class Factory<PythonHessian>;

  /** Default constructor */
  PythonHessian();

  /** The underlying Python callable object */
  PyObject * pyObj_;

}; /* class PythonHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONHESSIAN_HXX */

