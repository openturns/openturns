//                                               -*- C++ -*-
/**
 *  @brief An implementation class for functionalChaos random vectors
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
#include <algorithm>
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FunctionalChaosRandomVector.hxx"
#include "openturns/UsualRandomVector.hxx"
#include "openturns/EnumerateFunction.hxx"
#include "openturns/FunctionalChaosSobolIndices.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FunctionalChaosRandomVector)

static const Factory<FunctionalChaosRandomVector> Factory_FunctionalChaosRandomVector;

/* Default constructor */
FunctionalChaosRandomVector::FunctionalChaosRandomVector()
  : CompositeRandomVector()
{}

/* Default constructor */
FunctionalChaosRandomVector::FunctionalChaosRandomVector(const FunctionalChaosResult & functionalChaosResult)
  : CompositeRandomVector(functionalChaosResult.getComposedMetaModel(), UsualRandomVector(functionalChaosResult.getOrthogonalBasis().getMeasure()))
  , covariance_(0)
  , isAlreadyComputedCovariance_(false)
  , functionalChaosResult_(functionalChaosResult)
{
  // Nothing to do
}


/* Virtual constructor */
FunctionalChaosRandomVector * FunctionalChaosRandomVector::clone() const
{
  return new FunctionalChaosRandomVector(*this);
}

/* String converter */
String FunctionalChaosRandomVector::__repr__() const
{
  OSS oss;
  oss << "class=" << FunctionalChaosRandomVector::GetClassName()
      << " functional chaos result=" << functionalChaosResult_;
  return oss;
}


/* Mean accessor */
Point FunctionalChaosRandomVector::getMean() const
{
  return functionalChaosResult_.getCoefficients()[0];
}

/* Covariance accessor */
CovarianceMatrix FunctionalChaosRandomVector::getCovariance() const
{
  if (!isAlreadyComputedCovariance_) computeCovariance();
  return covariance_;
}

/* Compute the covariance */
void FunctionalChaosRandomVector::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  const Indices indices(functionalChaosResult_.getIndices());
  const Sample coefficients(functionalChaosResult_.getCoefficients());
  const UnsignedInteger size = indices.getSize();
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      covariance_(i, j) = 0.0;
      for (UnsignedInteger k = 0; k < size; ++k)
        // Take into account only non-zero indices as the null index is the mean of the vector
        if (indices[k] > 0) covariance_(i, j) += coefficients(k, i) * coefficients(k, j);
    } // Loop over the second index
  } // Loop over the first index
  isAlreadyComputedCovariance_ = true;
}

/* Functional chaos result accessor */
FunctionalChaosResult FunctionalChaosRandomVector::getFunctionalChaosResult() const
{
  return functionalChaosResult_;
}

/* Method save() stores the object through the StorageManager */
void FunctionalChaosRandomVector::save(Advocate & adv) const
{
  CompositeRandomVector::save(adv);
  adv.saveAttribute( "functionalChaosResult_", functionalChaosResult_ );
  adv.saveAttribute( "covariance_", covariance_ );
  adv.saveAttribute( "isAlreadyComputedCovariance_", isAlreadyComputedCovariance_ );
}

/* Method load() reloads the object from the StorageManager */
void FunctionalChaosRandomVector::load(Advocate & adv)
{
  CompositeRandomVector::load(adv);
  adv.loadAttribute( "functionalChaosResult_", functionalChaosResult_ );
  adv.loadAttribute( "covariance_", covariance_ );
  adv.loadAttribute( "isAlreadyComputedCovariance_", isAlreadyComputedCovariance_ );
}

END_NAMESPACE_OPENTURNS
