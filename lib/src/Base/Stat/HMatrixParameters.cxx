//                                               -*- C++ -*-
/**
 *  @file  HMatrixParameters.cxx
 *  @brief This file supplies support for HMat
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
 */
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/HMatrixParameters.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HMatrixParameters);

static const Factory<HMatrixParameters> RegisteredFactory;


HMatrixParameters::HMatrixParameters()
  : PersistentObject()
  , assemblyEpsilon_(ResourceMap::GetAsNumericalScalar("HMatrix-AssemblyEpsilon"))
  , recompressionEpsilon_(ResourceMap::GetAsNumericalScalar("HMatrix-RecompressionEpsilon"))
{
  // Nothing to do
}

/* Virtual constructor */
HMatrixParameters * HMatrixParameters::clone() const
{
  return new HMatrixParameters(*this);
}

/** accessor for assembly epsilon */
void HMatrixParameters::setAssemblyEpsilon(const NumericalScalar assemblyEpsilon)
{
  assemblyEpsilon_ = assemblyEpsilon;
}

NumericalScalar HMatrixParameters::getAssemblyEpsilon() const
{
  return assemblyEpsilon_;
}

/** accessor for recompression epsilon */
void HMatrixParameters::setRecompressionEpsilon(const NumericalScalar recompressionEpsilon)
{
  recompressionEpsilon_ = recompressionEpsilon;
}

NumericalScalar HMatrixParameters::getRecompressionEpsilon() const
{
  return recompressionEpsilon_;
}

/* String converter */
String HMatrixParameters::__repr__() const
{
  OSS oss(true);
  oss << "class= " << HMatrixParameters::GetClassName()
      << ", assembly epsilon= " << assemblyEpsilon_
      << ", recompression epsilon=" << recompressionEpsilon_;
  return oss;
}

String HMatrixParameters::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class= " << HMatrixParameters::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void HMatrixParameters::save(Advocate & adv) const
{
  PersistentObject::save(adv);

  adv.saveAttribute("assemblyEpsilon_", assemblyEpsilon_);
  adv.saveAttribute("recompressionEpsilon_", recompressionEpsilon_);
}

/* Method load() reloads the object from the StorageManager */
void HMatrixParameters::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("assemblyEpsilon_", assemblyEpsilon_);
  adv.loadAttribute("recompressionEpsilon_", recompressionEpsilon_);
}

END_NAMESPACE_OPENTURNS
