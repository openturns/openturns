//                                               -*- C++ -*-
/**
 * @brief Penalized gradient, returning zeros on failure
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

#ifndef OPENTURNS_PENALIZEDGRADIENT_HXX
#define OPENTURNS_PENALIZEDGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/Gradient.hxx"
#include "openturns/Point.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PenalizedGradient
 *
 * This class wraps a Gradient and returns zeros
 * when the underlying gradient throws, e.g. for optimization.
 * This is consistent with PenalizedEvaluation: the penalized
 * function is locally constant on failure, hence its gradient is zero.
 */
class OT_API PenalizedGradient
  : public GradientImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PenalizedGradient();

  /** Parameter constructor */
  explicit PenalizedGradient(const Gradient & gradient);

  /** Virtual constructor */
  PenalizedGradient * clone() const override;

  /** Gradient implementation accessors */
  void setGradient(const Gradient & gradient);
  Gradient getGradient() const;

  /** Comparison operator */
  using GradientImplementation::operator ==;
  Bool operator ==(const PenalizedGradient & other) const;
protected:
  Bool equals(const GradientImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /** Gradient method */
  Matrix gradient(const Point & inP) const override;

  /** Test for actual implementation */
  Bool isActualImplementation() const override;

  /** Get the gradient corresponding to indices components */
  using GradientImplementation::getMarginal;
  Gradient getMarginal(const UnsignedInteger i) const override;
  Gradient getMarginal(const Indices & indices) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Parameters value accessor */
  Point getParameter() const override;
  void setParameter(const Point & parameter) override;

  /** Get the number of calls to gradient() */
  using GradientImplementation::getCallsNumber;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The wrapped gradient */
  Gradient gradient_;

}; /* class PenalizedGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PENALIZEDGRADIENT_HXX */
