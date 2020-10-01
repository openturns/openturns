//                                               -*- C++ -*-
/**
 *  @brief The hessian part of functional linear combination of vectors
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
#ifndef OPENTURNS_DUALLINEARCOMBINATIONHESSIAN_HXX
#define OPENTURNS_DUALLINEARCOMBINATIONHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"
#include "openturns/DualLinearCombinationEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class DualLinearCombinationHessian
 *
 * The hessian part of functional linear combination of vectors
 */

class OT_API DualLinearCombinationHessian
  : public HessianImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  DualLinearCombinationHessian();

  /** Parameters constructor */
  explicit DualLinearCombinationHessian(const DualLinearCombinationEvaluation & evaluation);

#ifndef SWIG
  /** Parameters constructor */
  explicit DualLinearCombinationHessian(const Pointer<DualLinearCombinationEvaluation> & p_evaluation);
#endif

  /** Virtual constructor */
  DualLinearCombinationHessian * clone() const override;

  /** Hessian method */
  SymmetricTensor hessian(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:

private:
  /** The associated evaluation */
  Pointer<DualLinearCombinationEvaluation> p_evaluation_;

} ; /* class DualLinearCombinationHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DUALLINEARCOMBINATIONHESSIAN_HXX */
