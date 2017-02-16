//                                               -*- C++ -*-
/**
 *  @brief QR-decomposition based LS solver
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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


static const Factory<QRMethod> Factory_QRMethod;

/* Default constructor */
QRMethod::QRMethod()
  : LeastSquaresMethodImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
QRMethod::QRMethod(const DesignProxy & proxy,
                   const NumericalPoint & weight,
                   const Indices & indices)
  : LeastSquaresMethodImplementation(proxy, weight, indices)
{
  // Nothing to do
}

/* Parameters constructor */
QRMethod::QRMethod(const DesignProxy & proxy,
                   const Indices & indices)
  : LeastSquaresMethodImplementation(proxy, indices)
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
      const UnsignedInteger m = psiAk.getNbRows();
      const UnsignedInteger n = psiAk.getNbColumns();
      q_ = psiAk.computeQR(r_, m < n, false);
    } // row modification
    else
    {
      currentIndices_ = conservedIndices;
      currentIndices_.add(addedIndices);
      Matrix psiAk(computeWeightedDesign());
      const UnsignedInteger m = psiAk.getNbRows();
      const UnsignedInteger n = psiAk.getNbColumns();
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
  NumericalPoint b(rhs);
  if (!hasUniformWeight_)
  {
    const UnsignedInteger size = rhs.getSize();
    for (UnsignedInteger i = 0; i < size; ++i) b[i] *= weightSqrt_[i];
  }
  const NumericalPoint c(q_.getImplementation()->genVectProd(b, true));
  const NumericalPoint coefficients(r_.getImplementation()->solveLinearSystemTri(c, true, false, false)); // rhs, keep, lower, transpose
  return coefficients;
}

NumericalPoint QRMethod::solveNormal(const NumericalPoint & rhs)
{
  // G=R^T*R
  // This call insures that the decomposition has already been computed.
  // No cost if it is up to date.
  update(Indices(0), currentIndices_, Indices(0));
  NumericalPoint b(rhs);
  if (!hasUniformWeight_)
  {
    const UnsignedInteger size = rhs.getSize();
    for (UnsignedInteger i = 0; i < size; ++i) b[i] *= weight_[i];
  }
  const NumericalPoint c(r_.getImplementation()->solveLinearSystemTri(b, true, false, true)); // rhs, keep, lower, transpose
  const NumericalPoint coefficients(r_.getImplementation()->solveLinearSystemTri(c, true, false, false)); // rhs, keep, lower, transpose
  return coefficients;
}

NumericalPoint QRMethod::getHDiag() const
{
  const UnsignedInteger dimension = q_.getNbRows();
  const UnsignedInteger basisSize = currentIndices_.getSize();
  NumericalPoint diag(dimension);
  MatrixImplementation::const_iterator q_iterator(q_.getImplementation()->begin());
  for (UnsignedInteger j = 0; j < basisSize; ++ j)
  {
    for (MatrixImplementation::iterator diag_iterator = diag.begin(); diag_iterator != diag.end(); ++ diag_iterator)
    {
      *diag_iterator += (*q_iterator) * (*q_iterator);
      ++ q_iterator;
    }
  }

  return diag;
}


CovarianceMatrix QRMethod::getGramInverse() const
{
  // G^{-1}=R^-1*R*^-T
  const UnsignedInteger basisSize = currentIndices_.getSize();
  const MatrixImplementation b(*IdentityMatrix(basisSize).getImplementation());
  Matrix invR(r_.getImplementation()->solveLinearSystemTri(b, true, false));
  return invR.computeGram(false);
}

NumericalPoint QRMethod::getGramInverseDiag() const
{
  // G^{-1}=R^-1*R*^-T
  const UnsignedInteger dimension = r_.getNbRows();
  const UnsignedInteger basisSize = currentIndices_.getSize();
  const MatrixImplementation b(*IdentityMatrix(dimension).getImplementation());
  const MatrixImplementation invRT(r_.getImplementation()->solveLinearSystemTri(b, true, false, true));

  NumericalPoint diag(dimension);
  MatrixImplementation::const_iterator invRT_iterator(invRT.begin());
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    NumericalScalar value = 0.0;
    for (UnsignedInteger j = 0; j < basisSize; ++ j)
    {
      value += (*invRT_iterator) * (*invRT_iterator);
      ++ invRT_iterator;
    }
    diag[i] = value;
  }
  return diag;
}

NumericalScalar QRMethod::getGramInverseTrace() const
{
  // G^{-1}=R^-1*R*^-T
  const UnsignedInteger dimension = r_.getNbRows();
  const MatrixImplementation b(*IdentityMatrix(dimension).getImplementation());
  const MatrixImplementation invRT(r_.getImplementation()->solveLinearSystemTri(b, true, false, true));

  NumericalScalar traceInverse = 0.0;
  for (MatrixImplementation::const_iterator it = invRT.begin(); it != invRT.end(); ++it)
  {
    traceInverse += (*it) * (*it);
  }
  return traceInverse;
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
