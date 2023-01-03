//                                               -*- C++ -*-
/**
 *  @brief A factory for building ApproximationAlgorithmImplementation objects
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/ApproximationAlgorithmImplementationFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ApproximationAlgorithmImplementationFactory)

static const Factory<ApproximationAlgorithmImplementationFactory> Factory_ApproximationAlgorithmImplementationFactory;


/* Default constructor */
ApproximationAlgorithmImplementationFactory::ApproximationAlgorithmImplementationFactory()
  : PersistentObject()
{
  // Nothing to do
}

ApproximationAlgorithmImplementationFactory * ApproximationAlgorithmImplementationFactory::clone() const

{
  return new ApproximationAlgorithmImplementationFactory( *this );
}


ApproximationAlgorithmImplementation * ApproximationAlgorithmImplementationFactory::build(const Sample & x,
    const Sample & y,
    const FunctionCollection & psi,
    const Indices & indices) const
{
  const UnsignedInteger size = x.getSize();
  return build( x, y, Point( size, 1.0 ), psi, indices);
}

ApproximationAlgorithmImplementation * ApproximationAlgorithmImplementationFactory::build(const Sample &,
    const Sample &,
    const Point &,
    const FunctionCollection &,
    const Indices & ) const
{
  throw NotYetImplementedException(HERE) << "In ApproximationAlgorithmImplementationFactory::build(const Sample & x, const Sample & y, const Point & weight, const FunctionCollection & psi, const Indices & indices) const";
}

String ApproximationAlgorithmImplementationFactory::__repr__() const
{
  return OSS() << "class=" << getClassName();
}

/* Method save() stores the object through the StorageManager */
void ApproximationAlgorithmImplementationFactory::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void ApproximationAlgorithmImplementationFactory::load(Advocate & adv)
{
  PersistentObject::load(adv);
}


END_NAMESPACE_OPENTURNS
