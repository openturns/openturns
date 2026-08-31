//                                               -*- C++ -*-
/**
 *  @brief Tests for DataContainer, AlgebraEngine, StatisticsEngine
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <iostream>
#include <cmath>
#include <cassert>

#include "openturns/DataContainer.hxx"
#include "openturns/AlgebraEngine.hxx"
#include "openturns/StatisticsEngine.hxx"
#ifdef OPENTURNS_HAVE_HDF5
#include "openturns/HDF5Backend.hxx"
#include "openturns/ComplexHDF5Backend.hxx"
#endif

using namespace OT;

int main()
{
  std::cout << "=== DataContainer Tests ===" << std::endl;

  // Test default constructor
  DataContainer dc;
  assert(dc.isEmpty());
  assert(dc.getSize() == 0);
  assert(dc.getDimension() == 0);
  std::cout << "  Default constructor: OK" << std::endl;

  // Test 1D constructor
  DataContainer dc1d(5, 3.0);
  assert(dc1d.getSize() == 5);
  assert(dc1d.getDimension() == 1);
  assert(dc1d[0] == 3.0);
  assert(dc1d[4] == 3.0);
  std::cout << "  1D constructor: OK" << std::endl;

  // Test 2D constructor (column-major)
  DataContainer dc2d(3, 2, 0.0, DataContainer::COLUMN_MAJOR);
  assert(dc2d.getSize() == 3);
  assert(dc2d.getDimension() == 2);
  dc2d(0, 0) = 1.0;
  dc2d(1, 0) = 2.0;
  dc2d(2, 0) = 3.0;
  dc2d(0, 1) = 4.0;
  dc2d(1, 1) = 5.0;
  dc2d(2, 1) = 6.0;
  assert(dc2d(0, 0) == 1.0);
  assert(dc2d(2, 1) == 6.0);
  std::cout << "  2D constructor (column-major): OK" << std::endl;

  // Test 2D constructor (row-major)
  DataContainer dc2d_rm(3, 2, 0.0, DataContainer::ROW_MAJOR);
  dc2d_rm(0, 0) = 1.0;
  dc2d_rm(0, 1) = 2.0;
  dc2d_rm(1, 0) = 3.0;
  dc2d_rm(1, 1) = 4.0;
  dc2d_rm(2, 0) = 5.0;
  dc2d_rm(2, 1) = 6.0;
  // Row-major: element (i,j) is at flat index j + i*dimension
  assert(dc2d_rm[0] == 1.0);
  assert(dc2d_rm[1] == 2.0);
  assert(dc2d_rm[2] == 3.0);
  assert(dc2d_rm[3] == 4.0);
  assert(dc2d_rm[4] == 5.0);
  assert(dc2d_rm[5] == 6.0);
  std::cout << "  2D constructor (row-major): OK" << std::endl;

  // Test copy constructor
  DataContainer dc_copy(dc2d);
  assert(dc_copy == dc2d);
  dc_copy(0, 0) = 99.0;
  assert(dc2d(0, 0) == 1.0); // original unchanged
  std::cout << "  Copy constructor: OK" << std::endl;

  // Test stride
  assert(dc2d.stride(0) == 1);  // column-major: stride along rows = 1
  assert(dc2d.stride(1) == 3);  // column-major: stride along cols = size
  assert(dc2d_rm.stride(0) == 2); // row-major: stride along rows = dimension
  assert(dc2d_rm.stride(1) == 1); // row-major: stride along cols = 1
  std::cout << "  Stride: OK" << std::endl;

  // Test ViewBackend: non-owning view over external memory
  std::cout << "\n=== ViewBackend Tests ===" << std::endl;
  {
    // External buffer
    std::vector<Scalar> extBuf = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    DataContainer view(extBuf.data(), 3, 2, DataContainer::ROW_MAJOR);
    assert(view.isView());
    assert(view.getSize() == 3);
    assert(view.getDimension() == 2);
    assert(view(0, 0) == 1.0);
    assert(view(1, 1) == 4.0);
    assert(view(2, 1) == 6.0);

    // Mutation through view changes external memory
    view(0, 0) = 99.0;
    assert(extBuf[0] == 99.0);
    extBuf[0] = 1.0; // restore

    // Non-view is NOT a view
    DataContainer owned(3, 2, 0.0);
    assert(!owned.isView());
    std::cout << "  ViewBackend basic: OK" << std::endl;

    // Clone of view materializes into owned container
    std::unique_ptr<DataContainer> clonedPtr(view.clone());
    assert(!clonedPtr->isView());
    assert(*clonedPtr == view);
    (*clonedPtr)(0, 0) = 0.0;
    assert(view(0, 0) == 1.0); // original unchanged
    std::cout << "  ViewBackend clone materializes: OK" << std::endl;

    // subView
    DataContainer full(6, 2, 0.0, DataContainer::ROW_MAJOR);
    for (UnsignedInteger i = 0; i < 6; ++i)
      for (UnsignedInteger j = 0; j < 2; ++j)
        full(i, j) = static_cast<Scalar>(i * 2 + j);
    DataContainer sub = full.subView(2, 3);
    assert(sub.isView());
    assert(sub.getSize() == 3);
    assert(sub.getDimension() == 2);
    assert(sub(0, 0) == 4.0); // row 2 of full
    assert(sub(1, 1) == 7.0); // row 3 of full
    assert(sub(2, 1) == 11.0); // row 5 of full

    // Mutation through subView changes parent
    sub(0, 0) = 999.0;
    assert(full(2, 0) == 999.0);
    full(2, 0) = 4.0; // restore
    std::cout << "  ViewBackend subView: OK" << std::endl;

    // isView() correctly identifies owned vs view
    Pointer<DataContainer> clonedView = owned.clone();
    assert(!clonedView->isView()); // cloned view is owned
    std::cout << "  ViewBackend isView: OK" << std::endl;

    // makeUnique ensures exclusive ownership
    DataContainer shared1(3, 0.0);
    for (UnsignedInteger i = 0; i < 3; ++i) shared1[i] = static_cast<Scalar>(i);
    DataContainer shared2 = shared1;
    shared1.makeUnique();
    shared1[0] = 99.0;
    assert(shared2[0] == 0.0); // detaches before mutation
    assert(shared1[0] == 99.0);
    assert(shared1[1] == 1.0);
    std::cout << "  makeUnique: OK" << std::endl;

    // name/lifecycle accessors
    shared1.setName("named");
    assert(shared1.getName() == "named");
    assert(shared1.hasName());
    assert(shared1.getClassName() == "DataContainer");
    std::cout << "  name/lifecycle accessors: OK" << std::endl;
  }

  std::cout << "\n=== AlgebraEngine Tests ===" << std::endl;

  // Test Dot
  DataContainer a(3, 0.0);
  DataContainer b(3, 0.0);
  a[0] = 1.0; a[1] = 2.0; a[2] = 3.0;
  b[0] = 4.0; b[1] = 5.0; b[2] = 6.0;
  Scalar dotResult = AlgebraEngine::Dot(a, b);
  assert(std::abs(dotResult - 32.0) < 1e-12); // 1*4 + 2*5 + 3*6 = 32
  (void)dotResult;
  std::cout << "  Dot: OK" << std::endl;

  // Test Norm
  DataContainer v(3, 0.0);
  v[0] = 3.0; v[1] = 4.0; v[2] = 0.0;
  Scalar normResult = AlgebraEngine::Norm(v);
  assert(std::abs(normResult - 5.0) < 1e-12);
  (void)normResult;
  std::cout << "  Norm: OK" << std::endl;

  // Test MatrixProduct
  DataContainer A(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  A(0, 0) = 1.0; A(0, 1) = 2.0;
  A(1, 0) = 3.0; A(1, 1) = 4.0;
  DataContainer B(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  B(0, 0) = 5.0; B(0, 1) = 6.0;
  B(1, 0) = 7.0; B(1, 1) = 8.0;
  DataContainer C = AlgebraEngine::MatrixProduct(A, B);
  assert(std::abs(C(0, 0) - 19.0) < 1e-12); // 1*5 + 2*7
  assert(std::abs(C(0, 1) - 22.0) < 1e-12); // 1*6 + 2*8
  assert(std::abs(C(1, 0) - 43.0) < 1e-12); // 3*5 + 4*7
  assert(std::abs(C(1, 1) - 50.0) < 1e-12); // 3*6 + 4*8
  std::cout << "  MatrixProduct: OK" << std::endl;

  // Test Cholesky
  DataContainer S(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  S(0, 0) = 4.0; S(0, 1) = 2.0;
  S(1, 0) = 2.0; S(1, 1) = 5.0;
  DataContainer L = AlgebraEngine::ComputeCholesky(S);
  assert(std::abs(L(0, 0) - 2.0) < 1e-12);
  assert(std::abs(L(1, 0) - 1.0) < 1e-12);
  assert(std::abs(L(0, 1) - 0.0) < 1e-12);
  assert(std::abs(L(1, 1) - 2.0) < 1e-12);
  std::cout << "  Cholesky: OK" << std::endl;

  // Test Transpose
  DataContainer M(2, 3, 0.0, DataContainer::COLUMN_MAJOR);
  M(0, 0) = 1.0; M(0, 1) = 2.0; M(0, 2) = 3.0;
  M(1, 0) = 4.0; M(1, 1) = 5.0; M(1, 2) = 6.0;
  DataContainer Mt = AlgebraEngine::Transpose(M);
  assert(Mt.getSize() == 3);
  assert(Mt.getDimension() == 2);
  assert(std::abs(Mt(0, 0) - 1.0) < 1e-12);
  assert(std::abs(Mt(1, 0) - 2.0) < 1e-12);
  assert(std::abs(Mt(0, 1) - 4.0) < 1e-12);
  assert(std::abs(Mt(2, 1) - 6.0) < 1e-12);
  std::cout << "  Transpose: OK" << std::endl;

  // Test Determinant
  DataContainer D(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  D(0, 0) = 1.0; D(0, 1) = 2.0;
  D(1, 0) = 3.0; D(1, 1) = 4.0;
  Scalar det = AlgebraEngine::ComputeDeterminant(D);
  assert(std::abs(det - (-2.0)) < 1e-12);
  (void)det;
  std::cout << "  Determinant: OK" << std::endl;

  // Test SolveLinearSystem
  DataContainer AA(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  AA(0, 0) = 2.0; AA(0, 1) = 1.0;
  AA(1, 0) = 5.0; AA(1, 1) = 7.0;
  DataContainer rhs(2, 1, 0.0, DataContainer::COLUMN_MAJOR);
  rhs[0] = 11.0; rhs[1] = 14.0;
  DataContainer x = AlgebraEngine::SolveLinearSystem(AA, rhs);
  assert(std::abs(x[0] - 7.0) < 1e-10);   // x = [7, -3]
  assert(std::abs(x[1] - (-3.0)) < 1e-10);
  std::cout << "  SolveLinearSystem: OK" << std::endl;

  // Test block matrix product
  DataContainer Cblock = AlgebraEngine::MatrixProductBlockwise(A, B, 1);
  assert(std::abs(Cblock(0, 0) - 19.0) < 1e-12);
  assert(std::abs(Cblock(0, 1) - 22.0) < 1e-12);
  assert(std::abs(Cblock(1, 0) - 43.0) < 1e-12);
  assert(std::abs(Cblock(1, 1) - 50.0) < 1e-12);
  std::cout << "  MatrixProductBlockwise: OK" << std::endl;

  // Test block Cholesky
  DataContainer Lblock = AlgebraEngine::ComputeCholeskyBlockwise(S, 1);
  assert(std::abs(Lblock(0, 0) - 2.0) < 1e-12);
  assert(std::abs(Lblock(1, 0) - 1.0) < 1e-12);
  assert(std::abs(Lblock(0, 1) - 0.0) < 1e-12);
  assert(std::abs(Lblock(1, 1) - 2.0) < 1e-12);
  std::cout << "  ComputeCholeskyBlockwise: OK" << std::endl;

  // Test block solve (SPD): S * x = [11, 13]
  DataContainer rhsBlock(2, 1, 0.0, DataContainer::COLUMN_MAJOR);
  rhsBlock(0, 0) = 11.0;
  rhsBlock(1, 0) = 13.0;
  DataContainer xBlock = AlgebraEngine::SolveLinearSystemBlockwise(S, rhsBlock, 1);
  // S = [[4,2],[2,5]], S^-1 = (1/16)*[[5,-2],[-2,4]]
  // x0 = (5*11 - 2*13)/16 = 29/16
  // x1 = (-2*11 + 4*13)/16 = 30/16
  assert(std::abs(xBlock(0, 0) - 29.0/16.0) < 1e-10);
  assert(std::abs(xBlock(1, 0) - 30.0/16.0) < 1e-10);
  std::cout << "  SolveLinearSystemBlockwise: OK" << std::endl;

  // === New block algebra tests ===
  std::cout << "\n=== Additional Block Algebra Tests ===" << std::endl;

  // Test block QR
  DataContainer Qb, Rb;
  AlgebraEngine::ComputeQRBlockwise(A, Qb, Rb, false, 1);
  // A = Q * R
  DataContainer Aqr = AlgebraEngine::MatrixProduct(Qb, Rb);
  for (UnsignedInteger i = 0; i < 2; ++i)
    for (UnsignedInteger j = 0; j < 2; ++j)
      assert(std::abs(Aqr(i, j) - A(i, j)) < 1e-12);
  // Q^T * Q = I
  DataContainer QtQ = AlgebraEngine::MatrixProduct(AlgebraEngine::Transpose(Qb), Qb);
  for (UnsignedInteger i = 0; i < 2; ++i)
    for (UnsignedInteger j = 0; j < 2; ++j)
      assert(std::abs(QtQ(i, j) - (i == j ? 1.0 : 0.0)) < 1e-12);
  std::cout << "  ComputeQRBlockwise: OK" << std::endl;

  // Test block QR on a larger matrix
  DataContainer A34(3, 4, 0.0, DataContainer::COLUMN_MAJOR);
  A34(0, 0) = 1.0; A34(1, 0) = 2.0; A34(2, 0) = 3.0;
  A34(0, 1) = 4.0; A34(1, 1) = 5.0; A34(2, 1) = 6.0;
  A34(0, 2) = 7.0; A34(1, 2) = 8.0; A34(2, 2) = 9.0;
  A34(0, 3) = 10.0; A34(1, 3) = 11.0; A34(2, 3) = 12.0;
  DataContainer Q34, R34;
  AlgebraEngine::ComputeQRBlockwise(A34, Q34, R34, false, 2);
  DataContainer A34check = AlgebraEngine::MatrixProduct(Q34, R34);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 4; ++j)
      assert(std::abs(A34check(i, j) - A34(i, j)) < 1e-11);
  std::cout << "  ComputeQRBlockwise (3x4): OK" << std::endl;

  // Test block LU
  DataContainer Pb, Lb, Ub;
  AlgebraEngine::ComputeLUBlockwise(A, Pb, Lb, Ub, 1);
  DataContainer PA = AlgebraEngine::MatrixProduct(Pb, A);
  DataContainer Alu = AlgebraEngine::MatrixProduct(Lb, Ub);
  for (UnsignedInteger i = 0; i < 2; ++i)
    for (UnsignedInteger j = 0; j < 2; ++j)
      assert(std::abs(Alu(i, j) - PA(i, j)) < 1e-12);
  std::cout << "  ComputeLUBlockwise: OK" << std::endl;

  // Test block LU on larger non-symmetric matrix
  DataContainer A33(3, 3, 0.0, DataContainer::COLUMN_MAJOR);
  A33(0, 0) = 2.0; A33(1, 0) = 1.0; A33(2, 0) = 3.0;
  A33(0, 1) = 4.0; A33(1, 1) = 5.0; A33(2, 1) = 6.0;
  A33(0, 2) = 1.0; A33(1, 2) = 0.0; A33(2, 2) = 3.0;
  DataContainer P33, L33, U33;
  AlgebraEngine::ComputeLUBlockwise(A33, P33, L33, U33, 1);
  DataContainer PA33 = AlgebraEngine::MatrixProduct(P33, A33);
  DataContainer A33check = AlgebraEngine::MatrixProduct(L33, U33);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      assert(std::abs(A33check(i, j) - PA33(i, j)) < 1e-11);
  std::cout << "  ComputeLUBlockwise (3x3): OK" << std::endl;

  // Test block triangular solve (lower)
  DataContainer xTri = AlgebraEngine::SolveLinearSystemTriangularBlockwise(Lb, rhsBlock, true, false, 1);
  // L * x = rhsBlock
  DataContainer lhsTri = AlgebraEngine::MatrixProduct(Lb, xTri);
  for (UnsignedInteger i = 0; i < 2; ++i)
    assert(std::abs(lhsTri(i, 0) - rhsBlock(i, 0)) < 1e-10);
  std::cout << "  SolveLinearSystemTriangularBlockwise (lower): OK" << std::endl;

  // Test block triangular solve (upper)
  DataContainer xTriU = AlgebraEngine::SolveLinearSystemTriangularBlockwise(Ub, rhsBlock, false, false, 1);
  DataContainer lhsTriU = AlgebraEngine::MatrixProduct(Ub, xTriU);
  for (UnsignedInteger i = 0; i < 2; ++i)
    assert(std::abs(lhsTriU(i, 0) - rhsBlock(i, 0)) < 1e-10);
  std::cout << "  SolveLinearSystemTriangularBlockwise (upper): OK" << std::endl;

  // Test block Gram (A^T * A)
  DataContainer Gb = AlgebraEngine::ComputeGramBlockwise(A34, true, 2);
  DataContainer Gref = AlgebraEngine::ComputeGram(A34, true);
  for (UnsignedInteger i = 0; i < 4; ++i)
    for (UnsignedInteger j = 0; j < 4; ++j)
      assert(std::abs(Gb(i, j) - Gref(i, j)) < 1e-10);
  std::cout << "  ComputeGramBlockwise (transpose=true): OK" << std::endl;

  // Test block Gram (A * A^T)
  DataContainer Gb2 = AlgebraEngine::ComputeGramBlockwise(A34, false, 2);
  DataContainer Gref2 = AlgebraEngine::ComputeGram(A34, false);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      assert(std::abs(Gb2(i, j) - Gref2(i, j)) < 1e-10);
  std::cout << "  ComputeGramBlockwise (transpose=false): OK" << std::endl;

  // === Trivial/Low/Medium block methods ===
  std::cout << "\n=== Trivial/Low/Medium Block Methods ===" << std::endl;

  // Test block determinant
  Scalar detBlock = AlgebraEngine::ComputeDeterminantBlockwise(D, 1);
  assert(std::abs(detBlock - (-2.0)) < 1e-10);
  (void)detBlock;
  std::cout << "  ComputeDeterminantBlockwise: OK" << std::endl;

  // Test block log-absolute-determinant
  Scalar signOut = 0.0;
  Scalar logDetBlock = AlgebraEngine::ComputeLogAbsoluteDeterminantBlockwise(D, signOut, 1);
  assert(std::abs(signOut - (-1.0)) < 1e-10);
  assert(std::abs(logDetBlock - std::log(2.0)) < 1e-10);
  (void)logDetBlock;
  std::cout << "  ComputeLogAbsoluteDeterminantBlockwise: OK" << std::endl;

  // Test block inverse
  DataContainer invBlock = AlgebraEngine::InverseBlockwise(A, 1);
  DataContainer Icheck = AlgebraEngine::MatrixProduct(A, invBlock);
  for (UnsignedInteger i = 0; i < 2; ++i)
    for (UnsignedInteger j = 0; j < 2; ++j)
      assert(std::abs(Icheck(i, j) - (i == j ? 1.0 : 0.0)) < 1e-10);
  std::cout << "  InverseBlockwise: OK" << std::endl;

  // Test block inverse on 3x3
  DataContainer inv33 = AlgebraEngine::InverseBlockwise(A33, 1);
  DataContainer I33check = AlgebraEngine::MatrixProduct(A33, inv33);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      assert(std::abs(I33check(i, j) - (i == j ? 1.0 : 0.0)) < 1e-10);
  std::cout << "  InverseBlockwise (3x3): OK" << std::endl;

  // Test block inverse SPD
  DataContainer invSPD = AlgebraEngine::InverseSPDBlockwise(S, 1);
  DataContainer IspdCheck = AlgebraEngine::MatrixProduct(S, invSPD);
  for (UnsignedInteger i = 0; i < 2; ++i)
    for (UnsignedInteger j = 0; j < 2; ++j)
      assert(std::abs(IspdCheck(i, j) - (i == j ? 1.0 : 0.0)) < 1e-10);
  std::cout << "  InverseSPDBlockwise: OK" << std::endl;

  // Test block rectangular solve (overdetermined: 3x4 * x = b)
  DataContainer bRect(3, 1, 0.0, DataContainer::COLUMN_MAJOR);
  bRect(0, 0) = 1.0; bRect(1, 0) = 2.0; bRect(2, 0) = 3.0;
  DataContainer xRect = AlgebraEngine::SolveLinearSystemRectangularBlockwise(A34, bRect, 2);
  // Verify residual is minimized (normal equations: A^T*A*x = A^T*b)
  DataContainer AtA = AlgebraEngine::MatrixProduct(AlgebraEngine::Transpose(A34), A34);
  DataContainer Atb = AlgebraEngine::MatrixProduct(AlgebraEngine::Transpose(A34), bRect);
  DataContainer lhsCheck = AlgebraEngine::MatrixProduct(AtA, xRect);
  for (UnsignedInteger i = 0; i < 4; ++i)
    assert(std::abs(lhsCheck(i, 0) - Atb(i, 0)) < 1e-8);
  std::cout << "  SolveLinearSystemRectangularBlockwise: OK" << std::endl;

  std::cout << "\n=== Additional AlgebraEngine API Coverage ===" << std::endl;

  // SymMatrixPointProduct
  DataContainer Ssym(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  Ssym(0, 0) = 4.0; Ssym(1, 0) = 1.0;
  Ssym(0, 1) = 1.0; Ssym(1, 1) = 3.0;
  DataContainer xsym(2, 1, 0.0, DataContainer::COLUMN_MAJOR);
  xsym(0, 0) = 1.0; xsym(1, 0) = 2.0;
  DataContainer rsym = AlgebraEngine::SymMatrixPointProduct(Ssym, xsym);
  assert(std::abs(rsym(0, 0) - 6.0) < 1e-12);
  assert(std::abs(rsym(1, 0) - 7.0) < 1e-12);
  std::cout << "  SymMatrixPointProduct: OK" << std::endl;

  // SymProd: 'L' -> A B A^T, 'R' -> A^T B A
  DataContainer Adiag(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  Adiag(0, 0) = 1.0; Adiag(1, 1) = 2.0;
  DataContainer Bdiag(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  Bdiag(0, 0) = 2.0; Bdiag(1, 1) = 3.0;
  DataContainer symL = AlgebraEngine::SymProd(Adiag, Bdiag, 'L');
  assert(std::abs(symL(0, 0) - 2.0) < 1e-12);
  assert(std::abs(symL(1, 1) - 12.0) < 1e-12);
  DataContainer symR = AlgebraEngine::SymProd(Adiag, Bdiag, 'R');
  assert(std::abs(symR(0, 0) - 2.0) < 1e-12);
  assert(std::abs(symR(1, 1) - 12.0) < 1e-12);
  std::cout << "  SymProd: OK" << std::endl;

  // TriangularProd: lower/upper, left/right
  DataContainer Ltri(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  Ltri(0, 0) = 2.0; Ltri(1, 0) = 1.0; Ltri(1, 1) = 3.0; // lower triangular
  DataContainer Idc(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  Idc(0, 0) = 1.0; Idc(1, 1) = 1.0;
  DataContainer tL = AlgebraEngine::TriangularProd(Ltri, Idc, 'L', 'L');
  assert(std::abs(tL(0, 0) - 2.0) < 1e-12);
  assert(std::abs(tL(1, 1) - 3.0) < 1e-12);
  DataContainer tR = AlgebraEngine::TriangularProd(Idc, Ltri, 'R', 'L');
  assert(std::abs(tR(0, 0) - 2.0) < 1e-12);
  assert(std::abs(tR(1, 0) - 1.0) < 1e-12);
  assert(std::abs(tR(1, 1) - 3.0) < 1e-12);
  std::cout << "  TriangularProd: OK" << std::endl;

  // ComputeQR (economy + full, tall 3x2 and wide 2x3)
  DataContainer A3(3, 2, 0.0, DataContainer::COLUMN_MAJOR);
  A3(0, 0) = 1.0; A3(1, 0) = 2.0; A3(2, 0) = 3.0;
  A3(0, 1) = 4.0; A3(1, 1) = 5.0; A3(2, 1) = 6.0;
  DataContainer Q, R;
  AlgebraEngine::ComputeQR(A3, Q, R);
  assert(Q.getSize() == 3);
  assert(Q.getDimension() == 2);
  assert(R.getSize() == 2);
  assert(R.getDimension() == 2);
  DataContainer QRprod = AlgebraEngine::MatrixProduct(Q, R);
  for (UnsignedInteger idx = 0; idx < A3.getSize() * A3.getDimension(); ++idx)
    assert(std::abs(QRprod[idx] - A3[idx]) < 1e-10);
  DataContainer Qf, Rf;
  AlgebraEngine::ComputeQR(A3, Qf, Rf, true);
  assert(Qf.getSize() == 3);
  assert(Qf.getDimension() == 3);
  DataContainer Qw, Rw;
  AlgebraEngine::ComputeQR(AlgebraEngine::Transpose(A3), Qw, Rw);
  DataContainer Qfw, Rfw;
  AlgebraEngine::ComputeQR(AlgebraEngine::Transpose(A3), Qfw, Rfw, true);
  assert(Qfw.getSize() == 2);
  assert(Qfw.getDimension() == 2);
  std::cout << "  ComputeQR (tall + wide): OK" << std::endl;

  // ComputeSVD (economy + full)
  DataContainer Us, Ss, VTs;
  AlgebraEngine::ComputeSVD(A3, Us, Ss, VTs);
  assert(Ss.getSize() == 2);
  DataContainer Sdiag(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  Sdiag(0, 0) = Ss[0]; Sdiag(1, 1) = Ss[1];
  DataContainer USVT = AlgebraEngine::MatrixProduct(Us, AlgebraEngine::MatrixProduct(Sdiag, VTs));
  for (UnsignedInteger idx = 0; idx < A3.getSize() * A3.getDimension(); ++idx)
    assert(std::abs(USVT[idx] - A3[idx]) < 1e-9);
  DataContainer Uf, Sf, VTf;
  AlgebraEngine::ComputeSVD(A3, Uf, Sf, VTf, true);
  assert(Uf.getSize() == 3);
  assert(Uf.getDimension() == 3);
  std::cout << "  ComputeSVD (economy + full): OK" << std::endl;

  // ComputeLU (non-block): L unit lower-triangular, U upper-triangular, L*U equals A up to pivoting
  DataContainer ALU(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  ALU(0, 0) = 1.0; ALU(1, 0) = 3.0; ALU(0, 1) = 2.0; ALU(1, 1) = 1.0; // [[1, 2], [3, 1]]
  DataContainer Lc, Uc;
  AlgebraEngine::ComputeLU(ALU, Lc, Uc);
  // L is unit lower triangular
  assert(std::abs(Lc(0, 0) - 1.0) < 1e-12);
  assert(std::abs(Lc(1, 1) - 1.0) < 1e-12);
  assert(std::abs(Lc(0, 1)) < 1e-12);
  // U is upper triangular
  assert(std::abs(Uc(1, 0)) < 1e-12);
  // L * U reproduces A up to the internal row permutation
  DataContainer PALU = AlgebraEngine::MatrixProduct(Lc, Uc);
  if (std::abs(PALU(0, 0) - ALU(1, 0)) < 1e-10)
  {
    assert(std::abs(PALU(0, 1) - ALU(1, 1)) < 1e-10);
    assert(std::abs(PALU(1, 0) - ALU(0, 0)) < 1e-10);
    assert(std::abs(PALU(1, 1) - ALU(0, 1)) < 1e-10);
  }
  else
  {
    for (UnsignedInteger i = 0; i < 2; ++i)
      for (UnsignedInteger j = 0; j < 2; ++j)
        assert(std::abs(PALU(i, j) - ALU(i, j)) < 1e-10);
  }
  DataContainer bLU(2, 1, 0.0, DataContainer::COLUMN_MAJOR);
  bLU(0, 0) = 3.0; bLU(1, 0) = 4.0;
  DataContainer xLU = AlgebraEngine::SolveLinearSystem(ALU, bLU);
  assert(std::abs(xLU(0, 0) - 1.0) < 1e-10);
  assert(std::abs(xLU(1, 0) - 1.0) < 1e-10);
  std::cout << "  ComputeLU: OK" << std::endl;

  // ComputeLogAbsoluteDeterminant (non-block)
  Scalar signDet = 0.0;
  Scalar logDet = AlgebraEngine::ComputeLogAbsoluteDeterminant(ALU, signDet);
  assert(std::abs(signDet - (-1.0)) < 1e-10);
  assert(std::abs(logDet - std::log(5.0)) < 1e-10);
  std::cout << "  ComputeLogAbsoluteDeterminant: OK (logDet=" << logDet << ")" << std::endl;

  // ComputeEigenValuesSymmetric
  DataContainer eigSym = AlgebraEngine::ComputeEigenValuesSymmetric(Ssym);
  assert(eigSym.getSize() == 2);
  // eigenvalues of [[4,1],[1,3]]: (7 +/- sqrt(5))/2, ascending
  assert(std::abs(eigSym[0] - (7.0 - std::sqrt(5.0)) / 2.0) < 1e-10);
  assert(std::abs(eigSym[1] - (7.0 + std::sqrt(5.0)) / 2.0) < 1e-10);
  std::cout << "  ComputeEigenValuesSymmetric: OK" << std::endl;

  // ComputeEigenValuesSquare returns real/imag pairs: rotation [[0,-1],[1,0]] -> +/- i
  DataContainer Rrot(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  Rrot(0, 0) = 0.0; Rrot(1, 0) = 1.0; Rrot(0, 1) = -1.0; Rrot(1, 1) = 0.0;
  DataContainer eigRot = AlgebraEngine::ComputeEigenValuesSquare(Rrot);
  assert(eigRot.getSize() == 4);
  assert(std::abs(eigRot[0]) < 1e-10);
  assert(std::abs(std::abs(eigRot[1]) - 1.0) < 1e-10);
  assert(std::abs(eigRot[2]) < 1e-10);
  assert(std::abs(std::abs(eigRot[3]) - 1.0) < 1e-10);
  std::cout << "  ComputeEigenValuesSquare (complex pairs): OK" << std::endl;

  // ComputeSingularValues
  DataContainer Sdiag2(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  Sdiag2(0, 0) = 4.0; Sdiag2(1, 1) = 2.0;
  DataContainer sosv = AlgebraEngine::ComputeSingularValues(Sdiag2);
  assert(sosv.getSize() == 2);
  assert(std::abs(sosv[0] - 4.0) < 1e-10);
  assert(std::abs(sosv[1] - 2.0) < 1e-10);
  std::cout << "  ComputeSingularValues: OK" << std::endl;

  // Clean preserves shape and cleans all elements
  DataContainer Adirty(2, 2, 0.0, DataContainer::COLUMN_MAJOR);
  Adirty(0, 0) = 1e-15; Adirty(1, 0) = 1.0; Adirty(0, 1) = -1e-13; Adirty(1, 1) = 2.0;
  DataContainer Aclean = AlgebraEngine::Clean(Adirty, 1e-10);
  assert(Aclean.getSize() == 2);
  assert(Aclean.getDimension() == 2);
  assert(Aclean(0, 0) == 0.0);
  assert(std::abs(Aclean(1, 0) - 1.0) < 1e-15);
  assert(Aclean(0, 1) == 0.0);
  assert(std::abs(Aclean(1, 1) - 2.0) < 1e-15);
  std::cout << "  Clean: OK" << std::endl;

  // SolveLinearSystemRectangular (least-squares)
  DataContainer Arect(3, 3, 0.0, DataContainer::COLUMN_MAJOR);
  Arect(0, 0) = 1.0; Arect(1, 0) = 4.0; Arect(2, 0) = 7.0;
  Arect(0, 1) = 2.0; Arect(1, 1) = 5.0; Arect(2, 1) = 8.0;
  Arect(0, 2) = 3.0; Arect(1, 2) = 6.0; Arect(2, 2) = 10.0;
  DataContainer brect2(3, 1, 0.0, DataContainer::COLUMN_MAJOR);
  brect2(0, 0) = 1.0; brect2(1, 0) = 1.0; brect2(2, 0) = 1.0;
  DataContainer xrect2 = AlgebraEngine::SolveLinearSystemRectangular(Arect, brect2);
  assert(std::abs(xrect2(1, 0) - 1.0) < 1e-8);
  std::cout << "  SolveLinearSystemRectangular: OK" << std::endl;

  std::cout << "\n=== StatisticsEngine Tests ===" << std::endl;

  // Create a sample: 5 observations, 2 variables (row-major)
  DataContainer sample(5, 2, 0.0, DataContainer::ROW_MAJOR);
  // Variable 0: 1, 2, 3, 4, 5 (mean=3)
  // Variable 1: 10, 20, 30, 40, 50 (mean=30)
  sample(0, 0) = 1.0; sample(0, 1) = 10.0;
  sample(1, 0) = 2.0; sample(1, 1) = 20.0;
  sample(2, 0) = 3.0; sample(2, 1) = 30.0;
  sample(3, 0) = 4.0; sample(3, 1) = 40.0;
  sample(4, 0) = 5.0; sample(4, 1) = 50.0;

  // Test Mean
  DataContainer mean = StatisticsEngine::ComputeMean(sample);
  assert(mean.getSize() == 1);
  assert(mean.getDimension() == 2);
  assert(std::abs(mean[0] - 3.0) < 1e-12);
  assert(std::abs(mean[1] - 30.0) < 1e-12);
  std::cout << "  Mean: OK" << std::endl;

  // Test Variance (Bessel corrected: N-1)
  DataContainer variance = StatisticsEngine::ComputeVariance(sample);
  assert(variance.getSize() == 1);
  assert(variance.getDimension() == 2);
  // Var = sum((x-mean)^2)/(N-1) = ((1-3)^2+(2-3)^2+(3-3)^2+(4-3)^2+(5-3)^2)/4 = 10/4 = 2.5
  assert(std::abs(variance[0] - 2.5) < 1e-12);
  // Var1 = ((10-30)^2+...)/4 = 1000/4 = 250
  assert(std::abs(variance[1] - 250.0) < 1e-12);
  std::cout << "  Variance: OK" << std::endl;

  // Test StandardDeviation
  DataContainer sd = StatisticsEngine::ComputeStandardDeviation(sample);
  assert(std::abs(sd[0] - std::sqrt(2.5)) < 1e-12);
  assert(std::abs(sd[1] - std::sqrt(250.0)) < 1e-12);
  std::cout << "  StandardDeviation: OK" << std::endl;

  // Test Min/Max
  DataContainer minVal = StatisticsEngine::ComputeMin(sample);
  DataContainer maxVal = StatisticsEngine::ComputeMax(sample);
  assert(std::abs(minVal[0] - 1.0) < 1e-12);
  assert(std::abs(minVal[1] - 10.0) < 1e-12);
  assert(std::abs(maxVal[0] - 5.0) < 1e-12);
  assert(std::abs(maxVal[1] - 50.0) < 1e-12);
  std::cout << "  Min/Max: OK" << std::endl;

  // Test Quantile (median = q=0.5)
  DataContainer median = StatisticsEngine::ComputeQuantile(sample, 0.5);
  assert(std::abs(median[0] - 3.0) < 1e-12);
  assert(std::abs(median[1] - 30.0) < 1e-12);
  std::cout << "  Quantile (median): OK" << std::endl;

  // Test Covariance matrix
  DataContainer cov = StatisticsEngine::ComputeCovariance(sample);
  assert(cov.getSize() == 2);
  assert(cov.getDimension() == 2);
  // Cov[0,0] = 2.5, Cov[1,1] = 250, Cov[0,1] = 25
  assert(std::abs(cov(0, 0) - 2.5) < 1e-12);
  assert(std::abs(cov(1, 1) - 250.0) < 1e-12);
  assert(std::abs(cov(0, 1) - 25.0) < 1e-12);
  assert(std::abs(cov(1, 0) - 25.0) < 1e-12);
  std::cout << "  Covariance: OK" << std::endl;

  // Test Pearson correlation (perfect positive correlation)
  DataContainer corr = StatisticsEngine::ComputePearsonCorrelation(sample);
  assert(std::abs(corr(0, 0) - 1.0) < 1e-12);
  assert(std::abs(corr(1, 1) - 1.0) < 1e-12);
  assert(std::abs(corr(0, 1) - 1.0) < 1e-12);
  std::cout << "  PearsonCorrelation: OK" << std::endl;

  // Test Spearman rank correlation (monotonically increasing columns -> rank 1)
  DataContainer sp_r(4, 2, 0.0, DataContainer::ROW_MAJOR);
  sp_r(0, 0) = 1.0; sp_r(0, 1) = 1.0;
  sp_r(1, 0) = 2.0; sp_r(1, 1) = 4.0;
  sp_r(2, 0) = 3.0; sp_r(2, 1) = 9.0;
  sp_r(3, 0) = 4.0; sp_r(3, 1) = 16.0;
  DataContainer sp_corr = StatisticsEngine::ComputeSpearmanCorrelation(sp_r);
  assert(std::abs(sp_corr(0, 0) - 1.0) < 1e-12);
  assert(std::abs(sp_corr(1, 1) - 1.0) < 1e-12);
  assert(std::abs(sp_corr(0, 1) - 1.0) < 1e-12);
  std::cout << "  SpearmanCorrelation: OK" << std::endl;

  // Test block-based statistics
  DataContainer meanBlock = StatisticsEngine::ComputeMeanBlockwise(sample, 2);
  assert(std::abs(meanBlock[0] - 3.0) < 1e-12);
  assert(std::abs(meanBlock[1] - 30.0) < 1e-12);
  std::cout << "  MeanBlockwise: OK" << std::endl;

  DataContainer varBlock = StatisticsEngine::ComputeVarianceBlockwise(sample, 2);
  assert(std::abs(varBlock[0] - 2.5) < 1e-12);
  assert(std::abs(varBlock[1] - 250.0) < 1e-12);
  std::cout << "  VarianceBlockwise: OK" << std::endl;

  DataContainer covBlock = StatisticsEngine::ComputeCovarianceBlockwise(sample, 2);
  assert(std::abs(covBlock(0, 0) - 2.5) < 1e-12);
  assert(std::abs(covBlock(1, 1) - 250.0) < 1e-12);
  assert(std::abs(covBlock(0, 1) - 25.0) < 1e-12);
  std::cout << "  CovarianceBlockwise: OK" << std::endl;

#ifdef OPENTURNS_HAVE_HDF5
  std::cout << "\n=== HDF5Backend Tests ===" << std::endl;

  // Test create HDF5Backend
  DataContainer dc_h5(1, 6, 0.0, DataContainer::COLUMN_MAJOR);
  dc_h5[0] = 1.0; dc_h5[1] = 2.0; dc_h5[2] = 3.0;
  dc_h5[3] = 4.0; dc_h5[4] = 5.0; dc_h5[5] = 6.0;
  dc_h5.setBackend(new HDF5Backend("/tmp/test_ot_hdf5.h5", "mydata", 6));
  assert(dc_h5[0] == 1.0);
  assert(dc_h5[5] == 6.0);
  std::cout << "  HDF5Backend create: OK" << std::endl;

  // Test setBackend roundtrip
  DataContainer dc_rt(1, 3, 0.0, DataContainer::COLUMN_MAJOR);
  dc_rt[0] = 10.0; dc_rt[1] = 20.0; dc_rt[2] = 30.0;
  dc_rt.setBackend(new HDF5Backend("/tmp/test_ot_hdf5_rt.h5", "roundtrip", 3));
  assert(dc_rt[0] == 10.0);
  assert(dc_rt[1] == 20.0);
  assert(dc_rt[2] == 30.0);
  std::cout << "  HDF5Backend roundtrip: OK" << std::endl;

  // Test Open existing file
  Pointer<HDF5Backend> p_opened = HDF5Backend::Open("/tmp/test_ot_hdf5.h5", "mydata");
  assert(p_opened->size() == 6);
  assert(p_opened->data()[0] == 1.0);
  assert(p_opened->data()[5] == 6.0);
  std::cout << "  HDF5Backend Open: OK" << std::endl;

  // Test clone
  Pointer<DataContainer::StorageBackend> p_clone = dc_h5.getBackend()->clone();
  assert(p_clone->size() == 6);
  assert(p_clone->data()[0] == 1.0);
  assert(p_clone->data()[5] == 6.0);
  std::cout << "  HDF5Backend clone: OK" << std::endl;

  // Test resize
  DataContainer dc_res(1, 3, 0.0, DataContainer::COLUMN_MAJOR);
  dc_res[0] = 7.0; dc_res[1] = 8.0; dc_res[2] = 9.0;
  dc_res.setBackend(new HDF5Backend("/tmp/test_ot_hdf5_res.h5", "resizable", 3));
  dc_res.resize(5);
  dc_res[3] = 10.0; dc_res[4] = 11.0;
  assert(dc_res[0] == 7.0);
  assert(dc_res[4] == 11.0);
  std::cout << "  HDF5Backend resize: OK" << std::endl;

  // Clean up test files
  std::remove("/tmp/test_ot_hdf5.h5");
  std::remove("/tmp/test_ot_hdf5_rt.h5");
  std::remove("/tmp/test_ot_hdf5_res.h5");
  std::remove("/tmp/ot_hdf5_1.h5");
  std::cout << "  HDF5Backend cleanup: OK" << std::endl;

  std::cout << "\n=== ComplexHDF5Backend Tests ===" << std::endl;

  // Test create + roundtrip through re/im datasets
  ComplexDataContainer cdc(3);
  cdc[0] = Complex(1.0, 2.0); cdc[1] = Complex(3.0, 4.0); cdc[2] = Complex(5.0, 6.0);
  cdc.setBackend(new ComplexHDF5Backend("/tmp/test_ot_chdf5.h5", "cmydata", 3));
  assert(cdc[0] == Complex(1.0, 2.0));
  assert(cdc[2] == Complex(5.0, 6.0));
  std::cout << "  ComplexHDF5Backend create/roundtrip: OK" << std::endl;

  // Test Open existing file
  Pointer<ComplexHDF5Backend> p_copened = ComplexHDF5Backend::Open("/tmp/test_ot_chdf5.h5", "cmydata");
  assert(p_copened->size() == 3);
  assert(p_copened->data()[1] == Complex(3.0, 4.0));
  std::cout << "  ComplexHDF5Backend Open: OK" << std::endl;

  // Test clone
  Pointer<ComplexDataContainer::StorageBackend> p_cclone = cdc.getBackend()->clone();
  assert(p_cclone->size() == 3);
  assert(p_cclone->data()[0] == Complex(1.0, 2.0));
  std::cout << "  ComplexHDF5Backend clone: OK" << std::endl;

  // Test resize
  ComplexDataContainer cdc_res(3);
  cdc_res[0] = Complex(7.0, 0.0); cdc_res[1] = Complex(8.0, 0.0); cdc_res[2] = Complex(9.0, 0.0);
  cdc_res.setBackend(new ComplexHDF5Backend("/tmp/test_ot_chdf5_res.h5", "cresizable", 3));
  cdc_res.resize(5);
  cdc_res[3] = Complex(10.0, 0.0); cdc_res[4] = Complex(11.0, 0.0);
  assert(cdc_res[0] == Complex(7.0, 0.0));
  assert(cdc_res[4] == Complex(11.0, 0.0));
  std::cout << "  ComplexHDF5Backend resize: OK" << std::endl;

  std::remove("/tmp/test_ot_chdf5.h5");
  std::remove("/tmp/test_ot_chdf5_res.h5");
  std::remove("/tmp/ot_chdf5_1.h5");
  std::cout << "  ComplexHDF5Backend cleanup: OK" << std::endl;
#endif

  std::cout << "\n=== All tests passed! ===" << std::endl;
  return 0;
}
