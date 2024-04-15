//                                               -*- C++ -*-
/**
 *  @brief Factory for GeneralizedPareto distribution
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
#ifndef OPENTURNS_GENERALIZEDPARETOFACTORY_HXX
#define OPENTURNS_GENERALIZEDPARETOFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/GeneralizedPareto.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/DistributionFactoryLikelihoodResult.hxx"
#include "openturns/ProfileLikelihoodResult.hxx"
#include "openturns/GridLayout.hxx"
#include "openturns/CovariatesResult.hxx"
#include "openturns/TimeVaryingResult.hxx"
#include "openturns/Basis.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GeneralizedParetoFactory
 */
class OT_API GeneralizedParetoFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  GeneralizedParetoFactory();

  /** Virtual constructor */
  GeneralizedParetoFactory * clone() const override;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const override;
  Distribution build(const Point & parameters) const override;
  Distribution build() const override;
  GeneralizedPareto buildAsGeneralizedPareto(const Sample & sample) const;
  GeneralizedPareto buildAsGeneralizedPareto(const Point & parameters) const;
  GeneralizedPareto buildAsGeneralizedPareto() const;

  /** Optimization solver accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);

  /** Algorithm associated with the method of moments */
  GeneralizedPareto buildMethodOfMoments(const Sample & sample) const;

  /** Algorithm associated with the method of exponential regression model */
  GeneralizedPareto buildMethodOfExponentialRegression(const Sample & sample) const;

  /** Algorithm associated with the method of modified moments */
  GeneralizedPareto buildMethodOfProbabilityWeightedMoments(const Sample & sample) const;

  /** Mean residual life plot */
  Graph drawMeanResidualLife(const Sample & sample) const;

  /** Maximum likelihood */
protected:
  DistributionFactoryLikelihoodResult buildMethodOfLikelihoodMaximizationEstimatorWithStartingPoint(const Sample & sample,
                                                                                                    const Point & startingPoint, const Scalar u) const;
public:
  DistributionFactoryLikelihoodResult buildMethodOfLikelihoodMaximizationEstimator(const Sample & sample, const Scalar u) const;
  GeneralizedPareto buildMethodOfLikelihoodMaximization(const Sample & sample, const Scalar u) const;

  /** Profiled maximum likelihood */
  ProfileLikelihoodResult buildMethodOfXiProfileLikelihoodEstimator(const Sample & sample, const Scalar u) const;
  GeneralizedPareto buildMethodOfXiProfileLikelihood(const Sample & sample, const Scalar u) const;

  /** Parameter stability plot*/
  GridLayout drawParameterThresholdStability(const Sample & sample, const Interval & thresholdRange) const;

  /** Covariates */
  CovariatesResult buildCovariates(const Sample & sample,
                                   const Scalar u,
                                   const Sample & covariates,
                                   const Indices & sigmaIndices = Indices(),
                                   const Indices & xiIndices = Indices(),
                                   const Function & sigmaLink = Function(),
                                   const Function & xiLink = Function(),
                                   const String & initializationMethod = ResourceMap::GetAsString("GeneralizedParetoFactory-InitializationMethod"),
                                   const String & normalizationMethod = ResourceMap::GetAsString("GeneralizedParetoFactory-NormalizationMethod")) const;

  /** Time-varying */
  TimeVaryingResult buildTimeVarying(const Sample & sample,
                                     const Scalar u,
                                     const Sample & timeStamps,
                                     const Basis & basis,
                                     const Indices & sigmaIndices = Indices(),
                                     const Indices & xiIndices = Indices(),
                                     const Function & sigmaLink = Function(),
                                     const Function & xiLink = Function(),
                                     const String & initializationMethod = ResourceMap::GetAsString("GeneralizedParetoFactory-InitializationMethod"),
                                     const String & normalizationMethod = ResourceMap::GetAsString("GeneralizedParetoFactory-NormalizationMethod")) const;

  /** Return level */
  Distribution buildReturnLevelEstimator(const DistributionFactoryResult & result, const Sample & sample, const Scalar m, const Scalar theta = 1.0) const;

  /** Return level via profiled likelihood */
  ProfileLikelihoodResult buildReturnLevelProfileLikelihoodEstimator(const Sample & sample, const Scalar u, const Scalar m) const;
  GeneralizedPareto buildReturnLevelProfileLikelihood(const Sample & sample, const Scalar u, const Scalar m) const;

private:

  /** Optimization solver */
  OptimizationAlgorithm solver_;

}; /* class GeneralizedParetoFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GENERALIZEDPARETOFACTORY_HXX */
