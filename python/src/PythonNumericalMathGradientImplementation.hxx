//                                               -*- C++ -*-
/**
 * @brief PythonNumericalMathGradientImplementation declaration
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_PYTHONNUMERICALMATHGRADIENTIMPLEMENTATION_HXX
#define OPENTURNS_PYTHONNUMERICALMATHGRADIENTIMPLEMENTATION_HXX

#include "Python.h"
#include "NumericalMathGradientImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonNumericalMathGradientImplementation
 *
 * This class binds a Python function to a NumericalMathGradientImplementation
 */
class PythonNumericalMathGradientImplementation
  : public NumericalMathGradientImplementation
{
  CLASSNAME;
public:


  /** Constructor from Python object*/
  PythonNumericalMathGradientImplementation(PyObject * pyCallable);

  /** Copy constructor */
  PythonNumericalMathGradientImplementation(const PythonNumericalMathGradientImplementation & other);

  /** Virtual constructor */
  virtual PythonNumericalMathGradientImplementation * clone() const;

  /** Destructor */
  virtual ~PythonNumericalMathGradientImplementation();

  /** Comparison operator */
  Bool operator ==(const PythonNumericalMathGradientImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset) const;


  /** Test for actual implementation */
  virtual Bool isActualImplementation() const;


  /* Here is the interface that all derived class must implement */

  /** Gradient method */
  virtual Matrix gradient(const NumericalPoint & inP) const;

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

  friend class Factory<PythonNumericalMathGradientImplementation>;

  /** Default constructor */
  PythonNumericalMathGradientImplementation();

  /** The underlying Python callable object */
  PyObject * pyObj_;

}; /* class PythonNumericalMathGradientImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONNUMERICALMATHGRADIENTIMPLEMENTATION_HXX */

