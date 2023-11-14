//                                               -*- C++ -*-
/**
 *  @brief Class for Box Cox function implementation
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

#ifndef OPENTURNS_BOXCOXGRADIENT_HXX
#define OPENTURNS_BOXCOXGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/BoxCoxEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoxCoxGradient
 *
 * This class offers an easy gradient of the Box Cox function defined by :
 *  \Delta(h)(x) = (x-s)^{\lambda - 1} for \lambda non zero, 1/(x-s) otherwise
 *  Care that x should be > s
 */

class OT_API BoxCoxGradient
  : public GradientImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  BoxCoxGradient();

  /** Parameter constructor */
  explicit BoxCoxGradient(const BoxCoxEvaluation & evaluation);

#ifndef SWIG
  /** Parameters constructor */
  explicit BoxCoxGradient(const Pointer<BoxCoxEvaluation> & p_evaluation);
#endif

  /** Virtual constructor */
  BoxCoxGradient * clone() const override;

  /** Comparison operator */
  using GradientImplementation::operator ==;
  Bool operator ==(const BoxCoxGradient & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Gradient evaluation method */
  using GradientImplementation::gradient;
  Matrix gradient(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Accessor for the lambda point */
  Point getLambda() const;

  /** Accessor for the shift */
  Point getShift() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** The underlying evaluation */
  Pointer<BoxCoxEvaluation> p_evaluation_;

}; /* class BoxCoxGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOXCOXGRADIENT_HXX */
