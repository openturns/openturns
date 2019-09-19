//                                               -*- C++ -*-
/**
 *  @brief Implementation for sobol indices algorithms
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_SOBOLINDICESIMPLEMENTATION_HXX
#define OPENTURNS_SOBOLINDICESIMPLEMENTATION_HXX

#include "openturns/Sample.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SobolIndicesImplementation
 *
 */

class OT_API SobolIndicesImplementation
  : public PersistentObject
{
  CLASSNAME

public:

  /** Default constructor */
  SobolIndicesImplementation();

  /** Parameter constructor */
  SobolIndicesImplementation(const UnsignedInteger modelInputDimension,
                             const UnsignedInteger modelOutputDimension);

  /** Virtual constructor */
  virtual SobolIndicesImplementation * clone() const;

  /** Set the input and output model dimensions */
  void SetModelDimensions(const UnsignedInteger modelInputDimension,
                          const UnsignedInteger modelOutputDimension);

  /** Get the model input dimension */
  UnsignedInteger GetModelInputDimensions();

  /** Get the model output dimension */
  UnsignedInteger GetModelOutputDimensions();

  /** Method that compute first and total order indices using a collection of sample */
  virtual void computeIndices(const Sample & inputSample);

  /** Method that increment first and total order indices using a sample */
  virtual void incrementIndices(const Sample & inputSample);

  /** mean accessor */
  Point getMean() const;

  /** variance accessor */
  Point getVariance() const;

  /** First order indices accessor */
  Point getFirstOrderIndices(const UnsignedInteger marginalIndex = 0) const;

  /** Total order indices accessor */
  Point getTotalOrderIndices(const UnsignedInteger marginalIndex = 0) const;

  /** Interval for the first order indices accessor */
  virtual Interval getFirstOrderIndicesInterval() const;

  /** Interval for the total order indices accessor */
  virtual Interval getTotalOrderIndicesInterval() const;

  /** Aggregated first order indices accessor for multivariate samples */
  Point getAggregatedFirstOrderIndices() const;

  /** Aggregated total order indices accessor for multivariate samples */
  Point getAggregatedTotalOrderIndices() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

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

  /** Mean of the reference output sample */
  Point referenceMean_;

  /** Variance of the reference output sample */
  Point referenceVariance_;

  /** Number of currently computed iterations */
  UnsignedInteger iterations_;

  /** model dimensions */
  UnsignedInteger modelInputDimension_;
  UnsignedInteger modelOutputDimension_;

  /** Sobol indices values */
  Sample firstOrderValues_;
  Sample totalOrderValues_;

}; /* class SobolIndicesImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOBOLINDICESIMPLEMENTATION_HXX */
