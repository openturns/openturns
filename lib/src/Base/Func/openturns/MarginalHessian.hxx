//                                               -*- C++ -*-
/**
 *  @brief Marginal hessian
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_MARGINALHESSIAN_HXX
#define OPENTURNS_MARGINALHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class MarginalHessian
 *
 * Marginal hessian
 */
class OT_API MarginalHessian
  : public HessianImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  MarginalHessian();

  /** Parameters constructor */
  MarginalHessian(const HessianImplementation & hessian,
                  const Indices & indices);

#ifndef SWIG
  /** Parameters constructor */
  explicit MarginalHessian(const Pointer<HessianImplementation> & p_hessian,
                           const Indices & indices);
#endif

  /** Virtual constructor */
  virtual MarginalHessian * clone() const;

  /** Comparison operator */
  Bool operator ==(const MarginalHessian & other) const;

  /** String converter */
  virtual String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /** Hessian method */
  virtual SymmetricTensor hessian(const Point & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** The underlying function */
  Pointer<HessianImplementation> p_hessian_;

  /** The indices of the marginals */
  Indices indices_;

}; /* class MarginalHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MARGINALHESSIAN_HXX */
