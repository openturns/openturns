//                                               -*- C++ -*-
/**
 *  @brief HiGHS linear solver
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Experiment.hxx"

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
  HiGHS();

  /** Constructor that sets starting points */
  HiGHS(const OptimizationProblem & problem);

  /** Virtual constructor */
  HiGHS * clone() const override;

  /** String converter */
  String __repr__() const override;

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

}; /* class HiGHS */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HIGHS_HXX */
