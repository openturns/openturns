//                                               -*- C++ -*-
/**
 *  @brief Sparse approximation LS solver
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

#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SparseMethod.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(SparseMethod)


static const Factory<SparseMethod> Factory_SparseMethod;

/* Default constructor */
SparseMethod::SparseMethod()
  : LeastSquaresMethodImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
SparseMethod::SparseMethod(const LeastSquaresMethod & method,
                           const BasisSequenceFactory & basisSequenceFactory,
                           const FittingAlgorithm & fittingAlgorithm)
  : LeastSquaresMethodImplementation(DesignProxy(), method.getCurrentIndices())
  , method_(method)
  , basisSequenceFactory_(basisSequenceFactory)
  , fittingAlgorithm_(fittingAlgorithm)
{
  // Nothing to do
}


/* Virtual constructor */
SparseMethod * SparseMethod::clone() const
{
  return new SparseMethod( *this );
}


/* String converter */
String SparseMethod::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}

void SparseMethod::update(const Indices & addedIndices,
                          const Indices & conservedIndices,
                          const Indices & removedIndices,
                          const Bool row)
{
  method_.update(addedIndices, conservedIndices, removedIndices, row);
}


/* Solve least-squares problem */
Point SparseMethod::solve(const Point & rhs)
{
  // save state
  const Indices currentIndices(method_.getCurrentIndices());

  // for each sub-basis ...
  Scalar minimumError = SpecFunc::MaxScalar;

  Indices optimalBasisIndices;
  UnsignedInteger iterations = 0;

  Sample rhsSample(rhs.getSize(), 1);
  rhsSample.getImplementation()->setData(rhs.getCollection());

  basisSequenceFactory_.initialize();
  basisSequenceFactory_.updateBasis(method_, rhsSample);

  // for each sub-basis ...
  const Scalar alpha = std::max(1.0, ResourceMap::GetAsScalar("SparseMethod-MaximumErrorFactor"));
  const Scalar errorThreshold = std::max(0.0, ResourceMap::GetAsScalar("SparseMethod-ErrorThreshold"));
  while ((basisSequenceFactory_.getImplementation()->addedPsi_k_ranks_.getSize() > 0) || (basisSequenceFactory_.getImplementation()->removedPsi_k_ranks_.getSize() > 0))
  {
    // retrieve the i-th basis of the sequence
    const Scalar error = fittingAlgorithm_.run(method_, rhsSample);
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
    }
    if (!(minimumError >= errorThreshold))
    {
      LOGINFO(OSS() << "Minimum error=" << minimumError << " smaller than threshold=" << errorThreshold);
      break;
    }
    basisSequenceFactory_.updateBasis(method_, rhsSample);

    ++ iterations;
  }

  // solve in the optimal basis
  method_.update(Indices(), optimalBasisIndices, Indices());

  const Point optimalBasisCoefficients(method_.solve(rhs));

  // restore state
  method_.update(Indices(), currentIndices, Indices());

  // compute the coefficients in the master basis from the ones in the optimal sub-basis
  Point optimalCoefficients( currentIndices_.getSize() );
  for (UnsignedInteger i = 0; i < optimalBasisIndices.getSize(); ++ i)
  {
    for (UnsignedInteger j = 0; j < currentIndices_.getSize(); ++ j)
      if (currentIndices_[j] == optimalBasisIndices[i])
        optimalCoefficients[j] = optimalBasisCoefficients[i];
  }
  return optimalCoefficients;
}


SparseMethod::FunctionCollection SparseMethod::getBasis() const
{
  return method_.getBasis();
}

Indices SparseMethod::getCurrentIndices() const
{
  return method_.getCurrentIndices();
}

Indices SparseMethod::getInitialIndices() const
{
  return method_.getInitialIndices();
}

Sample SparseMethod::getInputSample() const
{
  return method_.getInputSample();
}

Point SparseMethod::getWeight() const
{
  return method_.getWeight();
}

CovarianceMatrix SparseMethod::getGramInverse() const
{
  return method_.getImplementation()->getGramInverse();
}

void SparseMethod::trashDecomposition()
{
}

MatrixImplementation SparseMethod::computeWeightedDesign(const Bool whole) const
{
  return method_.getImplementation()->computeWeightedDesign(whole);
}


/* Method save() stores the object through the StorageManager */
void SparseMethod::save(Advocate & adv) const
{
  LeastSquaresMethodImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void SparseMethod::load(Advocate & adv)
{
  LeastSquaresMethodImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
