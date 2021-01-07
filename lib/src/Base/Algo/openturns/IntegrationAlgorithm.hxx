//                                               -*- C++ -*-
/**
 *  @brief  This class is enables to integrate a function over an interval
 *  This is the interface class
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
#ifndef OPENTURNS_INTEGRATIONALGORITHM_HXX
#define OPENTURNS_INTEGRATIONALGORITHM_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/IntegrationAlgorithmImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IntegrationAlgorithm
 */

class OT_API IntegrationAlgorithm
  : public TypedInterfaceObject<IntegrationAlgorithmImplementation>
{

  CLASSNAME

public:

  typedef Pointer<IntegrationAlgorithmImplementation>    Implementation;

  /** Default onstructor */
  IntegrationAlgorithm();

  /** Copy constructors */
  IntegrationAlgorithm(const IntegrationAlgorithmImplementation & implementation);

  /** Constructor from implementation */
  IntegrationAlgorithm(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  IntegrationAlgorithm(IntegrationAlgorithmImplementation * p_implementation);
#endif

  /** Compute an approximation of \int_{[a,b]}f(x)dx, where [a,b]
   * is an n-D interval
   */
  Point integrate(const Function & function,
                  const Interval & interval) const;

#ifndef SWIG
  Point integrate(const Function & function,
                  const Interval & interval,
                  Scalar & error) const;
#endif

  Point integrate(const Function & function,
                  const Interval & interval,
                  Point & error) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;


} ; /* class IntegrationAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INTEGRATIONALGORITHM_HXX */
