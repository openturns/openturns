//                                               -*- C++ -*-
/**
 *  @brief DomainImplementation is defined as a domain of \mathbb{R}^d
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DomainImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DomainImplementation)

static const Factory<DomainImplementation> Factory_DomainImplementation;


/* Default constructor */
DomainImplementation::DomainImplementation(UnsignedInteger dimension)
  : PersistentObject()
  , dimension_(dimension)
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

String DomainImplementation::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Check if the given point is inside of the domain */
Bool DomainImplementation::contains(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In DomainImplementation::contains(const Point & point) const";
}

class DomainImplementationContainsSamplePolicy
{
  const Sample & input_;
  DomainImplementation::BoolCollection & output_;
  const DomainImplementation & domain_;

public:
  DomainImplementationContainsSamplePolicy(const Sample & input,
					   DomainImplementation::BoolCollection & output,
					   const DomainImplementation & domain)
    : input_(input)
    , output_(output)
    , domain_(domain)
  {
    // Nothing to do
  }

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
	output_[i] = domain_.contains(input_[i]);
  } // operator ()
};  // class DomainImplementationContainsSamplePolicy

/* Check if the given points are inside of the domain */
DomainImplementation::BoolCollection DomainImplementation::contains(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  BoolCollection result(size, 0);
  const DomainImplementationContainsSamplePolicy policy( sample, result, *this );
  TBBImplementation::ParallelFor( 0, size, policy);
  return result;
}

/* Compute the Euclidean distance from a given point to the domain */
Scalar DomainImplementation::computeDistance(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "Sorry, " << getClassName() << "::computeDistance(Point) is not yet implemented.";
}

/* Compute the Euclidean distance from given points to the domain */
Sample DomainImplementation::computeDistance(const Sample & sample) const
{
  Sample result(sample.getSize(), 1);
  for(UnsignedInteger i = 0; i < sample.getSize(); ++i)
    result(i, 0) = computeDistance(sample[i]);
  return result;
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
}

/* Method load() reloads the object from the StorageManager */
void DomainImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("dimension_", dimension_);
}

END_NAMESPACE_OPENTURNS
