//                                               -*- C++ -*-
/**
 *  @brief TemperatureProfileImplementation
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
#ifndef OPENTURNS_OPTIMALTEMPERATUREPROFILEIMPLEMENTATION_HXX
#define OPENTURNS_OPTIMALTEMPERATUREPROFILEIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/StorageManager.hxx"

namespace OT
{

/**
 * @class TemperatureProfileImplementation
 *
 * TemperatureProfileImplementation is a generic temperature profile for SimulatedAnnealing
 */
class OT_API TemperatureProfileImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  TemperatureProfileImplementation();

  /** Constructor with temperature T0 & iMax*/
  TemperatureProfileImplementation(const Scalar T0,
                                   const UnsignedInteger iMax);

  /** Virtual constructor method */
  TemperatureProfileImplementation * clone() const;

  /** Compute temperature T(i) */
  virtual Scalar operator()(UnsignedInteger i) const;

  /** get T0 */
  virtual Scalar getT0() const;

  /** get iMax */
  virtual UnsignedInteger getIMax() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  Scalar T0_;
  UnsignedInteger iMax_;

}; /* class TemperatureProfileImplementation */

} /* namespace OT */

#endif /* OT_OPTIMALTEMPERATUREPROFILEIMPLEMENTATION_HXX */
