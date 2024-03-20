//                                               -*- C++ -*-
/**
 *  @brief SpaceFillingPhiP
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
#ifndef OPENTURNS_SPACEFILLINGPHIP_HXX
#define OPENTURNS_SPACEFILLINGPHIP_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/SpaceFillingImplementation.hxx"

namespace OT
{

/**
 * @class SpaceFillingPhiP
 *
 * This class computes centered L2-discrepancy of samples.
 */
class OT_API SpaceFillingPhiP
  : public SpaceFillingImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  explicit SpaceFillingPhiP(const UnsignedInteger p = 50);

  /** Virtual constructor method */
  SpaceFillingPhiP * clone() const override;

  /** Evaluate criterion on a sample */
  Scalar evaluate(const Sample& sample) const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Compute criterion when performing an elementary perturbation */
  Scalar perturbLHS(Sample& oldDesign, Scalar oldCriterion,
                    UnsignedInteger row1, UnsignedInteger row2, UnsignedInteger column) const override;

private:
  UnsignedInteger p_;

}; /* class SpaceFillingPhiP */

} /* namespace OT */

#endif /* OT_SPACEFILLINGPHIP_HXX */
