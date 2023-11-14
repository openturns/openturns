//                                               -*- C++ -*-
/**
 *  @brief PhysicalSpaceCrossEntropyImportanceSampling implements Importance Sampling by Cross Entropy algorithm in Physical Space
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PHYSICALSPACECROSSENTROPYIMPORTANCESAMPLING_HXX
#define OPENTURNS_PHYSICALSPACECROSSENTROPYIMPORTANCESAMPLING_HXX


#include "openturns/CrossEntropyImportanceSampling.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PhysicalSpaceCrossEntropyImportanceSampling
 *
 * PhysicalSpaceCrossEntropyImportanceSampling is some PhysicalSpaceCrossEntropyImportanceSampling type to illustrate how to add some classes in OpenTURNS
 */


class OT_API PhysicalSpaceCrossEntropyImportanceSampling
  : public CrossEntropyImportanceSampling
{
  CLASSNAME
public:
  /** Default constructor */
  PhysicalSpaceCrossEntropyImportanceSampling();

  /** Default constructor */
  PhysicalSpaceCrossEntropyImportanceSampling(const RandomVector & event,
      const Distribution &auxiliaryDistribution,
      const Indices &activeParameters,
      const Point &initialAuxiliaryDistributionParameters,
      const Interval &bounds,
      const Scalar quantileLevel = ResourceMap::GetAsScalar("CrossEntropyImportanceSampling-DefaultQuantileLevel"));

  /** Virtual constructor */
  PhysicalSpaceCrossEntropyImportanceSampling * clone() const override;

  /** Set solver */
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);


  /** Get solver */
  OptimizationAlgorithm getOptimizationAlgorithm() const;

protected:

  /** Function computing the output samples from the input samples*/
  Sample computeOutputSamples(const Sample & inputSamples) const override;

  /** Function updating the auxiliary distribution as a function of auxiliary distribution parameters */
  void updateAuxiliaryDistribution(const Point & auxiliaryDistributionParameters) override;

  /** Function updating the auxiliary distribution with initial parameters (in case of multiple runs of algorithm) */
  void resetAuxiliaryDistribution() override;

  Point optimizeAuxiliaryDistributionParameters(const Sample &  auxiliaryCriticInputSamples) const override;

private:

  // active parameters
  Indices activeParameters_;

  // Initial auxiliary distribution parameters
  Point initialAuxiliaryDistributionParameters_;

  // bounds for optimization algorithm
  Interval bounds_;

  // Optimization solver
  OptimizationAlgorithm solver_;

}; /* class PhysicalSpaceCrossEntropyImportanceSampling */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PHYSICALSPACECROSSENTROPYIMPORTANCESAMPLING_HXX */
