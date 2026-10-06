//                                               -*- C++ -*-
/**
 *  @file  HODLRMatrixParameters.cxx
 *  @brief Parameters for HODLR compressed matrices
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
 */
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/HODLRMatrixParameters.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HODLRMatrixParameters)

static const Factory<HODLRMatrixParameters> Factory_HODLRMatrixParameters;

HODLRMatrixParameters::HODLRMatrixParameters()
  : PersistentObject()
  , assemblyEpsilon_(ResourceMap::GetAsScalar("HODLRMatrix-AssemblyEpsilon"))
  , recompressionEpsilon_(ResourceMap::GetAsScalar("HODLRMatrix-RecompressionEpsilon"))
  , minLeafSize_(ResourceMap::GetAsUnsignedInteger("HODLRMatrix-MinLeafSize"))
  , maxRank_(ResourceMap::GetAsUnsignedInteger("HODLRMatrix-MaxRank"))
  , useSpatialOrdering_(ResourceMap::GetAsBool("HODLRMatrix-UseSpatialOrdering"))
{
}

HODLRMatrixParameters * HODLRMatrixParameters::clone() const
{
  return new HODLRMatrixParameters(*this);
}

void HODLRMatrixParameters::setAssemblyEpsilon(const Scalar assemblyEpsilon)
{
  assemblyEpsilon_ = assemblyEpsilon;
}

Scalar HODLRMatrixParameters::getAssemblyEpsilon() const
{
  return assemblyEpsilon_;
}

void HODLRMatrixParameters::setRecompressionEpsilon(const Scalar recompressionEpsilon)
{
  recompressionEpsilon_ = recompressionEpsilon;
}

Scalar HODLRMatrixParameters::getRecompressionEpsilon() const
{
  return recompressionEpsilon_;
}

void HODLRMatrixParameters::setMinLeafSize(const UnsignedInteger minLeafSize)
{
  minLeafSize_ = minLeafSize;
}

UnsignedInteger HODLRMatrixParameters::getMinLeafSize() const
{
  return minLeafSize_;
}

void HODLRMatrixParameters::setMaxRank(const UnsignedInteger maxRank)
{
  maxRank_ = maxRank;
}

UnsignedInteger HODLRMatrixParameters::getMaxRank() const
{
  return maxRank_;
}

void HODLRMatrixParameters::setUseSpatialOrdering(const Bool useSpatialOrdering)
{
  useSpatialOrdering_ = useSpatialOrdering;
}

Bool HODLRMatrixParameters::getUseSpatialOrdering() const
{
  return useSpatialOrdering_;
}

String HODLRMatrixParameters::__repr__() const
{
  OSS oss(true);
  oss << "class= " << HODLRMatrixParameters::GetClassName()
      << ", assembly epsilon= " << assemblyEpsilon_
      << ", recompression epsilon= " << recompressionEpsilon_
      << ", min leaf size= " << minLeafSize_
      << ", max rank= " << maxRank_
      << ", use spatial ordering= " << useSpatialOrdering_;
  return oss;
}

String HODLRMatrixParameters::__str__(const String &) const
{
  OSS oss(false);
  oss << "class= " << HODLRMatrixParameters::GetClassName();
  return oss;
}

void HODLRMatrixParameters::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("assemblyEpsilon_", assemblyEpsilon_);
  adv.saveAttribute("recompressionEpsilon_", recompressionEpsilon_);
  adv.saveAttribute("minLeafSize_", minLeafSize_);
  adv.saveAttribute("maxRank_", maxRank_);
  adv.saveAttribute("useSpatialOrdering_", useSpatialOrdering_);
}

void HODLRMatrixParameters::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("assemblyEpsilon_", assemblyEpsilon_);
  adv.loadAttribute("recompressionEpsilon_", recompressionEpsilon_);
  adv.loadAttribute("minLeafSize_", minLeafSize_);
  adv.loadAttribute("maxRank_", maxRank_);
  adv.loadAttribute("useSpatialOrdering_", useSpatialOrdering_);
}

END_NAMESPACE_OPENTURNS
