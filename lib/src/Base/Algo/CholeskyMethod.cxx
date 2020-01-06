//                                               -*- C++ -*-
/**
 *  @brief Cholesky decomposition based LS solver
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/CholeskyMethod.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/BasisSequenceFactoryImplementation.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CholeskyMethod)


static const Factory<CholeskyMethod> Factory_CholeskyMethod;

/* Default constructor */
CholeskyMethod::CholeskyMethod()
  : LeastSquaresMethodImplementation()
  , l_(0)
{
  // Nothing to do
}

/* Parameters constructor */
CholeskyMethod::CholeskyMethod(const DesignProxy & proxy,
                               const Point & weight,
                               const Indices & indices)
  : LeastSquaresMethodImplementation(proxy, weight, indices)
  , l_(0)
{
  // Nothing to do
}


/* Parameters constructor */
CholeskyMethod::CholeskyMethod(const DesignProxy & proxy,
                               const Indices & indices)
  : LeastSquaresMethodImplementation(proxy, indices)
  , l_(0)
{
  // Nothing to do
}


/* Parameters constructor */
CholeskyMethod::CholeskyMethod(const Matrix & matrix)
  : LeastSquaresMethodImplementation(matrix)
  , l_(0)
{
  // Nothing to do
}


/* Virtual constructor */
CholeskyMethod * CholeskyMethod::clone() const
{
  return new CholeskyMethod( *this );
}


/* String converter */
String CholeskyMethod::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}

/* Update the decomposition depending on the way the database or the basis have evolved */
void CholeskyMethod::update(const Indices & addedIndices,
                            const Indices & conservedIndices,
                            const Indices & removedIndices,
                            const Bool row)
{
  const UnsignedInteger addedSize = addedIndices.getSize();
  const UnsignedInteger removedSize = removedIndices.getSize();

  // Early exit if nothing has changed
  if (addedSize == 0 && removedSize == 0)
  {
    // initialize decomposition if needed
    if (l_.getNbRows() == 0)
    {
      Indices previousRowFilter(proxy_.getRowFilter());
      proxy_.setRowFilter(Indices(0));
      MatrixImplementation mPsiAk2(computeWeightedDesign()); // current design
      l_ = mPsiAk2.computeGram(true).computeCholesky();
      proxy_.setRowFilter(previousRowFilter);
    }
    else
      return;
  }

  // On row modification
  if (row)
  {
    // Early exit if no computation is reused from the previous call. A more
    // elaborate strategy could be implemented here, comparing the size of the
    // reused information and the size of the new information.
    if (conservedIndices.getSize() == 0)
    {
      proxy_.setRowFilter(addedIndices);
      MatrixImplementation mPsiAk(computeWeightedDesign()); // current design
      l_ = mPsiAk.computeGram(true).computeCholesky();
      return;
    }
    // Here we know that some rows have been preserved, so l_ must have been initialized
    // Compute the full design matrix as we need also the removed rows
    Indices previousRowFilter(proxy_.getRowFilter());
    proxy_.setRowFilter(Indices(0));
    MatrixImplementation mPsiAk(computeWeightedDesign()); // current design
    const UnsignedInteger nbRows = mPsiAk.getNbRows();
    const UnsignedInteger nbColumns = mPsiAk.getNbColumns();
    proxy_.setRowFilter(previousRowFilter);
    Point vector(nbColumns);
    // Start by the added vectors, as it increases the positiveness of
    // the Gram matrix
    for (UnsignedInteger i = 0; i < addedSize; ++i)
    {
      UnsignedInteger shift = addedIndices[i];
      // Extract the row giving the update
      for (UnsignedInteger j = 0; j < nbColumns; ++j)
      {
        const Scalar value = mPsiAk[shift];
        vector[j] = value;
        shift += nbRows;
      }
      MatrixImplementation::CholeskyUpdate(*l_.getImplementation(), vector);
    } // Added rows
    // Then the removed vectors
    for (UnsignedInteger i = 0; i < removedSize; ++i)
    {
      UnsignedInteger shift = removedIndices[i];
      // Extract the row giving the update
      for (UnsignedInteger j = 0; j < nbColumns; ++j)
      {
        vector[j] = mPsiAk[shift];
        shift += nbRows;
      }
      // The downdating process may failed due to bad conditioning or catastrophic propagation of errors
      try
      {
        MatrixImplementation::CholeskyDowndate(*l_.getImplementation(), vector);
      }
      // In case of problems, try a full factorization
      catch (...)
      {
        LOGINFO(OSS() << "In CholeskyMethod::update: failed to downdate row " << removedIndices[i]);
        MatrixImplementation mPsiAk2(computeWeightedDesign()); // current design
        l_ = mPsiAk2.computeGram(true).computeCholesky();
        return;
      }
    } // Added rows
  } // Update on row modification
  // On column modification
  else
  {
    Indices newBasis(conservedIndices);
    newBasis.add(addedIndices);
    if (removedIndices.getSize() > 0) throw NotYetImplementedException(HERE) << " in CholeskyMethod::update removedIndices.getSize() > 0";

    // Update decomposition
    if (addedIndices.getSize() > 0)
    {
      // Do incremental update only if the matrix size is large enough
      static const UnsignedInteger LargeCase = ResourceMap::GetAsUnsignedInteger("CholeskyMethod-LargeCase");
      if (newBasis.getSize() >= LargeCase)
      {
        MatrixImplementation mPsiAk(computeWeightedDesign());// old design

        currentIndices_ = newBasis;
        if (addedIndices.getSize() != 1) throw InvalidArgumentException(HERE) << " in CholeskyMethod::update addedIndices.getSize() != 1";
        const UnsignedInteger basisSize = currentIndices_.getSize();

        // update the cholesky decomposition of the Gram matrix
        const Point xk(computeWeightedDesign(addedIndices));
        const Scalar diagk = xk.normSquare();

        // solve lower triangular system L*rk=xk'*A to get the extra line panel
        const Point colk(mPsiAk.genVectProd(xk, true));
        const Point rk(l_.solveLinearSystem(colk));
        const Scalar rk2 = rk.normSquare();

        // Check if the pivot is positive
        if (diagk > rk2)
        {
          // the extra diagonal term
          const Scalar rkk = sqrt(diagk - rk2);

          // reconstitute the whole decomposition matrix
          MatrixImplementation newL(basisSize, basisSize);
          MatrixImplementation::iterator startNewL(newL.begin());
          const MatrixImplementation & l_impl(*l_.getImplementation());
          MatrixImplementation::const_iterator startL(l_impl.begin());
          UnsignedInteger newShift = 0;
          UnsignedInteger toCopy = basisSize - 1;
          for (UnsignedInteger j = 0; j < basisSize - 1; ++ j)
          {
            std::copy(startL, startL + toCopy, startNewL);
            startL += basisSize;
            startNewL += basisSize + 1;
            --toCopy;
          }
          newShift = basisSize - 1;
          for (UnsignedInteger j = 0; j < basisSize - 1; ++ j)
          {
            newL[newShift] = rk[j];
            newShift += basisSize;
          }
          newL[newShift] = rkk;
          // The MatrixImplementation is automatically converted into a TriangularMatrix of the proper shape
          l_ = newL;
          return;
        } // Positive pivot
        else LOGINFO(OSS() << "In CholeskyMethod::update: failed to update column " << addedIndices[0]);
      } // newBasis.getSize() > LargeCase
      // For small problems or negative pivot, use full factorization
      currentIndices_ = newBasis;
      // Compute the Cholesky factor
      l_ = computeWeightedDesign().computeGram(true).computeCholesky();
    } // New indices
  } // Update on column modification
}

Point CholeskyMethod::solve(const Point & rhs)
{
  // This call insures that the decomposition has already been computed.
  // No cost if it is up to date.
  update(Indices(0), currentIndices_, Indices(0));
  Point b(rhs);
  if (!hasUniformWeight_)
  {
    const UnsignedInteger size = rhs.getSize();
    for (UnsignedInteger i = 0; i < size; ++i) b[i] *= weightSqrt_[i];
  }
  const MatrixImplementation psiAk(computeWeightedDesign());
  const Point c(psiAk.genVectProd(b, true));
  // We first solve Ly=b then L^Tx=y. The flags given to solveLinearSystemTri() are:
  // 1) To keep the matrix intact
  // 2) To say that the matrix L is lower triangular
  // 3) To say that it is L^Tx=y that is solved instead of Lx=y
  return l_.getImplementation()->solveLinearSystemTri(l_.solveLinearSystem(c), true, true, true);
}


Point CholeskyMethod::solveNormal(const Point & rhs)
{
  const UnsignedInteger basisSize = currentIndices_.getSize();

  if (rhs.getDimension() != basisSize) throw InvalidArgumentException(HERE) << "CholeskyMethod::solve invalid rhs!";

  // This call insures that the decomposition has already been computed.
  // No cost if it is up to date.
  update(Indices(0), currentIndices_, Indices(0));

  Point b(rhs);
  if (!hasUniformWeight_)
  {
    const UnsignedInteger size = rhs.getSize();
    for (UnsignedInteger i = 0; i < size; ++i) b[i] *= weight_[i];
  }
  // We first solve Ly=b then L^Tx=y. The flags given to solveLinearSystemTri() are:
  // 1) To keep the matrix intact
  // 2) To say that the matrix L is lower triangular
  // 3) To say that it is L^Tx=y that is solved instead of Lx=y
  return l_.getImplementation()->solveLinearSystemTri(l_.solveLinearSystem(b), true, true, true);
}


CovarianceMatrix CholeskyMethod::getGramInverse() const
{
  const UnsignedInteger basisSize = currentIndices_.getSize();
  TriangularMatrix invL(l_.solveLinearSystem(IdentityMatrix(basisSize)).getImplementation());
  return invL.computeGram(true);
}


SymmetricMatrix CholeskyMethod::getH() const
{
  const UnsignedInteger basisSize = currentIndices_.getSize();
  TriangularMatrix invL(l_.solveLinearSystem(IdentityMatrix(basisSize)).getImplementation());
  MatrixImplementation psiAk(computeWeightedDesign());
  return invL.getImplementation()->genProd(psiAk, false, true).computeGram(true);
}


Point CholeskyMethod::getHDiag() const
{
  const UnsignedInteger basisSize = currentIndices_.getSize();
  const MatrixImplementation invL(*l_.solveLinearSystem(IdentityMatrix(basisSize)).getImplementation());
  const MatrixImplementation psiAk(computeWeightedDesign());
  const MatrixImplementation invLPsiAk(invL.genProd(psiAk, false, true));

  const UnsignedInteger dimension = psiAk.getNbRows();
  Point diag(dimension);
  MatrixImplementation::const_iterator invLPsiAk_iterator(invLPsiAk.begin());
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    Scalar value = 0.0;
    for (UnsignedInteger j = 0; j < basisSize; ++ j)
    {
      value += (*invLPsiAk_iterator) * (*invLPsiAk_iterator);
      ++invLPsiAk_iterator;
    }
    diag[i] = value;
  }

  return diag;
}

Point CholeskyMethod::getGramInverseDiag() const
{
  const UnsignedInteger basisSize = currentIndices_.getSize();
  const MatrixImplementation invL(*l_.solveLinearSystem(IdentityMatrix(basisSize)).getImplementation());
  Point diag(basisSize);
  MatrixImplementation::const_iterator invL_iterator(invL.begin());
  for (UnsignedInteger i = 0; i < basisSize; ++ i)
  {
    Scalar value = 0.0;
    for (UnsignedInteger j = 0; j < basisSize; ++ j)
    {
      value += (*invL_iterator) * (*invL_iterator);
      ++invL_iterator;
    }
    diag[i] = value;
  }

  return diag;
}

Scalar CholeskyMethod::getGramInverseTrace() const
{
  Scalar traceInverse = 0.0;
  const UnsignedInteger basisSize = currentIndices_.getSize();
  const MatrixImplementation invL(*l_.solveLinearSystem(IdentityMatrix(basisSize)).getImplementation());
  for (MatrixImplementation::const_iterator it = invL.begin(); it != invL.end(); ++it)
  {
    traceInverse += (*it) * (*it);
  }
  return traceInverse;
}


void CholeskyMethod::trashDecomposition()
{
  l_ = TriangularMatrix();
}

/* Method save() stores the object through the StorageManager */
void CholeskyMethod::save(Advocate & adv) const
{
  LeastSquaresMethodImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void CholeskyMethod::load(Advocate & adv)
{
  LeastSquaresMethodImplementation::load(adv);
}



END_NAMESPACE_OPENTURNS
