//                                               -*- C++ -*-
/**
 *  @brief Cross-validation algorithm interface
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

#include "openturns/FittingAlgorithm.hxx"
#include "openturns/FittingAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(FittingAlgorithm)

/* Default constructor */
FittingAlgorithm::FittingAlgorithm()
  : TypedInterfaceObject<FittingAlgorithmImplementation>(new FittingAlgorithmImplementation())
{
  // Nothing to do
}

/* Constructor from implementation */
FittingAlgorithm::FittingAlgorithm(const Implementation & p_implementation)
  : TypedInterfaceObject<FittingAlgorithmImplementation>( p_implementation )
{
  // Nothing to do
}

/* Constructor from implementation */
FittingAlgorithm::FittingAlgorithm(const FittingAlgorithmImplementation & implementation)
  : TypedInterfaceObject<FittingAlgorithmImplementation>( implementation.clone() )
{
  // Nothing to do
}

/* String converter */
String FittingAlgorithm::__repr__() const
{
  return getImplementation()->__repr__();
}

String FittingAlgorithm::__str__(const String & offset) const
{
  return getImplementation()->__str__( offset );
}

Scalar FittingAlgorithm::run(const Sample & x,
                             const Sample & y,
                             const Point & weight,
                             const FunctionCollection & basis,
                             const Indices & indices) const
{
  return getImplementation()->run(x, y, weight, basis, indices);
}

Scalar FittingAlgorithm::run(const Sample & x,
                             const Sample & y,
                             const FunctionCollection & basis,
                             const Indices & indices) const
{
  return getImplementation()->run(x, y, basis, indices);
}

Scalar FittingAlgorithm::run(const Sample & y,
                             const Point & weight,
                             const Indices & indices,
                             const DesignProxy & proxy) const
{
  return getImplementation()->run(y, weight, indices, proxy);
}

Scalar FittingAlgorithm::run(LeastSquaresMethod & method,
                             const Sample & y) const
{
  return getImplementation()->run(method, y);
}
END_NAMESPACE_OPENTURNS
