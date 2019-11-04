//                                               -*- C++ -*-
/**
 *  @brief Space filling criterion based on centered L2-discrepancy
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SPACEFILLINGC2_HXX
#define OPENTURNS_SPACEFILLINGC2_HXX

#include "openturns/SpaceFillingImplementation.hxx"

namespace OT
{

/**
 * @class SpaceFillingC2
 *
 * This class computes centered L2-discrepancy of samples.
 */
class OT_API SpaceFillingC2
  : public SpaceFillingImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  SpaceFillingC2();

  /** Virtual constructor method */
  SpaceFillingC2 * clone() const;

  /** Evaluate criterion on a sample */
  Scalar evaluate(const Sample& sample) const;

  /** String converter */
  String __repr__() const;

  /** Compute criterion when performing an elementary perturbation */
  Scalar perturbLHS(Sample& oldDesign, Scalar oldCriterion,
                    UnsignedInteger row1, UnsignedInteger row2, UnsignedInteger column) const;

private:

}; /* class SpaceFillingC2 */

} /* namespace OT */

#endif /* OT_SPACEFILLINGC2_HXX */
