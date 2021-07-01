//                                               -*- C++ -*-
/**
 *  @brief Basis selection algorithm
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include <limits>
#include "openturns/Log.hxx"
#include "openturns/Indices.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/PenalizedLeastSquaresAlgorithm.hxx"
#include "openturns/LeastSquaresMetaModelSelection.hxx"
#include "openturns/SVDMethod.hxx"
#include "openturns/CholeskyMethod.hxx"
#include "openturns/QRMethod.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LeastSquaresMetaModelSelection)

static const Factory<LeastSquaresMetaModelSelection> Factory_LeastSquaresMetaModelSelection;

/* Default constructor */
LeastSquaresMetaModelSelection::LeastSquaresMetaModelSelection()
  : ApproximationAlgorithmImplementation()
{
  // Nothing to do
}

/* Default constructor */
LeastSquaresMetaModelSelection::LeastSquaresMetaModelSelection(const Sample & x,
    const Sample & y,
    const FunctionCollection & psi,
    const Indices & indices,
    const BasisSequenceFactory & basisSequenceFactory,
    const FittingAlgorithm & fittingAlgorithm)
  : ApproximationAlgorithmImplementation( x, y, psi, indices )
  , basisSequenceFactory_(basisSequenceFactory)
  , fittingAlgorithm_(fittingAlgorithm)

{
  // Nothing to do
}

/* Default constructor */
LeastSquaresMetaModelSelection::LeastSquaresMetaModelSelection(const Sample & x,
    const Sample & y,
    const Point & weight,
    const FunctionCollection & psi,
    const Indices & indices,
    const BasisSequenceFactory & basisSequenceFactory,
    const FittingAlgorithm & fittingAlgorithm)
  : ApproximationAlgorithmImplementation( x, y, weight, psi, indices )
  , basisSequenceFactory_(basisSequenceFactory)
  , fittingAlgorithm_(fittingAlgorithm)
{
  // Nothing to do
}


/* Virtual constructor */
LeastSquaresMetaModelSelection * LeastSquaresMetaModelSelection::clone() const
{
  return new LeastSquaresMetaModelSelection( *this );
}

/* BasisSequenceFactory accessor */
void LeastSquaresMetaModelSelection::setBasisSequenceFactory(const BasisSequenceFactory & basisSequenceFactory)
{
  basisSequenceFactory_ = basisSequenceFactory;
}

BasisSequenceFactory LeastSquaresMetaModelSelection::getBasisSequenceFactory() const
{
  return basisSequenceFactory_;
}

/* FittingAlgorithm accessor */
void LeastSquaresMetaModelSelection::setFittingAlgorithm(const FittingAlgorithm & fittingAlgorithm)
{
  fittingAlgorithm_ = fittingAlgorithm;
}

FittingAlgorithm LeastSquaresMetaModelSelection::getFittingAlgorithm() const
{
  return fittingAlgorithm_;
}


/* String converter */
String LeastSquaresMetaModelSelection::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " basisSequenceFactory=" << basisSequenceFactory_
         << " fittingAlgorithm=" << fittingAlgorithm_;
}

/* Perform the selection */
void LeastSquaresMetaModelSelection::run(const DesignProxy & proxy)
{
  // for each sub-basis ...
  Scalar minimumError = SpecFunc::MaxScalar;

  const String methodName(ResourceMap::GetAsString("LeastSquaresMetaModelSelection-DecompositionMethod"));
  LeastSquaresMethod method(LeastSquaresMethod::Build(methodName, proxy, weight_, currentIndices_));

  Indices optimalBasisIndices;
  UnsignedInteger iterations = 0;

  basisSequenceFactory_.initialize();
  basisSequenceFactory_.updateBasis(method, y_);

  // for each sub-basis ...
  const Scalar alpha = std::max(1.0, ResourceMap::GetAsScalar("LeastSquaresMetaModelSelection-MaximumErrorFactor"));
  const Scalar errorThreshold = std::max(0.0, ResourceMap::GetAsScalar("LeastSquaresMetaModelSelection-ErrorThreshold"));
  const Scalar maximumError = std::max(0.0, ResourceMap::GetAsScalar("LeastSquaresMetaModelSelection-MaximumError"));
  while ((basisSequenceFactory_.getImplementation()->addedPsi_k_ranks_.getSize() > 0) || (basisSequenceFactory_.getImplementation()->removedPsi_k_ranks_.getSize() > 0))
  {
    // retrieve the i-th basis of the sequence
    const Scalar error = fittingAlgorithm_.run(method, y_);
    LOGINFO(OSS() << "\nsubbasis=" << iterations << ", size=" << basisSequenceFactory_.getImplementation()->currentIndices_.getSize() << ", error=" << error << ", qSquare=" << 1.0 - error);

    if (error < minimumError)
    {
      optimalBasisIndices = basisSequenceFactory_.getImplementation()->currentIndices_;
      minimumError = error;
    }
    else
    {
      if (!(error <= alpha * minimumError))
      {
        LOGINFO(OSS() << "Error=" << error << " larger than " << alpha << "*" << minimumError << "=" << alpha * minimumError);
        break;
      }
      if (error > maximumError)
      {
        LOGINFO(OSS() << "Error=" << error << " larger than " << maximumError);
        break;
      }
    }
    if (!(minimumError >= errorThreshold))
    {
      LOGINFO(OSS() << "Minimum error=" << minimumError << " smaller than threshold=" << errorThreshold);
      break;
    }
    basisSequenceFactory_.updateBasis(method, y_);

    ++ iterations;
  }

  // recompute the coefficients of the selected sparse metamodel by least-square regression
  PenalizedLeastSquaresAlgorithm penalizedLeastSquaresAlgorithm(x_, y_, weight_, method.getBasis(), optimalBasisIndices);
  penalizedLeastSquaresAlgorithm.run(proxy);
  const Point optimalBasisCoefficients(penalizedLeastSquaresAlgorithm.getCoefficients());
  const Scalar optimalResidual = penalizedLeastSquaresAlgorithm.getResidual();
  // New relative error based on cross-validation error
  const Scalar optimalRelativeError = minimumError / y_.getSize();

  // compute the coefficients in the master basis from the ones in the optimal sub-basis
  Point optimalCoefficients( currentIndices_.getSize() );
  for (UnsignedInteger i = 0; i < optimalBasisIndices.getSize(); ++ i)
  {
    for (UnsignedInteger j = 0; j < currentIndices_.getSize(); ++ j)
      if (currentIndices_[j] == optimalBasisIndices[i])
        optimalCoefficients[j] = optimalBasisCoefficients[i];
  }

  setCoefficients(optimalCoefficients);
  setResidual(optimalResidual);
  setRelativeError(optimalRelativeError);

  LOGINFO(OSS() << "optimalBasisIndices=" << optimalBasisIndices);
  LOGINFO(OSS() << "optimalError=" << minimumError);
  LOGINFO(OSS() << "optimalResidual=" << optimalResidual);
  LOGINFO(OSS() << "optimalRelativeError=" << optimalRelativeError);
}


/* Method save() stores the object through the StorageManager */
void LeastSquaresMetaModelSelection::save(Advocate & adv) const
{
  ApproximationAlgorithmImplementation::save(adv);
  adv.saveAttribute( "basisSequenceFactory_", basisSequenceFactory_ );
  adv.saveAttribute( "fittingAlgorithm_", fittingAlgorithm_ );
}


/* Method load() reloads the object from the StorageManager */
void LeastSquaresMetaModelSelection::load(Advocate & adv)
{
  ApproximationAlgorithmImplementation::load(adv);
  adv.loadAttribute( "basisSequenceFactory_", basisSequenceFactory_ );
  adv.loadAttribute( "fittingAlgorithm_", fittingAlgorithm_ );
}


END_NAMESPACE_OPENTURNS
