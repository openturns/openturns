//                                               -*- C++ -*-
/**
 * @brief PythonHessian declaration
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
  PythonHessian * clone() const override;

  /** Copy assignment operator */
  PythonHessian & operator=(const PythonHessian & rhs);

  /** Destructor */
  virtual ~PythonHessian();

  /** Comparison operator */
  Bool operator ==(const PythonHessian & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /** Gradient method */
  SymmetricTensor hessian(const Point & inP) const override;

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

  friend class Factory<PythonHessian>;

  /** Default constructor */
  PythonHessian();

  /** The underlying Python callable object */
  PyObject * pyObj_;

}; /* class PythonHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONHESSIAN_HXX */

