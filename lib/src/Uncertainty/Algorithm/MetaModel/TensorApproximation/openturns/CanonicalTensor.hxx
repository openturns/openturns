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
#include "openturns/UniVariateFunctionEvaluationImplementation.hxx"
#include "openturns/Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS



struct RankOneTensor {
  typedef Collection<NumericalPoint> NumericalPointCollection;
  typedef Collection<OrthogonalUniVariateFunctionFamily> FunctionFamilyCollection;
  typedef PersistentCollection<OrthogonalUniVariateFunctionFamily> FunctionFamilyPersistentCollection;

  RankOneTensor(const FunctionFamilyCollection & functionFamilies = FunctionFamilyCollection(0),
                const Indices & nk = Indices());

  // constant factor
  NumericalScalar alpha_;

  // subbasis sizes
  Indices nk_;

  // subbasis coefficients
  NumericalPointCollection coefficients_;

  FunctionFamilyPersistentCollection functionFamilies_;
  Collection<Basis> basis_;
};




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
                  const Indices & nk,
                  const UnsignedInteger rank = 1);

  UnsignedInteger getRank() const;

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
  friend class TensorApproximationResult;

  FunctionFamilyPersistentCollection functionFamilies_;

  Collection<RankOneTensor> rank1tensors_;

  // weight of each rank-one tensor
  NumericalPoint weight_;

}; // class CanonicalTensor

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CANONICALTENSOR_HXX */
