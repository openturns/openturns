//                                               -*- C++ -*-
/**
 *  @brief DomainImplementation is defined as a domain of \mathbb{R}^d
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/DomainImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DomainImplementation)

static const Factory<DomainImplementation> Factory_DomainImplementation;


/* Default constructor */
DomainImplementation::DomainImplementation(UnsignedInteger dimension)
  : PersistentObject()
  , dimension_(dimension)
  , volumeOld_(0.0)
  , isAlreadyComputedVolumeOld_(false)
{
  // Nothing to do
}

/* Clone method */
DomainImplementation * DomainImplementation::clone() const
{
  return new DomainImplementation(*this);
}

/* String converter */
String DomainImplementation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " dimension=" << dimension_;

}

String DomainImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << __repr__();
}

/* Check if the given point is inside of the domain */
Bool DomainImplementation::contains(const Point & point) const
{
  throw NotYetImplementedException(HERE) << "In DomainImplementation::contains(const Point & point) const";
}

/* Check if the given point is inside of the discretization of the domain */
Bool DomainImplementation::numericallyContains(const Point & point) const
{
  LOGWARN(OSS() << "DomainImplementation::numericallyContains is deprecated in favor of derived classes.");
  return contains(point);
}

/* Check if the domain is empty, i.e if its numerical volume is zero */
Bool DomainImplementation::isEmpty() const
{
  LOGWARN(OSS() << "DomainImplementation::isEmpty is deprecated in favor of derived classes.");
  return isNumericallyEmpty();
}

/* Check if the domain is numerically empty, i.e if its numerical volume is zero */
Bool DomainImplementation::isNumericallyEmpty() const
{
  LOGWARN(OSS() << "DomainImplementation::isNumericallyEmpty is deprecated in favor of derived classes.");
  return getNumericalVolume() <= ResourceMap::GetAsScalar("Domain-SmallVolume");
}

/* Get the volume of the domain */
Scalar DomainImplementation::getVolume() const
{
  LOGWARN(OSS() << "DomainImplementation::getVolume is deprecated in favor of derived classes.");
  return getNumericalVolume();
}

/* Get the numerical volume of the domain */
Scalar DomainImplementation::getNumericalVolume() const
{
  LOGWARN(OSS() << "DomainImplementation::getNumericalVolume is deprecated in favor of getVolume method in derived classes.");
  if (!isAlreadyComputedVolumeOld_) volumeOld_ = computeVolume();
  return volumeOld_;
}

/* Compute the volume of the mesh */
Scalar DomainImplementation::computeVolume() const
{
  LOGWARN(OSS() << "DomainImplementation::computeVolume is deprecated in favor of derived classes.");
  throw NotYetImplementedException(HERE);
}

/* Lower bound of the bounding box */
Point DomainImplementation::getLowerBound() const
{
  LOGWARN(OSS() << "DomainImplementation::getLowerBound is deprecated in favor of derived classes.");
  throw NotYetImplementedException(HERE) << "In DomainImplementation::getLowerBound()";
}

/* Upper bound of the bounding box */
Point DomainImplementation::getUpperBound() const
{
  LOGWARN(OSS() << "DomainImplementation::getUpperBound is deprecated in favor of derived classes.");
  throw NotYetImplementedException(HERE) << "In DomainImplementation::getUpperBound()";
}

/* Get the dimension of the domain*/
UnsignedInteger DomainImplementation::getDimension() const
{
  return dimension_;
}

/* Method save() stores the object through the StorageManager */
void DomainImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute("volumeOld_", volumeOld_);
  adv.saveAttribute("isAlreadyComputedVolumeOld_", isAlreadyComputedVolumeOld_);
}

/* Method load() reloads the object from the StorageManager */
void DomainImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("dimension_", dimension_);
  adv.loadAttribute("volumeOld_", volumeOld_);
  adv.loadAttribute("isAlreadyComputedVolumeOld_", isAlreadyComputedVolumeOld_);
}

END_NAMESPACE_OPENTURNS
