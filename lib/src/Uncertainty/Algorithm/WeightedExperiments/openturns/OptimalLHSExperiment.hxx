//                                               -*- C++ -*-
/**
 *  @brief Abstract class for algorithms generating optimized LHS
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_OPTIMALLHSEXPERIMENT_HXX
#define OPENTURNS_OPTIMALLHSEXPERIMENT_HXX

#include <openturns/PersistentObject.hxx>
#include <openturns/StorageManager.hxx>
#include <openturns/LHSExperiment.hxx>
#include <openturns/ComposedDistribution.hxx>
#include "openturns/MarginalTransformationEvaluation.hxx"

#include "openturns/SpaceFilling.hxx"
#include "openturns/LHSResult.hxx"

namespace OT
{

/**
 * @class OptimalLHSExperiment
 *
 * OptimalLHSExperiment is some optimallhs type to illustrate how to add some classes in OpenTURNS
 */
class OT_API OptimalLHSExperiment
  : public WeightedExperimentImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  explicit OptimalLHSExperiment(const LHSExperiment & lhs);

  /** Default constructor */
  OptimalLHSExperiment(const LHSExperiment & lhs, const SpaceFilling & spaceFilling);

  /** Virtual constructor method */
  OptimalLHSExperiment * clone() const override;

  /** Attributes for LHSExperiment */
  LHSExperiment getLHS() const;

  /** Attributes for SpaceFilling */
  SpaceFilling getSpaceFilling() const;

  /** Result accessor */
  virtual LHSResult getResult() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  void setLHS(const LHSExperiment & lhs);

  LHSExperiment lhs_;

  SpaceFilling spaceFilling_;

  OptimalLHSExperiment() {};
  friend class Factory<OptimalLHSExperiment>;

  mutable LHSResult result_;

  // Transformation from uniform space to distribution space
  MarginalTransformationEvaluation transformation_;

}; /* class OptimalLHSExperiment */

} /* namespace OT */

#endif /* OT_OPTIMALLHSEXPERIMENT_HXX */
