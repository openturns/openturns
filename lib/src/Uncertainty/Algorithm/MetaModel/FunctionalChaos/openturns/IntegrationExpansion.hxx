//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions based on a dot-product
 *         approach.
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
#ifndef OPENTURNS_INTEGRATIONEXPANSION_HXX
#define OPENTURNS_INTEGRATIONEXPANSION_HXX

#include "openturns/FunctionalChaosAlgorithm.hxx"
#include "openturns/OrthogonalBasis.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class IntegrationExpansion
 *
 * The class building chaos expansions
 */

class OT_API IntegrationExpansion
  : public FunctionalChaosAlgorithm
{
  CLASSNAME

public:
  /** Default constructor */
  IntegrationExpansion();

  /** Constructor */
  IntegrationExpansion(const Sample & inputSample,
                       const Sample & outputSample,
                       const Distribution & distribution);

  /** Constructor */
  IntegrationExpansion(const Sample & inputSample,
                       const Point & weights,
                       const Sample & outputSample,
                       const Distribution & distribution);

  /** Constructor */
  IntegrationExpansion(const Sample & inputSample,
                       const Sample & outputSample,
                       const Distribution & distribution,
                       const OrthogonalBasis & basis,
                       const UnsignedInteger basisSize = ResourceMap::GetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize"));

  /** Constructor */
  IntegrationExpansion(const Sample & inputSample,
                       const Point & weights,
                       const Sample & outputSample,
                       const Distribution & distribution,
                       const OrthogonalBasis & basis,
                       const UnsignedInteger basisSize = ResourceMap::GetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize"));

  /** Virtual constructor */
  IntegrationExpansion * clone() const override;

  /** Method to get/set the active functions */
  Indices getActiveFunctions() const;
  void setActiveFunctions(const Indices & activeFunctions);

  /** Computes the functional chaos */
  void run() override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The orthogonal basis used for the projection */
  OrthogonalBasis basis_;

  /** The size of the basis used for the projection */
  UnsignedInteger basisSize_;

  /** The selected basis functions */
  Indices activeFunctions_;

  /** Attributes to speed-up recomputations on different active functions */
  DesignProxy designProxy_;

  Function transformation_;

  Function inverseTransformation_;
} ; /* class IntegrationExpansion */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INTEGRATIONEXPANSION_HXX */
