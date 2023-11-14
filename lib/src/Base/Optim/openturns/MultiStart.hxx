//                                               -*- C++ -*-
/**
 *  @brief MultiStart optimization algorithm
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
#ifndef OPENTURNS_MULTISTART_HXX
#define OPENTURNS_MULTISTART_HXX

#include "openturns/OptimizationAlgorithmImplementation.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/Experiment.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class MultiStart
*/

class OT_API MultiStart
  : public OptimizationAlgorithmImplementation
{

  CLASSNAME
public:
  typedef OT::Collection<OT::OptimizationResult>           OptimizationResultCollection;
  typedef OT::PersistentCollection<OT::OptimizationResult> OptimizationResultPersistentCollection;

  /** Default constructor */
  MultiStart();

  /** Constructor that sets starting points */
  MultiStart(const OptimizationAlgorithm & solver,
             const Sample & startingSample);

  /** Virtual constructor */
  MultiStart * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Performs the actual computation. */
  void run() override;

  /** Problem accessor */
  void setProblem(const OptimizationProblem & problem) override;

  /** Accessor to the underlying solver */
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);
  OptimizationAlgorithm getOptimizationAlgorithm() const;

  /** Useless inherited method: throw */
  void setStartingPoint(const Point & point) override;

  /** Starting points accessor */
  void setStartingSample(const Sample & startingSample);
  Sample getStartingSample() const;

  /** Useless inherited method: throw */
  Point getStartingPoint() const override;

  /** Flag for results management accessors */
  Bool getKeepResults() const;
  void setKeepResults(const Bool keepResults);

  OptimizationResultCollection getResultCollection() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Check that the solver is compatible with MultiStart */
  void checkSolver(const OptimizationAlgorithm &) const;

  /** Check whether this problem can be solved by this solver */
  void checkProblem(const OptimizationProblem & problem) const override;

  /** Check that the optimization problem is consistent with the starting sample */
  void checkStartingSampleConsistentWithOptimizationProblem(const Sample & startingSample, const OptimizationProblem & problem) const;

private:
  OptimizationAlgorithm solver_;
  Sample startingSample_;

  /** Flag to tell if the collection of optimization results have to be kept */
  Bool keepResults_;
  OptimizationResultPersistentCollection resultCollection_;

} ; /* class MultiStart */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MULTISTART_HXX */
