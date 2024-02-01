//                                               -*- C++ -*-
/**
 *  @brief The class that implements parametric functions.
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
#ifndef OPENTURNS_PARAMETRICFUNCTION_HXX
#define OPENTURNS_PARAMETRICFUNCTION_HXX

#include "openturns/Function.hxx"
#include "openturns/Point.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ParametricFunction
 *
 */
class OT_API ParametricFunction
  : public Function
{
  CLASSNAME
public:

  /* Default constructor */
  ParametricFunction();

  /** Parameter constructor */
  ParametricFunction (const Function & function,
                      const Indices & set,
                      const Point & referencePoint,
                      const Bool parametersSet = true);

  /** Comparison operator */
  using Function::operator ==;
  Bool operator ==(const ParametricFunction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class QuadraticFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PARAMETRICFUNCTION_HXX */
