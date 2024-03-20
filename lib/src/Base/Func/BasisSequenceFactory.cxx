//                                               -*- C++ -*-
/**
 *  @brief A factory for building BasisSequence objects
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

#include "openturns/BasisSequenceFactory.hxx"
#include "openturns/LARS.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(BasisSequenceFactory)

/* Default constructor */
BasisSequenceFactory::BasisSequenceFactory()
  : TypedInterfaceObject<BasisSequenceFactoryImplementation>(new LARS())
{
  // Nothing to do
}

/* Constructor from implementation */
BasisSequenceFactory::BasisSequenceFactory(const BasisSequenceFactoryImplementation & implementation)
  : TypedInterfaceObject<BasisSequenceFactoryImplementation>( implementation.clone() )
{
  // Nothing to do
}

/* String converter */
String BasisSequenceFactory::__repr__() const
{
  return getImplementation()->__repr__();
}

String BasisSequenceFactory::__str__(const String & offset) const
{
  return getImplementation()->__str__( offset );
}

/* Method to create new BasisSequence objects */
BasisSequence BasisSequenceFactory::build(const Sample & x,
    const Sample & y,
    const FunctionCollection & psi,
    const Indices & indices)
{
  return getImplementation()->build(x, y, psi, indices);
}

BasisSequence BasisSequenceFactory::build(const Sample & y,
    const Indices & indices,
    const DesignProxy & proxy)
{
  return getImplementation()->build(y, indices, proxy);
}

BasisSequence BasisSequenceFactory::build(LeastSquaresMethod & method,
    const Sample & y)
{
  return getImplementation()->build(method, y);
}

void BasisSequenceFactory::initialize()
{
  getImplementation()->initialize();
}

void BasisSequenceFactory::updateBasis(LeastSquaresMethod & method,
                                       const Sample & y)
{
  getImplementation()->updateBasis(method, y);
}


/* Stopping criterion on the L1-norm of the coefficients accessor */
void BasisSequenceFactory::setMaximumRelativeConvergence(const Scalar coefficientsPaths)
{
  copyOnWrite();
  getImplementation()->setMaximumRelativeConvergence(coefficientsPaths);
}

Scalar BasisSequenceFactory::getMaximumRelativeConvergence() const
{
  return getImplementation()->getMaximumRelativeConvergence();
}


END_NAMESPACE_OPENTURNS
