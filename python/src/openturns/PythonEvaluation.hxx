//                                               -*- C++ -*-
/**
 * @brief PythonEvaluation declaration
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

#ifndef OPENTURNS_PYTHONEVALUATION_HXX
#define OPENTURNS_PYTHONEVALUATION_HXX

#include <Python.h>
#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonEvaluation
 *
 * This class binds a Python function to an OpenTURNS' NumericalMathFunction
 */
class PythonEvaluation
  : public EvaluationImplementation
{
  CLASSNAME;
public:


  /** Constructor from Python object*/
  PythonEvaluation(PyObject * pyCallable);

  /** Copy constructor */
  PythonEvaluation(const PythonEvaluation & other);

  /** Virtual constructor */
  virtual PythonEvaluation * clone() const;

  /** Destructor */
  virtual ~PythonEvaluation();

  /** Comparison operator */
  Bool operator ==(const PythonEvaluation & other) const;

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

  friend class Factory<PythonEvaluation>;

  /** Default constructor */
  PythonEvaluation();

  /** The underlying Python callable object */
  PyObject * pyObj_;

}; /* class PythonEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONEVALUATION_HXX */

