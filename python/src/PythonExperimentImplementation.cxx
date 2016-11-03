//                                               -*- C++ -*-
/**
 * @brief This class binds a Python object to an OpenTURNS' RandomVector
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

#include "openturns/PythonExperimentImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Description.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(PythonExperimentImplementation);

static const Factory<PythonExperimentImplementation> Factory_PythonExperimentImplementation;



/* Default constructor */
PythonExperimentImplementation::PythonExperimentImplementation()
  : ExperimentImplementation(),
    pyObj_(0)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonExperimentImplementation::PythonExperimentImplementation(PyObject * pyObject)
  : ExperimentImplementation(),
    pyObj_(pyObject)
{
  if ( !PyObject_HasAttrString( pyObj_, const_cast<char *>("generate") ) ) throw InvalidArgumentException(HERE) << "Error: the given object does not have a generate() method.";

  Py_XINCREF( pyObj_ );

  // Set the name of the object as its Python classname
  ScopedPyObjectPointer cls(PyObject_GetAttrString ( pyObj_,
                            const_cast<char *>( "__class__" ) ));
  ScopedPyObjectPointer name(PyObject_GetAttrString( cls.get(),
                             const_cast<char *>( "__name__" ) ));

  setName( checkAndConvert< _PyString_, String >(name.get()) );
}

/* Virtual constructor */
PythonExperimentImplementation * PythonExperimentImplementation::clone() const
{
  return new PythonExperimentImplementation(*this);
}

/* Copy constructor */
PythonExperimentImplementation::PythonExperimentImplementation(const PythonExperimentImplementation & other)
  : ExperimentImplementation(other),
    pyObj_(other.pyObj_)
{
  Py_XINCREF(pyObj_);
}

/* Destructor */
PythonExperimentImplementation::~PythonExperimentImplementation()
{
  Py_XDECREF(pyObj_);
}

/* Comparison operator */
Bool PythonExperimentImplementation::operator ==(const PythonExperimentImplementation & other) const
{
  return true;
}

/* String converter */
String PythonExperimentImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonExperimentImplementation::GetClassName()
      << " name=" << getName();
//       << " description=" << getDescription();
  return oss;
}

/* String converter */
String PythonExperimentImplementation::__str__(const String & offset) const
{
  OSS oss;
  oss << "class=" << PythonExperimentImplementation::GetClassName()
      << " name=" << getName();
  return oss;
}


/* Here is the interface that all derived class must implement */


NumericalSample PythonExperimentImplementation::generate() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod ( pyObj_,
                               const_cast<char *>( "generate" ),
                               const_cast<char *>( "()" ) ));
  if (result.isNull())
  {
    handleException();
  }
  NumericalSample point(convert<_PySequence_, NumericalSample>(result.get()));
  return point;
}


/* Method save() stores the object through the StorageManager */
void PythonExperimentImplementation::save(Advocate & adv) const
{
  ExperimentImplementation::save( adv );

  pickleSave(adv, pyObj_);
}


/* Method save() reloads the object from the StorageManager */
void PythonExperimentImplementation::load(Advocate & adv)
{
  ExperimentImplementation::load( adv );

  pickleLoad(adv, pyObj_);
}


END_NAMESPACE_OPENTURNS
