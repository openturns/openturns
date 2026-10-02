//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions based on a least-squares
 *         approach with OMP or LARS model selection.
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
#ifndef OPENTURNS_SPARSEEXPANSION_HXX
#define OPENTURNS_SPARSEEXPANSION_HXX

#include "openturns/FunctionalChaosAlgorithm.hxx"
#include "openturns/OrthogonalBasis.hxx"
#include "openturns/FittingAlgorithm.hxx"
#include "openturns/CorrectedLeaveOneOut.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SparseExpansion
 *
 * The class building chaos expansions
 */

class OT_API SparseExpansion
  : public FunctionalChaosAlgorithm
{
  CLASSNAME

public:
  /** Default constructor */
  SparseExpansion();

  /** Constructor */
  SparseExpansion(const Sample & inputSample,
		const Point & weights,
		const Sample & outputSample,
		const Distribution & distribution);

  /** Constructor */
  SparseExpansion(const Sample & inputSample,
		const Point & weights,
		const Sample & outputSample,
		const Distribution & distribution,
		const OrthogonalBasis & basis,
		const UnsignedInteger basisSize = ResourceMap::GetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize"),
		const String & methodName = ResourceMap::GetAsString("SparseExpansion-DecompositionMethod"),
		const FittingAlgorithm & fitting = CorrectedLeaveOneOut());

  /** Constructor */
  SparseExpansion(const Sample & inputSample,
		const Sample & outputSample,
		const Distribution & distribution);

  /** Constructor */
  SparseExpansion(const Sample & inputSample,
		const Sample & outputSample,
		const Distribution & distribution,
		const OrthogonalBasis & basis,
		const UnsignedInteger basisSize = ResourceMap::GetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize"),
		const String & methodName = ResourceMap::GetAsString("SparseExpansion-DecompositionMethod"),
		const FittingAlgorithm & fitting = CorrectedLeaveOneOut());

  /** Constructor with active functions */
  SparseExpansion(const Sample & inputSample,
		const Point & weights,
		const Sample & outputSample,
		const Distribution & distribution,
		const OrthogonalBasis & basis,
		const UnsignedInteger basisSize,
		const Indices & activeFunctions,
		const String & methodName = ResourceMap::GetAsString("SparseExpansion-DecompositionMethod"),
		const FittingAlgorithm & fitting = CorrectedLeaveOneOut());

  /** Constructor with active functions */
  SparseExpansion(const Sample & inputSample,
		const Sample & outputSample,
		const Distribution & distribution,
		const OrthogonalBasis & basis,
		const UnsignedInteger basisSize,
		const Indices & activeFunctions,
		const String & methodName = ResourceMap::GetAsString("SparseExpansion-DecompositionMethod"),
		const FittingAlgorithm & fitting = CorrectedLeaveOneOut());

  /** Virtual constructor */
  SparseExpansion * clone() const override;

  /** Computes the functional chaos */
  void run() override;

  /** Method to get/set the active functions */
  Indices getActiveFunctions() const;
  void setActiveFunctions(const Indices & activeFunctions);

  /** Method to get/set the fitting algorithm */
  FittingAlgorithm getFittingAlgorithm() const;
  void setFittingAlgorithm(const FittingAlgorithm & fitting);

  /** Method to get/set the selection method ("OMP" or "LARS") */
  String getSelectionMethod() const;
  void setSelectionMethod(const String & method);

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
  UnsignedInteger basisSize_ = 0;

  /** The active functions in the basis */
  Indices activeFunctions_;

  /** The name of the decomposition method */
  String methodName_;

  /** The fitting algorithm to select the best metamodel */
  FittingAlgorithm fitting_;

  /** The selection method: "OMP" or "LARS" */
  String selectionMethod_;

  /** Attributes to speed-up recomputations on different active functions */
  DesignProxy designProxy_;

  /** Run OMP selection */
  void runOMP(const Collection<Function> & functions,
              const UnsignedInteger outputDimension,
              const UnsignedInteger sampleSize,
              const UnsignedInteger maximumNumberOfIterations,
              const Scalar alpha,
              const Scalar errorThreshold,
              const Scalar maximumError,
              const Scalar smallCoefficient,
              const UnsignedInteger consecutiveIncreases);

  /** Run LARS selection */
  void runLARS(const Collection<Function> & functions,
               const UnsignedInteger outputDimension,
               const UnsignedInteger sampleSize,
               const UnsignedInteger maximumNumberOfIterations,
               const Scalar alpha,
               const Scalar errorThreshold,
               const Scalar maximumError,
               const Scalar smallCoefficient,
               const UnsignedInteger consecutiveIncreases);

} ; /* class SparseExpansion */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPARSEEXPANSION_HXX */
