//                                               -*- C++ -*-
/**
 *  @brief Base class for simulation results
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
#ifndef OPENTURNS_SIMULATIONRESULTIMPLEMENTATION_HXX
#define OPENTURNS_SIMULATIONRESULTIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimulationResultImplementation
 * Implementation of SimulationResultImplementation
 */
class OT_API SimulationResultImplementation
  : public PersistentObject
{

  CLASSNAME
public:


  /** Default constructor */
  SimulationResultImplementation();

  /** Standard constructor */
  SimulationResultImplementation(const UnsignedInteger outerSampling,
                   const UnsignedInteger blockSize);

  /** Virtual constructor */
  SimulationResultImplementation * clone() const override;

  /** Sample size accessor */
  UnsignedInteger getOuterSampling() const;
  void setOuterSampling(const UnsignedInteger outerSampling);

  /** Block size accessor */
  UnsignedInteger getBlockSize() const;
  void setBlockSize(const UnsignedInteger blockSize);

  /** Elapsed time accessor */
  void setTimeDuration(const Scalar time);
  Scalar getTimeDuration() const;

  using PersistentObject::operator==;
  /** Comparison operator */
  Bool operator ==(const PersistentObject & other) const override;

  using PersistentObject::operator!=;
  /** Comparison operator */
  Bool operator !=(const PersistentObject & other) const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  UnsignedInteger outerSampling_ = 0;
  UnsignedInteger blockSize_ = 0;
  Scalar timeDuration_ = 0.0;

}; // class SimulationResultImplementation

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATIONRESULTIMPLEMENTATION_HXX */
