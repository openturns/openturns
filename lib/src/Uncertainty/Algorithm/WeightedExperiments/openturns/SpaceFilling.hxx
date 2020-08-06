//                                               -*- C++ -*-
/**
 *  @brief Base class for space filling criteria
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
#ifndef OPENTURNS_SPACEFILLING_HXX
#define OPENTURNS_SPACEFILLING_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/SpaceFillingImplementation.hxx"

namespace OT
{

/**
 * @class SpaceFilling
 *
 * SpaceFilling is some optimallhs type to illustrate how to add some classes in Open TURNS
 */
class OT_API SpaceFilling
  : public TypedInterfaceObject<SpaceFillingImplementation>
{
  CLASSNAME

  /* Allow OptimalLHS derived classes to call perturb method */
  friend class OptimalLHS;
  friend class SimulatedAnnealingLHS;

public:

  typedef Pointer<SpaceFillingImplementation>  Implementation;

  /** Default constructor */
  SpaceFilling();

  /** Copy constructor */
  SpaceFilling(const SpaceFillingImplementation & implementation);

  /** Constructor from implementation */
  SpaceFilling(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  SpaceFilling(SpaceFillingImplementation * p_implementation);
#endif


  /** Evaluate criterion on a sample */
  Scalar evaluate(const Sample & sample) const;

  /** Accessor */
  Bool isMinimizationProblem() const;

  /** String converter */
  String __repr__() const override;

  /** Compute criterion when performing an elementary perturbation */
  Scalar perturbLHS(Sample& oldDesign, Scalar oldCriterion,
                    UnsignedInteger row1, UnsignedInteger row2, UnsignedInteger column) const;

}; /* class SpaceFilling */

} /* namespace OT */

#endif /* OT_SPACEFILLING_HXX */
