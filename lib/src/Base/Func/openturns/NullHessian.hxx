//                                               -*- C++ -*-
/**
 *  @brief Null hessian
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
#ifndef OPENTURNS_NULLHESSIAN_HXX
#define OPENTURNS_NULLHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class NullHessian
 *
 * Null hessian
 */
class OT_API NullHessian
  : public HessianImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  NullHessian();

  /** Default constructor */
  NullHessian(const UnsignedInteger inputDimension, const UnsignedInteger outputDimension);

  /** Virtual constructor */
  NullHessian * clone() const override;

  /** Comparison operator */
  Bool operator ==(const NullHessian & other) const;

  /** String converter */
  String __repr__() const override;

  /* Here is the interface that all derived class must implement */

  /** Hessian method */
  SymmetricTensor hessian(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  UnsignedInteger inputDimension_;

  UnsignedInteger outputDimension_;
private:

}; /* class NullHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NULLHESSIAN_HXX */
