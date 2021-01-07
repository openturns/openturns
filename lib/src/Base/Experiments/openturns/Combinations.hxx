//                                               -*- C++ -*-
/**
 *  @brief Combinations experiment plane
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_COMBINATIONS_HXX
#define OPENTURNS_COMBINATIONS_HXX

#include "openturns/CombinatorialGeneratorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Combinations
 *
 * The class describes the concept of combinations generator
 */
class OT_API Combinations
  : public CombinatorialGeneratorImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  Combinations();

  Combinations(const UnsignedInteger k,
               const UnsignedInteger n);

  /** Virtual constructor */
  Combinations * clone() const override;

  /** Experiment plane generation :
   *  all the combinations of k elements amongst {0, ..., n-1}
   */
  IndicesCollection generate() override;

  /** String converter */
  String __repr__() const override;

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

}; /* class Combinations */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMBINATIONS_HXX */
