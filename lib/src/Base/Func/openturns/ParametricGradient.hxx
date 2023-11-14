//                                               -*- C++ -*-
/**
 *  @brief ParametricGradient
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PARAMETRICGRADIENT_HXX
#define OPENTURNS_PARAMETRICGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/ParametricEvaluation.hxx"
#include "openturns/Evaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ParametricGradient
 *
 * ParametricGradient implements the concept of parametric function for its gradient part
 */
class OT_API ParametricGradient
  : public GradientImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  ParametricGradient();

  /** Parameters constructor */
  explicit ParametricGradient(const ParametricEvaluation & evaluation);

#ifndef SWIG
  /** Parameters constructor */
  explicit ParametricGradient(const Pointer<ParametricEvaluation> & p_evaluation);
#endif

  /** Virtual constructor method */
  ParametricGradient * clone() const override;

  /** Gradient operator */
  using GradientImplementation::gradient;
  Matrix gradient(const Point & point) const override;

  /** Dimension accessor */
  UnsignedInteger getInputDimension() const override;
  UnsignedInteger getParameterDimension() const;
  UnsignedInteger getOutputDimension() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The underlying function */
  Pointer<ParametricEvaluation> p_evaluation_;

}; /* class ParametricGradient */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PARAMETRICGRADIENT_HXX */
