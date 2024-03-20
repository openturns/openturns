//                                               -*- C++ -*-
/**
 *  @brief Implementation of strict less comparison
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
#ifndef OPENTURNS_LESS_HXX
#define OPENTURNS_LESS_HXX

#include "openturns/ComparisonOperatorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Less
 *
 * @brief The class implements the strict less comparison
 *
 * This class is derived from ComparisonOperatorImplementation. It performs the comparison
 * of two Scalar and is mainly used in threshold exceedence algorithms.
 */
class OT_API Less
  : public ComparisonOperatorImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  Less();

  /* String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /* Virtual constructor */
  Less * clone() const override;

  /** @copydoc ComparisonOperator::operator()(const Scalar, const Scalar) const
   * Return true if a < b
   */
  Bool operator() (const Scalar a, const Scalar b) const override;

  /* Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /* Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

}; /* class Less */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LESS_HXX */
