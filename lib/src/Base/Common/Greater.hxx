//                                               -*- C++ -*-
/**
 *  @brief Implementation of strict greater comparison
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
#ifndef OPENTURNS_GREATER_HXX
#define OPENTURNS_GREATER_HXX

#include "ComparisonOperatorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Greater
 *
 * @brief The class implements the strict greater comparison
 *
 * This class is derived from ComparisonOperatorImplementation. It performs the comparison
 * of two NumericalScalar and is mainly used in threshold exceedence algorithms.
 */
class OT_API Greater
  : public ComparisonOperatorImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  Greater();

  /* String converter */
  String __repr__() const;


  /* Here is the interface that all derived class must implement */

  /* Virtual constructor */
  virtual Greater * clone() const;

  /**
   * Return true if a > b
   * @param a The left hand side of the expression
   * @param b The right hand side of the expression
   * @return True if comparison succeeds
   */
  virtual Bool operator() (const NumericalScalar a,
                           const NumericalScalar b) const;

  /* Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /* Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

}; /* class Greater */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GREATER_HXX */
