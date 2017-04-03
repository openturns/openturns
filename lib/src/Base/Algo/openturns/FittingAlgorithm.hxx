//                                               -*- C++ -*-
/**
 *  @brief Cross-validation algorithm interface
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_FITTINGALGORITHM_HXX
#define OPENTURNS_FITTINGALGORITHM_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/FittingAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FittingAlgorithm
 *
 * Cross-validation algorithm interface
 */
class OT_API FittingAlgorithm
  : public TypedInterfaceObject<FittingAlgorithmImplementation>
{
  CLASSNAME;

public:

  typedef TypedInterfaceObject<FittingAlgorithmImplementation>::Implementation  Implementation;

  /** Default constructor */
  FittingAlgorithm();

  /** Constructor from implementation */
  FittingAlgorithm(const Implementation & p_implementation);

  /** Constructor from implementation */
  FittingAlgorithm(const FittingAlgorithmImplementation & implementation);

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Perform cross-validation */
  NumericalScalar run(const Sample & x,
                      const Sample & y,
                      const Point & weight,
                      const Basis & psi,
                      const Indices & indices) const;

  NumericalScalar run(const Sample & x,
                      const Sample & y,
                      const Basis & psi,
                      const Indices & indices) const;

#ifndef SWIG
  NumericalScalar run(const Sample & y,
                      const Point & weight,
                      const Indices & indices,
                      const DesignProxy & proxy) const;

  NumericalScalar run(LeastSquaresMethod & method,
                      const Sample & y) const;
#endif

}; /* class FittingAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FITTINGALGORITHM_HXX */
