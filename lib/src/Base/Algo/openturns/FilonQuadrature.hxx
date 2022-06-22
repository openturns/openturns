//                                               -*- C++ -*-
/**
 *  @brief Implement the Filon quadrature for oscilatory integrands
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FILONQUADRATURE_HXX
#define OPENTURNS_FILONQUADRATURE_HXX

#include "openturns/IntegrationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FilonQuadrature
 */

class OT_API FilonQuadrature
  : public IntegrationAlgorithmImplementation
{

  CLASSNAME

public:

  /** Default constructor without parameters */
  FilonQuadrature();

  /** Parameter constructor */
  explicit FilonQuadrature(const UnsignedInteger n,
                           const Scalar omega = 1.0,
                           const UnsignedInteger kind = 0);

  /** Virtual copy constructor */
  FilonQuadrature * clone() const override;

  /** Compute an approximation of \int_{[a,b]}f(x)w(omega*x)dx, where [a,b]
   * is an 1D interval and:
   * if kind == 0, w(omega*x) = cos(omega*x)
   * if kind == 1, w(omega*x) = sin(omega*x)
   * if kind >= 2, w(omega*x) = exp(I*omega*x) with I^2=-1
   */
  using IntegrationAlgorithmImplementation::integrate;
  Point integrate(const Function & function,
                  const Interval & interval) const override;

  Point integrate(const Function & function,
                  const Scalar omega,
                  const Interval & interval) const;

  /** N accessor */
  UnsignedInteger getN() const;
  void setN(const UnsignedInteger n);

  /** Omega accessor */
  Scalar getOmega() const;
  void setOmega(const Scalar omega);

  /** Kind accessor */
  UnsignedInteger getKind() const;
  void setKind(const UnsignedInteger kind);

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;
private:

  /* Discretization 2*n+1 points */
  UnsignedInteger n_;

  /* Pulsation */
  Scalar omega_;

  /* Kind: nature of the oscilating kernel */
  UnsignedInteger kind_;
} ; /* class FilonQuadrature */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FILONQUADRATURE_HXX */
