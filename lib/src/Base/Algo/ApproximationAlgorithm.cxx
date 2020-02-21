//                                               -*- C++ -*-
/**
 *  @brief Regression from a data sample upon a particular basis
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/ApproximationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ApproximationAlgorithm)

/* Default constructor */
ApproximationAlgorithm::ApproximationAlgorithm()
  : TypedInterfaceObject<ApproximationAlgorithmImplementation>(new ApproximationAlgorithmImplementation)
{
  // Nothing to do
}

/* Constructor from implementation */
ApproximationAlgorithm::ApproximationAlgorithm(const ApproximationAlgorithmImplementation & implementation)
  : TypedInterfaceObject<ApproximationAlgorithmImplementation>( implementation.clone() )
{
  // Nothing to do
}

/* Constructor from implementation */
ApproximationAlgorithm::ApproximationAlgorithm(const Implementation & p_implementation)
  : TypedInterfaceObject<ApproximationAlgorithmImplementation>( p_implementation )
{
  // Nothing to do
}

/* Accessors */
Sample ApproximationAlgorithm::getX() const
{
  return getImplementation()->getX();
}

Sample ApproximationAlgorithm::getY() const
{
  return getImplementation()->getY();
}

Point ApproximationAlgorithm::getWeight() const
{
  return getImplementation()->getWeight();
}

Basis ApproximationAlgorithm::getPsi() const
{
  return getImplementation()->getPsi();
}

/* Verbosity accessor */
void ApproximationAlgorithm::setVerbose(const Bool verbose)
{
  copyOnWrite();
  getImplementation()->setVerbose(verbose);
}

Bool ApproximationAlgorithm::getVerbose() const
{
  return getImplementation()->getVerbose();
}

/* String converter */
String ApproximationAlgorithm::__repr__() const
{
  return getImplementation()->__repr__();
}

String ApproximationAlgorithm::__str__(const String & offset) const
{
  return getImplementation()->__str__( offset );
}

/* Perform approximation */
void ApproximationAlgorithm::run()
{
  getImplementation()->run();
}

void ApproximationAlgorithm::run(const DesignProxy & proxy)
{
  getImplementation()->run(proxy);
}

/* Acccessor to the coefficients of the selected metamodel on the provided basis */
Point ApproximationAlgorithm::getCoefficients()
{
  return getImplementation()->getCoefficients();
}

Scalar ApproximationAlgorithm::getResidual()
{
  return getImplementation()->getResidual();
}

/* Acccessor to the residual of the selected metamodel on the provided basis */
Scalar ApproximationAlgorithm::getRelativeError()
{
  return getImplementation()->getRelativeError();
}

END_NAMESPACE_OPENTURNS
