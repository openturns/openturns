//                                               -*- C++ -*-
/**
 *  @brief Sensitivity analysis based on functional chaos expansion
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
 *
 */
#ifndef OPENTURNS_FUNCTIONALCHAOSSOBOLINDICES_HXX
#define OPENTURNS_FUNCTIONALCHAOSSOBOLINDICES_HXX

#include "openturns/FunctionalChaosResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FunctionalChaosSobolIndices
 *
 * Sensitivity analysis based on functional chaos expansion
 */
class OT_API FunctionalChaosSobolIndices
  : public PersistentObject
{
  CLASSNAME

public:

  /** Default constructor */
  FunctionalChaosSobolIndices();

  /** Default constructor */
  explicit FunctionalChaosSobolIndices(const FunctionalChaosResult & functionalChaosResult);

  /** Virtual constructor */
  FunctionalChaosSobolIndices * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Summary table */
  String summary() const;

  /* Here is the interface that all derived class must implement */

  /** Sobol index accessor */
  Scalar getSobolIndex(const Indices & variableIndices,
                       const UnsignedInteger marginalIndex = 0) const;
  Scalar getSobolIndex(const UnsignedInteger variableIndex,
                       const UnsignedInteger marginalIndex = 0) const;

  /** Sobol total index accessor */
  Scalar getSobolTotalIndex(const Indices & variableIndices,
                            const UnsignedInteger marginalIndex = 0) const;
  Scalar getSobolTotalIndex(const UnsignedInteger variableIndex,
                            const UnsignedInteger marginalIndex = 0) const;

  /** Sobol grouped (first order) index accessor */
  Scalar getSobolGroupedIndex(const Indices & variableIndices,
                              const UnsignedInteger marginalIndex = 0) const;
  Scalar getSobolGroupedIndex(const UnsignedInteger variableIndex,
                              const UnsignedInteger marginalIndex = 0) const;

  /** Sobol grouped total index accessor */
  Scalar getSobolGroupedTotalIndex(const Indices & variableIndices,
                                   const UnsignedInteger marginalIndex = 0) const;
  Scalar getSobolGroupedTotalIndex(const UnsignedInteger variableIndex,
                                   const UnsignedInteger marginalIndex = 0) const;

  /** Functional chaos result accessor */
  FunctionalChaosResult getFunctionalChaosResult() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The functional chaos result tht allows to build the random vector */
  FunctionalChaosResult functionalChaosResult_;

}; /* class FunctionalChaosSobolIndices */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FUNCTIONALCHAOSSOBOLINDICES_HXX */
