//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all ComparisonOperatorImplementation
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
#ifndef OPENTURNS_COMPARISONOPERATORIMPLEMENTATION_HXX
#define OPENTURNS_COMPARISONOPERATORIMPLEMENTATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComparisonOperatorImplementation
 *
 * @brief The class implements the concept of comparison operator defined in ComparisonOperator.
 *
 * This class is abstract so it can not be instanciated. It must be derived.
 * @see ComparisonOperator
 */
class OT_API ComparisonOperatorImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  ComparisonOperatorImplementation();

  /* String converter */
  String __repr__() const;


  /* Here is the interface that all derived class must implement */

  /* Virtual constructor */
  virtual ComparisonOperatorImplementation * clone() const;

  /** @copydoc ComparisonOperator::operator()() const */
  virtual Bool operator() (const Scalar a,
                           const Scalar b) const;

  /* Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /* Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

}; /* class ComparisonOperatorImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPARISONOPERATORIMPLEMENTATION_HXX */
