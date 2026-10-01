//                                               -*- C++ -*-
/**
 *  @brief GenericSimulationResult is a handle to any concrete
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
#ifndef OPENTURNS_GENERICSIMULATIONRESULT_HXX
#define OPENTURNS_GENERICSIMULATIONRESULT_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/ProbabilitySimulationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GenericSimulationResult
 *
 * GenericSimulationResult holds any concrete simulation result
 * (ProbabilitySimulationResult, NAISResult, SubsetSamplingResult,
 * CrossEntropyResult, ...) without slicing it. It is the element type
 * of per-iteration histories, e.g. in ActiveLearningReliabilityResult:
 * generic quantities are readable directly, concrete details through a
 * downcast to the known concrete type.
 */

class OT_API GenericSimulationResult
  : public TypedInterfaceObject<ProbabilitySimulationResult>
{

  CLASSNAME
public:

  /** Default constructor */
  GenericSimulationResult();

  /** Constructor from a concrete result: the result is cloned, so the
      handle is independent of the source object and later runs never
      mutate stored history elements */
  explicit GenericSimulationResult(const ProbabilitySimulationResult & result);

#ifndef SWIG
  /** Constructor from implementation */
  GenericSimulationResult(const Implementation & p_implementation);

  /** Constructor from implementation pointer */
  GenericSimulationResult(ProbabilitySimulationResult * p_implementation);
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

  /** Block size accessor */
  UnsignedInteger getBlockSize() const;

  /** Event accessor */
  RandomVector getEvent() const;

} ; /* class GenericSimulationResult */

typedef Collection<GenericSimulationResult> GenericSimulationResultCollection;

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GENERICSIMULATIONRESULT_HXX */
