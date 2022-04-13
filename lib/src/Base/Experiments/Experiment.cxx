//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an experiment plane
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Experiment.hxx"
#include "openturns/Axial.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Experiment)

/* Default constructor */
Experiment::Experiment()
  : TypedInterfaceObject<ExperimentImplementation>(new Axial())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
Experiment::Experiment(const Implementation & p_implementation)
  : TypedInterfaceObject<ExperimentImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation */
Experiment::Experiment(const ExperimentImplementation & implementation)
  : TypedInterfaceObject<ExperimentImplementation>(implementation.clone())
{
  // Nothing to do
}

/* String converter */
String Experiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName ()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* Sample generation */
Sample Experiment::generate() const
{
  return getImplementation()->generate();
}

END_NAMESPACE_OPENTURNS
