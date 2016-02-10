//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an experiment plane
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
#ifndef OPENTURNS_EXPERIMENTIMPLEMENTATION_HXX
#define OPENTURNS_EXPERIMENTIMPLEMENTATION_HXX

#include "PersistentObject.hxx"
#include "Pointer.hxx"
#include "NumericalSample.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ExperimentImplementation
 *
 * The class describes the probabilistic concept of experimentImplementation plan
 */
class OT_API ExperimentImplementation
  : public PersistentObject
{
  CLASSNAME;
public:

  typedef Pointer<ExperimentImplementation> Implementation;

  /** Default constructor */
  ExperimentImplementation();

  /** Virtual constructor */
  virtual ExperimentImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /** Sample generation */
  virtual NumericalSample generate();

protected:

private:

}; /* class ExperimentImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXPERIMENTIMPLEMENTATION_HXX */
