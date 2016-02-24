//                                               -*- C++ -*-
/**
 *  @brief This class allows to compute integrals of a function over an interval
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_INTEGRATIONALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_INTEGRATIONALGORITHMIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IntegrationAlgorithmImplementation
 */

class OT_API IntegrationAlgorithmImplementation
  : public PersistentObject
{

  CLASSNAME;

public:

  /** Default constructor without parameters */
  IntegrationAlgorithmImplementation();

  /** Virtual copy constructor */
  virtual IntegrationAlgorithmImplementation * clone() const;

  /** Compute an approximation of \int_{[a,b]}f(x)dx, where [a,b]
   * is an n-D interval
   */
  virtual NumericalPoint integrate(const NumericalMathFunction & function,
                                   const Interval & interval) const;

#ifndef SWIG
  virtual NumericalPoint integrate(const NumericalMathFunction & function,
                                   const Interval & interval,
                                   NumericalScalar & error) const;
#endif

  virtual NumericalPoint integrate(const NumericalMathFunction & function,
                                   const Interval & interval,
                                   NumericalPoint & error) const;
  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

} ; /* class IntegrationAlgorithmImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INTEGRATIONALGORITHMIMPLEMENTATION_HXX */
