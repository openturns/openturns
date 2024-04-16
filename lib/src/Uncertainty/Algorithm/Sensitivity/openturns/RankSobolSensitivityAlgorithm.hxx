//                                               -*- C++ -*-
/**
 *  @brief Implementation of Rank Sobol Sensitivity Algorithm 
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_RANKSOBOLENSITIVITYALGORITHM_HXX
#define OPENTURNS_RANKSOBOLENSITIVITYALGORITHM_HXX

#include "openturns/Point.hxx"
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RankSobolSensitivityAlgorithm
 *
 */

class OT_API RankSobolSensitivityAlgorithm
  : public SobolIndicesAlgorithmImplementation
{
  CLASSNAME
  friend struct RankSobolBootstrapPolicy;
public:

  /** Default constructor */
  RankSobolSensitivityAlgorithm();

  /** Constructor with parameters */
  RankSobolSensitivityAlgorithm(const Sample & inputDesign,
                                const Sample & outputDesign,
                                const UnsignedInteger size);

  /** Virtual constructor */
  RankSobolSensitivityAlgorithm * clone() const override;
  
  /** Method that draw (plot) the sensitivity graph */
  Graph draw() const override;
  
  /** Design accessor */
  void setDesign(const Sample & inputDesign,
                 const Sample & outputDesign,
                 const UnsignedInteger size) override;
                                 
  /** First order indices accessor */
  Point getFirstOrderIndices(const UnsignedInteger marginalIndex = 0) const override;
  
  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;  
  
  /** Second order indices accessor */
  SymmetricMatrix getSecondOrderIndices(const UnsignedInteger marginalIndex = 0) const override;
  
  /** Total order indices accessor */
  Point getTotalOrderIndices(const UnsignedInteger marginalIndex = 0) const override;

  /** Interval for the total order indices accessor */
  Interval getTotalOrderIndicesInterval() const override;
  
  /** Aggregated total order indices accessor for multivariate samples */
  Point getAggregatedTotalOrderIndices() const override;
  
  /** Whether to use bootstrap or asymptotic distribution */
  void setUseAsymptoticDistribution(Bool ) override {};
  Bool getUseAsymptoticDistribution() const override;

  /** Estimator distribution accessor */
  Distribution getTotalOrderIndicesDistribution() const override;
   
protected:

  Sample computeIndices() const;
  
  /** void method that computes bootstrap confidence interval */
  void computeBootstrapDistribution() const override;

private:

  static Graph DrawSobolFirstIndices(const Description & inputDescription,
                                     const Point & firstOrderIndices,
                                     const Interval & firstOrderConfidenceInterval = Interval());

  /** Internal method that compute Vi/VTi using a sample */
  Sample computeIndices(const Sample & sample, Sample & VTi) const override;
  
  Sample computeIndicesFromSample(const Sample &, const Sample &) const;
  
  /** Function that computes aggregated indices using Vi/VTi + variance  */
  Point computeAggregatedIndices(const Sample & Vi,
                                 const Point & variance) const;
                                 
  /** Constructor with distribution / model parameters */
  RankSobolSensitivityAlgorithm(const Distribution & ,
                                const UnsignedInteger ,
                                const Function & ,
                                const Bool ){};

  /** Constructor with experiment / model parameters */
  RankSobolSensitivityAlgorithm(const WeightedExperiment & ,
                                const Function & ,
                                const Bool ){};
                               
  /** void method that computes asymptotic distribution */
  void computeAsymptoticDistribution() const override {};

  /** Aggregated first order indices accessor for multivariate samples */
  Point getAggregatedFirstOrderIndices() const override;

  /** Method that draw  the sensitivity graph of a fixed marginal */
  Graph draw(const UnsignedInteger marginalIndex) const override;
    
  /** Designs : input & output designs */
  Sample inputDesign_;
  Sample outputDesign_;
  
}; /* class RankSobolSensitivityAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANKSOBOLENSITIVITYALGORITHM_HXX */
