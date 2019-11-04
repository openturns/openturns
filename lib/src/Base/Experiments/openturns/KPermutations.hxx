//                                               -*- C++ -*-
/**
 *  @brief KPermutations experiment plane
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_KPERMUTATIONS_HXX
#define OPENTURNS_KPERMUTATIONS_HXX

#include "openturns/CombinatorialGeneratorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KPermutations
 *
 * The class describes the concept of kPermutations generator
 */
class OT_API KPermutations
  : public CombinatorialGeneratorImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  KPermutations();

  explicit KPermutations(const UnsignedInteger n);

  KPermutations(const UnsignedInteger k,
                const UnsignedInteger n);

  /** Virtual constructor */
  virtual KPermutations * clone() const;

  /** Experiment plane generation :
   *  all the kPermutations of k elements amongst {0, ..., n-1}
   */
  virtual IndicesCollection generate();

  /** String converter */
  virtual String __repr__() const;

  /** Subset size accessor */
  void setK(const UnsignedInteger k);

  UnsignedInteger getK() const;

  /** Set size accessor */
  void setN(const UnsignedInteger n);

  UnsignedInteger getN() const;

private:
  /** Size of the subsets */
  UnsignedInteger k_;

  /** Size of the set */
  UnsignedInteger n_;

}; /* class KPermutations */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KPERMUTATIONS_HXX */
