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
#ifndef OPENTURNS_RANKONETENSOR_HXX
#define OPENTURNS_RANKONETENSOR_HXX

#include "openturns/OrthogonalUniVariateFunctionFamily.hxx"
#include "openturns/Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API RankOneTensor : public PersistentObject
{
  CLASSNAME;
public:

  typedef Collection<OrthogonalUniVariateFunctionFamily> FunctionFamilyCollection;
  typedef PersistentCollection<OrthogonalUniVariateFunctionFamily> FunctionFamilyPersistentCollection;

  explicit RankOneTensor(const FunctionFamilyCollection & functionFamilies = FunctionFamilyCollection(0),
                         const Indices & nk = Indices());

  /** Virtual constructor */
  virtual RankOneTensor * clone() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  friend class TensorApproximationAlgorithm;
  friend class TensorApproximationResult;
  friend class RankOneTensorEvaluation;

  // constant factor
  NumericalScalar radius_;

  // subbasis sizes
  Indices nk_;

  // subbasis coefficients
  PersistentCollection<NumericalPoint> coefficients_;

  FunctionFamilyPersistentCollection functionFamilies_;
  PersistentCollection<Basis> basis_;
};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANKONETENSOR_HXX */
