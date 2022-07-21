//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all space filling criteria implementations
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
#ifndef OPENTURNS_SPACEFILLINGIMPLEMENTATION_HXX
#define OPENTURNS_SPACEFILLINGIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/Sample.hxx"

namespace OT
{

/**
 * @class SpaceFillingImplementation
 *
 * The class that implements space filling criteria.
 * This class serves an interface for derived classes.
 */
class OT_API SpaceFillingImplementation
  : public PersistentObject
{
  CLASSNAME

  /* Allow OptimalLHS derived classes to call perturb method */
  friend class SpaceFilling;

public:
  /** Default constructor */
  explicit SpaceFillingImplementation(Bool minimization = true);

  /** Virtual constructor method */
  SpaceFillingImplementation * clone() const override;

  /** Evaluate criterion on a sample */
  virtual Scalar evaluate(const Sample& sample) const;

  /** Accessor */
  Bool isMinimizationProblem() const
  {
    return minimization_;
  }

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Compute criterion when performing an elementary perturbation */
  virtual Scalar perturbLHS(Sample& oldDesign, Scalar oldCriterion,
                            UnsignedInteger row1, UnsignedInteger row2, UnsignedInteger column) const;

protected:
  /** Normalize argument before computing criterion */
  Sample normalize(const Sample & sample) const;


private:
  /* True if this criterion is to be minimized, false otherwise */
  Bool minimization_;

}; /* class SpaceFillingImplementation */

} /* namespace OT */

#endif /* OT_SPACEFILLINGIMPLEMENTATION_HXX */
