//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all space filling criteria implementations
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SpaceFillingImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"


namespace OT
{

CLASSNAMEINIT(SpaceFillingImplementation)

static const Factory<SpaceFillingImplementation> Factory_SpaceFillingImplementation;


/* Default constructor */
SpaceFillingImplementation::SpaceFillingImplementation(OT::Bool minimization)
  : PersistentObject()
  , minimization_(minimization)
{
  // Nothing to do
}

/* Virtual constructor method */
SpaceFillingImplementation * SpaceFillingImplementation::clone() const
{
  return new SpaceFillingImplementation(*this);
}

/** Evaluate criterion on a sample */
Scalar SpaceFillingImplementation::evaluate(const Sample & ) const
{
  throw NotYetImplementedException(HERE);
}

/** Compute criterion when performing an elementary perturbation */
Scalar SpaceFillingImplementation::perturbLHS(Sample& oldDesign, Scalar,
    UnsignedInteger row1, UnsignedInteger row2, UnsignedInteger column) const
{
  // Default method uses O(N*N) computations
  // Swap coordinates
  std::swap(oldDesign(row1, column), oldDesign(row2, column));
  const Scalar criterion = evaluate(oldDesign);
  // Swap coordinates to restore original sample
  std::swap(oldDesign(row1, column), oldDesign(row2, column));
  // Return criterion
  return criterion;
}

/** Rescale argument if it does not belong to the unit cube */
Sample SpaceFillingImplementation::normalize(const Sample & sample) const
{
  const UnsignedInteger dimension(sample.getDimension());
  const Point minPoint(sample.getMin());
  const Point maxPoint(sample.getMax());
  Bool toNormalize(false);
  for (UnsignedInteger d = 0; d < dimension; ++d)
  {
    if (minPoint[d] < 0.0 || maxPoint[d] > 1.0)
    {
      toNormalize = true;
      break;
    }
  }
  if (!toNormalize) return sample;

  LOGWARN(OSS() << "Sample must be normalized");
  const Point delta(maxPoint - minPoint);
  Sample result(sample - minPoint);
  return result / delta;
}

/* String converter */
String SpaceFillingImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << SpaceFillingImplementation::GetClassName()
      << " minimization=" << minimization_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SpaceFillingImplementation::save(Advocate & adv) const
{
  PersistentObject::save( adv );
  adv.saveAttribute( "minimization_", minimization_ );
}

/* Method load() reloads the object from the StorageManager */
void SpaceFillingImplementation::load(Advocate & adv)
{
  PersistentObject::load( adv );
  adv.loadAttribute( "minimization_", minimization_ );
}

} /* namespace OT */
