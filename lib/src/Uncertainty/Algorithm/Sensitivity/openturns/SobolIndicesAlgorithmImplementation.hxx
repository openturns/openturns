//                                               -*- C++ -*-
/**
 *  @brief Implementation for sobol indices algorithms
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
#ifndef OPENTURNS_SOBOLINDICESALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_SOBOLINDICESALGORITHMIMPLEMENTATION_HXX

#include "openturns/Distribution.hxx"
#include "openturns/Indices.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/SymmetricTensor.hxx"
#include "openturns/WeightedExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SobolIndicesAlgorithmImplementation
 *
 */

class OT_API SobolIndicesAlgorithmImplementation
  : public PersistentObject
{
  CLASSNAME;
  friend struct BootstrapPolicy;
public:

  /** Default constructor */
  SobolIndicesAlgorithmImplementation();

  /** Constructor with parameters */
  SobolIndicesAlgorithmImplementation(const NumericalSample & inputDesign,
                                      const NumericalSample & outputDesign,
                                      const UnsignedInteger size);

  /** Constructor with distribution / model parameters */
  SobolIndicesAlgorithmImplementation(const Distribution & distribution,
                                      const UnsignedInteger size,
                                      const NumericalMathFunction & model,
                                      const Bool computeSecondOrder = true);

  /** Constructor with experiment / model parameters */
  SobolIndicesAlgorithmImplementation(const WeightedExperiment & experiment,
                                      const NumericalMathFunction & model,
                                      const Bool computeSecondOrder = true);
  /** Virtual constructor */
  virtual SobolIndicesAlgorithmImplementation * clone() const;

  /** First order indices accessor */
  virtual NumericalPoint getFirstOrderIndices(const UnsignedInteger marginalIndex = 0) const;

  /** Interval for the first order indices accessor */
  virtual Interval getFirstOrderIndicesInterval() const;

  /** Second order indices accessor */
  virtual SymmetricMatrix getSecondOrderIndices(const UnsignedInteger marginalIndex = 0) const;

  /** Total indices accessor */
  virtual NumericalPoint getTotalIndices(const UnsignedInteger marginalIndex = 0) const;

  /** Interval for the total indices accessor */
  virtual Interval getTotalIndicesInterval() const;

  /** Aggregated first order indices accessor for multivariate samples */
  NumericalPoint getAggregatedFirstOrderIndices() const;

  /** Aggregated total indices accessor for multivariate samples */
  NumericalPoint getAggregatedTotalIndices() const;

  // Setters for bootstrap size
  UnsignedInteger getBootstrapSize() const;
  void setBootstrapSize(const UnsignedInteger bootstrapSize);

  // Setters for bootstrap confidence level
  NumericalScalar getBootstrapConfidenceLevel() const;
  void setBootstrapConfidenceLevel(const NumericalScalar confidenceLevel);

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  static NumericalSample Generate(const Distribution & distribution,
                                  const UnsignedInteger size,
                                  const Bool computeSecondOrder = true);

  static NumericalSample Generate(const WeightedExperiment & experiment,
                                  const Bool computeSecondOrder = true);

  static Graph DrawImportanceFactors(const NumericalPointWithDescription & importanceFactors,
                                     const String & title);

  static Graph DrawImportanceFactors(const NumericalPoint & values,
                                     const Description & names,
                                     const String & title);

  /** Method that draw (plot) the sensitivity graph */
  virtual Graph draw() const;

  /** Method that draw  the sensitivity graph of a fixed marginal */
  virtual Graph draw(UnsignedInteger marginalIndex) const;

protected:


  /** Internal method that compute Vi/VTi using a collection of samples */
  virtual NumericalSample computeIndices(const NumericalSample & design,
                                         NumericalSample & VTi) const;

  /** Internal method that returns a boostrap NS collection of size inputDimension + 2 */
  NumericalSample getBootstrapDesign(const Indices & indices) const;

  /** Function that computes merged indices using Vi/VTi + variance  */
  NumericalPoint computeAggregatedIndices(const NumericalSample & Vi,
                                          const NumericalSample & VTi,
                                          const NumericalPoint & variance,
                                          NumericalPoint & mergedTotal) const;

  /** void method that computes confidence interval */
  void computeIndicesInterval() const;

  /** Multiplication and sum of two NumericalSamples */
  NumericalPoint computeSumDotSamples(const NumericalSample & x,
                                      const NumericalSample & y) const;

  /** Multiplication and sum of two (sub)samples that are in the same NumericalSamples */
  NumericalPoint computeSumDotSamples(const NumericalSample & sample,
                                      const UnsignedInteger size,
                                      const UnsignedInteger indexX,
                                      const UnsignedInteger indexY) const;


  /** Designs : input & output designs */
  NumericalSample inputDesign_;

  /** Designs : input & output designs */
  NumericalSample outputDesign_;

  /** Simulation size */
  UnsignedInteger size_;

  /** Number of Bootstrap sampling size */
  UnsignedInteger bootstrapSize_;

  /** Confidence level for Bootstrap */
  NumericalScalar confidenceLevel_;

  /** Variance of the reference output sample */
  NumericalPoint referenceVariance_;

  /** Variance conditionnaly to the i-th variable => Si = Vi/Var*/
  mutable NumericalSample varianceI_;

  /** Variance totale of the i-th variable => STi = VTi/Var */
  mutable NumericalSample varianceTI_;

  /** Aggregated first order indices */
  mutable NumericalPoint mergedFirstOrderIndices_;

  /** Aggregated total indices */
  mutable NumericalPoint mergedTotalIndices_;

  /** Second order indices */
  mutable SymmetricTensor secondOrderIndices_;

  /** Confidence interval for first order indices (merged indices) */
  mutable Interval firstOrderIndiceInterval_;

  /** Confidence interval for total indices (merged indices) */
  mutable Interval totalIndiceInterval_;

}; /* class SobolIndicesAlgorithmImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOBOLINDICESALGORITHMIMPLEMENTATION_HXX */
