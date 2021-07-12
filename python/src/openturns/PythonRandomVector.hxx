//                                               -*- C++ -*-
/**
 * @brief This class binds a Python object to an OpenTURNS' RandomVector
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

#ifndef OPENTURNS_PYTHONRANDOMVECTOR_HXX
#define OPENTURNS_PYTHONRANDOMVECTOR_HXX

#include <Python.h>
#include "openturns/RandomVectorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class PythonRandomVector
 *
 * This class binds a Python object to an OpenTURNS' RandomVector
 */
class PythonRandomVector
  : public RandomVectorImplementation
{
  CLASSNAME
public:


  /** Constructor from Python object*/
  PythonRandomVector(PyObject * pyObject);

  /** Copy constructor */
  PythonRandomVector(const PythonRandomVector & other);

  /** Virtual constructor */
  PythonRandomVector * clone() const override;

  /** Copy assignment operator */
  PythonRandomVector & operator=(const PythonRandomVector & rhs);

  /** Destructor */
  virtual ~PythonRandomVector();

  /** Comparison operator */
  Bool operator ==(const PythonRandomVector & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /* Here is the interface that all derived class must implement */

  /** Dimension accessor */
  UnsignedInteger getDimension() const override;

  /** Realization accessor */
  Point getRealization() const override;

  /** Numerical sample accessor */
  Sample getSample(const UnsignedInteger size) const override;

  /** Mean accessor */
  Point getMean() const override;

  /** Covariance accessor */
  CovarianceMatrix getCovariance() const override;

  /** Whether it is an event */
  Bool isEvent() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method save() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  friend class Factory<PythonRandomVector>;

  /** Default constructor */
  PythonRandomVector();

  /** The underlying Python object */
  PyObject * pyObj_;

}; /* class PythonRandomVector */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONRANDOMVECTOR_HXX */
