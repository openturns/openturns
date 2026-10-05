//                                               -*- C++ -*-
/**
 *  @brief Abstract class for Riemannian manifolds embedded in R^n
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ManifoldImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ManifoldImplementation)

/* Default constructor, for subclasses and persistence only */
ManifoldImplementation::ManifoldImplementation()
  : PersistentObject()
  , ambientDimension_(0)
  , intrinsicDimension_(0)
  , epsilon_(ResourceMap::GetAsScalar("Manifold-Epsilon"))
{
  // Nothing to do
}

/* Ambient dimension accessor */
UnsignedInteger ManifoldImplementation::getAmbientDimension() const
{
  return ambientDimension_;
}

/* Intrinsic dimension accessor */
UnsignedInteger ManifoldImplementation::getIntrinsicDimension() const
{
  return intrinsicDimension_;
}

/* Tolerance accessor */
void ManifoldImplementation::setEpsilon(const Scalar epsilon)
{
  if (!(epsilon > 0.0)) throw InvalidArgumentException(HERE) << "Error: the tolerance must be positive, here epsilon=" << epsilon;
  epsilon_ = epsilon;
}

Scalar ManifoldImplementation::getEpsilon() const
{
  return epsilon_;
}

/* Check that a point has the ambient dimension */
void ManifoldImplementation::checkAmbientDimension(const Point & point,
                                     const String & where) const
{
  if (point.getDimension() != ambientDimension_) throw InvalidArgumentException(HERE) << "Error in " << where << ": the point must have dimension=" << ambientDimension_ << ", here dimension=" << point.getDimension();
}

/* Comparison operator */
Bool ManifoldImplementation::operator ==(const ManifoldImplementation & other) const
{
  if (this == &other) return true;
  return (getClassName() == other.getClassName()) && (ambientDimension_ == other.ambientDimension_) && (intrinsicDimension_ == other.intrinsicDimension_) && (epsilon_ == other.epsilon_);
}

/* String converter */
String ManifoldImplementation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " ambientDimension=" << ambientDimension_
         << " intrinsicDimension=" << intrinsicDimension_
         << " epsilon=" << epsilon_;
}

String ManifoldImplementation::__str__(const String & offset) const
{
  return OSS() << offset << getClassName()
         << "(ambientDimension=" << ambientDimension_
         << ", intrinsicDimension=" << intrinsicDimension_ << ")";
}

/* Method save() stores the object through the StorageManager */
void ManifoldImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("ambientDimension_", ambientDimension_);
  adv.saveAttribute("intrinsicDimension_", intrinsicDimension_);
  adv.saveAttribute("epsilon_", epsilon_);
}

/* Method load() reloads the object from the StorageManager */
void ManifoldImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("ambientDimension_", ambientDimension_);
  adv.loadAttribute("intrinsicDimension_", intrinsicDimension_);
  adv.loadAttribute("epsilon_", epsilon_);
}

END_NAMESPACE_OPENTURNS
