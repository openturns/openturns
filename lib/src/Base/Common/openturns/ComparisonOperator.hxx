//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all ComparisonOperator
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
#ifndef OPENTURNS_COMPARISONOPERATOR_HXX
#define OPENTURNS_COMPARISONOPERATOR_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComparisonOperator
 *
 * @brief The class describes the concept of a comparison operator.
 *
 * The implementation defined what comparison is actually performed.
 * @see ComparisonOperatorImplementation
 */
class OT_API ComparisonOperator
  : public TypedInterfaceObject<ComparisonOperatorImplementation>
{
  CLASSNAME
public:

  /** Default constructor */
  ComparisonOperator();

  /** Constructor from implementation */
  ComparisonOperator(const ComparisonOperatorImplementation & op);


  /**
   * Return true if comparison succeeds
   * @param a The left hand side of the expression
   * @param b The right hand side of the expression
   * @return True if comparison succeeds
   */
  Bool operator()(Scalar a, Scalar b) const;

  /** @copydoc operator()()
   * This method is an alias for ComparisonOperator::operator()(const Scalar a, const Scalar b).
   */
  Bool compare(const Scalar a,
               const Scalar b) const;

  String __repr__() const override;
}; /* class ComparisonOperator */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPARISONOPERATOR_HXX */
