//                                               -*- C++ -*-
/**
 *  @brief GeometricProfile
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
#ifndef OPENTURNS_OPTIMALGEOMETRICPROFILE_HXX
#define OPENTURNS_OPTIMALGEOMETRICPROFILE_HXX

#include "openturns/TemperatureProfileImplementation.hxx"

namespace OT
{

/**
 * @class GeometricProfile
 *
 * GeometricProfile is a geometric temperature profile for SimulatedAnnealing
 */
class OT_API GeometricProfile
  : public TemperatureProfileImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  GeometricProfile(const Scalar T0 = 10, const Scalar c = 0.95, const UnsignedInteger iMax = 2000);

  /** Virtual constructor method */
  GeometricProfile * clone() const;

  /** Compute temperature T(i) */
  Scalar operator()(UnsignedInteger i) const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  Scalar c_;
  mutable Scalar logc_;

}; /* class GeometricProfile */

} /* namespace OT */

#endif /* OT_OPTIMALGEOMETRICPROFILE_HXX */
