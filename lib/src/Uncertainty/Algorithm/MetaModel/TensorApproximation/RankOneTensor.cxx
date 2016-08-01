//                                               -*- C++ -*-
/**
 *  @brief Rank-1 tensor
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/RankOneTensor.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/UniVariateFunctionEvaluationImplementation.hxx"
#include "openturns/CanonicalTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RankOneTensor);

static const Factory<RankOneTensor> Factory_RankOneTensor;


RankOneTensor::RankOneTensor()
: radius_(1.0)
{
  // Nothing to do
}


RankOneTensor::RankOneTensor(const CanonicalTensor & canonicalTensor)
: radius_(1.0)
, coefficients_(canonicalTensor.getDegrees().getSize())
, basis_(canonicalTensor.getDegrees().getSize())
{
  const Indices nk(canonicalTensor.getDegrees());
  const UnsignedInteger dimension = nk.getSize();
  const FunctionFamilyCollection functionFamilies(canonicalTensor.getFunctionFamilies());
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    coefficients_[i].resize(nk[i]);
    basis_[i] = Basis(nk[i]);
    for (UnsignedInteger l = 0; l < nk[i]; ++ l)
    {
      basis_[i][l] = NumericalMathFunction(UniVariateFunctionEvaluationImplementation(functionFamilies[i].build(l)));
    }
  }
}


void RankOneTensor::setRadius(const NumericalScalar radius)
{
  radius_ = radius;
}

NumericalScalar RankOneTensor::getRadius() const
{
  return radius_;
}


/* Coefficients accessor along i-th component */
void RankOneTensor::setCoefficients(const UnsignedInteger i, const NumericalPoint & coefficients)
{
  coefficients_[i] = coefficients;
}


NumericalPoint RankOneTensor::getCoefficients(const UnsignedInteger i) const
{
  return coefficients_[i];
}


/* Basis accessor along i-th component */
Basis RankOneTensor::getBasis(const UnsignedInteger i) const
{
  return basis_[i];
}


UnsignedInteger RankOneTensor::getDimension() const
{
  return coefficients_.getSize();
}

RankOneTensor * RankOneTensor::clone() const
{
  return new RankOneTensor(*this);
}

/* Method save() stores the object through the StorageManager */
void RankOneTensor::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("radius_", radius_);
  adv.saveAttribute("coefficients_", coefficients_);
  adv.saveAttribute("basis_", basis_);
}

/* Method load() reloads the object from the StorageManager */
void RankOneTensor::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("radius_", radius_);
  adv.loadAttribute("coefficients_", coefficients_);
  adv.loadAttribute("basis_", basis_);
}

END_NAMESPACE_OPENTURNS
