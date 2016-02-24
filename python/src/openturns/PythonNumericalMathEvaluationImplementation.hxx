//                                               -*- C++ -*-
/**
 * @brief PythonNumericalMathEvaluationImplementation declaration
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_PYTHONNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX
#define OPENTURNS_PYTHONNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX

#include <Python.h>
#include "openturns/NumericalMathEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonNumericalMathEvaluationImplementation
 *
 * This class binds a Python function to an OpenTURNS' NumericalMathFunction
 */
class PythonNumericalMathEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:


  /** Constructor from Python object*/
  PythonNumericalMathEvaluationImplementation(PyObject * pyCallable);

  /** Copy constructor */
  PythonNumericalMathEvaluationImplementation(const PythonNumericalMathEvaluationImplementation & other);

  /** Virtual constructor */
  virtual PythonNumericalMathEvaluationImplementation * clone() const;

  /** Destructor */
  virtual ~PythonNumericalMathEvaluationImplementation();

  /** Comparison operator */
  Bool operator ==(const PythonNumericalMathEvaluationImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset) const;


  /** Test for actual implementation */
  virtual Bool isActualImplementation() const;


  /* Here is the interface that all derived class must implement */

  /** Operator () */
  virtual NumericalPoint operator() (const NumericalPoint & inP) const;

  virtual NumericalSample operator() (const NumericalSample & inS) const;

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

  friend class Factory<PythonNumericalMathEvaluationImplementation>;

  /** Default constructor */
  PythonNumericalMathEvaluationImplementation();

  /** The underlying Python callable object */
  PyObject * pyObj_;

}; /* class PythonNumericalMathEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX */

