//                                               -*- C++ -*-
/**
 *  @brief MonteCarlo is an implementation of the hit or miss Monte Carlo simulation method
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "MonteCarlo.hxx"
#include "ComparisonOperatorImplementation.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/*
 * @class MonteCarlo
 */

CLASSNAMEINIT(MonteCarlo);

static Factory<MonteCarlo> RegisteredFactory("MonteCarlo");

/* Constructor with parameters */
MonteCarlo::MonteCarlo(const Event & event):
  Simulation(event)
{
  // Nothing to do
}

/* Virtual constructor */
MonteCarlo * MonteCarlo::clone() const
{
  return new MonteCarlo(*this);
}

/* Compute the block sample */
NumericalSample MonteCarlo::computeBlockSample()
{
  return event_.getSample(blockSize_);
}

/* String converter */
String MonteCarlo::__repr__() const
{
  OSS oss;
  oss << "class=" << MonteCarlo::GetClassName()
      << " derived from " << Simulation::__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS
