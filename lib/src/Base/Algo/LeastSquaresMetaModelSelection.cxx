//                                               -*- C++ -*-
/**
 *  @brief Basis selection algorithm
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <limits>
#include "Log.hxx"
#include "Indices.hxx"
#include "PersistentObjectFactory.hxx"
#include "PenalizedLeastSquaresAlgorithm.hxx"
#include "LeastSquaresMetaModelSelection.hxx"
#include "SVDMethod.hxx"
#include "CholeskyMethod.hxx"
#include "QRMethod.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LeastSquaresMetaModelSelection);

static const Factory<LeastSquaresMetaModelSelection> RegisteredFactory;

/* Default constructor */
LeastSquaresMetaModelSelection::LeastSquaresMetaModelSelection()
  : ApproximationAlgorithmImplementation()
{
  // Nothing to do
}

/* Default constructor */
LeastSquaresMetaModelSelection::LeastSquaresMetaModelSelection(const NumericalSample & x,
    const NumericalSample & y,
    const Basis & psi,
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
LeastSquaresMetaModelSelection::LeastSquaresMetaModelSelection(const NumericalSample & x,
    const NumericalSample & y,
    const NumericalPoint & weight,
    const Basis & psi,
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

void LeastSquaresMetaModelSelection::run()
{
  ApproximationAlgorithmImplementation::run();
}

/* Perform the selection */
void LeastSquaresMetaModelSelection::run(const DesignProxy & proxy)
{
  // for each sub-basis ...
  NumericalScalar minimumError = SpecFunc::MaxNumericalScalar;

  LeastSquaresMethod method;
  const String methodName(ResourceMap::Get("LeastSquaresMetaModelSelection-DecompositionMethod"));
  if      (methodName == "SVD")      method = SVDMethod(proxy, y_, weight_, currentIndices_);
  else if (methodName == "Cholesky") method = CholeskyMethod(proxy, y_, weight_, currentIndices_);
  else if (methodName == "QR")       method = QRMethod(proxy, y_, weight_, currentIndices_);
  else throw InvalidArgumentException(HERE) << "Error: invalid value for \"LeastSquaresMetaModelSelection-DecompositionMethod\" in ResourceMap";

  Indices optimalBasisIndices;
  UnsignedInteger iterations = 0;

  basisSequenceFactory_.initialize();
  basisSequenceFactory_.updateBasis(method);

  // for each sub-basis ...
  const NumericalScalar alpha = std::max(1.0, ResourceMap::GetAsNumericalScalar("LeastSquaresMetaModelSelection-MaximumErrorFactor"));
  const NumericalScalar errorThreshold = std::max(0.0, ResourceMap::GetAsNumericalScalar("LeastSquaresMetaModelSelection-ErrorThreshold"));
  while ((basisSequenceFactory_.getImplementation()->addedPsi_k_ranks_.getSize() > 0) || (basisSequenceFactory_.getImplementation()->removedPsi_k_ranks_.getSize() > 0))
  {
    // retrieve the i-th basis of the sequence
    const NumericalScalar error = fittingAlgorithm_.run(method);
    LOGINFO(OSS() << "\nsubbasis=" << iterations << ", size=" << basisSequenceFactory_.getImplementation()->currentIndices_.getSize() << ", error=" << error << ", qSquare=" << 1.0 - error);

    if (error < minimumError)
    {
      optimalBasisIndices = basisSequenceFactory_.getImplementation()->currentIndices_;
      minimumError = error;
    }
    else
    {
      if (error > alpha * minimumError)
      {
        LOGINFO(OSS() << "Error=" << error << " larger than " << alpha << "*" << minimumError << "=" << alpha * minimumError);
        break;
      }
    }
    if (minimumError < errorThreshold)
    {
      LOGINFO(OSS() << "Minimum error=" << minimumError << " smaller than threshold=" << errorThreshold);
      break;
    }
    basisSequenceFactory_.updateBasis(method);

    ++ iterations;
  }

  // recompute the coefficients of the selected sparse metamodel by least-square regression
  PenalizedLeastSquaresAlgorithm penalizedLeastSquaresAlgorithm(x_, y_, weight_, method.getBasis(), optimalBasisIndices);
  penalizedLeastSquaresAlgorithm.run(proxy);
  const NumericalPoint optimalBasisCoefficients(penalizedLeastSquaresAlgorithm.getCoefficients());
  const NumericalScalar optimalResidual(penalizedLeastSquaresAlgorithm.getResidual());
  // New relative error based on cross-validation error
  const NumericalScalar optimalRelativeError(minimumError / y_.getSize());

  // compute the coefficients in the master basis from the ones in the optimal sub-basis
  NumericalPoint optimalCoefficients( currentIndices_.getSize() );
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
