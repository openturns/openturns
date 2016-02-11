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
#ifndef OPENTURNS_MONTECARLO_HXX
#define OPENTURNS_MONTECARLO_HXX

#include "openturns/Simulation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class MonteCarlo
 */

class OT_API MonteCarlo :
  public Simulation
{

  CLASSNAME;
public:


  /** Constructor with parameters */
  MonteCarlo(const Event & event);

  /** for save/load mecanism */
  MonteCarlo() {};

  /** Virtual constructor */
  virtual MonteCarlo * clone() const;

  /** String converter */
  String __repr__() const;

private:

  /** Compute the block sample and the points that realized the event */
  NumericalSample computeBlockSample();

} ; /* class MonteCarlo */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MONTECARLO_HXX */
