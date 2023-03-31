//                                               -*- C++ -*-
/**
 *  @brief StandardSpaceCrossEntropyImportanceSamplingAlgorithm implements Importance Sampling by Cross Entropy algorithm in Standard Space
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
#ifndef OPENTURNS_STANDARDSPACECROSSENTROPYIMPORTANCESAMPLINGALGORITHM_HXX
#define OPENTURNS_STANDARDSPACECROSSENTROPYIMPORTANCESAMPLINGALGORITHM_HXX


#include "openturns/CrossEntropyImportanceSamplingAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class StandardSpaceCrossEntropyImportanceSamplingAlgorithm
 *
 * StandardSpaceCrossEntropyImportanceSamplingAlgorithm is some StandardSpaceCrossEntropyImportanceSamplingAlgorithm type to illustrate how to add some classes in OpenTURNS
 */


class OT_API StandardSpaceCrossEntropyImportanceSamplingAlgorithm
  : public CrossEntropyImportanceSamplingAlgorithm
{
CLASSNAME
public:
/** Default constructor */
  StandardSpaceCrossEntropyImportanceSamplingAlgorithm();

  /** Default constructor */
  explicit  StandardSpaceCrossEntropyImportanceSamplingAlgorithm(const RandomVector & event,
                 const Scalar rhoQuantile = ResourceMap::GetAsScalar("StandardSpaceCrossEntropyImportanceSamplingAlgorithm-DefaultRhoQuantile"));

  /** Virtual constructor */
  StandardSpaceCrossEntropyImportanceSamplingAlgorithm * clone() const override;
  
protected:

/** Function computing the output samples from the input samples*/
Sample computeOutputSamples(const Sample & inputSamples) const override;
  
/** Function updating the auxiliary distribution as a function of auxiliary distribution parameters */
void updateAuxiliaryDistribution(const Point & auxiliaryDistributionParameters) override;
  
  
Point optimizeAuxiliaryDistributionParameters(const Sample &  auxiliaryCriticInputSamples) const override;
}; /* class StandardSpaceCrossEntropyImportanceSamplingAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STANDARDSPACECROSSENTROPYIMPORTANCESAMPLINGALGORITHM_HXX */
