//                                               -*- C++ -*-
/**
 *  @brief Pagmo optimization algorithm
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PAGMO_HXX
#define OPENTURNS_PAGMO_HXX

#include "openturns/OptimizationAlgorithmImplementation.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/Experiment.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class Pagmo
*/

class OT_API Pagmo
  : public OptimizationAlgorithmImplementation
{
  friend struct PagmoProblem;
  CLASSNAME
public:
  typedef OT::Collection<OT::OptimizationResult>           OptimizationResultCollection;
  typedef OT::PersistentCollection<OT::OptimizationResult> OptimizationResultPersistentCollection;

  /** Default constructor */
  explicit Pagmo(const String & algoName = "gaco");

  /** Parameter constructor */
  explicit Pagmo(const OptimizationProblem & problem,
                 const String & algoName = "gaco",
                 const Sample & startingSample = Sample());

  /** Virtual constructor */
  Pagmo * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Performs the actual computation. */
  void run() override;

  /** Algorithm names accessor */
  static Description GetAlgorithmNames();

  void setAlgorithmName(const String & algoName);
  String getAlgorithmName() const;

  /** @deprecated Number of generations to evolve */
  void setGenerationNumber(const UnsignedInteger generationNumber);
  UnsignedInteger getGenerationNumber() const;

  /** Random generator seed accessor */
  void setSeed(const UnsignedInteger seed);
  UnsignedInteger getSeed() const;

  /** Block size accessor */
  void setBlockSize(const UnsignedInteger blockSize);
  UnsignedInteger getBlockSize() const;

  /** Useless inherited method: throw */
  void setStartingPoint(const Point & point) override;
  Point getStartingPoint() const override;

  /** Starting points accessor */
  void setStartingSample(const Sample & startingSample);
  Sample getStartingSample() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Check whether this problem can be solved by this solver */
  void checkProblem(const OptimizationProblem & problem) const override;

private:
  String algoName_;

  // initial population
  Sample startingSample_;

  // random generator seed
  UnsignedInteger seed_ = 0;

  // batch evaluation granularity
  UnsignedInteger blockSize_ = 1;

} ; /* class Pagmo */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PAGMO_HXX */
