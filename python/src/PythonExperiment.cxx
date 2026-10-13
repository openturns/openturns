//                                               -*- C++ -*-
/**
 * @brief This class binds a Python object to an OpenTURNS' RandomVector
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/PythonExperiment.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(PythonExperiment)

static const Factory<PythonExperiment> Factory_PythonExperiment;



/* Default constructor */
PythonExperiment::PythonExperiment()
  : ExperimentImplementation(),
    pyObj_(0)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonExperiment::PythonExperiment(PyObject * pyObject)
  : ExperimentImplementation(),
    pyObj_(pyObject)
{
  if ( !PyObject_HasAttrString( pyObj_, const_cast<char *>("generate") ) ) throw InvalidArgumentException(HERE) << "Error: the given object does not have a generate() method.";

  Py_XINCREF(pyObj_);

  // Set the name of the object as its Python classname
  ScopedPyObjectPointer cls(PyObject_GetAttrString ( pyObj_,
                            const_cast<char *>( "__class__" ) ));
  ScopedPyObjectPointer name(PyObject_GetAttrString( cls.get(),
                             const_cast<char *>( "__name__" ) ));

  setName( checkAndConvert< _PyString_, String >(name.get()) );
}

/* Virtual constructor */
PythonExperiment * PythonExperiment::clone() const
{
  return new PythonExperiment(*this);
}

/* Copy constructor */
PythonExperiment::PythonExperiment(const PythonExperiment & other)
  : ExperimentImplementation(other),
    pyObj_()
{
  ScopedPyObjectPointer pyObjClone(deepCopy(other.pyObj_));
  pyObj_ = pyObjClone.get();
  Py_XINCREF(pyObj_);
}

/* Copy assignment operator */
PythonExperiment & PythonExperiment::operator=(const PythonExperiment & rhs)
{
  if (this != &rhs)
  {
    ExperimentImplementation::operator=(rhs);
    ScopedPyObjectPointer pyObjClone(deepCopy(rhs.pyObj_));
    pyObj_ = pyObjClone.get();
    Py_XINCREF(pyObj_);
  }
  return *this;
}

/* Destructor */
PythonExperiment::~PythonExperiment()
{
  Py_XDECREF(pyObj_);
}

/* Comparison operator */
Bool PythonExperiment::operator ==(const PythonExperiment & ) const
{
  return true;
}

/* String converter */
String PythonExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonExperiment::GetClassName()
      << " name=" << getName();
//       << " description=" << getDescription();
  return oss;
}

/* String converter */
String PythonExperiment::__str__(const String & ) const
{
  OSS oss;
  oss << "class=" << PythonExperiment::GetClassName()
      << " name=" << getName();
  return oss;
}


/* Here is the interface that all derived class must implement */


Sample PythonExperiment::generate() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod ( pyObj_,
                               const_cast<char *>( "generate" ),
                               const_cast<char *>( "()" ) ));
  if (result.isNull())
  {
    handleException();
  }
  Sample point(convert<_PySequence_, Sample>(result.get()));
  return point;
}


Sample PythonExperiment::generateWithWeights(Point & weights) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("generateWithWeights")))
  {
    ScopedPyObjectPointer result(PyObject_CallMethod ( pyObj_,
                                 const_cast<char *>( "generateWithWeights" ),
                                 const_cast<char *>( "()" ) ));
    if (result.isNull())
    {
      handleException();
    }
    // The Python method must return a (sample, weights) pair
    check<_PySequence_>(result.get());
    if (PySequence_Length(result.get()) != 2) throw InvalidArgumentException(HERE) << "Error: the generateWithWeights() method must return a (sample, weights) pair.";
    ScopedPyObjectPointer pySample(PySequence_GetItem(result.get(), 0));
    ScopedPyObjectPointer pyWeights(PySequence_GetItem(result.get(), 1));
    const Sample sample(convert<_PySequence_, Sample>(pySample.get()));
    weights = convert<_PySequence_, Point>(pyWeights.get());
    if (weights.getSize() != sample.getSize()) throw InvalidArgumentException(HERE) << "Error: the weights size (" << weights.getSize() << ") does not match the sample size (" << sample.getSize() << ").";
    return sample;
  }
  // Fallback to generate() with uniform weights
  const Sample result(generate());
  weights = Point(result.getSize(), 1.0 / result.getSize());
  return result;
}


/* Method save() stores the object through the StorageManager */
void PythonExperiment::save(Advocate & adv) const
{
  ExperimentImplementation::save( adv );

  pickleSave(adv, pyObj_);
}


/* Method save() reloads the object from the StorageManager */
void PythonExperiment::load(Advocate & adv)
{
  ExperimentImplementation::load( adv );

  pickleLoad(adv, pyObj_);
}


END_NAMESPACE_OPENTURNS
