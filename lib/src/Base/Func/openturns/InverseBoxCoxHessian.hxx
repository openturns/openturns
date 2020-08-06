//                                               -*- C++ -*-
/**
 *  @brief Class for Box Cox function implementation
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

#ifndef OPENTURNS_INVERSEBOXCOXHESSIAN_HXX
#define OPENTURNS_INVERSEBOXCOXHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"
#include "openturns/InverseBoxCoxEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseBoxCoxHessian
 *
 * This class offers an easy hessian of the Box Cox function defined by :
 *  \Delta(h)(x) = (\lambda - 1)(x-s)^{\lambda - 2} for \lambda non zero, -1/(x-s)^2 otherwise
 *  Care that x should be > s
 */

class OT_API InverseBoxCoxHessian
  : public HessianImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  InverseBoxCoxHessian();

  /** Parameter constructor */
  explicit InverseBoxCoxHessian(const InverseBoxCoxEvaluation & evaluation);

#ifndef SWIG
  /** Parameter constructor */
  explicit InverseBoxCoxHessian(const Pointer<InverseBoxCoxEvaluation> & p_evaluation);
#endif

  /** Virtual constructor */
  InverseBoxCoxHessian * clone() const override;

  /** Comparison operator */
  Bool operator ==(const InverseBoxCoxHessian & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Hessian evaluation method */
  using HessianImplementation::hessian;
  SymmetricTensor hessian(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Accessor for the lambda point */
  Point getLambda() const;

  /** Accessor for the shift */
  Point getShift() const;

  /** Accessor for the evaluation */
  Evaluation getEvaluation() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** The underlying evaluation */
  Pointer<InverseBoxCoxEvaluation>  p_evaluation_;

}; /* class InverseBoxCoxHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSEBOXCOXHESSIAN_HXX */
