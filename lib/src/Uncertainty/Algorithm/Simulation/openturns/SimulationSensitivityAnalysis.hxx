//                                               -*- C++ -*-
/**
 *  @brief Implementation of SimulationResult
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
#ifndef OPENTURNS_SIMULATIONSENSITIVITYANALYSIS_HXX
#define OPENTURNS_SIMULATIONSENSITIVITYANALYSIS_HXX

#include <cmath>
#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/ComparisonOperator.hxx"
#include "openturns/Function.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Event.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ProbabilitySimulationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimulationSensitivityAnalysis
 * Implementation of EventSimulation sensitivity analysis, it means computation of standardized importance factors
 */
class OT_API SimulationSensitivityAnalysis
  : public PersistentObject
{

  CLASSNAME
public:

  typedef Function IsoProbabilisticTransformation;

  /** Default constructor */
  SimulationSensitivityAnalysis();

  /** Standard constructor */
  SimulationSensitivityAnalysis(const Sample & inputSample,
                                const Sample & outputSample,
                                const IsoProbabilisticTransformation & transformation,
                                const ComparisonOperator & comparisonOperator,
                                const Scalar threshold);

  /** Standard constructor */
  explicit SimulationSensitivityAnalysis(const RandomVector & event);

  /** Standard constructor */
  explicit SimulationSensitivityAnalysis(const ProbabilitySimulationResult & result);

  /** Virtual constructor */
  virtual SimulationSensitivityAnalysis * clone() const;

  /** Mean point in event domain computation */
  Point computeMeanPointInEventDomain() const;
  Point computeMeanPointInEventDomain(const Scalar threshold) const;

  /** Importance factors computation */
  PointWithDescription computeImportanceFactors() const;
  PointWithDescription computeImportanceFactors(const Scalar threshold) const;

public:

  /** Importance factors drawing */
  Graph drawImportanceFactors() const;
  Graph drawImportanceFactorsRange(const Bool probabilityScale = true,
                                   const Scalar lower = -SpecFunc::MaxScalar,
                                   const Scalar upper = SpecFunc::MaxScalar) const;

  /** Input sample accessors */
  Sample getInputSample() const;

  /** Output sample accessors */
  Sample getOutputSample() const;

  /** Threshold accessors */
  Scalar getThreshold() const;
  void setThreshold(const Scalar threshold);

  /** Iso-probabilistic transformation accessor */
  IsoProbabilisticTransformation getTransformation() const;

  /** Comparison operator accessors */
  ComparisonOperator getComparisonOperator() const;
  void setComparisonOperator(const ComparisonOperator & comparisonOperator);

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  /* Input sample of the model */
  Sample inputSample_;

  /* Associated output sample */
  Sample outputSample_;

  /* Iso probabilistic transformation associated with the input sample distribution */
  IsoProbabilisticTransformation transformation_;

  /* Comparison operator defining the event we are interested in */
  ComparisonOperator comparisonOperator_;

  /* Main threshold of interest */
  Scalar threshold_;

}; // class SimulationSensitivityAnalysis

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATIONSENSITIVITYANALYSIS_HXX */
