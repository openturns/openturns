//                                               -*- C++ -*-
/**
 *  @brief Implementation of less or equal comparison
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
#ifndef OPENTURNS_LESSOREQUAL_HXX
#define OPENTURNS_LESSOREQUAL_HXX

#include "openturns/ComparisonOperatorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LessOrEqual
 *
 * @brief The class implements the less or equal comparison
 *
 * This class is derived from ComparisonOperatorImplementation. It performs the comparison
 * of two Scalar and is mainly used in threshold exceedence algorithms.
 */
class OT_API LessOrEqual
  : public ComparisonOperatorImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  LessOrEqual();

  /* String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Here is the interface that all derived class must implement */

  /* Virtual constructor */
  virtual LessOrEqual * clone() const;

  /**
   * Return true if a <= b
   * @param a The left hand side of the expression
   * @param b The right hand side of the expression
   * @return True if comparison succeeds
   */
  virtual Bool operator() (const Scalar a,
                           const Scalar b) const;

  /* Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /* Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

}; /* class LessOrEqual */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LESSOREQUAL_HXX */
