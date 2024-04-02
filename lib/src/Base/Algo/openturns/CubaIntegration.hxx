//                                               -*- C++ -*-
/**
 *  @brief Implement the Cuba method for multidimensional integration
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

  /** Default constructor without parameters */
  CubaIntegration();

  /** Parameter constructor */
  explicit CubaIntegration(const String optRoutine);

  /** Virtual copy constructor */
  CubaIntegration * clone() const override;

  /* Compute an approximation of \int_{I}f(x)dx, where I is the integration interval */
  using IntegrationAlgorithmImplementation::integrate;
  Point integrate(const Function & function,
                  const Interval & interval) const override;

  /** epsRel accessor */
  Scalar getEpsRel() const;
  void setEpsRel(const Scalar epsRel);

  /** epsAbs accessor */
  Scalar getEpsAbs() const;
  void setEpsAbs(const Scalar epsAbs);

  /** optRoutine accessor */
  String getOptRoutine() const;
  void setOptRoutine(const String optRoutine);

  /** maxeval accessor */
  UnsignedInteger getMaxeval() const;
  void setMaxeval(const UnsignedInteger maxEval);

  /** flags accessor */
  UnsignedInteger getFlags() const;
  void setFlags(const UnsignedInteger flags);

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;
private:

  /* Friend C routine to compute the integrand function with parameter types
     suitable for Cuba */
  friend int computeIntegrand(const int *ndim, const double x[], const int *ncomp, double f[], void *userdata);

  /* The number of dimensions of the integral */
  UnsignedInteger nDim_ = 0;

  /* The number of components of the integrand */
  UnsignedInteger nComp_ = 0;

  /* The requested relative accuracy */
  Scalar epsRel_ = 1.e-4;

  /* The requested absolute accuracy */
  Scalar epsAbs_ = 1.e-4;

  /* The name of the optimization routine to use */
  String optRoutine_;

  /* The maximal number of evaluations of the integrand */
  UnsignedInteger maxeval_ = 1000000;

  /* The flags to use for the integration */
  UnsignedInteger flags_ = 2;
} ; /* class CubaIntegration */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CUBAINTEGRATION_HXX */
