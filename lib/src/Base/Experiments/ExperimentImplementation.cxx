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
#include "OTprivate.hxx"
#include "ExperimentImplementation.hxx"
#include "Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ExperimentImplementation);

/* Default constructor */
ExperimentImplementation::ExperimentImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
ExperimentImplementation * ExperimentImplementation::clone() const
{
  return new ExperimentImplementation(*this);
}

/* String converter */
String ExperimentImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}

/* Sample generation */
NumericalSample ExperimentImplementation::generate()
{
  throw NotYetImplementedException(HERE) << "In ExperimentImplementation::generate()";
}

END_NAMESPACE_OPENTURNS
