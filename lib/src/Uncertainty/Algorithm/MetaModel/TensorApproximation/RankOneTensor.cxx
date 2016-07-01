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

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RankOneTensor);

static const Factory<RankOneTensor> Factory_RankOneTensor;

RankOneTensor::RankOneTensor(const FunctionFamilyCollection & functionFamilies,
                             const Indices & nk)
: radius_(1.0)
, nk_(nk)
, coefficients_(nk.getSize())
, functionFamilies_(functionFamilies)
, basis_(nk.getSize())
{
  const UnsignedInteger dimension = nk.getSize();
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    coefficients_[i].resize(nk[i]);
    basis_[i] = Basis(nk[i]);
    for (UnsignedInteger l = 0; l < nk[i]; ++ l)
    {
      basis_[i][l] = NumericalMathFunction(UniVariateFunctionEvaluationImplementation(functionFamilies_[i].build(l)));
    }
  }
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
