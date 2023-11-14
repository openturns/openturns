//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions based on a least-squares
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
#ifndef OPENTURNS_LEASTSQUARESEXPANSION_HXX
#define OPENTURNS_LEASTSQUARESEXPANSION_HXX

#include "openturns/FunctionalChaosAlgorithm.hxx"
#include "openturns/OrthogonalBasis.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LeastSquaresExpansion
 *
 * The class building chaos expansions
 */

class OT_API LeastSquaresExpansion
  : public FunctionalChaosAlgorithm
{
  CLASSNAME

public:
  /** Default constructor */
  LeastSquaresExpansion();

  /** Constructor */
  LeastSquaresExpansion(const Sample & inputSample,
                        const Point & weights,
                        const Sample & outputSample,
                        const Distribution & distribution);

  /** Constructor */
  LeastSquaresExpansion(const Sample & inputSample,
                        const Point & weights,
                        const Sample & outputSample,
                        const Distribution & distribution,
                        const OrthogonalBasis & basis,
                        const UnsignedInteger basisSize = ResourceMap::GetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize"),
                        const String & methodName = ResourceMap::GetAsString("LeastSquaresExpansion-DecompositionMethod"));

  /** Constructor */
  LeastSquaresExpansion(const Sample & inputSample,
                        const Sample & outputSample,
                        const Distribution & distribution);

  /** Constructor */
  LeastSquaresExpansion(const Sample & inputSample,
                        const Sample & outputSample,
                        const Distribution & distribution,
                        const OrthogonalBasis & basis,
                        const UnsignedInteger basisSize = ResourceMap::GetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize"),
                        const String & methodName = ResourceMap::GetAsString("LeastSquaresExpansion-DecompositionMethod"));

  /** Virtual constructor */
  LeastSquaresExpansion * clone() const override;

  /** Computes the functional chaos */
  void run() override;

  /** Method to get/set the active functions */
  Indices getActiveFunctions() const;
  void setActiveFunctions(const Indices & activeFunctions);

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

  /** The active functions in the basis */
  Indices activeFunctions_;

  /** The name of the decomposition method */
  String methodName_;

  /** Attributes to speed-up recomputations on different active functions */
  DesignProxy designProxy_;

  Function transformation_;

  Function inverseTransformation_;
} ; /* class LeastSquaresExpansion */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEASTSQUARESEXPANSION_HXX */
