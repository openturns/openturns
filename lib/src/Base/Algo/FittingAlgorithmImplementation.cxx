//                                               -*- C++ -*-
/**
 *  @brief Cross-validation algorithm implementation
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FittingAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FittingAlgorithmImplementation)

static const Factory<FittingAlgorithmImplementation> Factory_FittingAlgorithmImplementation;

/* Default constructor */
FittingAlgorithmImplementation::FittingAlgorithmImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
FittingAlgorithmImplementation * FittingAlgorithmImplementation::clone() const
{
  return new FittingAlgorithmImplementation(*this);
}

/* Perform cross-validation */
Scalar FittingAlgorithmImplementation::run(const Sample & x,
    const Sample & y,
    const Point & weight,
    const FunctionCollection & basis,
    const Indices & indices) const
{
  const DesignProxy proxy(x, basis);
  return run(y, weight, indices, proxy);
}

Scalar FittingAlgorithmImplementation::run(const Sample & x,
    const Sample & y,
    const FunctionCollection & basis,
    const Indices & indices) const
{
  const DesignProxy proxy(x, basis);
  return run(y, indices, proxy);
}

/* Perform cross-validation */
Scalar FittingAlgorithmImplementation::run(const Sample & y,
    const Point & weight,
    const Indices & indices,
    const DesignProxy & proxy) const
{
  LeastSquaresMethod method(proxy, weight, indices);
  return run(method, y);
}

Scalar FittingAlgorithmImplementation::run(const Sample & y,
    const Indices & indices,
    const DesignProxy & proxy) const
{
  LeastSquaresMethod method(proxy, indices);
  return run(method, y);
}

Scalar FittingAlgorithmImplementation::run(LeastSquaresMethod &, const Sample & ) const
{
  throw NotYetImplementedException(HERE);
}

/* String converter */
String FittingAlgorithmImplementation::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}

/* Method save() stores the object through the StorageManager */
void FittingAlgorithmImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void FittingAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
}

END_NAMESPACE_OPENTURNS
