//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math hessian implementations
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_NOHESSIAN_HXX
#define OPENTURNS_NOHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class NoHessian
 *
 * This class offers an interface for an real numerical mathematical hessian
 * implementations into the platform. It realizes the computation of the image vector
 * of a vector (aka Point) through the hessian. The
 * hessian may be loaded in an external way through a wrapper.
 */
class OT_API NoHessian
  : public HessianImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  NoHessian();

  /** Virtual constructor */
  NoHessian * clone() const override;

  /** Comparison operator */
  Bool operator ==(const NoHessian & other) const;

  /** String converter */
  String __repr__() const override;

  /** Test for actual implementation */
  Bool isActualImplementation() const override;



  /* Here is the interface that all derived class must implement */

  /** Hessian method */
  SymmetricTensor hessian(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager
   *  The inherited method is sufficient as we do not have any
   *  attribute
   */

  /** Method load() reloads the object from the StorageManager
   *  The inherited method is sufficient as we do not have any
   *  attribute
   */



protected:

  /** The hessian that actually realizes assignment between objects */

private:

}; /* class NoHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NOHESSIAN_HXX */
