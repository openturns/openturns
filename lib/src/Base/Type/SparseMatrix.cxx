//                                               -*- C++ -*-
/**
 *  @brief SparseMatrix implements Eigen sparse matrix class interface
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/SparseMatrix.hxx"
#ifdef OPENTURNS_HAVE_EIGEN
  #include <Eigen/Core>
  #include <Eigen/SparseCore>
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SparseMatrix)

/* Default constructor */
SparseMatrix::SparseMatrix()
  : PersistentObject()
  , impl_(0)
{
  // Nothing to do
}

/* Constructor with dimensions */
SparseMatrix::SparseMatrix( const UnsignedInteger nbRows,
                            const UnsignedInteger nbCols)
  : PersistentObject()
{
#ifdef OPENTURNS_HAVE_EIGEN
  impl_ = new Eigen::SparseMatrix<Scalar>(nbRows, nbCols);
#else
  impl_ = new Matrix(nbRows, nbCols);
#endif
}

/** Constructor from matrix */
SparseMatrix::SparseMatrix(const Matrix & m)
  : PersistentObject()
{
#ifdef OPENTURNS_HAVE_EIGEN
  impl_ = new Eigen::SparseMatrix<Scalar>(m.getNbRows(), m.getNbColumns());
  for (UnsignedInteger i=0; i<m.getNbRows(); ++i)
    for (UnsignedInteger j=0; j<m.getNbColumns(); ++j)
      if (m(i,j) != 0)
        static_cast<Eigen::SparseMatrix<Scalar> *>(impl_)->coeffRef(i,j) += m(i,j);
#else
  impl_ = new Matrix(m);
#endif
}

/** Constructor from implementation */
SparseMatrix::SparseMatrix(void * impl)
  : PersistentObject()
  , impl_(impl)
{
  // Nothing to do
}


/* Virtual constructor */
SparseMatrix * SparseMatrix::clone() const
{
  return new SparseMatrix(*this);
}

/** Read-only accessor to values */
Scalar SparseMatrix::operator()(const UnsignedInteger i, const UnsignedInteger j) const
{
#ifdef OPENTURNS_HAVE_EIGEN
  return static_cast< Eigen::SparseMatrix<Scalar> *>(impl_)->coeff(i,j);
#else
  return static_cast<Matrix *>(impl_)->operator()(i,j);
#endif
}

/** Filling matrix from coordinates and value */
Scalar & SparseMatrix::operator()(const UnsignedInteger i, const UnsignedInteger j)
{
#ifdef OPENTURNS_HAVE_EIGEN
  return static_cast<Eigen::SparseMatrix<Scalar> *>(impl_)->coeffRef(i,j);
#else
  return static_cast<Matrix *>(impl_)->operator()(i,j);
#endif
}

/** Multiplication by a vector */
Point SparseMatrix::operator *(const Point & rhs) const
{
#ifdef OPENTURNS_HAVE_EIGEN  
  Eigen::VectorXd v(rhs.getSize());
  std::copy(rhs.begin(), rhs.end(), v.data());
  
  Eigen::VectorXd w( *(static_cast<Eigen::SparseMatrix<Scalar> *>(impl_)) * v);
  
  Point output(w.size());
  std::copy(w.data(), w.data() + w.size(), output.begin());
  
  return output;
#else
  return static_cast<Matrix *>(impl_)->operator*(rhs);
#endif
}

/** Get the dimensions of the matrix */
/** Number of rows */
UnsignedInteger SparseMatrix::getNbRows() const
{
#ifdef OPENTURNS_HAVE_EIGEN  
  return static_cast<Eigen::SparseMatrix<Scalar> *>(impl_)->rows();
#else
  return static_cast<Matrix *>(impl_)->getNbRows();
#endif  
}

/** Number of columns */
UnsignedInteger SparseMatrix::getNbColumns() const
{
#ifdef OPENTURNS_HAVE_EIGEN  
  return static_cast<Eigen::SparseMatrix<Scalar> *>(impl_)->cols();
#else
  return static_cast<Matrix *>(impl_)->getNbColumns();
#endif
}

/** Get the number of non-zeros elements */
UnsignedInteger SparseMatrix::getNbNonZeros() const
{
#ifdef OPENTURNS_HAVE_EIGEN  
  return static_cast<Eigen::SparseMatrix<Scalar> *>(impl_)->nonZeros();
#else
  Matrix m(* static_cast<Matrix *>(impl_));
  return m.getNbColumns() * m.getNbRows();
#endif
}

/** Transposition */
SparseMatrix SparseMatrix::transpose() const
{
#ifdef OPENTURNS_HAVE_EIGEN  
  // Casting impl_ to actual implementation of Eigen::SparseMatrix
  Eigen::SparseMatrix<Scalar> sparseMat(*static_cast<Eigen::SparseMatrix<Scalar> *>(impl_));
  
  // Create OT::Matrix with relevant dimensions
  SparseMatrix output(sparseMat.rows(), sparseMat.cols());
  
  // Iterate on non-zeros elements of Eigen::SparseMatrix, populate OT::Matrix
  for (int k=0; k<sparseMat.outerSize(); ++k)
    for (Eigen::SparseMatrix<double>::InnerIterator it(sparseMat,k); it; ++it)
      output(it.col(), it.row()) += it.value();
    
  return output;
#else
  return new SparseMatrix(static_cast<Matrix *>(impl_)->transpose());
#endif  
}
    
/** Sparse / dense conversions */
Matrix SparseMatrix::asDenseMatrix() const
{
#ifdef OPENTURNS_HAVE_EIGEN
  // Casting impl_ to actual implementation of Eigen::SparseMatrix
  Eigen::SparseMatrix<Scalar> sparseMat(*static_cast<Eigen::SparseMatrix<Scalar> *>(impl_));
  
  // Create OT::Matrix with relevant dimensions
  Matrix output(sparseMat.rows(), sparseMat.cols());
  
  // Iterate on non-zeros elements of Eigen::SparseMatrix, populate OT::Matrix
  for (int k=0; k<sparseMat.outerSize(); ++k)
    for (Eigen::SparseMatrix<double>::InnerIterator it(sparseMat,k); it; ++it)
    {
      UnsignedInteger i(it.row());
      UnsignedInteger j(it.col());
      output(i,j) = it.value();
    }

  return output;
#else
  return Matrix(* static_cast<Matrix *>(impl_));
#endif
  
}

END_NAMESPACE_OPENTURNS
