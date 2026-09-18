//                                               -*- C++ -*-
/**
 * @brief Penalized hessian, returning zeros on failure
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_PENALIZEDHESSIAN_HXX
#define OPENTURNS_PENALIZEDHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"
#include "openturns/Hessian.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/Point.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PenalizedHessian
 *
 * This class wraps a Hessian and returns zeros
 * when the underlying hessian throws, e.g. for optimization.
 * This is consistent with PenalizedEvaluation: the penalized
 * function is locally constant on failure, hence its hessian is zero.
 * If an evaluation is provided, zeros are also returned wherever
 * the evaluation fails, even when the hessian itself would succeed.
 * This keeps the derivative consistent with the penalized values.
 */
class OT_API PenalizedHessian
  : public HessianImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PenalizedHessian();

  /** Parameter constructor */
  explicit PenalizedHessian(const Hessian & hessian);

  /** Parameter constructor with coordinated evaluation */
  PenalizedHessian(const Hessian & hessian,
                   const Evaluation & evaluation);

  /** Virtual constructor */
  PenalizedHessian * clone() const override;

  /** Hessian implementation accessors */
  void setHessian(const Hessian & hessian);
  Hessian getHessian() const;

  /** Coordinated evaluation accessors */
  void setEvaluation(const Evaluation & evaluation);
  Evaluation getEvaluation() const;

  /** Comparison operator */
  using HessianImplementation::operator ==;
  Bool operator ==(const PenalizedHessian & other) const;
protected:
  Bool equals(const HessianImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /** Hessian method */
  SymmetricTensor hessian(const Point & inP) const override;

  /** Test for actual implementation */
  Bool isActualImplementation() const override;

  /** Get the hessian corresponding to indices components */
  using HessianImplementation::getMarginal;
  Hessian getMarginal(const UnsignedInteger i) const override;
  Hessian getMarginal(const Indices & indices) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Parameters value accessor */
  Point getParameter() const override;
  void setParameter(const Point & parameter) override;

  /** Get the number of calls to hessian() */
  using HessianImplementation::getCallsNumber;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Check that the coordinated evaluation matches the hessian dimensions */
  void checkDimensions() const;

  /** The wrapped hessian */
  Hessian hessian_;

  /** The coordinated evaluation (returns zeros where it fails) */
  Evaluation evaluation_;

}; /* class PenalizedHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PENALIZEDHESSIAN_HXX */
