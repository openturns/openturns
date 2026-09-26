//                                               -*- C++ -*-
/**
 *  @file  HODLRMatrixParameters.hxx
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
#ifndef OPENTURNS_HODLRMATRIXPARAMETERS_HXX
#define OPENTURNS_HODLRMATRIXPARAMETERS_HXX

#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API HODLRMatrixParameters
  : public PersistentObject
{
  CLASSNAME

public:
  HODLRMatrixParameters();

  HODLRMatrixParameters * clone() const override;

  void setAssemblyEpsilon(const Scalar assemblyEpsilon);
  Scalar getAssemblyEpsilon() const;

  void setRecompressionEpsilon(const Scalar recompressionEpsilon);
  Scalar getRecompressionEpsilon() const;

  void setMinLeafSize(const UnsignedInteger minLeafSize);
  UnsignedInteger getMinLeafSize() const;

  void setMaxRank(const UnsignedInteger maxRank);
  UnsignedInteger getMaxRank() const;

  void setUseSpatialOrdering(const Bool useSpatialOrdering);
  Bool getUseSpatialOrdering() const;

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

private:
  Scalar assemblyEpsilon_;
  Scalar recompressionEpsilon_;
  UnsignedInteger minLeafSize_;
  UnsignedInteger maxRank_;
  Bool useSpatialOrdering_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HODLRMATRIXPARAMETERS_HXX */
