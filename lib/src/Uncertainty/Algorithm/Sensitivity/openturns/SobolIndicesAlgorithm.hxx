//                                               -*- C++ -*-
/**
 *  @brief SobolIndicesAlgorithm provides capabilities for sensitivity algorithm
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
#ifndef OPENTURNS_SOBOLINDICESALGORITHM_HXX
#define OPENTURNS_SOBOLINDICESALGORITHM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"
#include "openturns/Interval.hxx"
#include "openturns/SymmetricMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SobolIndicesAlgorithm
 * This class provides capabilities for sensitivity algorithm
 */

class OT_API SobolIndicesAlgorithm
  : public TypedInterfaceObject<SobolIndicesAlgorithmImplementation>
{

  CLASSNAME;
public:

  typedef Pointer<SobolIndicesAlgorithmImplementation> Implementation;

  /** Default constructor */
  SobolIndicesAlgorithm();

  /** Constructor from an implementation */
  SobolIndicesAlgorithm(const SobolIndicesAlgorithmImplementation & implementation);

  /** Constructor from a Pointer to an implementation */
  SobolIndicesAlgorithm(const Implementation & p_implementation);

  /** Constructor with parameters */
  SobolIndicesAlgorithm(const NumericalSample & inputDesign,
                        const NumericalSample & outputDesign,
                        const UnsignedInteger size);

  /** Constructor with parameters */
  SobolIndicesAlgorithm(const Distribution & distribution,
                        const UnsignedInteger size,
                        const NumericalMathFunction & model,
                        const Bool computeSecondOrder = true);

  /** Constructor with experiment / model parameters */
  SobolIndicesAlgorithm(const WeightedExperiment & experiment,
                        const NumericalMathFunction & model,
                        const Bool computeSecondOrder = true);

  /** First order indices accessor */
  NumericalPoint getFirstOrderIndices(const UnsignedInteger marginalIndex=0) const;

  /** Interval for the first order indices accessor */
  Interval getFirstOrderIndicesInterval() const;

  /** Second order indices accessor */
  SymmetricMatrix getSecondOrderIndices(const UnsignedInteger marginalIndex=0) const;

  /** Total order indices accessor */
  NumericalPoint getTotalOrderIndices(const UnsignedInteger marginalIndex=0) const;

  /** Interval for the total order indices accessor */
  Interval getTotalOrderIndicesInterval() const;

  /** Aggregated first order indices accessor for multivariate samples */
  NumericalPoint getAggregatedFirstOrderIndices() const;

  /** Aggregated total order indices accessor for multivariate samples */
  NumericalPoint getAggregatedTotalOrderIndices() const;

  // Setter for bootstrap number
  void setBootstrapSize(const UnsignedInteger bootstrapSize);

  // Setter for bootstrap confidence level
  void setBootstrapConfidenceLevel(const NumericalScalar confidenceLevel);

  /** Method that draw (plot) the sensitivity graph */
  virtual Graph draw() const;

  /** Method that draw  the sensitivity graph of a fixed marginal */
  virtual Graph draw(UnsignedInteger marginalIndex) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

} ; /* class SobolIndicesAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOBOLINDICESALGORITHM_HXX */
