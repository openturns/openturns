//                                               -*- C++ -*-
/**
 * @brief This class binds a Python object to an OpenTURNS' Experiment
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

#ifndef OPENTURNS_PYTHONEXPERIMENTIMPLEMENTATION_HXX
#define OPENTURNS_PYTHONEXPERIMENTIMPLEMENTATION_HXX

#include <Python.h>
#include "openturns/ExperimentImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class PythonExperimentImplementation
 *
 * This class binds a Python object to an OpenTURNS' Experiment
 */
class PythonExperimentImplementation
  : public ExperimentImplementation
{
  CLASSNAME;
public:


  /** Constructor from Python object*/
  PythonExperimentImplementation(PyObject * pyObject);

  /** Copy constructor */
  PythonExperimentImplementation(const PythonExperimentImplementation & other);

  /** Virtual constructor */
  virtual PythonExperimentImplementation * clone() const;

  /** Destructor */
  virtual ~PythonExperimentImplementation();

  /** Comparison operator */
  Bool operator ==(const PythonExperimentImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset) const;


  /* Here is the interface that all derived class must implement */

  /** Numerical sample accessor */
  virtual NumericalSample generate() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method save() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:


private:

  friend class Factory<PythonExperimentImplementation>;

  /** Default constructor */
  PythonExperimentImplementation();

  /** The underlying Python object */
  PyObject * pyObj_;

}; /* class PythonExperimentImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONEXPERIMENTIMPLEMENTATION_HXX */
