//                                               -*- C++ -*-
/**
 *  @brief LeastSquares solving algorithm implementation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LeastSquaresMethodImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LeastSquaresMethodImplementation);

static const Factory<LeastSquaresMethodImplementation> Factory_LeastSquaresMethodImplementation;

/* Default constructor */
LeastSquaresMethodImplementation::LeastSquaresMethodImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Parameters constructor */
LeastSquaresMethodImplementation::LeastSquaresMethodImplementation(const DesignProxy & proxy,
    const NumericalSample & outputSample,
    const NumericalPoint & weight,
    const Indices & indices)
  : PersistentObject()
  , proxy_(proxy)
  , outputSample_(outputSample)
  , weight_(0)
  , weightSqrt_(0)
  , hasUniformWeight_(false)
  , currentIndices_(indices)
  , initialIndices_(indices)
{
  const UnsignedInteger size = outputSample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: expected a non-empty output sample";
  // Check the argument compatibility
  if (proxy.getInputSample().getSize() != size) throw InvalidArgumentException(HERE) << "Error: the size of the output sample=" << size << " is different from the size of the input sample=" << proxy.getInputSample().getSize();
  if (weight.getSize() != size) throw InvalidArgumentException(HERE) << "Error: the size of the output sample=" << size << " is different from the size of the weight=" << weight.getSize();
  // Check the weight uniformity
  setWeight(weight);
}

/* Parameters constructor */
LeastSquaresMethodImplementation::LeastSquaresMethodImplementation(const DesignProxy & proxy,
    const NumericalSample & outputSample,
    const Indices & indices)
  : PersistentObject()
  , proxy_(proxy)
  , outputSample_(outputSample)
  , weight_(1, 1.0)
  , weightSqrt_(1, 1.0)
  , hasUniformWeight_(true)
  , currentIndices_(indices)
  , initialIndices_(indices)
{
  const UnsignedInteger size = outputSample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: expected a non-empty output sample";
  // Check the argument compatibility
  if (proxy.getInputSample().getSize() != size) throw InvalidArgumentException(HERE) << "Error: the size of the output sample=" << size << " is different from the size of the input sample=" << proxy.getInputSample().getSize();
}

/* Weight accessor */
void LeastSquaresMethodImplementation::setWeight(const NumericalPoint & weight)
{
  const UnsignedInteger size = weight.getSize();
  // First, check for uniformity
  const NumericalScalar w0 = weight[0];
  if (w0 <= 0.0) throw InvalidArgumentException(HERE) << "Error: expected positive weights, here w[0]=" << w0;
  hasUniformWeight_ = true;
  for (UnsignedInteger i = 1; i < size; ++i)
    if (weight[i] != w0)
    {
      hasUniformWeight_ = false;
      break;
    }
  if (hasUniformWeight_)
  {
    weight_ = NumericalPoint(1, w0);
    weightSqrt_ = NumericalPoint(1, std::sqrt(w0));
    return;
  }
  // Second, check for positiveness and compute the square roots
  weight_ = weight;
  weightSqrt_ = NumericalPoint(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (weight[i] <= 0.0) throw InvalidArgumentException(HERE) << "Error: expected positive weights, here w[" << i << "]=" << weight[i];
    weightSqrt_[i] = std::sqrt(weight[i]);
  }
}

NumericalPoint LeastSquaresMethodImplementation::getWeight() const
{
  return weight_;
}

/* Virtual constructor */
LeastSquaresMethodImplementation * LeastSquaresMethodImplementation::clone() const
{
  return new LeastSquaresMethodImplementation(*this);
}

NumericalPoint LeastSquaresMethodImplementation::solve(const NumericalPoint & rhs)
{
  // If uniform weights, no scaling of the rhs
  if (hasUniformWeight_) return solveNormal(computeWeightedDesign().genVectProd(rhs, true));
  NumericalPoint y(rhs);
  const UnsignedInteger size = rhs.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) y[i] *= weightSqrt_[i];
  return solveNormal(computeWeightedDesign().genVectProd(y, true));
}

NumericalPoint LeastSquaresMethodImplementation::solveNormal(const NumericalPoint & rhs)
{
  throw NotYetImplementedException(HERE) << " in LeastSquaresMethodImplementation::solve";
}

CovarianceMatrix LeastSquaresMethodImplementation::getGramInverse() const
{
  throw NotYetImplementedException(HERE) << " in LeastSquaresMethodImplementation::getGramInverse";
}

SymmetricMatrix LeastSquaresMethodImplementation::getH() const
{
  MatrixImplementation psiAk(computeWeightedDesign());
  // H = Psi.G.Psi^T
  return getGramInverse().getImplementation()->symProd(psiAk, 'R').genProd(psiAk, false, true);
}


NumericalPoint LeastSquaresMethodImplementation::getHDiag() const
{
  const SymmetricMatrix H(getH());
  const UnsignedInteger dimension = H.getDimension();
  NumericalPoint diag(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i) diag[i] = H(i, i);

  return diag;
}


NumericalPoint LeastSquaresMethodImplementation::getGramInverseDiag() const
{
  const CovarianceMatrix G(getGramInverse());
  const UnsignedInteger dimension = G.getDimension();
  NumericalPoint diag(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i) diag[i] = G(i, i);

  return diag;
}


NumericalScalar LeastSquaresMethodImplementation::getGramInverseTrace() const
{
  // subclasses better override this
  return getGramInverse().getImplementation()->computeTrace();
}

/* String converter */
String LeastSquaresMethodImplementation::__repr__() const
{
  return OSS() << "class=" << GetClassName()
         << ", proxy=" << proxy_
         << ", outputSample=" << outputSample_
         << ", weight=" << weight_
         << ", weightSqrt=" << weightSqrt_
         << ", hasUniformWeight=" << hasUniformWeight_
         << ", currentIndices=" << currentIndices_
         << ", initialIndices=" << initialIndices_;
}

Basis LeastSquaresMethodImplementation::getBasis() const
{
  return proxy_.getBasis();
}

Indices LeastSquaresMethodImplementation::getCurrentIndices() const
{
  return currentIndices_;
}

Indices LeastSquaresMethodImplementation::getInitialIndices() const
{
  return initialIndices_;
}

NumericalSample LeastSquaresMethodImplementation::getInputSample() const
{
  return proxy_.getInputSample();
}

NumericalSample LeastSquaresMethodImplementation::getOutputSample() const
{
  return outputSample_;
}


MatrixImplementation LeastSquaresMethodImplementation::computeWeightedDesign(const Bool whole) const
{
  if (whole) return computeWeightedDesign(initialIndices_);
  return computeWeightedDesign(currentIndices_);
}

MatrixImplementation LeastSquaresMethodImplementation::computeWeightedDesign(const Indices & indices) const
{
  MatrixImplementation design(proxy_.computeDesign(indices));
  if (hasUniformWeight_) return design;
  // Here we take the weights into account
  UnsignedInteger flatIndex = 0;
  const UnsignedInteger basisDimension = design.getNbColumns();
  const UnsignedInteger sampleSize = design.getNbRows();
  for (UnsignedInteger j = 0; j < basisDimension; ++j)
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      design[flatIndex] *= weightSqrt_[i];
      ++flatIndex;
    }
  return design;
}


Basis LeastSquaresMethodImplementation::buildCurrentBasis() const
{
  const UnsignedInteger currentBasisSize = currentIndices_.getSize();
  const Basis fullBasis(proxy_.getBasis());
  Basis basis(currentBasisSize);
  for (UnsignedInteger j = 0; j < currentBasisSize; ++j)
    basis[j] = fullBasis[currentIndices_[j]];
  return basis;
}

// Update the decomposition and the by-products when the given indices
// are added and removed from the current indices.
// We always must have conservedIndices + removedIndices == currentIndices
void LeastSquaresMethodImplementation::update(const Indices & addedIndices,
    const Indices & conservedIndices,
    const Indices & removedIndices,
    const Bool row)
{
  if ((addedIndices.getSize() > 0) || (removedIndices.getSize() > 0))
  {
    trashDecomposition();
    currentIndices_ = conservedIndices;
    currentIndices_.add(addedIndices);
  }
}


/* Destroy the internal decomposition of the design matrix */
void LeastSquaresMethodImplementation::trashDecomposition()
{
  throw NotYetImplementedException(HERE);
}


/* Method save() stores the object through the StorageManager */
void LeastSquaresMethodImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void LeastSquaresMethodImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
}

END_NAMESPACE_OPENTURNS
