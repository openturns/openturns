//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math hessian implementations
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_NONUMERICALMATHHESSIANIMPLEMENTATION_HXX
#define OPENTURNS_NONUMERICALMATHHESSIANIMPLEMENTATION_HXX

#include "NumericalMathHessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class NoNumericalMathHessianImplementation
 *
 * This class offers an interface for an real numerical mathematical hessian
 * implementations into the platform. It realizes the computation of the image vector
 * of a vector (aka NumericalPoint) through the hessian. The
 * hessian may be loaded in an external way through a wrapper.
 */
class OT_API NoNumericalMathHessianImplementation
  : public NumericalMathHessianImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  NoNumericalMathHessianImplementation();

  /** Virtual constructor */
  virtual NoNumericalMathHessianImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const NoNumericalMathHessianImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** Test for actual implementation */
  virtual Bool isActualImplementation() const;



  /* Here is the interface that all derived class must implement */

  /** Hessian method */
  virtual SymmetricTensor hessian(const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

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

}; /* class NoNumericalMathHessianImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NONUMERICALMATHHESSIANIMPLEMENTATION_HXX */
