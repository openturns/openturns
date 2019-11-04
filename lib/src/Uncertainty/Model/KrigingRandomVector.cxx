//                                               -*- C++ -*-
/**
 *  @brief An implementation class for kriging random vectors
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/KrigingRandomVector.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(KrigingRandomVector)

static const Factory<KrigingRandomVector> Factory_KrigingRandomVector;


/* Default constructor */
KrigingRandomVector::KrigingRandomVector()
  : UsualRandomVector()
{}

/* Default constructor */
KrigingRandomVector::KrigingRandomVector(const KrigingResult & krigingResult,
    const Point & point)
  : UsualRandomVector(krigingResult(point))
  , krigingResult_(krigingResult)
  , sample_(Sample(1, point))
{
  // Nothing to do
}

/* Default constructor */
KrigingRandomVector::KrigingRandomVector(const KrigingResult & krigingResult,
    const Sample & sample)
  : UsualRandomVector(krigingResult(sample))
  , krigingResult_(krigingResult)
  , sample_(sample)
{
  // Nothing to do
}

/* Virtual constructor */
KrigingRandomVector * KrigingRandomVector::clone() const
{
  return new KrigingRandomVector(*this);
}

/* String converter */
String KrigingRandomVector::__repr__() const
{
  OSS oss;
  oss << "class=" << KrigingRandomVector::GetClassName()
      << " kriging result=" << krigingResult_;
  return oss;
}

/** Get a realization of the process */
Point KrigingRandomVector::getRealization() const
{
  return getDistribution().getRealization();
}

/** Get a sample of realizations of the process */
Sample KrigingRandomVector::getSample(const UnsignedInteger size) const
{
  return getDistribution().getSample(size);
}

/* Kriging result accessor */
KrigingResult KrigingRandomVector::getKrigingResult() const
{
  return krigingResult_;
}

/* Method save() stores the object through the StorageManager */
void KrigingRandomVector::save(Advocate & adv) const
{
  UsualRandomVector::save(adv);
  adv.saveAttribute( "krigingResult_", krigingResult_ );
  adv.saveAttribute( "sample_", sample_ );
}

/* Method load() reloads the object from the StorageManager */
void KrigingRandomVector::load(Advocate & adv)
{
  UsualRandomVector::load(adv);
  adv.loadAttribute( "krigingResult_", krigingResult_ );
  adv.loadAttribute( "sample_", sample_ );
}

END_NAMESPACE_OPENTURNS
