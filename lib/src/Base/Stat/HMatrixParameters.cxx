//                                               -*- C++ -*-
/**
 *  @file  HMatrixParameters.cxx
 *  @brief This file supplies support for HMat
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/HMatrixParameters.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HMatrixParameters)

static const Factory<HMatrixParameters> Factory_HMatrixParameters;


HMatrixParameters::HMatrixParameters()
  : PersistentObject()
  , assemblyEpsilon_(ResourceMap::GetAsScalar("HMatrix-AssemblyEpsilon"))
  , recompressionEpsilon_(ResourceMap::GetAsScalar("HMatrix-RecompressionEpsilon"))
  , admissibilityFactor_(ResourceMap::GetAsScalar("HMatrix-AdmissibilityFactor"))
  , clusteringAlgorithm_(ResourceMap::GetAsString("HMatrix-ClusteringAlgorithm"))
  , compressionMethod_(ResourceMap::GetAsString("HMatrix-CompressionMethod"))
{
  // Nothing to do
}

/* Virtual constructor */
HMatrixParameters * HMatrixParameters::clone() const
{
  return new HMatrixParameters(*this);
}

/** accessor for assembly epsilon */
void HMatrixParameters::setAssemblyEpsilon(const Scalar assemblyEpsilon)
{
  assemblyEpsilon_ = assemblyEpsilon;
}

Scalar HMatrixParameters::getAssemblyEpsilon() const
{
  return assemblyEpsilon_;
}

/** accessor for recompression epsilon */
void HMatrixParameters::setRecompressionEpsilon(const Scalar recompressionEpsilon)
{
  recompressionEpsilon_ = recompressionEpsilon;
}

Scalar HMatrixParameters::getRecompressionEpsilon() const
{
  return recompressionEpsilon_;
}

/** accessor for admissibility factor */
void HMatrixParameters::setAdmissibilityFactor(const Scalar admissibilityFactor)
{
  admissibilityFactor_ = admissibilityFactor;
}

Scalar HMatrixParameters::getAdmissibilityFactor() const
{
  return admissibilityFactor_;
}

/** accessor for clustering algorithm */
void HMatrixParameters::setClusteringAlgorithm(const String & clusteringAlgorithm)
{
  clusteringAlgorithm_ = clusteringAlgorithm;
}

String HMatrixParameters::getClusteringAlgorithm() const
{
  return clusteringAlgorithm_;
}

/** accessor for compression method */
void HMatrixParameters::setCompressionMethod(const String & compressionMethod)
{
  compressionMethod_ = compressionMethod;
}

String HMatrixParameters::getCompressionMethod() const
{
  return compressionMethod_;
}

UnsignedInteger HMatrixParameters::getCompressionMethodAsUnsignedInteger() const
{
  if (compressionMethod_ == "Svd")
    return 0;
  else if (compressionMethod_ == "AcaFull")
    return 1;
  else if (compressionMethod_ == "AcaPartial")
    return 2;
  else if (compressionMethod_ == "AcaPlus")
    return 3;
  else if (compressionMethod_ == "AcaRandom")
    return 4;
  else
    throw InvalidArgumentException(HERE) << "Unknown compression method: " << compressionMethod_ << ", valid choices are: Svd, AcaFull, AcaPartial, AcaPlus or AcaRandom";
}

/* String converter */
String HMatrixParameters::__repr__() const
{
  OSS oss(true);
  oss << "class= " << HMatrixParameters::GetClassName()
      << ", assembly epsilon= " << assemblyEpsilon_
      << ", recompression epsilon=" << recompressionEpsilon_
      << ", admissibility factor=" << admissibilityFactor_
      << ", clustering algorithm=" << clusteringAlgorithm_
      << ", compression method=" << compressionMethod_;
  return oss;
}

String HMatrixParameters::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class= " << HMatrixParameters::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void HMatrixParameters::save(Advocate & adv) const
{
  PersistentObject::save(adv);

  adv.saveAttribute("assemblyEpsilon_", assemblyEpsilon_);
  adv.saveAttribute("recompressionEpsilon_", recompressionEpsilon_);
  adv.saveAttribute("admissibilityFactor_", admissibilityFactor_);
  adv.saveAttribute("clusteringAlgorithm_", clusteringAlgorithm_);
  adv.saveAttribute("compressionMethod_", compressionMethod_);
}

/* Method load() reloads the object from the StorageManager */
void HMatrixParameters::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("assemblyEpsilon_", assemblyEpsilon_);
  adv.loadAttribute("recompressionEpsilon_", recompressionEpsilon_);
  adv.loadAttribute("admissibilityFactor_", admissibilityFactor_);
  adv.loadAttribute("clusteringAlgorithm_", clusteringAlgorithm_);
  adv.loadAttribute("compressionMethod_", compressionMethod_);
}

END_NAMESPACE_OPENTURNS
