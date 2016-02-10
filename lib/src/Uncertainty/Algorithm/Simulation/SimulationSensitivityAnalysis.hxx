//                                               -*- C++ -*-
/**
 *  @brief Implementation of SimulationResult
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
#ifndef OPENTURNS_SIMULATIONSENSITIVITYANALYSIS_HXX
#define OPENTURNS_SIMULATIONSENSITIVITYANALYSIS_HXX

#include <cmath>
#include "PersistentObject.hxx"
#include "NumericalSample.hxx"
#include "NumericalPointWithDescription.hxx"
#include "ComparisonOperator.hxx"
#include "NumericalMathFunction.hxx"
#include "Graph.hxx"
#include "Event.hxx"
#include "SpecFunc.hxx"
#include "SimulationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimulationSensitivityAnalysis
 * Implementation of Simulation sensitivity analysis, it means computation of standardized importance factors
 */
class OT_API SimulationSensitivityAnalysis
  : public PersistentObject
{

  CLASSNAME;
public:

  typedef NumericalMathFunction IsoProbabilisticTransformation;

  /** Default constructor */
  SimulationSensitivityAnalysis();

  /** Standard constructor */
  SimulationSensitivityAnalysis(const NumericalSample & inputSample,
                                const NumericalSample & outputSample,
                                const IsoProbabilisticTransformation & transformation,
                                const ComparisonOperator & comparisonOperator,
                                const NumericalScalar threshold);

  /** Standard constructor */
  explicit SimulationSensitivityAnalysis(const Event & event);

  /** Standard constructor */
  explicit SimulationSensitivityAnalysis(const SimulationResult & result);

  /** Virtual constructor */
  virtual SimulationSensitivityAnalysis * clone() const;

  /** Mean point in event domain computation */
  NumericalPoint computeMeanPointInEventDomain() const;
  NumericalPoint computeMeanPointInEventDomain(const NumericalScalar threshold) const;

  /** Importance factors computation */
  NumericalPointWithDescription computeImportanceFactors() const;
  NumericalPointWithDescription computeImportanceFactors(const NumericalScalar threshold) const;

public:

  /** Importance factors drawing */
  Graph drawImportanceFactors() const;
  Graph drawImportanceFactorsRange(const Bool probabilityScale = true,
                                   const NumericalScalar lower = -SpecFunc::MaxNumericalScalar,
                                   const NumericalScalar upper = SpecFunc::MaxNumericalScalar) const;

  /** Input sample accessors */
  NumericalSample getInputSample() const;

  /** Output sample accessors */
  NumericalSample getOutputSample() const;

  /** Threshold accessors */
  NumericalScalar getThreshold() const;
  void setThreshold(const NumericalScalar threshold);

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
  NumericalSample inputSample_;

  /* Associated output sample */
  NumericalSample outputSample_;

  /* Iso probabilistic transformation associated with the input sample distribution */
  IsoProbabilisticTransformation transformation_;

  /* Comparison operator defining the event we are interested in */
  ComparisonOperator comparisonOperator_;

  /* Main threshold of interest */
  NumericalScalar threshold_;

}; // class SimulationSensitivityAnalysis

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATIONSENSITIVITYANALYSIS_HXX */
