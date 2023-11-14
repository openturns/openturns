//                                               -*- C++ -*-
/**
 *  @brief Base class for simulation results
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimulationResult
 * Implementation of SimulationResult
 */
class OT_API SimulationResult
  : public PersistentObject
{

  CLASSNAME
public:


  /** Default constructor */
  SimulationResult();

  /** Standard constructor */
  SimulationResult(const UnsignedInteger outerSampling,
                   const UnsignedInteger blockSize);

  /** Virtual constructor */
  SimulationResult * clone() const override;

  /** Sample size accessor */
  UnsignedInteger getOuterSampling() const;
  void setOuterSampling(const UnsignedInteger outerSampling);

  /** Block size accessor */
  UnsignedInteger getBlockSize() const;
  void setBlockSize(const UnsignedInteger blockSize);

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  UnsignedInteger outerSampling_;
  UnsignedInteger blockSize_;

}; // class SimulationResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATIONRESULT_HXX */
