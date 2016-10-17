//                                               -*- C++ -*-
/**
 *  @brief Canonical tensor format
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
#ifndef OPENTURNS_CANONICALTENSOR_HXX
#define OPENTURNS_CANONICALTENSOR_HXX

#include "openturns/OrthogonalUniVariateFunctionFamily.hxx"
#include "openturns/RankOneTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class CanonicalTensor
 * Canonical tensor format
 */
class OT_API CanonicalTensor
  : public PersistentObject
{

  CLASSNAME;
public:
  typedef Collection<OrthogonalUniVariateFunctionFamily>           FunctionFamilyCollection;
  typedef PersistentCollection<OrthogonalUniVariateFunctionFamily> FunctionFamilyPersistentCollection;

  /** Default constructor */
  CanonicalTensor();

  /** Standard constructor */
  CanonicalTensor(const FunctionFamilyCollection & functionFamilies,
                  const Indices & degrees,
                  const UnsignedInteger rank = 1);

  void setRank(const UnsignedInteger rank);
  UnsignedInteger getRank() const;

  FunctionFamilyCollection getFunctionFamilies() const;

  Indices getDegrees() const;

  void setRankOneTensor(const UnsignedInteger k, const RankOneTensor & rankOneTensor);
  RankOneTensor getRankOneTensor(const UnsignedInteger k) const;

  /** Virtual constructor */
  virtual CanonicalTensor * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  friend class TensorApproximationAlgorithm;

  FunctionFamilyPersistentCollection functionFamilies_;

  // subbasis sizes
  Indices degrees_;

  PersistentCollection<RankOneTensor> rank1tensors_;

}; // class CanonicalTensor

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CANONICALTENSOR_HXX */
