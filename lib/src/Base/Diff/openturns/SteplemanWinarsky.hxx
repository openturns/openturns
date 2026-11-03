//                                               -*- C++ -*-
/**
 *  @brief Adaptive step search of Stepleman and Winarsky
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

#ifndef OPENTURNS_STEPLEMANWINARSKY_HXX
#define OPENTURNS_STEPLEMANWINARSKY_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Function.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SteplemanWinarskyResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SteplemanWinarsky
 * @brief Adaptive numerical differentiation step search
 *
 * This class implements the adaptive procedure of
 * R. S. Stepleman and N. D. Winarsky, Adaptive Numerical Differentiation,
 * Mathematics of Computation, 33(148), 1257-1264, 1979, which selects
 * a finite-difference step balancing truncation and round-off errors
 * by successive reductions h_{i+1} = alpha h_i with 0 < alpha < 1.
 * The selected step is designed for the first-order centered
 * finite-difference gradient.
 */
class OT_API SteplemanWinarsky
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  SteplemanWinarsky();

  /** Parameters constructor */
  SteplemanWinarsky(const Function & function,
                    const Point & x,
                    const Point & initialStep);

  /** Virtual constructor */
  SteplemanWinarsky * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Comparison operator */
  using PersistentObject::operator ==;
  Bool operator ==(const SteplemanWinarsky & other) const;

  /** Run the adaptive search, populates result_ */
  void run();

  /** Result accessor */
  SteplemanWinarskyResult getResult() const;

  /** Function accessor */
  void setFunction(const Function & function);
  Function getFunction() const;

  /** Point accessor */
  void setX(const Point & x);
  Point getX() const;

  /** Initial step accessor */
  void setInitialStep(const Point & initialStep);
  Point getInitialStep() const;

  /** Alpha accessor */
  void setAlpha(const Scalar alpha);
  Scalar getAlpha() const;

  /** Maximum iterations accessor */
  void setMaximumIterationNumber(const UnsignedInteger maximumIterationNumber);
  UnsignedInteger getMaximumIterationNumber() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Function function_;
  Point x_;
  Point initialStep_;
  Scalar alpha_ = 0.0;
  UnsignedInteger maximumIterationNumber_ = 0;
  SteplemanWinarskyResult result_;

}; /* class SteplemanWinarsky */


END_NAMESPACE_OPENTURNS


#endif
