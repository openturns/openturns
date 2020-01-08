//                                               -*- C++ -*-
/**
 *  @brief MonteCarloLHS
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MONTECARLOLHS_HXX
#define OPENTURNS_MONTECARLOLHS_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/SpaceFilling.hxx"
#include "openturns/OptimalLHSExperiment.hxx"
#include "openturns/LHSExperiment.hxx"
#include "openturns/SpaceFillingMinDist.hxx"

namespace OT
{

/**
 * @class MonteCarloLHS
 *
 * MonteCarloLHS is the algorithm used for LHS optimisation (Random Brute Force)
 */
class OT_API MonteCarloLHS
  : public OptimalLHSExperiment
{
  CLASSNAME

public:
  /** Default constructor */
  MonteCarloLHS();

  /** Default constructor */
  MonteCarloLHS(const LHSExperiment & lhs, const UnsignedInteger N, const SpaceFilling & spaceFilling = SpaceFillingMinDist());

  /** Virtual constructor method */
  MonteCarloLHS * clone() const;

  /** Compute next design method **/
  Sample generateWithWeights(Point & weightsOut) const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:
  UnsignedInteger N_;
}; /* class MonteCarloLHS */

} /* namespace OT */

#endif /* OT_MONTECARLOLHS_HXX */
