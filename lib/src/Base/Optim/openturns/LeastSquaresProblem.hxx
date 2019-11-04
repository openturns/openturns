//                                               -*- C++ -*-
/**
 *  @brief LeastSquaresProblem allows to describe an optimization problem
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
#ifndef OPENTURNS_LEASTSQUARESPROBLEM_HXX
#define OPENTURNS_LEASTSQUARESPROBLEM_HXX

#include "openturns/OptimizationProblemImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LeastSquaresProblem
 * LeastSquaresProblem allows to describe an optimization problem
 */

class OT_API LeastSquaresProblem
  : public OptimizationProblemImplementation
{

  CLASSNAME

public:

  /** Default constructor */
  LeastSquaresProblem();

  /** Constructor with parameters */
  explicit LeastSquaresProblem(const Function & residualFunction);

  /** Virtual constructor */
  virtual LeastSquaresProblem * clone() const;

  /** Residual function accessor */
  Function getResidualFunction() const;
  void setResidualFunction(const Function & residualFunction);
  Bool hasResidualFunction() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  // The residual function
  Function residualFunction_;

} ; /* class LeastSquaresProblem */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEASTSQUARESPROBLEM_HXX */
