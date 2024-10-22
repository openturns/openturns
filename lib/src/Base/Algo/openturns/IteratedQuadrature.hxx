//                                               -*- C++ -*-
/**
 *  @brief This class allows one to compute integrals of a function over a
 *         domain defined by functions
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

  CLASSNAME

public:

  typedef Collection< Function > FunctionCollection;

  /** Default constructor without parameters */
  IteratedQuadrature();

  /** Parameter constructor */
  IteratedQuadrature(const IntegrationAlgorithm & algorithm);

  /** Virtual copy constructor */
  IteratedQuadrature * clone() const override;

  /** Compute an approximation of \int_a^b\int_{L_1(x_1)}^{U_1(x_1)}\int_{L_1(x_1,x_2)}^{U_2(x_1,x_2)}\dots\int_{L_1(x_1,\dots,x_{n-1})}^{U_2(x_1,\dots,x_{n-1})} f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an 1D interval, L_k and U_k are functions from R^k into R.
   */
  using IntegrationAlgorithmImplementation::integrate;
  Point integrate(const Function & function,
                  const Interval & interval) const override;

  /** This method allows one to get the estimated integration error as a scalar */
  Point integrate(const Function & function,
                  const Scalar a,
                  const Scalar b,
                  const FunctionCollection & lowerBounds,
                  const FunctionCollection & upperBounds,
                  const Bool check = true) const;

  /** Accessors to the underlying integration algorithm */
  IntegrationAlgorithm getAlgorithm() const;

  void setAlgorithm(const IntegrationAlgorithm & algorithm);

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /* Underlying integration algorithm */
  IntegrationAlgorithm algorithm_;

} ; /* class IteratedQuadrature */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATEDQUADRATURE_HXX */
