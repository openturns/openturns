//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math gradient implementations
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
#ifndef OPENTURNS_NOGRADIENT_HXX
#define OPENTURNS_NOGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class NoGradient
 *
 * This class offers an interface for an real numerical mathematical gradient
 * implementations into the platform. It realizes the computation of the image vector
 * of a vector (aka Point) through the gradient. The
 * gradient may be loaded in an external way through a wrapper.
 */
class OT_API NoGradient
  : public GradientImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  NoGradient();

  /** Virtual constructor */
  NoGradient * clone() const override;

  /** Comparison operator */
  using GradientImplementation::operator ==;
  Bool operator ==(const NoGradient & other) const;

  /** String converter */
  String __repr__() const override;



  /* Here is the interface that all derived class must implement */

  /** Gradient method */
  Matrix gradient(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Test for actual implementation */
  Bool isActualImplementation() const override;

  /** Method save() stores the object through the StorageManager
   *  The inherited method is sufficient as we do not have any
   *  attribute
   */

  /** Method load() reloads the object from the StorageManager
   *  The inherited method is sufficient as we do not have any
   *  attribute
   */

protected:

  /** The gradient that actually realizes assignment between objects */

private:

}; /* class NoGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NOGRADIENT_HXX */
