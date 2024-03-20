//                                               -*- C++ -*-
/**
 *  @file  HMatrixParameters.hxx
 *  @brief This file supplies support for HMat. It stores parameters used by
 *         HMat applications
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
#ifndef OPENTURNS_HMATRIXPARAMETERS_HXX
#define OPENTURNS_HMATRIXPARAMETERS_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

// HMatrixParameters
class OT_API HMatrixParameters
  : public PersistentObject
{

  CLASSNAME

public:
  /** Default constructor */
  HMatrixParameters();

  /** Virtual copy constructor */
  HMatrixParameters * clone() const override;

  /** accessor for assembly epsilon */
  void setAssemblyEpsilon(const Scalar assemblyEpsilon);
  Scalar getAssemblyEpsilon() const;

  /** accessor for recompression epsilon */
  void setRecompressionEpsilon(const Scalar recompressionEpsilon);
  Scalar getRecompressionEpsilon() const;

  /** accessor for admissibility factor */
  void setAdmissibilityFactor(const Scalar admissibilityFactor);
  Scalar getAdmissibilityFactor() const;

  /** accessor for clustering algorithm */
  void setClusteringAlgorithm(const String & clusteringAlgorithm);
  String getClusteringAlgorithm() const;

  /** accessor for compression method */
  void setCompressionMethod(const String & compressionMethod);
  String getCompressionMethod() const;
  UnsignedInteger getCompressionMethodAsUnsignedInteger() const;

  /** accessor for factorization method */
  void setFactorizationMethod(const String & factorization);
  String getFactorizationMethod() const;


  /* String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Scalar assemblyEpsilon_;
  Scalar recompressionEpsilon_;
  Scalar admissibilityFactor_;
  String clusteringAlgorithm_;
  String compressionMethod_;
  String factorizationMethod_;
};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HMATRIXPARAMETERS_HXX */
