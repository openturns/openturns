//                                               -*- C++ -*-
/**
 *  @brief Base class for quantile simulation algorithms
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
#ifndef OPENTURNS_QUANTILESIMULATIONIMPLEMENTATION_HXX
#define OPENTURNS_QUANTILESIMULATIONIMPLEMENTATION_HXX

#include "openturns/SimulationAlgorithm.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/QuantileSimulationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QuantileSimulationImplementation
 *
 * Common state and contract of the quantile simulation algorithms.  The
 * base stores the scalar output, the quantile and confidence levels, the
 * estimator and coverage conventions, and the result handle.  Concrete
 * algorithms implement runFixedSample().
 */
class OT_API QuantileSimulationImplementation
  : public SimulationAlgorithm
{
  CLASSNAME
public:
  typedef QuantileSimulationResultImplementation::EstimatorType EstimatorType;
  typedef QuantileSimulationResultImplementation::CoverageType CoverageType;

  /** Default constructor */
  QuantileSimulationImplementation();

  /** Standard constructor */
  QuantileSimulationImplementation(const RandomVector & output,
                                   const Scalar alpha = 0.95,
                                   const Scalar beta = 0.95);

  /** Virtual constructor */
  QuantileSimulationImplementation * clone() const override;

  /** Output random vector accessor */
  void setOutput(const RandomVector & output);
  RandomVector getOutput() const;

  /** Quantile level accessor */
  void setAlpha(const Scalar alpha);
  Scalar getAlpha() const;

  /** Confidence level accessor */
  void setConfidenceLevel(const Scalar beta);
  Scalar getConfidenceLevel() const;

  /** Sample size accessor */
  virtual void setSampleSize(const UnsignedInteger sampleSize);
  UnsignedInteger getSampleSize() const;

  /** Estimator type accessor */
  void setEstimatorType(const EstimatorType type);
  EstimatorType getEstimatorType() const;

  /** Required coverage accessor */
  void setRequiredCoverage(const CoverageType coverage);
  CoverageType getRequiredCoverage() const;

  /** Performs the actual computation */
  void run() override;

  /** Result accessor */
  QuantileSimulationResult getResult() const;
  void setResult(const QuantileSimulationResult & result);

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Run a fixed-sample experiment */
  virtual QuantileSimulationResult runFixedSample();

  /** Check the input parameters */
  virtual void checkInputs() const;

  /** Check the required coverage against the achieved coverage */
  virtual void checkCoverage(const QuantileSimulationResult & result) const;

  RandomVector output_;
  Scalar alpha_ = 0.95;
  Scalar beta_ = 0.95;
  UnsignedInteger sampleSize_ = 0;
  EstimatorType estimatorType_ = QuantileSimulationResultImplementation::EMPIRICAL_INVERSE;
  CoverageType requiredCoverage_ = QuantileSimulationResultImplementation::COVERAGE_NONE;
  QuantileSimulationResult result_;

}; /* class QuantileSimulationImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUANTILESIMULATIONIMPLEMENTATION_HXX */
