//                                               -*- C++ -*-
/**
 *  @brief Sensitivity analysis for field functional chaos
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FIELDFUNCTIONALCHAOSSOBOLINDICES_HXX
#define OPENTURNS_FIELDFUNCTIONALCHAOSSOBOLINDICES_HXX

#include "openturns/FieldFunctionalChaosResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FieldFunctionalChaosSobolIndices
 *
 */

class OT_API FieldFunctionalChaosSobolIndices
  : public PersistentObject
{
  CLASSNAME
public:
  /** Default constructor */
  FieldFunctionalChaosSobolIndices();

  /** Constructor with parameters */
  FieldFunctionalChaosSobolIndices(const FieldFunctionalChaosResult & result);

  /** Virtual constructor */
  FieldFunctionalChaosSobolIndices * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Sobol index accessor */
  Scalar getSobolIndex(const Indices & variableIndices, const UnsignedInteger marginalIndex = 0) const;
  Scalar getSobolIndex(const UnsignedInteger variableIndex, const UnsignedInteger marginalIndex = 0) const;

  /** Sobol total index accessor */
  Scalar getSobolTotalIndex(const Indices & variableIndices, const UnsignedInteger marginalIndex = 0) const;
  Scalar getSobolTotalIndex(const UnsignedInteger variableIndex, const UnsignedInteger marginalIndex = 0) const;

  /** Vector indices accessor */
  Point getFirstOrderIndices(const UnsignedInteger marginalIndex = 0) const;
  Point getTotalOrderIndices(const UnsignedInteger marginalIndex = 0) const;

  /** Draw indices */
  Graph draw(const UnsignedInteger marginalIndex = 0) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  FieldFunctionalChaosResult result_;

}; /* class FieldFunctionalChaosSobolIndices */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDFUNCTIONALCHAOSSOBOLINDICES_HXX */
