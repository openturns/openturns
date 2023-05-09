//                                               -*- C++ -*-
/**
 *  @brief StandardSpaceCrossEntropyImportanceSampling implements Importance Sampling by Cross Entropy algorithm in Standard Space
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
#ifndef OPENTURNS_STANDARDSPACECROSSENTROPYIMPORTANCESAMPLING_HXX
#define OPENTURNS_STANDARDSPACECROSSENTROPYIMPORTANCESAMPLING_HXX


#include "openturns/CrossEntropyImportanceSampling.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class StandardSpaceCrossEntropyImportanceSampling
 *
 * StandardSpaceCrossEntropyImportanceSampling is some StandardSpaceCrossEntropyImportanceSampling type to illustrate how to add some classes in OpenTURNS
 */


class OT_API StandardSpaceCrossEntropyImportanceSampling
  : public CrossEntropyImportanceSampling
{
CLASSNAME
public:
/** Default constructor */
  StandardSpaceCrossEntropyImportanceSampling();

  /** Default constructor */
  explicit  StandardSpaceCrossEntropyImportanceSampling(const RandomVector & event,
                 const Scalar quantileLevel = ResourceMap::GetAsScalar("CrossEntropyImportanceSampling-DefaultQuantileLevel"));

  /** Virtual constructor */
  StandardSpaceCrossEntropyImportanceSampling * clone() const override;
  
protected:

/** Function computing the output samples from the input samples*/
Sample computeOutputSamples(const Sample & inputSamples) const override;
  
/** Function updating the auxiliary distribution as a function of auxiliary distribution parameters */
void updateAuxiliaryDistribution(const Point & auxiliaryDistributionParameters) override;
  
  
Point optimizeAuxiliaryDistributionParameters(const Sample &  auxiliaryCriticInputSamples) const override;
}; /* class StandardSpaceCrossEntropyImportanceSampling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STANDARDSPACECROSSENTROPYIMPORTANCESAMPLING_HXX */
