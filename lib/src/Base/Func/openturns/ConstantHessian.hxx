//                                               -*- C++ -*-
/**
 *  @brief Class for a constant numerical math hessian implementation
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
#ifndef OPENTURNS_CONSTANTHESSIAN_HXX
#define OPENTURNS_CONSTANTHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ConstantHessian
 *
 * This class offers an interface for a real numerical mathematical hessian
 * implementations into the platform. It realizes the computation of the image symmetric tensor
 * of a vector (aka Point) through the hessian.
 */
class OT_API ConstantHessian
  : public HessianImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  ConstantHessian();

  /** Parameter constructor */
  ConstantHessian(const SymmetricTensor & constant);

  /** Virtual constructor */
  virtual ConstantHessian * clone() const;

  /** Comparison operator */
  Bool operator ==(const ConstantHessian & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /* Here is the interface that all derived class must implement */

  /** Hessian method */
  virtual SymmetricTensor hessian(const Point & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Accessor for the constant term */
  SymmetricTensor getConstant() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:

  SymmetricTensor constant_;

}; /* class ConstantHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONSTANTHESSIAN_HXX */
