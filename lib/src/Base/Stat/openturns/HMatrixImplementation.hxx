//                                               -*- C++ -*-
/**
 *  @file  HMatrixImplementation.hxx
 *  @brief This file supplies support for HMat
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
#ifndef OPENTURNS_HMATRIXIMPLEMENTATION_HXX
#define OPENTURNS_HMATRIXIMPLEMENTATION_HXX

#include "openturns/OTconfig.hxx"
#include "openturns/OTprivate.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/CovarianceModel.hxx"

#ifdef OPENTURNS_HAVE_HMAT
# include <hmat/hmat.h>
#endif

BEGIN_NAMESPACE_OPENTURNS

// Forward declaration
class HMatrixFactory;

// In order to create an H-matrix, one has first to generate an interface with
// HMatrixFactory::build, then compute its coefficients, and after that, it is
// possible to factorize it, solve linear systems, etc.  As H-matrices are
// designed to solve very large systems, most operations are performed inplace,
// which is an unusual pattern in OpenTURNS.

// There are two ways to compute matrix coefficients:
//   1. The simplest solution is to provide a method which computes (i,j) coefficient.
//      This is what HMatrixRealAssemblyFunction is designed for, and such an object
//      is then passed to HMatrixImplementation::simpleAssemble.  But most of the time,
//      this involves heavy computations which could be reused when computing values
//      for different coefficients.
//   2. HMat provides a 2nd interface to compute tiled blocks.  But it is cumbersome,
//      so we defined a more intuitive interface which should be suited for our needs.
//      When dimension is greater than 1, HMatrixFactory::build duplicates input vertices
//      to have local d x d matrices, and HMatrixTensorRealAssemblyFunction is an interface
//      to compute all coefficients of this local matrix, which had already been allocated.
//      It will be automatically copied at the right place in HMatrixImplementation.

class OT_API HMatrixRealAssemblyFunction
{
public:
  virtual ~HMatrixRealAssemblyFunction() {}

  // Compute matrix coefficient for degrees of freedom i and j
  virtual Scalar operator() (UnsignedInteger i, UnsignedInteger j) const = 0;
};

class OT_API HMatrixTensorRealAssemblyFunction
{
public:
  virtual ~HMatrixTensorRealAssemblyFunction() {}

  HMatrixTensorRealAssemblyFunction(const UnsignedInteger outputDimension)
    : dimension_(outputDimension) {}

  UnsignedInteger getDimension() const
  {
    return dimension_;
  }
  // Compute local matrix for input vertices i and j
  virtual void compute(UnsignedInteger i, UnsignedInteger j, Matrix* localValues) const = 0;

protected:
  UnsignedInteger dimension_;
};

class OT_API HMatrixClusterTree
{
public:
  HMatrixClusterTree(void* ptr_cluster_tree, int size) : hmatClusterTree_(ptr_cluster_tree), size_(size) {}
  void* get()
  {
    return hmatClusterTree_;
  }
  int getSize()
  {
    return size_;
  }
  ~HMatrixClusterTree();
private:
  void* hmatClusterTree_;
  int size_;
};

// HMatrixImplementation is not persistent
class OT_API HMatrixImplementation
  : public PersistentObject
{
  friend class HMatrixFactory;

  CLASSNAME

private:
  // Only visible from HMatrixFactory::build
  HMatrixImplementation(void* ptr_hmat_interface, void* ptr_cluster_tree, int cluster_size, void* ptr_hmatrix);

public:
  /** Default constructor */
  HMatrixImplementation();

  /** Copy constructor */
  HMatrixImplementation(const HMatrixImplementation& other);

  /** Virtual copy constructor */
  virtual HMatrixImplementation * clone() const;

  // Destructor
  virtual ~HMatrixImplementation();

  /** Get the dimensions of the matrix */
  /** Number of rows */
  UnsignedInteger getNbRows() const;
  /** Number of columns */
  UnsignedInteger getNbColumns() const;

  void assemble(const HMatrixRealAssemblyFunction& f, char symmetry);
  void assemble(const HMatrixTensorRealAssemblyFunction& f, char symmetry);
  void factorize(const String& method);

  /** Compute this <- alpha * this */
  void scale(Scalar alpha);

  /** Compute y <- alpha op(this) * x + beta * y */
  void gemv(char trans, Scalar alpha, const Point& x, Scalar beta, Point& y) const;

  /** Compute this <- alpha op(A) * p(B) + beta * this */
  void gemm(char transA, char transB, Scalar alpha, const HMatrixImplementation& a, const HMatrixImplementation& b, Scalar beta);

  /** Transpose matrix */
  void transpose();

  /** Get the Frobenius norm */
  Scalar norm() const;

  /** Get the diagonal */
  Point getDiagonal() const;

  /** Solve system op(A)*X = b */
  Point solve(const Point& b, Bool trans) const;

  /** Solve system op(A)*X = m */
  Matrix solve(const Matrix& m, Bool trans) const;

  /** Solve system op(L)*X = b */
  Point solveLower(const Point& b, Bool trans) const;

  /** Solve system op(L)*X = m */
  Matrix solveLower(const Matrix& m, Bool trans) const;

  /** Get number of HMatrix elements in compressed and uncompressed forms */
  std::pair<size_t, size_t> compressionRatio() const;

  /** Get number of HMatrix elements in full blocks and low rank blocks */
  std::pair<size_t, size_t> fullrkRatio() const;

  /** Dump HMatrix onto file */
  void dump(const String & name) const;

  /** Change HMatrix settings */
  Bool setKey(const String & name, const String & value);

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

private:
  // DO NOT USE
  void * hmatInterface_;
  Pointer<HMatrixClusterTree> hmatClusterTree_;
  void * hmat_;

};

// First implementation, by using HMatrixRealAssemblyFunction
// This is going to be very slow, because each local
// covariance matrix of size 3x3 is computed, but a single
// coefficient is stored.
class CovarianceAssemblyFunction : public HMatrixRealAssemblyFunction
{
private:
  const CovarianceModel covarianceModel_;
  const Sample vertices_;
  const UnsignedInteger covarianceDimension_;
  const double epsilon_;

public:
  CovarianceAssemblyFunction(const CovarianceModel & covarianceModel, const Sample & vertices, double epsilon)
    : HMatrixRealAssemblyFunction()
    , covarianceModel_(covarianceModel)
    , vertices_(vertices)
    , covarianceDimension_(covarianceModel.getOutputDimension())
    , epsilon_(epsilon) {}

  Scalar operator()(UnsignedInteger i, UnsignedInteger j) const
  {
    const UnsignedInteger rowIndex = i / covarianceDimension_;
    const UnsignedInteger columnIndex = j / covarianceDimension_;
    const CovarianceMatrix localCovarianceMatrix(covarianceModel_( vertices_[rowIndex],  vertices_[columnIndex] ));
    const UnsignedInteger rowIndexLocal = i % covarianceDimension_;
    const UnsignedInteger columnIndexLocal = j % covarianceDimension_;
    return localCovarianceMatrix(rowIndexLocal, columnIndexLocal) + (i != j ? 0.0 : epsilon_);
  }
};

// Second implementation, by using HMatrixTensorRealAssemblyFunction
// Each local covariance matrix is built only once, and its components
// are dispatched into the global covariance matrix
class CovarianceBlockAssemblyFunction : public HMatrixTensorRealAssemblyFunction
{
private:
  const CovarianceModel covarianceModel_;
  const Sample vertices_;
  const double epsilon_;
  CovarianceMatrix epsilonId_;

public:
  CovarianceBlockAssemblyFunction(const CovarianceModel & covarianceModel, const Sample & vertices, double epsilon)
    : HMatrixTensorRealAssemblyFunction(covarianceModel.getOutputDimension())
    , covarianceModel_(covarianceModel)
    , vertices_(vertices)
    , epsilon_(epsilon)
  {
    Matrix eps = epsilon_ * IdentityMatrix(covarianceModel.getOutputDimension());
    Pointer<MatrixImplementation> impl = eps.getImplementation();
    epsilonId_ = CovarianceMatrix(covarianceModel.getOutputDimension(), *impl.get());
  }

  void compute(UnsignedInteger i, UnsignedInteger j, Matrix* localValues) const
  {
    CovarianceMatrix localResult(covarianceModel_( vertices_[i],  vertices_[j] ));
    if (i == j && epsilon_ != 0.0)
      localResult = localResult + epsilonId_;
    memcpy( &localValues->getImplementation()->operator[](0), &localResult.getImplementation()->operator[](0), dimension_ * dimension_ * sizeof(Scalar) );
  }
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HMATRIXIMPLEMENTATION_HXX */
