//                                               -*- C++ -*-
/**
 *  @brief Class for Box Cox function implementation
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

#ifndef OPENTURNS_BOXCOXHESSIAN_HXX
#define OPENTURNS_BOXCOXHESSIAN_HXX

#include "openturns/Hessian.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/BoxCoxEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoxCoxHessian
 *
 * This class offers an easy hessian of the Box Cox function defined by :
 *  \Delta(h)(x) = (\lambda - 1)(x-s)^{\lambda - 2} for \lambda non zero, -1/(x-s)^2 otherwise
 *  Care that x should be > s
 */

class OT_API BoxCoxHessian
  : public HessianImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  BoxCoxHessian();

  /** Parameter constructor */
  explicit BoxCoxHessian(const BoxCoxEvaluation & evaluation);

#ifndef SWIG
  /** Parameters constructor */
  explicit BoxCoxHessian(const Pointer<BoxCoxEvaluation> & p_evaluation);
#endif

  /** Virtual constructor */
  virtual BoxCoxHessian * clone() const;

  /** Comparison operator */
  Bool operator ==(const BoxCoxHessian & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Hessian evaluation method */
  using HessianImplementation::hessian;
  SymmetricTensor hessian(const Point & inP) const;

  /** Accessor for evaluation */
  Evaluation getEvaluation() const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for the lambda point */
  Point getLambda() const;

  /** Accessor for the shift */
  Point getShift() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** The underlying evaluation */
  Pointer<BoxCoxEvaluation> p_evaluation_;

}; /* class BoxCoxHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOXCOXHESSIAN_HXX */
