//                                               -*- C++ -*-
/**
 *  @brief QR-decomposition based LS solver
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

#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/QRMethod.hxx"
#include "openturns/TriangularMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(QRMethod);


static const Factory<QRMethod> RegisteredFactory;

/* Default constructor */
QRMethod::QRMethod()
  : LeastSquaresMethodImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
QRMethod::QRMethod(const DesignProxy & proxy,
                   const NumericalSample & outputSample,
                   const NumericalPoint & weight,
                   const Indices & indices)
  : LeastSquaresMethodImplementation(proxy, outputSample, weight, indices)
{
  // Nothing to do
}

/* Parameters constructor */
QRMethod::QRMethod(const DesignProxy & proxy,
                   const NumericalSample & outputSample,
                   const Indices & indices)
  : LeastSquaresMethodImplementation(proxy, outputSample, indices)
{
  // Nothing to do
}


/* Virtual constructor */
QRMethod * QRMethod::clone() const
{
  return new QRMethod( *this );
}


/* String converter */
String QRMethod::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}

void QRMethod::update(const Indices & addedIndices,
                      const Indices & conservedIndices,
                      const Indices & removedIndices,
                      const Bool row)
{
  // Update the SVD according to the row/column modifications of the design matrix.
  // It should be done in an incremental way to improve speed
  // If something has changed since last call
  if ((addedIndices.getSize() > 0) || (removedIndices.getSize() > 0) || (conservedIndices != currentIndices_) || (q_.getNbColumns() == 0))
  {
    // If it is a row modification, update the row filter
    if (row)
    {
      Matrix psiAk(computeWeightedDesign());
      const UnsignedInteger m(psiAk.getNbRows());
      const UnsignedInteger n(psiAk.getNbColumns());
      q_ = psiAk.computeQR(r_, m < n, false);
    } // row modification
    else
    {
      currentIndices_ = conservedIndices;
      currentIndices_.add(addedIndices);
      Matrix psiAk(computeWeightedDesign());
      const UnsignedInteger m(psiAk.getNbRows());
      const UnsignedInteger n(psiAk.getNbColumns());
      q_ = psiAk.computeQR(r_, m < n, false);
    } // column modification
  } // Something has changed
}


/* Solve least-squares problem */
NumericalPoint QRMethod::solve(const NumericalPoint & rhs)
{
  // This call insures that the decomposition has already been computed.
  // No cost if it is up to date.
  update(Indices(0), currentIndices_, Indices(0));
  NumericalPoint b;
  if (hasUniformWeight_) b = q_.getImplementation()->genVectProd(rhs, true);
  else
  {
    NumericalPoint y(rhs);
    const UnsignedInteger size = rhs.getSize();
    for (UnsignedInteger i = 0; i < size; ++i) y[i] *= weightSqrt_[i];
    b = q_.getImplementation()->genVectProd(y, true);
  }
  const NumericalPoint coefficients(r_.getImplementation()->solveLinearSystemTri(b, true, true, false));
  return coefficients;
}

NumericalPoint QRMethod::solveNormal(const NumericalPoint & rhs)
{
  // G=R^T*R
  // This call insures that the decomposition has already been computed.
  // No cost if it is up to date.
  update(Indices(0), currentIndices_, Indices(0));
  NumericalPoint b(r_.getImplementation()->solveLinearSystemTri(rhs, true, true, true));
  const NumericalPoint coefficients(r_.getImplementation()->solveLinearSystemTri(b, true, true, false));
  return coefficients;
}

CovarianceMatrix QRMethod::getGramInverse() const
{
  // G^{-1}=R^-1*R*^-T
  const UnsignedInteger basisSize = currentIndices_.getSize();
  const MatrixImplementation b(*IdentityMatrix(basisSize).getImplementation());
  Matrix invR(r_.getImplementation()->solveLinearSystemTri(b));
  return invR.computeGram(false);
}

void QRMethod::trashDecomposition()
{
  q_ = Matrix();
  r_ = Matrix();
}


/* Method save() stores the object through the StorageManager */
void QRMethod::save(Advocate & adv) const
{
  LeastSquaresMethodImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void QRMethod::load(Advocate & adv)
{
  LeastSquaresMethodImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
