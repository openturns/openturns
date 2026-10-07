//                                               -*- C++ -*-
/**
 *  @brief HiGHS linear solver
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
#ifndef OPENTURNS_HIGHS_HXX
#define OPENTURNS_HIGHS_HXX

#include "openturns/OptimizationAlgorithmImplementation.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class HiGHS
*/

class OT_API HiGHS
  : public OptimizationAlgorithmImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  explicit HiGHS(const String & algoName = "choose");

  /** Constructor with parameters */
  explicit HiGHS(const OptimizationProblem & problem,
                 const String & algoName = "choose");

  /** Virtual constructor */
  HiGHS * clone() const override;

  /** Algorithm names accessor */
  static Description GetAlgorithmNames();

  /** Algorithm name accessor */
  void setAlgorithmName(const String & algoName);
  String getAlgorithmName() const;

  /** Dual solution accessors (from last run) */
  Point getDualPoint() const;
  Point getReducedCosts() const;
  Point getConstraintValues() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Performs the actual computation. */
  void run() override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Check whether this problem can be solved by this solver */
  void checkProblem(const OptimizationProblem & problem) const override;

private:

  String algoName_;
  Point dualPoint_;
  Point reducedCosts_;
  Point constraintValues_;

}; /* class HiGHS */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HIGHS_HXX */
