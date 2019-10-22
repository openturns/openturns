//                                               -*- C++ -*-
/**
 * @brief PythonEvaluation declaration
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_PYTHONEVALUATION_HXX
#define OPENTURNS_PYTHONEVALUATION_HXX

#include <Python.h>
#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonEvaluation
 *
 * This class binds a Python function to an OpenTURNS' Function
 */
class PythonEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:


  /** Constructor from Python object*/
  PythonEvaluation(PyObject * pyCallable);

  /** Copy constructor */
  PythonEvaluation(const PythonEvaluation & other);

  /** Virtual constructor */
  virtual PythonEvaluation * clone() const;

  /** Copy assignment operator */
  PythonEvaluation & operator=(const PythonEvaluation & rhs);

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
  virtual Point operator() (const Point & inP) const;

  virtual Sample operator() (const Sample & inS) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Linearity accessors */
  Bool isLinear() const;
  Bool isLinearlyDependent(const UnsignedInteger index) const;


  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method save() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:


private:

  friend class Factory<PythonEvaluation>;

  /** Default constructor */
  PythonEvaluation();

  /** Set pyBufferClass_ and pyObj_*_ members */
  void initializePythonState();

  /** The underlying Python callable object */
  PyObject * pyObj_;

  /** Tell whether Python callable object define these members */
  Bool pyObj_has_exec_;
  Bool pyObj_has_exec_sample_;
  Bool pyObj_discard_openturns_memoryview_;

  /** Python openturns.memoryview.Buffer class */
  PyObject * pyBufferClass_;

}; /* class PythonEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONEVALUATION_HXX */

