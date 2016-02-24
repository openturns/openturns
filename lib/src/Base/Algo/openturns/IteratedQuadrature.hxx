//                                               -*- C++ -*-
/**
 *  @brief This class allows to compute integrals of a function over a
 *         domain defined by functions
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_ITERATEDQUADRATURE_HXX
#define OPENTURNS_ITERATEDQUADRATURE_HXX

#include "openturns/IntegrationAlgorithmImplementation.hxx"
#include "openturns/IntegrationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IteratedQuadrature
 */

class OT_API IteratedQuadrature
  : public IntegrationAlgorithmImplementation
{

  CLASSNAME;

public:

  typedef Collection< NumericalMathFunction > NumericalMathFunctionCollection;

  /** Default constructor without parameters */
  IteratedQuadrature();

  /** Parameter constructor */
  IteratedQuadrature(const IntegrationAlgorithm & algorithm);

  /** Virtual copy constructor */
  virtual IteratedQuadrature * clone() const;

  /** Compute an approximation of \int_a^b\int_{L_1(x_1)}^{U_1(x_1)}\int_{L_1(x_1,x_2)}^{U_2(x_1,x_2)}\dots\int_{L_1(x_1,\dots,x_{n-1})}^{U_2(x_1,\dots,x_{n-1})} f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an 1D interval, L_k and U_k are functions from R^k into R.
   */
  using IntegrationAlgorithmImplementation::integrate;
  NumericalPoint integrate(const NumericalMathFunction & function,
                           const Interval & interval) const;

  // This method allows to get the estimated integration error as a scalar
  NumericalPoint integrate(const NumericalMathFunction & function,
                           const NumericalScalar a,
                           const NumericalScalar b,
                           const NumericalMathFunctionCollection & lowerBounds,
                           const NumericalMathFunctionCollection & upperBounds,
                           const Bool check = true) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

private:

  /* Underlying integration algorithm */
  IntegrationAlgorithm algorithm_;

} ; /* class IteratedQuadrature */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATEDQUADRATURE_HXX */
