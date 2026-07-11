//                                               -*- C++ -*-
/**
 *  @brief Result of a simulation
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
#ifndef OPENTURNS_SIMULATIONRESULT_HXX
#define OPENTURNS_SIMULATIONRESULT_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/SimulationResultImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimulationResult
 *
 * Result of a simulation.
 */
class OT_API SimulationResult
  : public TypedInterfaceObject<SimulationResultImplementation>
{
  CLASSNAME
public:

  /** Default constructor */
  SimulationResult();

  /** Constructor from implementation */
  SimulationResult(const SimulationResultImplementation & implementation);

  /** Constructor from implementation pointer */
#ifndef SWIG
  SimulationResult(SimulationResultImplementation * p_implementation);
#endif

  /** Constructor from implementation */
  SimulationResult(const Implementation & p_implementation);

  /** Comparison operator */
  using TypedInterfaceObject<SimulationResultImplementation>::operator ==;
  Bool operator ==(const SimulationResult & other) const;

  /** Comparison operator */
  using TypedInterfaceObject<SimulationResultImplementation>::operator !=;
  Bool operator !=(const SimulationResult & other) const;

  /** Sample size accessor */
  UnsignedInteger getOuterSampling() const;
  void setOuterSampling(const UnsignedInteger outerSampling);

  /** Block size accessor */
  UnsignedInteger getBlockSize() const;
  void setBlockSize(const UnsignedInteger blockSize);

  /** Elapsed time accessor */
  void setTimeDuration(const Scalar time);
  Scalar getTimeDuration() const;

  /** String converter */
  String __repr__() const override;

}; // class SimulationResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATIONRESULT_HXX */
