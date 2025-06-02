//                                               -*- C++ -*-
/**
 *  @brief An implementation class for kriging random vectors
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/GaussianProcessRandomVector.hxx"
#include "openturns/GaussianProcessConditionalCovariance.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(GaussianProcessRandomVector)

static const Factory<GaussianProcessRandomVector> Factory_GaussianProcessRandomVector;


/* Default constructor */
GaussianProcessRandomVector::GaussianProcessRandomVector()
  : UsualRandomVector()
{}

/* Default constructor */
GaussianProcessRandomVector::GaussianProcessRandomVector(const GaussianProcessRegressionResult & gprResult,
    const Point & point)
  : UsualRandomVector()
  , gprResult_(gprResult)
  , sample_(Sample(1, point))
{
  const GaussianProcessConditionalCovariance gpcc(gprResult);
  setDistribution(gpcc(point));
}

/* Default constructor */
GaussianProcessRandomVector::GaussianProcessRandomVector(const GaussianProcessRegressionResult & gprResult,
    const Sample & sample)
  : UsualRandomVector(GaussianProcessConditionalCovariance(gprResult)(sample))
  , gprResult_(gprResult)
  , sample_(sample)
{
  // Nothing to do
}

/* Virtual constructor */
GaussianProcessRandomVector * GaussianProcessRandomVector::clone() const
{
  return new GaussianProcessRandomVector(*this);
}

/* String converter */
String GaussianProcessRandomVector::__repr__() const
{
  OSS oss;
  oss << "class=" << GaussianProcessRandomVector::GetClassName()
      << " gaussian process regression result=" << gprResult_;
  return oss;
}

/** Get a realization of the process */
Point GaussianProcessRandomVector::getRealization() const
{
  return getDistribution().getRealization();
}

/** Get a sample of realizations of the process */
Sample GaussianProcessRandomVector::getSample(const UnsignedInteger size) const
{
  return getDistribution().getSample(size);
}

/* Kriging result accessor */
GaussianProcessRegressionResult GaussianProcessRandomVector::getGaussianProcessRegressionResult() const
{
  return gprResult_;
}

/* Method save() stores the object through the StorageManager */
void GaussianProcessRandomVector::save(Advocate & adv) const
{
  UsualRandomVector::save(adv);
  adv.saveAttribute( "gprResult_", gprResult_ );
  adv.saveAttribute( "sample_", sample_ );
}

/* Method load() reloads the object from the StorageManager */
void GaussianProcessRandomVector::load(Advocate & adv)
{
  UsualRandomVector::load(adv);
  adv.loadAttribute( "gprResult_", gprResult_ );
  adv.loadAttribute( "sample_", sample_ );
}

END_NAMESPACE_OPENTURNS
