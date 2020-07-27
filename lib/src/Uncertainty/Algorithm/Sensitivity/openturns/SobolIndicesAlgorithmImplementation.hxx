//                                               -*- C++ -*-
/**
 *  @brief Implementation for sobol indices algorithms
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SOBOLINDICESALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_SOBOLINDICESALGORITHMIMPLEMENTATION_HXX

#include "openturns/Distribution.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Function.hxx"
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
  CLASSNAME
  friend struct BootstrapPolicy;
public:

  /** Default constructor */
  SobolIndicesAlgorithmImplementation();

  /** Constructor with parameters */
  SobolIndicesAlgorithmImplementation(const Sample & inputDesign,
                                      const Sample & outputDesign,
                                      const UnsignedInteger size);

  /** Constructor with distribution / model parameters */
  SobolIndicesAlgorithmImplementation(const Distribution & distribution,
                                      const UnsignedInteger size,
                                      const Function & model,
                                      const Bool computeSecondOrder = false);

  /** Constructor with experiment / model parameters */
  SobolIndicesAlgorithmImplementation(const WeightedExperiment & experiment,
                                      const Function & model,
                                      const Bool computeSecondOrder = false);
  /** Virtual constructor */
  virtual SobolIndicesAlgorithmImplementation * clone() const;

  /** First order indices accessor */
  virtual Point getFirstOrderIndices(const UnsignedInteger marginalIndex = 0) const;

  /** Interval for the first order indices accessor */
  virtual Interval getFirstOrderIndicesInterval() const;

  /** Second order indices accessor */
  virtual SymmetricMatrix getSecondOrderIndices(const UnsignedInteger marginalIndex = 0) const;

  /** Total order indices accessor */
  virtual Point getTotalOrderIndices(const UnsignedInteger marginalIndex = 0) const;

  /** Interval for the total order indices accessor */
  virtual Interval getTotalOrderIndicesInterval() const;

  /** Aggregated first order indices accessor for multivariate samples */
  Point getAggregatedFirstOrderIndices() const;

  /** Aggregated total order indices accessor for multivariate samples */
  Point getAggregatedTotalOrderIndices() const;

  // Setters for bootstrap size
  UnsignedInteger getBootstrapSize() const;
  void setBootstrapSize(const UnsignedInteger bootstrapSize);

  /** Setters for confidence level */
  Scalar getConfidenceLevel() const;
  void setConfidenceLevel(const Scalar confidenceLevel);

  /** Whether to use bootstrap or asymptotic distribution */
  void setUseAsymptoticDistribution(Bool useAsymptoticDistribution);
  Bool getUseAsymptoticDistribution() const;

  /** Estimator distribution accessor */
  Distribution getFirstOrderIndicesDistribution() const;
  Distribution getTotalOrderIndicesDistribution() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  static Graph DrawImportanceFactors(const PointWithDescription & importanceFactors,
                                     const String & title);

  static Graph DrawImportanceFactors(const Point & values,
                                     const Description & names,
                                     const String & title);

  static Graph DrawSobolIndices(const Description & inputDescription,
                                const Point & firstOrderIndices,
                                const Point & secondOrderIndices,
                                const Interval & firstOrderConfidenceInterval = Interval(),
                                const Interval & totalOrderConfidenceInterval = Interval());

  static Graph DrawCorrelationCoefficients(const Point & values,
      const Description & names,
      const String & title);

  static Graph DrawCorrelationCoefficients(const PointWithDescription & correlationCoefficients,
      const String & title);

  /** Method that draw (plot) the sensitivity graph */
  virtual Graph draw() const;

  /** Method that draw  the sensitivity graph of a fixed marginal */
  virtual Graph draw(UnsignedInteger marginalIndex) const;

  /** Design accessor */
  void setDesign(const Sample & inputDesign,
                 const Sample & outputDesign,
                 const UnsignedInteger size);
protected:

  /** Internal method that compute Vi/VTi using a collection of samples */
  virtual Sample computeIndices(const Sample & design,
                                Sample & VTi) const;

  /** Internal method that returns a boostrap NS collection of size inputDimension + 2 */
  Sample getBootstrapDesign(const Indices & indices) const;

  /** Function that computes aggregated indices using Vi/VTi + variance  */
  Point computeAggregatedIndices(const Sample & Vi,
                                 const Sample & VTi,
                                 const Point & variance,
                                 Point & aggregatedTotal) const;

  /** void method that computes bootstrap confidence interval */
  void computeBootstrapDistribution() const;

  /** void method that computes asymptotic confidence interval */
  virtual void computeAsymptoticDistribution() const;

  void computeIndicesDistribution() const;

  /** Helper function to compute variance of the estimators */
  Scalar computeVariance(const Sample & u, const Function & psi) const;

  /** Set asymptotic confidence interval from variance */
  void setConfidenceInterval(const Point & varianceFO,
                             const Point & varianceTO) const;

  /** Multiplication and sum of two Samples */
  Point computeSumDotSamples(const Sample & x,
                             const Sample & y) const;

  /** Multiplication and sum of two (sub)samples that are in the same Samples */
  Point computeSumDotSamples(const Sample & sample,
                             const UnsignedInteger size,
                             const UnsignedInteger indexX,
                             const UnsignedInteger indexY) const;

  /** Multiplication of two 1d sub-samples */
  static Sample ComputeProdSample(const Sample & sample,
                                  const UnsignedInteger marginalIndex,
                                  const UnsignedInteger size,
                                  const UnsignedInteger indexX,
                                  const UnsignedInteger indexY);

  /** Designs : input & output designs */
  Sample inputDesign_;
  Sample outputDesign_;

  /** Simulation size */
  UnsignedInteger size_;

  /** Number of Bootstrap sampling size */
  UnsignedInteger bootstrapSize_;

  /** Confidence level for Bootstrap */
  Scalar confidenceLevel_;

  /** Variance of the reference output sample */
  Point referenceVariance_;

  /** Variance conditionnaly to the i-th variable => Si = Vi/Var*/
  mutable Sample varianceI_;

  /** Variance totale of the i-th variable => STi = VTi/Var */
  mutable Sample varianceTI_;

  /** Aggregated first order indices */
  mutable Point aggregatedFirstOrderIndices_;

  /** Aggregated total order indices */
  mutable Point aggregatedTotalOrderIndices_;

  /** Second order indices */
  mutable SymmetricTensor secondOrderIndices_;

  /** Distribution of the first order indices (aggregated indices) */
  mutable Distribution firstOrderIndiceDistribution_;

  /** Distribution of the total order indices (aggregated indices) */
  mutable Distribution totalOrderIndiceDistribution_;

  /** Whether the indices distribution is computed already */
  mutable Bool alreadyComputedIndicesDistribution_;

  /** Whether to use bootstrap or asymptotic distribution */
  Bool useAsymptoticDistribution_;

}; /* class SobolIndicesAlgorithmImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOBOLINDICESALGORITHMIMPLEMENTATION_HXX */
