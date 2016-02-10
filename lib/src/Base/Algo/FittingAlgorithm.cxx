//                                               -*- C++ -*-
/**
 *  @brief Cross-validation algorithm interface
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

#include "FittingAlgorithm.hxx"
#include "FittingAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(FittingAlgorithm);

/* Default constructor */
FittingAlgorithm::FittingAlgorithm()
  : TypedInterfaceObject<FittingAlgorithmImplementation>()
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

NumericalScalar FittingAlgorithm::run(const NumericalSample & x,
                                      const NumericalSample & y,
                                      const NumericalPoint & weight,
                                      const Basis & basis,
                                      const Indices & indices) const
{
  return getImplementation()->run(x, y, weight, basis, indices);
}

NumericalScalar FittingAlgorithm::run(const NumericalSample & x,
                                      const NumericalSample & y,
                                      const Basis & basis,
                                      const Indices & indices) const
{
  return getImplementation()->run(x, y, basis, indices);
}

NumericalScalar FittingAlgorithm::run(const NumericalSample & y,
                                      const NumericalPoint & weight,
                                      const Indices & indices,
                                      const DesignProxy & proxy) const
{
  return getImplementation()->run(y, weight, indices, proxy);
}

NumericalScalar FittingAlgorithm::run(LeastSquaresMethod & method) const
{
  return getImplementation()->run(method);
}
END_NAMESPACE_OPENTURNS
