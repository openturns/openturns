//                                               -*- C++ -*-
/**
 *  @brief This class implements the adaptive Fehlberg method of order p/p+1
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FEHLBERG_HXX
#define OPENTURNS_FEHLBERG_HXX

#include "openturns/ODESolverImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Fehlberg
 *
 * Adaptive Fehlberg method of order p/p+1
 */
class OT_API Fehlberg
  : public ODESolverImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  Fehlberg();

  /** Parameter constructor */
  explicit Fehlberg(const Function & transitionFunction,
                    const Scalar localPrecision = ResourceMap::GetAsScalar("Fehlberg-LocalPrecision"),
                    const UnsignedInteger order = ResourceMap::GetAsUnsignedInteger("Fehlberg-DefaultOrder"));

  /** Virtual constructor */
  Fehlberg * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Solve the ODE */
  using ODESolverImplementation::solve;

  Sample solve(const Point & initialState,
               const Point & timeGrid) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** Perform one step of the Fehlberg method */
  Point computeStep(Pointer<EvaluationImplementation> & transitionFunction,
                    const Scalar t,
                    const Point & state,
                    Point & gradient,
                    Scalar & h) const;

  /* Local absolute error target */
  Scalar localPrecision_;

  /* Integration order p/p+1 */
  UnsignedInteger order_;

  /* Alpha coefficients */
  Point alpha_;

  /* Beta coefficients */
  Point beta_;

  /* C coefficients */
  Point c_;

  /* Chat coefficients */
  Point cHat_;
}; /* class Fehlberg */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FEHLBERG_HXX */
