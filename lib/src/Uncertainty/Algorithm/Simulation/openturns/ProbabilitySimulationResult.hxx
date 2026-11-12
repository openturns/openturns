//                                               -*- C++ -*-
/**
 *  @brief ProbabilitySimulationResult is a handle to any concrete
 *  simulation result, preserving its dynamic type
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
#ifndef OPENTURNS_PROBABILITYSIMULATIONRESULT_HXX
#define OPENTURNS_PROBABILITYSIMULATIONRESULT_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/ProbabilitySimulationResultImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProbabilitySimulationResult
 *
 * ProbabilitySimulationResult holds any concrete simulation result
 * (ProbabilitySimulationResultImplementation, NAISResult, SubsetSamplingResult,
 * CrossEntropyResult, ...) without slicing it. It is the element type
 * of per-iteration histories, e.g. in ActiveLearningReliabilityResult:
 * generic quantities are readable directly, concrete details through a
 * downcast to the known concrete type.
 */

class OT_API ProbabilitySimulationResult
  : public TypedInterfaceObject<ProbabilitySimulationResultImplementation>
{

  CLASSNAME
public:

  /** Default constructor */
  ProbabilitySimulationResult();

  /** Standard constructor, forwarded to the implementation */
  ProbabilitySimulationResult(const RandomVector & event,
                              const Scalar probabilityEstimate,
                              const Scalar varianceEstimate,
                              const UnsignedInteger outerSampling,
                              const UnsignedInteger blockSize);

  /** Constructor from a concrete result: the result is cloned, so the
      handle is independent of the source object and later runs never
      mutate stored history elements. Implicit to preserve the former
      conversion from a derived result (CrossEntropyResult,
      SubsetSamplingResult, ...) to the base result type. */
  ProbabilitySimulationResult(const ProbabilitySimulationResultImplementation & result);

#ifndef SWIG
  /** Constructor from implementation */
  ProbabilitySimulationResult(const Implementation & p_implementation);

  /** Constructor from implementation pointer */
  ProbabilitySimulationResult(ProbabilitySimulationResultImplementation * p_implementation);

  /** Implicit conversion to the base result type: preserves the former
      derived-to-base conversion for C++ callers that pass a result to a
      function taking a const SimulationResult & reference. The conversion
      returns a copy of the base subobject; use getImplementation() to
      access the concrete result. */
  operator SimulationResult() const;
#endif

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Probability estimate accessor */
  Scalar getProbabilityEstimate() const;

  /** Variance estimate accessor */
  Scalar getVarianceEstimate() const;

  /** Coefficient of variation estimate accessor */
  Scalar getCoefficientOfVariation() const;

  /** Standard deviation estimate accessor */
  Scalar getStandardDeviation() const;

  /** Outer sampling accessor */
  UnsignedInteger getOuterSampling() const;
  void setOuterSampling(const UnsignedInteger outerSampling);

  /** Block size accessor */
  UnsignedInteger getBlockSize() const;
  void setBlockSize(const UnsignedInteger blockSize);

  /** Elapsed time accessor */
  Scalar getTimeDuration() const;
  void setTimeDuration(const Scalar timeDuration);

  /** Event accessor */
  RandomVector getEvent() const;

  /** Event accessor @deprecated */
  void setEvent(const RandomVector & event);

  /** Probability estimate accessor @deprecated */
  void setProbabilityEstimate(const Scalar probabilityEstimate);

  /** Variance estimate accessor @deprecated */
  void setVarianceEstimate(const Scalar varianceEstimate);

  /** Probability estimate distribution accessor */
  Distribution getProbabilityDistribution() const;

  /** Mean point conditioned to the event realization accessor */
  Point getMeanPointInEventDomain() const;

  /** Importance factors accessor @deprecated */
  PointWithDescription getImportanceFactors() const;

  /** Draw the importance factors @deprecated */
  Graph drawImportanceFactors() const;

  /** Confidence length @deprecated */
  Scalar getConfidenceLength(const Scalar level = ProbabilitySimulationResultImplementation::GetDefaultConfidenceLevel()) const;

} ; /* class ProbabilitySimulationResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROBABILITYSIMULATIONRESULT_HXX */
