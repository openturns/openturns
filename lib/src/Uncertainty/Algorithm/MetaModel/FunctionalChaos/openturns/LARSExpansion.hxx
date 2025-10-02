//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions based on a least-squares
 *         approach.
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LARSEXPANSION_HXX
#define OPENTURNS_LARSEXPANSION_HXX

#include "openturns/FunctionalChaosAlgorithm.hxx"
#include "openturns/OrthogonalBasis.hxx"
#include "openturns/FittingAlgorithm.hxx"
#include "openturns/CorrectedLeaveOneOut.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LARSExpansion
 *
 * The class building chaos expansions
 */

class OT_API LARSExpansion
  : public FunctionalChaosAlgorithm
{
  CLASSNAME

public:
  /** Default constructor */
  LARSExpansion();

  /** Constructor */
  LARSExpansion(const Sample & inputSample,
		const Point & weights,
		const Sample & outputSample,
		const Distribution & distribution,
		const Bool isLARS = true);

  /** Constructor */
  LARSExpansion(const Sample & inputSample,
		const Point & weights,
		const Sample & outputSample,
		const Distribution & distribution,
		const OrthogonalBasis & basis,
		const UnsignedInteger basisSize = ResourceMap::GetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize"),
		const String & methodName = ResourceMap::GetAsString("LARSExpansion-DecompositionMethod"),
		const Bool isLARS = true,
		const FittingAlgorithm & fitting = CorrectedLeaveOneOut());

  /** Constructor */
  LARSExpansion(const Sample & inputSample,
		const Sample & outputSample,
		const Distribution & distribution,
		const Bool isLARS = true);

  /** Constructor */
  LARSExpansion(const Sample & inputSample,
		const Sample & outputSample,
		const Distribution & distribution,
		const OrthogonalBasis & basis,
		const UnsignedInteger basisSize = ResourceMap::GetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize"),
		const String & methodName = ResourceMap::GetAsString("LARSExpansion-DecompositionMethod"),
		const Bool isLARS = true,
		const FittingAlgorithm & fitting = CorrectedLeaveOneOut());

  /** Virtual constructor */
  LARSExpansion * clone() const override;

  /** Computes the functional chaos */
  void run() override;

  /** Method to get/set the active functions */
  Indices getActiveFunctions() const;
  void setActiveFunctions(const Indices & activeFunctions);

  /** Method to get/set the fitting algorithm */
  FittingAlgorithm getFittingAlgorithm() const;
  void setFittingAlgorithm(const FittingAlgorithm & fitting);

  /** Method to get/set the LARS update wrt the OMP update */
  Bool getIsLARS() const;
  void setIsLARS(const Bool isLARS);

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

  /** The isLARS flag to choose between LARS and OMP update */
  Bool isLARS_;

  /** The fitting algorithm to select the best metamodel */
  FittingAlgorithm fitting_;

  /** Attributes to speed-up recomputations on different active functions */
  DesignProxy designProxy_;

  Function transformation_;

  Function inverseTransformation_;
} ; /* class LARSExpansion */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LARSEXPANSION_HXX */
