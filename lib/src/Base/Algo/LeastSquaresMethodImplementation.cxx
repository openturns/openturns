//                                               -*- C++ -*-
/**
 *  @brief LeastSquares solving algorithm implementation
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LeastSquaresMethodImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LeastSquaresMethodImplementation)

static const Factory<LeastSquaresMethodImplementation> Factory_LeastSquaresMethodImplementation;

/* Default constructor */
LeastSquaresMethodImplementation::LeastSquaresMethodImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Parameters constructor */
LeastSquaresMethodImplementation::LeastSquaresMethodImplementation(const DesignProxy & proxy,
    const Point & weight,
    const Indices & indices)
  : PersistentObject()
  , proxy_(proxy)
  , weight_(0)
  , weightSqrt_(0)
  , hasUniformWeight_(false)
  , currentIndices_(indices)
  , initialIndices_(indices)
{
  const UnsignedInteger size = proxy.getInputSample().getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: expected a non-empty output sample";
  // Check the argument compatibility
  if (proxy.getSampleSize() != size) throw InvalidArgumentException(HERE) << "Error: the size of the output sample=" << size << " is different from the size of the input sample=" << proxy.getSampleSize();
  if (weight.getSize() != size) throw InvalidArgumentException(HERE) << "Error: the size of the output sample=" << size << " is different from the size of the weight=" << weight.getSize();
  // Check the weight uniformity
  setWeight(weight);
}

/* Parameters constructor */
LeastSquaresMethodImplementation::LeastSquaresMethodImplementation(const DesignProxy & proxy,
    const Indices & indices)
  : PersistentObject()
  , proxy_(proxy)
  , weight_(1, 1.0)
  , weightSqrt_(1, 1.0)
  , hasUniformWeight_(true)
  , currentIndices_(indices)
  , initialIndices_(indices)
{
//   const UnsignedInteger size = proxy.getInputSample().getSize();
//   if (size == 0) throw InvalidArgumentException(HERE) << "Error: expected a non-empty output sample";
}

/* Parameters constructor */
LeastSquaresMethodImplementation::LeastSquaresMethodImplementation(const Matrix & matrix)
  : PersistentObject()
  , proxy_(matrix)
  , weight_(1, 1.0)
  , weightSqrt_(1, 1.0)
  , hasUniformWeight_(true)
  , currentIndices_(0)
  , initialIndices_(0)
{
  // Initialize the indices
  Indices indices(matrix.getNbColumns());
  indices.fill();
  currentIndices_ = indices;
  initialIndices_ = indices;
}

/* Weight accessor */
void LeastSquaresMethodImplementation::setWeight(const Point & weight)
{
  const UnsignedInteger size = weight.getSize();
  // First, check for uniformity
  const Scalar w0 = weight[0];
  if (!(w0 > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected positive weights, here w[0]=" << w0;
  hasUniformWeight_ = true;
  for (UnsignedInteger i = 1; i < size; ++i)
    if (weight[i] != w0)
    {
      hasUniformWeight_ = false;
      break;
    }
  if (hasUniformWeight_)
  {
    weight_ = Point(1, w0);
    weightSqrt_ = Point(1, std::sqrt(w0));
    return;
  }
  // Second, check for positiveness and compute the square roots
  weight_ = weight;
  weightSqrt_ = Point(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (!(weight[i] > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected positive weights, here w[" << i << "]=" << weight[i];
    weightSqrt_[i] = std::sqrt(weight[i]);
  }
}

Point LeastSquaresMethodImplementation::getWeight() const
{
  return weight_;
}

/* Virtual constructor */
LeastSquaresMethodImplementation * LeastSquaresMethodImplementation::clone() const
{
  return new LeastSquaresMethodImplementation(*this);
}

Point LeastSquaresMethodImplementation::solve(const Point & )
{
  throw NotYetImplementedException(HERE) << " in LeastSquaresMethodImplementation::solve";
}

Point LeastSquaresMethodImplementation::solveNormal(const Point & )
{
  throw NotYetImplementedException(HERE) << " in LeastSquaresMethodImplementation::solveNormal";
}

CovarianceMatrix LeastSquaresMethodImplementation::getGramInverse() const
{
  throw NotYetImplementedException(HERE) << " in LeastSquaresMethodImplementation::getGramInverse";
}

SymmetricMatrix LeastSquaresMethodImplementation::getH() const
{
  MatrixImplementation psiAk(computeWeightedDesign());
  // H = Psi.G^{-1}.Psi^T
  return getGramInverse().getImplementation()->symProd(psiAk, 'R').genProd(psiAk, false, true);
}


Point LeastSquaresMethodImplementation::getHDiag() const
{
  const SymmetricMatrix H(getH());
  const UnsignedInteger dimension = H.getDimension();
  Point diag(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i) diag[i] = H(i, i);

  return diag;
}


Point LeastSquaresMethodImplementation::getGramInverseDiag() const
{
  const CovarianceMatrix G(getGramInverse());
  const UnsignedInteger dimension = G.getDimension();
  Point diag(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i) diag[i] = G(i, i);

  return diag;
}


Scalar LeastSquaresMethodImplementation::getGramInverseTrace() const
{
  // subclasses better override this
  return getGramInverse().getImplementation()->computeTrace();
}

/* String converter */
String LeastSquaresMethodImplementation::__repr__() const
{
  return OSS() << "class=" << GetClassName()
         << ", proxy=" << proxy_
         << ", weight=" << weight_
         << ", weightSqrt=" << weightSqrt_
         << ", hasUniformWeight=" << hasUniformWeight_
         << ", currentIndices=" << currentIndices_
         << ", initialIndices=" << initialIndices_;
}

LeastSquaresMethodImplementation::FunctionCollection LeastSquaresMethodImplementation::getBasis() const
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

Sample LeastSquaresMethodImplementation::getInputSample() const
{
  return proxy_.getInputSample();
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


// Update the decomposition and the by-products when the given indices
// are added and removed from the current indices.
// We always must have conservedIndices + removedIndices == currentIndices
void LeastSquaresMethodImplementation::update(const Indices & addedIndices,
    const Indices & conservedIndices,
    const Indices & removedIndices,
    const Bool )
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
