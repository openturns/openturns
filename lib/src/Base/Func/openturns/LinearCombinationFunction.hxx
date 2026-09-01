//                                               -*- C++ -*-
/**
 *  @brief The class that implements linear combination of functions.
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LINEARCOMBINATIONFUNCTION_HXX
#define OPENTURNS_LINEARCOMBINATIONFUNCTION_HXX

#include "openturns/Function.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class LinearCombinationFunction
 *
 */
class OT_API LinearCombinationFunction
  : public Function
{
  CLASSNAME
public:
  /* Default constructor */
  LinearCombinationFunction();

  /** Parameter constructor */
  LinearCombinationFunction(const FunctionCollection & functionCollection,
                            const Point & coefficients);

  /** Merge the given function, multiplied by the given coefficient, into this
      linear combination: nested linear combinations are flattened and the
      weights of identical atoms are summed. This method may always be called
      explicitly, whatever the value of the
      "LinearCombinationFunction-MergeDuplicates" ResourceMap key. */
  void mergeLinearCombination(const Function & function,
                              const Scalar coefficient);

  /** Build the linear combination equal to the sum of the two given functions,
      the second one being multiplied by the given coefficient, merging
      duplicate atoms and flattening nested linear combinations */
  static LinearCombinationFunction add(const Function & left,
                                       const Function & right,
                                       const Scalar rightCoefficient);

  /** Build the linear combination equal to the negation of the given function,
      i.e. the same atoms with opposite coefficients, flattening nested linear
      combinations */
  static LinearCombinationFunction negate(const Function & function);

  /** Comparison operator */
  using Function::operator ==;
  Bool operator ==(const LinearCombinationFunction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

private:
  /** Append the given function, multiplied by the given coefficient, to the
      atom/coefficient lists of a linear combination, flattening nested linear
      combinations and summing the weights of identical atoms */
  static void appendMerged(FunctionCollection & functionCollection,
                           Point & coefficients,
                           const Function & function,
                           const Scalar coefficient);

}; /* class LinearCombinationFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARCOMBINATIONFUNCTION_HXX */
