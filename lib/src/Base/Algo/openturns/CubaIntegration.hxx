//                                               -*- C++ -*-
/**
 *  @brief Implement the Cuba method for multidimensional integration
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
#ifndef OPENTURNS_CUBAINTEGRATION_HXX
#define OPENTURNS_CUBAINTEGRATION_HXX

#include "openturns/IntegrationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CubaIntegration
 */

class OT_API CubaIntegration
  : public IntegrationAlgorithmImplementation
{

  CLASSNAME

public:

  /** Parameter constructor */
  explicit CubaIntegration(const String algorithmName = "vegas");

  /** Virtual copy constructor */
  CubaIntegration * clone() const override;

  /* Compute an approximation of \int_{I}f(x)dx, where I is the integration interval */
  using IntegrationAlgorithmImplementation::integrate;
  Point integrate(const Function & function,
                  const Interval & interval) const override;

  /** Maximum relative error accessor */
  Scalar getMaximumRelativeError() const;
  void setMaximumRelativeError(const Scalar maximumRelativeError);

  /** Maximum absolute error accessor */
  Scalar getMaximumAbsoluteError() const;
  void setMaximumAbsoluteError(const Scalar maximumAbsoluteError);

  /** Algorithm name accessor */
  String getAlgorithmName() const;
  void setAlgorithmName(const String optAlgo);

  /** Maximum calls number accessor */
  UnsignedInteger getMaximumCallsNumber() const;
  void setMaximumCallsNumber(const UnsignedInteger maximumCallsNumber);

  /** Available algorithm names accessor */
  static Description GetAlgorithmNames();

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;
private:

  /* Static routine to compute the integrand function with parameter types
     suitable for Cuba */
  static int ComputeIntegrand(const int *ndim, const double x[], const int *ncomp, double f[], void *userdata);

  /* The name of the optimization routine to use */
  String algorithmName_ = "vegas";

  /* The requested relative accuracy */
  Scalar maximumRelativeError_ = 0.0;

  /* The requested absolute accuracy */
  Scalar maximumAbsoluteError_ = 0.0;

  /* The maximal number of evaluations of the integrand */
  UnsignedInteger maximumCallsNumber_ = 0;

} ; /* class CubaIntegration */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CUBAINTEGRATION_HXX */
