//                                               -*- C++ -*-
/**
 *  @file  HODLRCore.cxx
 *  @brief Implementation of the recursive HODLR tree node
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
 */
#include "openturns/HODLRCore.hxx"
#include "openturns/Lapack.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/ResourceMap.hxx"

#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstdio>
#include <functional>

BEGIN_NAMESPACE_OPENTURNS

BEGIN_C_DECLS
#ifdef OPENTURNS_HAVE_OPENBLAS
int goto_get_num_procs(void);
void openblas_set_num_threads(int num_threads);
#endif
END_C_DECLS

namespace {

// Optional factorization profile, enabled via HODLRMatrix-ProfileFactorization.
// Prints a summary line and per-block timings of the recompression to stderr.
struct FactorTiming
{
  double recomp = 0.0;
  double recompCopy = 0.0;
  double recompQR = 0.0;
  double recompMcat = 0.0;
  double recompGram = 0.0;
  double recompSyevd = 0.0;
  double recompFinal = 0.0;
  double leaf = 0.0;
  double leafCorr = 0.0;
  double leafCorrected = 0.0;
  double W = 0.0;
  double Kgram = 0.0;
  double UK = 0.0;
  double dpotrf = 0.0;
  int nRecomp = 0;
  int nLeaf = 0;
  int nLeafCorr = 0;
  int nLeafCorrected = 0;
  double k1sum = 0.0;
  double nRowsSum = 0.0;
  double totalRankSum = 0.0;
  struct BlockTiming
  {
    unsigned nRows = 0;
    unsigned nCols = 0;
    unsigned totalRank = 0;
    unsigned kept = 0;
    unsigned k1 = 0;
    unsigned rankOut = 0;
    double copy = 0.0;
    double qr = 0.0;
    double gram = 0.0;
    double syevd = 0.0;
    double final = 0.0;
  };
  std::vector<BlockTiming> blocks;
  ~FactorTiming()
  {
    if (printProfile)
    {
      std::fprintf(stderr, "[FT] recomp: %.2f ms (%d) [copy %.2f QR %.2f Mcat %.2f Gram %.2f syevd %.2f final %.2f; k1 %.0f nRows %.0f totRank %.0f], leaf: %.2f ms (%d) of which corrections %.2f ms, leafCorrected: %.2f ms (%d), W: %.2f, Kgram: %.2f, UK: %.2f, dpotrf: %.2f\n",
                   (recompCopy + recompQR + recompMcat + recompGram + recompSyevd + recompFinal) * 1e3, nRecomp,
                   recompCopy * 1e3, recompQR * 1e3, recompMcat * 1e3,
                   recompGram * 1e3, recompSyevd * 1e3, recompFinal * 1e3,
                   k1sum, nRowsSum, totalRankSum,
                   leaf * 1e3, nLeaf, leafCorr * 1e3, leafCorrected * 1e3, nLeafCorrected,
                   W * 1e3, Kgram * 1e3, UK * 1e3, dpotrf * 1e3);
      std::sort(blocks.begin(), blocks.end(),
                [](const BlockTiming& a, const BlockTiming& b)
                { return a.copy + a.qr + a.gram + a.syevd + a.final > b.copy + b.qr + b.gram + b.syevd + b.final; });
      for (const auto& b : blocks)
        std::fprintf(stderr, "[FTb] nR %u nC %u tR %u kt %u k1 %u ro %u  copy %.2f QR %.2f Gram %.2f syevd %.2f final %.2f  tot %.2f ms\n",
                     b.nRows, b.nCols, b.totalRank, b.kept, b.k1, b.rankOut,
                     b.copy * 1e3, b.qr * 1e3, b.gram * 1e3, b.syevd * 1e3, b.final * 1e3,
                     (b.copy + b.qr + b.gram + b.syevd + b.final) * 1e3);
    }
  }
  // Set once during factorization: only print the profile when explicitly
  // requested, to avoid polluting stderr in normal use.
  bool printProfile = false;
} g_factorTiming;

}  // namespace

namespace {

void HODLRDgemm(const char* transa, const char* transb, int* m, int* n, int* k,
                double* alpha, double* a, int* lda, double* b, int* ldb,
                double* beta, double* c, int* ldc)
{
  int ltransa = 1;
  int ltransb = 1;
  dgemm_(const_cast<char*>(transa), const_cast<char*>(transb), m, n, k, alpha,
         a, lda, b, ldb, beta, c, ldc, &ltransa, &ltransb);
}

void HODLRDtrsm(const char* side, const char* uplo, const char* transa, const char* diag,
                int* m, int* n, double* alpha, double* a, int* lda,
                double* b, int* ldb)
{
  int lside = 1;
  int luplo = 1;
  int ltransa = 1;
  int ldiag = 1;
  dtrsm_(const_cast<char*>(side), const_cast<char*>(uplo), const_cast<char*>(transa),
         const_cast<char*>(diag), m, n, alpha, a, lda, b, ldb,
         &lside, &luplo, &ltransa, &ldiag);
}

void HODLRDtrmv(const char* uplo, const char* transa, const char* diag,
                int* n, double* a, int* lda, double* x, int* incx)
{
  int luplo = 1;
  int ltransa = 1;
  int ldiag = 1;
  dtrmv_(const_cast<char*>(uplo), const_cast<char*>(transa), const_cast<char*>(diag),
         n, a, lda, x, incx, &luplo, &ltransa, &ldiag);
}

void HODLRDpotrf(const char* uplo, int* n, double* a, int* lda, int* info)
{
  int luplo = 1;
  dpotrf_(const_cast<char*>(uplo), n, a, lda, info, &luplo);
}

// Fill a symmetric dense leaf block using only the lower-triangular kernel
// evaluations and mirror them into the upper triangle. The HODLR tree is
// symmetric by construction, and the entry evaluators used (kernel,
// permuted, corrected) all satisfy f(i,j) = f(j,i), so the mirrored values
// are exact.
void HODLRSymmetricLeafFill(MatrixImplementation& Sfact,
                            UnsignedInteger size,
                            UnsignedInteger start,
                            const HODLREntryEvaluator& eval)
{
  for (UnsignedInteger j = 0; j < size; ++j)
  {
    for (UnsignedInteger i = j; i < size; ++i)
    {
      const Scalar v = eval(start + i, start + j);
      Sfact[i + j * size] = v;
      if (i != j)
        Sfact[j + i * size] = v;
    }
  }
}

}  // namespace

HODLRNode::HODLRNode(Pointer<const HODLREntryEvaluator> eval,
                     const Scalar* diag,
                     UnsignedInteger start,
                     UnsignedInteger size,
                     UnsignedInteger minLeafSize,
                     UnsignedInteger maxRank,
                     Scalar tolerance,
                     SignedInteger direction,
                     HODLRNode* parent)
  : p_diag_(diag)
  , p_eval_(eval)
  , start_(start)
  , size_(size)
  , direction_(direction)
  , rank_(0)
  , maxRank_(maxRank)
  , minLeafSize_(minLeafSize)
  , denseThreshold_(0)
  , tolerance_(tolerance)
  , isLeaf_(false)
  , logDet_(0.0)
  , shift_(0.0)
  , totalRank_(0)
  , numLeaves_(0)
  , numStarvedBlocks_(0)
  , p_parent_(parent)
  , p_child0_()
  , p_child1_()
  , U_(2)
  , V_(2)
{
  denseThreshold_ = ResourceMap::GetAsUnsignedInteger("HODLRMatrix-DenseThreshold");

  const UnsignedInteger half = size_ / 2;

  if (half >= minLeafSize)
  {
    isLeaf_ = false;

    // Low-rank approximation of off-diagonal block A01:
    // A[child1, child0] = U_[1] * V_[0]^T
    // Max-element (partial-pivoting) ACA.
    rank_ = lowRankApproxPartialPivot(
        start_ + half, size_ - half,
        start_, half,
        tolerance, U_[1], V_[0]);

    // Fall back to dense when the block is not low-rank: if the ACA reached
    // the full rank min(rows, cols), store the block exactly as U_[1] = A01
    // and V_[0] = I instead of keeping a truncated (and inexact) product.
    const UnsignedInteger s1 = size_ - half;
    if (rank_ == std::min(s1, half))
    {
      U_[1] = Matrix(s1, half);
      V_[0] = Matrix(half, half);
      MatrixImplementation& U1Impl = *U_[1].getImplementation();
      MatrixImplementation& V0Impl = *V_[0].getImplementation();
      for (UnsignedInteger j = 0; j < half; ++j)
        V0Impl[j + j * half] = 1.0;
      for (UnsignedInteger j = 0; j < half; ++j)
        for (UnsignedInteger i = 0; i < s1; ++i)
          U1Impl[i + j * s1] = (*p_eval_)(start_ + half + i, start_ + j);
    }

    // Symmetric: A10 = A01^T = V_[0] * U_[1]^T, so:
    U_[0] = V_[0];
    V_[1] = U_[1];

    totalRank_ = rank_;

    p_child0_ = new HODLRNode(p_eval_, p_diag_, start_, half, minLeafSize, maxRank_, tolerance_, 0, this);
    p_child1_ = new HODLRNode(p_eval_, p_diag_, start_ + half, size_ - half, minLeafSize, maxRank_, tolerance_, 1, this);

    // Count the blocks that had to be stored at full rank (dense fallback
    // above): the HODLR low-rank assumption does not hold for them at the
    // assembly tolerance. Blocks smaller than the leaf-size scale are expected
    // to be full-rank, so only count blocks large enough to be significant.
    numStarvedBlocks_ = p_child0_->numStarvedBlocks_ + p_child1_->numStarvedBlocks_;
    if ((rank_ == std::min(s1, half)) && (std::min(s1, half) >= 64))
      ++numStarvedBlocks_;

    // Single aggregated warning at the root instead of one per block.
    if ((parent == nullptr) && (numStarvedBlocks_ > 0))
      LOGWARN(OSS() << "HODLRMatrix: " << numStarvedBlocks_ << " off-diagonal block(s) reached the full rank "
             << "at the assembly tolerance " << tolerance_ << " and are stored as dense factors; "
             << "accuracy is preserved but the storage/factor cost is not reduced for these blocks. "
             << "Consider loosening HODLRMatrix-AssemblyEpsilon or increasing HODLRMatrix-MinLeafSize");
  }
  else
  {
    isLeaf_ = true;
    // Cache the raw kernel leaf block at construction time so that the
    // factorization never re-evaluates the kernel entries.
    leafKernel_ = Matrix(size_, size_);
    MatrixImplementation& leafData = *leafKernel_.getImplementation();
    HODLRSymmetricLeafFill(leafData, size_, start_, *p_eval_);
  }
}

HODLRNode::~HODLRNode()
{
  // Nothing to do
}

void HODLRNode::setShift(Scalar shift)
{
  shift_ = shift;
  if (p_child0_) p_child0_->setShift(shift);
  if (p_child1_) p_child1_->setShift(shift);
}

HODLRBlasGuard::HODLRBlasGuard()
{
#ifdef OPENTURNS_HAVE_OPENBLAS
  savedNumThreads_ = goto_get_num_procs();
  openblas_set_num_threads(1);
#endif
  (void)savedNumThreads_;
}

HODLRBlasGuard::~HODLRBlasGuard()
{
#ifdef OPENTURNS_HAVE_OPENBLAS
  openblas_set_num_threads(savedNumThreads_);
#endif
}

UnsignedInteger HODLRNode::lowRankApproxPartialPivot(UnsignedInteger startRow, UnsignedInteger nRows,
    UnsignedInteger startCol, UnsignedInteger nCols,
    Scalar tol, Matrix& Uout, Matrix& Vout)
{
  const UnsignedInteger maxRankBound = std::min(nRows, nCols);
  const UnsignedInteger maxRank = (maxRank_ > 0) ? std::min(maxRankBound, maxRank_) : maxRankBound;

  // The factor buffers are fully written below before any read (each new U/V
  // column fills every row), so the zero-initialization would only add a
  // useless memset of the whole block. They are kept in transposed layout
  // (factor row l contiguous in the block row/column index) so that the
  // residual downdates are contiguous rank-1 loops that the compiler can
  // vectorize, instead of dot products with a strided access.
  //
  // The buffers grow geometrically with the rank revealed by the ACA. The
  // worst-case allocation min(nRows, nCols) * (nRows + nCols) would otherwise
  // be paid on every block even when the rank stays small (e.g. ~100 MB for a
  // 2500 x 2500 block of an exactly low-rank 1D kernel), and the value
  // initialization of std::vector would zero it all.
  UnsignedInteger bufferCols = 0;
  std::vector<Scalar> Udata;
  std::vector<Scalar> Vdata;
  const auto ensureBuffer = [&](const UnsignedInteger needed)
  {
    if (needed <= bufferCols)
      return;
    const UnsignedInteger grown =
        std::max(needed, std::max(UnsignedInteger(1), 2 * bufferCols));
    Udata.resize(grown * nRows);
    Vdata.resize(grown * nCols);
    bufferCols = grown;
  };

  // Partial-pivoting ACA, ported from the "ACA partial" scheme of hmat-oss
  // (src/compression.cpp, doCompressionAcaPartial). Unlike the full-pivoting
  // variant, the residual block is never materialized: only the residual row
  // at the pivot row and the residual column at the pivot column are assembled
  // from the evaluator on demand, so the cost is O(rank * (nRows + nCols))
  // instead of O(rank * nRows * nCols). Two ingredients make the pivot choice
  // as reliable as full pivoting:
  //  - rows and columns already used as pivots are excluded (rowFree/colFree),
  //    which prevents the stagnation of naive partial pivoting, and
  //  - the pivot column is scanned over the residual of the pivot row only,
  //    while the next pivot row is the row with the largest residual in the
  //    pivot column.
  // The stopping criterion accumulates the same Frobenius-norm estimate as
  // the full-pivoting variant (without the cross terms between the factors).
  UnsignedInteger rank = 0;
  Scalar norm = 0.0;
  const Scalar tol2 = tol * tol;

  std::vector<unsigned char> rowFree(nRows, 1);
  std::vector<unsigned char> colFree(nCols, 1);
  std::vector<Scalar> aCol(nRows, 0.0);
  std::vector<Scalar> bCol(nCols, 0.0);
  UnsignedInteger pivotRow = 0;
  UnsignedInteger pivotCol = 0;

  // The pivot cutoffs below must be relative to the magnitude of the block,
  // not absolute: a fixed constant would be scale-dependent, exhausting a
  // block scaled down by 1e-6 too early and never terminating one scaled up
  // by 1e6. The reference magnitude is the largest entry of the first row and
  // the first column of the block. An identically zero block yields a zero
  // cutoff, so the (<=) comparisons below still terminate on the first pass.
  Scalar blockScale = 0.0;
  for (UnsignedInteger j = 0; j < nCols; ++j)
    blockScale = std::max(blockScale, std::abs((*p_eval_)(startRow, startCol + j)));
  for (UnsignedInteger i = 0; i < nRows; ++i)
    blockScale = std::max(blockScale, std::abs((*p_eval_)(startRow + i, startCol)));
  const Scalar cutoff = blockScale * 1e-14;

  // Matrix-scale cutoff: a block whose entries are all below the assembly
  // tolerance relative to the matrix magnitude is numerically zero and must
  // NOT be compressed. Without this reference, the relative criteria below
  // keep pivoting the noise floor of such a block (each new pivot captures the
  // same negligible magnitude, so the residual never shrinks relative to the
  // accumulated norm): a locally-correlated kernel then stores every
  // negligible off-diagonal block at the max rank (rank-starved 4D benchmark,
  // ratio 0.185) or, with the adaptive rank default, at the full block rank,
  // blowing up both the storage and the assembly time (4D n=50625 > 300 s).
  // The reference magnitude is the largest diagonal entry in the block's rows
  // and columns: for a covariance matrix it is the marginal variance, i.e.
  // the natural scale against which the assembly epsilon is defined.
  Scalar globalScale = 0.0;
  if (p_diag_)
  {
    for (UnsignedInteger i = startRow; i < startRow + nRows; ++i)
      globalScale = std::max(globalScale, std::abs(p_diag_[i]));
    for (UnsignedInteger j = startCol; j < startCol + nCols; ++j)
      globalScale = std::max(globalScale, std::abs(p_diag_[j]));
  }
  if (globalScale == 0.0)
    globalScale = 1.0;
  const Scalar absoluteCutoff = tol * globalScale;
  if (blockScale < absoluteCutoff)
  {
    // The whole block is below the assembly tolerance: store it as an empty
    // low-rank factor (rank 0). Downstream code guards on rank_ == 0.
    Uout = Matrix(nRows, 0);
    Vout = Matrix(nCols, 0);
    return 0;
  }

  while (rank < maxRank)
  {
    // Residual row at the pivot row: A(pivotRow, :) - sum_l U[pivotRow, l] * V[:, l]
    for (UnsignedInteger j = 0; j < nCols; ++j)
      bCol[j] = (*p_eval_)(startRow + pivotRow, startCol + j);
    for (UnsignedInteger l = 0; l < rank; ++l)
    {
      const Scalar uval = Udata[l * nRows + pivotRow];
      Scalar* const b = bCol.data();
      const Scalar* const vptr = Vdata.data() + l * nCols;
      for (UnsignedInteger j = 0; j < nCols; ++j)
        b[j] -= uval * vptr[j];
    }
    Scalar maxVal = 0.0;
    for (UnsignedInteger j = 0; j < nCols; ++j)
    {
      const Scalar absVal = std::abs(bCol[j]);
      if (colFree[j] && (absVal > maxVal))
      {
        maxVal = absVal;
        pivotCol = j;
      }
    }
    rowFree[pivotRow] = 0;

    if (maxVal <= cutoff)
    {
      // The residual row is (numerically) zero on all free columns. Because
      // pivotRow is the free row with the largest residual in the previous
      // pivot column (see below), a zero residual there means the block is
      // exhausted: for a low-rank block the remaining rows are near-proportional
      // to it and would also be below the cutoff. Terminate immediately
      // instead of re-evaluating every remaining row (which is O(nRows*nCols)
      // kernel evaluations on an already-captured block, e.g. an exactly
      // low-rank 1D block such as a degree-2 exponential-polynomial kernel).
      break;
    }

    // Residual column at the pivot column: A(:, pivotCol) - sum_l U[:, l] * V[pivotCol, l]
    for (UnsignedInteger i = 0; i < nRows; ++i)
      aCol[i] = (*p_eval_)(startRow + i, startCol + pivotCol);
    for (UnsignedInteger l = 0; l < rank; ++l)
    {
      const Scalar vval = Vdata[l * nCols + pivotCol];
      Scalar* const a = aCol.data();
      const Scalar* const uptr = Udata.data() + l * nRows;
      for (UnsignedInteger i = 0; i < nRows; ++i)
        a[i] -= vval * uptr[i];
    }
    colFree[pivotCol] = 0;

    // Same scaling convention as the full-pivoting variant: the pivot column
    // is stored unscaled in U, the pivot row divided by the pivot in V.
    const Scalar pivot = bCol[pivotCol];
    ensureBuffer(rank + 1);
    Scalar uNorm2 = 0.0;
    {
      Scalar* const uptr = Udata.data() + rank * nRows;
      for (UnsignedInteger i = 0; i < nRows; ++i)
      {
        const Scalar v = aCol[i];
        uptr[i] = v;
        uNorm2 += v * v;
      }
    }
    Scalar vNorm2 = 0.0;
    {
      Scalar* const vptr = Vdata.data() + rank * nCols;
      const Scalar invPivot = 1.0 / pivot;
      for (UnsignedInteger j = 0; j < nCols; ++j)
      {
        const Scalar v = bCol[j] * invPivot;
        vptr[j] = v;
        vNorm2 += v * v;
      }
    }
    const Scalar rowcolNorm = uNorm2 * vNorm2;

    norm += rowcolNorm;
    ++rank;

    if (rowcolNorm < tol2 * norm) break;

    if (rank == maxRank) break;

    // Next pivot row: the row with the largest residual in the pivot column,
    // among the rows not yet used as pivots.
    maxVal = 0.0;
    pivotRow = 0;
    for (UnsignedInteger i = 0; i < nRows; ++i)
    {
      const Scalar absVal = std::abs(aCol[i]);
      if (rowFree[i] && (absVal > maxVal))
      {
        maxVal = absVal;
        pivotRow = i;
      }
    }
    if (maxVal <= cutoff)
      break;
  }

  if (rank == 0)
  {
    // The block is numerically zero: no pivot survives the relative cutoff.
    // Keep the deterministic rank-maxRank representation used
    // by lowRankApprox (U = first columns of A, V = I) so downstream code
    // never has to deal with an empty U/V pair.
    if ((maxRank_ > 0) && (maxRank_ < maxRankBound))
      LOGWARN(OSS() << "HODLRMatrix: rank-starved block (" << nRows << "x" << nCols
             << ") reached the max rank " << maxRank_ << " before the assembly tolerance; "
             << "accuracy may be degraded. Set HODLRMatrix-MaxRank to 0 for adaptive (tolerance-driven) rank");
    Uout = Matrix(nRows, maxRank);
    Vout = Matrix(nCols, maxRank);
    MatrixImplementation& UoutImpl = *Uout.getImplementation();
    MatrixImplementation& VoutImpl = *Vout.getImplementation();
    if (nCols <= nRows)
    {
      const UnsignedInteger maxCols = std::min(nCols, maxRank);
      for (UnsignedInteger m = 0; m < maxCols; ++m)
        VoutImpl[m + m * nCols] = 1.0;
      for (UnsignedInteger n = 0; n < nRows; ++n)
        for (UnsignedInteger m = 0; m < maxCols; ++m)
          UoutImpl[n + m * nRows] = (*p_eval_)(startRow + n, startCol + m);
    }
    else
    {
      const UnsignedInteger maxRows = std::min(nRows, maxRank);
      for (UnsignedInteger m = 0; m < maxRows; ++m)
        UoutImpl[m + m * nRows] = 1.0;
      for (UnsignedInteger n = 0; n < maxRows; ++n)
        for (UnsignedInteger m = 0; m < nCols; ++m)
          VoutImpl[m + n * nCols] = (*p_eval_)(startRow + n, startCol + m);
    }
    return maxRank;
  }

  Uout = Matrix(nRows, rank);
  Vout = Matrix(nCols, rank);
  {
    MatrixImplementation& UoutImpl = *Uout.getImplementation();
    MatrixImplementation& VoutImpl = *Vout.getImplementation();
    Scalar* const Uout_data = &UoutImpl[0];
    Scalar* const Vout_data = &VoutImpl[0];
    for (UnsignedInteger l = 0; l < rank; ++l)
    {
      const Scalar* const up = Udata.data() + l * nRows;
      const Scalar* const vp = Vdata.data() + l * nCols;
      std::copy(up, up + nRows, Uout_data + l * nRows);
      std::copy(vp, vp + nCols, Vout_data + l * nCols);
    }
  }

  if ((maxRank_ > 0) && (rank >= maxRank_) && (rank < maxRankBound))
    LOGWARN(OSS() << "HODLRMatrix: rank-starved block (" << nRows << "x" << nCols
           << ") reached the max rank " << maxRank_ << " before the assembly tolerance; "
           << "accuracy may be degraded. Set HODLRMatrix-MaxRank to 0 for adaptive (tolerance-driven) rank");

  return rank;
}

void HODLRNode::recompressLowRank(Matrix& Uout, Matrix& Vout,
    UnsignedInteger startRow, UnsignedInteger nRows,
    UnsignedInteger startCol, UnsignedInteger nCols,
    const std::vector<HODLRCorrectedEvaluator::Correction>& corrections,
    Scalar tolerance)
{
  // Recompress the low-rank block A01' = Uout * Vout^T - sum_k (UK_k * U1_k^T)
  // where Uout/Vout approximate the pure kernel block (from the initial ACA)
  // and each correction term is an exact low-rank matrix of the hierarchical
  // Schur complement. Instead of re-running ACA with a corrected evaluator,
  // the correction is subtracted algebraically and the sum of low-rank terms
  // is truncated back to the assembly tolerance by SVD.
  //
  // The block row range [startRow, startRow+nRows) and column range
  // [startCol, startCol+nCols) are assumed fully covered by every correction,
  // which is guaranteed by the Schur complement recursion of computeCholesky.
  const UnsignedInteger rankIn = Uout.getNbColumns();

  UnsignedInteger totalRank = rankIn;
  for (const auto& corr : corrections)
    totalRank += corr.rank;
  if (totalRank == 0)
    return;

  const auto ft0 = std::chrono::steady_clock::now();
  ++g_factorTiming.nRecomp;
  g_factorTiming.nRowsSum += nRows;
  g_factorTiming.totalRankSum += totalRank;
  FactorTiming::BlockTiming bt;
  bt.nRows = static_cast<unsigned>(nRows);
  bt.nCols = static_cast<unsigned>(nCols);
  bt.totalRank = static_cast<unsigned>(totalRank);

  // Ucat: nRows x totalRank, Vcat: nCols x totalRank
  // A rank-0 base block (negligible at assembly) is corrected block alone:
  // A01' = - sum_k (UK_k * U1_k^T), so only the correction columns are
  // stacked, with the base columns contributing nothing.
  MatrixImplementation Ucat(nRows, totalRank);
  MatrixImplementation Vcat(nCols, totalRank);
  Point uNormSq(totalRank);
  Point vNormSq(totalRank);
  {
    Scalar* Ucat_data = &Ucat[0];
    Scalar* Vcat_data = &Vcat[0];
    if (rankIn > 0)
    {
      const Scalar* Uout_data = &(*Uout.getImplementation())[0];
      const Scalar* Vout_data = &(*Vout.getImplementation())[0];
      const UnsignedInteger ldUout = Uout.getNbRows();
      const UnsignedInteger ldVout = Vout.getNbRows();
      for (UnsignedInteger c = 0; c < rankIn; ++c)
      {
        Scalar* uc = Ucat_data + c * nRows;
        Scalar* vc = Vcat_data + c * nCols;
        const Scalar* us = Uout_data + c * ldUout;
        const Scalar* vs = Vout_data + c * ldVout;
        Scalar u2 = 0.0;
        Scalar v2 = 0.0;
        for (UnsignedInteger r = 0; r < nRows; ++r)
        {
          uc[r] = us[r];
          u2 += us[r] * us[r];
        }
        for (UnsignedInteger r = 0; r < nCols; ++r)
        {
          vc[r] = vs[r];
          v2 += vs[r] * vs[r];
        }
        uNormSq[c] = u2;
        vNormSq[c] = v2;
      }
    }
    UnsignedInteger col = rankIn;
    for (const auto& corr : corrections)
    {
      if (corr.rank == 0) continue;
      const UnsignedInteger rowOffset = startRow - corr.offset;
      const UnsignedInteger colOffset = startCol - corr.offset;
      const Scalar* uk_data = &(*corr.UK.getImplementation())[0];
      const Scalar* u1_data = &(*corr.U1.getImplementation())[0];
      const UnsignedInteger ldCorr = corr.UK.getNbRows();
      for (UnsignedInteger c = 0; c < corr.rank; ++c)
      {
        Scalar* uc = Ucat_data + col * nRows;
        Scalar* vc = Vcat_data + col * nCols;
        const Scalar* us = uk_data + rowOffset + c * ldCorr;
        const Scalar* vs = u1_data + colOffset + c * ldCorr;
        Scalar u2 = 0.0;
        Scalar v2 = 0.0;
        for (UnsignedInteger r = 0; r < nRows; ++r)
        {
          const Scalar x = us[r];
          uc[r] = -x;
          u2 += x * x;
        }
        for (UnsignedInteger r = 0; r < nCols; ++r)
        {
          const Scalar x = vs[r];
          vc[r] = x;
          v2 += x * x;
        }
        uNormSq[col] = u2;
        vNormSq[col] = v2;
        ++col;
      }
    }
    totalRank = col;
  }

  // Optional pre-truncation of the stacked columns before the QR+Gram+SVD.
  // Each stacked column j contributes the rank-1 term u_j v_j^T to the block
  // A01'. By the triangle inequality the sum of the column weights
  // w_j = ||u_j||_2 * ||v_j||_2 is an upper bound of ||A01'||_F, so dropping
  // columns whose cumulative weight stays below a small fraction of the
  // tolerance budget perturbs the block by at most that amount. The exact SVD
  // on the reduced stack then still meets the assembly tolerance (with <10%
  // slack). The norm squares are accumulated during the stack build above.
  if (ResourceMap::GetAsBool("HODLRMatrix-StackTruncation") && (tolerance > 0.0) && (totalRank > 0))
  {
    Point weights(totalRank);
    Scalar weightSum = 0.0;
    for (UnsignedInteger c = 0; c < totalRank; ++c)
    {
      weights[c] = std::sqrt(uNormSq[c]) * std::sqrt(vNormSq[c]);
      weightSum += weights[c];
    }
    if (weightSum > 0.0)
    {
      const Scalar truncFactor = ResourceMap::GetAsScalar("HODLRMatrix-StackTruncationFactor");
      const Scalar dropBudget = truncFactor * tolerance * weightSum;
      std::vector<UnsignedInteger> order(totalRank);
      for (UnsignedInteger c = 0; c < totalRank; ++c)
        order[c] = c;
      std::sort(order.begin(), order.end(),
                [&weights](UnsignedInteger a, UnsignedInteger b)
                { return weights[a] < weights[b]; });
      std::vector<char> drop(totalRank, 0);
      Scalar dropped = 0.0;
      UnsignedInteger nDropped = 0;
      for (const UnsignedInteger c : order)
      {
        if ((nDropped + 1 >= totalRank) || (dropped + weights[c] > dropBudget))
          break;
        drop[c] = 1;
        dropped += weights[c];
        ++nDropped;
      }
      const UnsignedInteger kept = totalRank - nDropped;
      if (kept < totalRank)
      {
        // Compact the kept columns in place (k <= c, forward copies are safe).
        Scalar* Ud = &Ucat[0];
        Scalar* Vd = &Vcat[0];
        UnsignedInteger k = 0;
        for (UnsignedInteger c = 0; c < totalRank; ++c)
        {
          if (drop[c])
            continue;
          if (k != c)
          {
            std::copy(Ud + c * nRows, Ud + c * nRows + nRows, Ud + k * nRows);
            std::copy(Vd + c * nCols, Vd + c * nCols + nCols, Vd + k * nCols);
          }
          ++k;
        }
        totalRank = kept;
      }
      bt.kept = static_cast<unsigned>(kept);
    }
    else
      bt.kept = static_cast<unsigned>(totalRank);
  }
  else
    bt.kept = static_cast<unsigned>(totalRank);
  g_factorTiming.recompCopy += std::chrono::duration<double>(std::chrono::steady_clock::now() - ft0).count();
  bt.copy = std::chrono::duration<double>(std::chrono::steady_clock::now() - ft0).count();
  g_factorTiming.k1sum += std::min(nRows, totalRank);

  // QR of Ucat in place: Ucat = Q * R (thin QR, k1 = min(nRows, totalRank)).
  // Q is never formed explicitly: Ucat keeps the Householder reflectors (used
  // later by dormqr) and R is extracted as the leading k1 x totalRank upper
  // triangle.
  const UnsignedInteger k1 = std::min(nRows, totalRank);
  MatrixImplementation Rcat(k1, totalRank);
  Point tau(k1);
  {
    const auto t0 = std::chrono::steady_clock::now();
    int m = static_cast<int>(nRows);
    int n = static_cast<int>(totalRank);
    int lda = static_cast<int>(nRows);
    double lworkQuery = 0.0;
    int lwork = -1;
    int info = 0;
    dgeqrf_(&m, &n, &Ucat[0], &lda, &tau[0], &lworkQuery, &lwork, &info);
    lwork = static_cast<int>(lworkQuery);
    Point work(lwork);
    dgeqrf_(&m, &n, &Ucat[0], &lda, &tau[0], &work[0], &lwork, &info);
    if (info != 0)
      throw InternalException(HERE) << "dgeqrf failed (info=" << info << ")";
    // Extract R (upper trapezoid) from the Householder storage of Ucat.
    {
      const Scalar* Ud = &Ucat[0];
      Scalar* Rd = &Rcat[0];
      for (UnsignedInteger c = 0; c < totalRank; ++c)
      {
        const UnsignedInteger lim = std::min(c + 1, k1);
        std::copy(Ud + c * nRows, Ud + c * nRows + lim, Rd + c * k1);
      }
    }
    g_factorTiming.recompQR += std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    bt.qr = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    bt.k1 = static_cast<unsigned>(k1);
  }
  // SVD of M = Vcat * R^T (nCols x k1) through its Gram matrix G = M^T * M.
  // Two algebraically equivalent routes:
  //  - direct:  form M, then G = M^T M              (cost O(nCols*k1*totalRank))
  //  - merged:  H = Vcat^T Vcat, G = R * H * R^T    (cost O(nCols*totalRank^2))
  // The merged route avoids the large nCols x k1 intermediate and is cheaper
  // when the block is much wider than the rank (nCols > 2*k1).
  const bool useMergedGram = (nCols > 2 * k1);
  MatrixImplementation G(k1, k1);
  MatrixImplementation Mcat;  // only allocated in the direct route
  {
    const auto t0 = std::chrono::steady_clock::now();
    if (useMergedGram)
    {
      MatrixImplementation H(totalRank, totalRank);
      {
        int mm = static_cast<int>(totalRank);
        int nn = static_cast<int>(totalRank);
        int kk = static_cast<int>(nCols);
        double one = 1.0;
        double zero = 0.0;
        int ldV = static_cast<int>(nCols);
        int ldH = static_cast<int>(totalRank);
        HODLRDgemm("T", "N", &mm, &nn, &kk, &one, &Vcat[0], &ldV, &Vcat[0], &ldV, &zero, &H[0], &ldH);
      }
      MatrixImplementation Tmp(totalRank, k1);
      {
        int mm = static_cast<int>(totalRank);
        int nn = static_cast<int>(k1);
        int kk = static_cast<int>(totalRank);
        double one = 1.0;
        double zero = 0.0;
        int ldH = static_cast<int>(totalRank);
        int ldR = static_cast<int>(k1);
        int ldT = static_cast<int>(totalRank);
        HODLRDgemm("N", "T", &mm, &nn, &kk, &one, &H[0], &ldH, &Rcat[0], &ldR, &zero, &Tmp[0], &ldT);
      }
      {
        int mm = static_cast<int>(k1);
        int nn = static_cast<int>(k1);
        int kk = static_cast<int>(totalRank);
        double one = 1.0;
        double zero = 0.0;
        int ldR = static_cast<int>(k1);
        int ldT = static_cast<int>(totalRank);
        int ldG = static_cast<int>(k1);
        HODLRDgemm("N", "N", &mm, &nn, &kk, &one, &Rcat[0], &ldR, &Tmp[0], &ldT, &zero, &G[0], &ldG);
      }
    }
    else
    {
      Mcat = MatrixImplementation(nCols, k1);
      {
        int mm = static_cast<int>(nCols);
        int nn = static_cast<int>(k1);
        int kk = static_cast<int>(totalRank);
        double one = 1.0;
        double zero = 0.0;
        int ldV = static_cast<int>(nCols);
        int ldR = static_cast<int>(k1);
        int ldM = static_cast<int>(nCols);
        HODLRDgemm("N", "T", &mm, &nn, &kk, &one, &Vcat[0], &ldV, &Rcat[0], &ldR, &zero, &Mcat[0], &ldM);
      }
      {
        int mm = static_cast<int>(k1);
        int nn = static_cast<int>(k1);
        int kk = static_cast<int>(nCols);
        double one = 1.0;
        double zero = 0.0;
        int ldM = static_cast<int>(nCols);
        int ldG = static_cast<int>(k1);
        HODLRDgemm("T", "N", &mm, &nn, &kk, &one, &Mcat[0], &ldM, &Mcat[0], &ldM, &zero, &G[0], &ldG);
      }
    }
    g_factorTiming.recompGram += std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    bt.gram = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  }

  Point eigenvalues(k1);
  {
    const auto t0 = std::chrono::steady_clock::now();
    int n = static_cast<int>(k1);
    char jobz = 'V';
    char uplo = 'L';
    int ljobz = 1;
    int luplo = 1;
    int lwork = -1;
    int liwork = -1;
    double lworkQuery = 0.0;
    int liworkQuery = 0;
    int info = 0;
    dsyevd_(&jobz, &uplo, &n, &G[0], &n, &eigenvalues[0], &lworkQuery, &lwork, &liworkQuery, &liwork, &info, &ljobz, &luplo);
    lwork = static_cast<int>(lworkQuery);
    liwork = static_cast<int>(liworkQuery);
    Point work(lwork);
    std::vector<int> iwork(static_cast<std::size_t>(liwork));
    dsyevd_(&jobz, &uplo, &n, &G[0], &n, &eigenvalues[0], &work[0], &lwork, &iwork[0], &liwork, &info, &ljobz, &luplo);
    if (info != 0)
      throw InternalException(HERE) << "dsyevd failed to converge (info=" << info << ")";
    g_factorTiming.recompSyevd += std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    bt.syevd = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  }
  const UnsignedInteger nSing = std::min(k1, nCols);
  const UnsignedInteger maxRank = std::min(nRows, nCols);
  UnsignedInteger rankOut = std::min(nSing, maxRank);
  Scalar totalNorm2 = 0.0;
  for (UnsignedInteger i = 0; i < k1; ++i)
    if (eigenvalues[i] > 0.0)
      totalNorm2 += eigenvalues[i];
  if ((rankOut > 0) && (tolerance > 0.0) && (totalNorm2 > 0.0))
  {
    // Keep the largest r singular values such that the Frobenius norm of the
    // truncated part is below tolerance relative to the block norm (same
    // criterion as the ACA stopping rule).
    const Scalar tol2 = tolerance * tolerance;
    Scalar droppedNorm2 = 0.0;
    rankOut = nSing;
    for (UnsignedInteger i = k1 - nSing; i < k1; ++i)
    {
      const Scalar lambda = (eigenvalues[i] > 0.0) ? eigenvalues[i] : 0.0;
      droppedNorm2 += lambda;
      if (droppedNorm2 <= tol2 * totalNorm2)
        rankOut = k1 - 1 - i;
      else
        break;
    }
    if (rankOut == 0) rankOut = 1;
    if (rankOut > maxRank) rankOut = maxRank;
  }
  bt.rankOut = static_cast<unsigned>(rankOut);

  // Eigenvectors live in the columns of G (ascending eigenvalues); take the
  // rankOut largest, in descending order (columns k1-1 down to k1-rankOut),
  // scaled by s and 1/s so that U_new = Qcat * Ps, V_new = Mcat * Pi (direct
  // route) or V_new = Vcat * (Rcat^T * Pi) (merged route).
  Uout = Matrix(nRows, rankOut);
  Vout = Matrix(nCols, rankOut);
  if (rankOut > 0)
  {
    const auto t0 = std::chrono::steady_clock::now();
    const Scalar* G_data = &G[0];
    MatrixImplementation Ps(k1, rankOut);
    MatrixImplementation Pi(k1, rankOut);
    Scalar* Ps_data = &Ps[0];
    Scalar* Pi_data = &Pi[0];
    for (UnsignedInteger c = 0; c < rankOut; ++c)
    {
      const UnsignedInteger col = k1 - 1 - c;
      const Scalar s = std::sqrt(eigenvalues[col] > 0.0 ? eigenvalues[col] : 0.0);
      const Scalar invS = (s > 0.0) ? (1.0 / s) : 0.0;
      for (UnsignedInteger r = 0; r < k1; ++r)
      {
        const Scalar p = G_data[r + col * k1];
        Ps_data[r + c * k1] = p * s;
        Pi_data[r + c * k1] = p * invS;
      }
    }
    // U_new = Qcat * Ps: apply the Householder reflectors stored in Ucat
    // directly to Ps with dormqr instead of materializing the nRows x k1
    // orthogonal factor with dorgqr and then multiplying. This reduces the
    // flop count from O(nRows * k1^2) to O(nRows * k1 * rankOut). Ps is laid
    // in the top k1 rows of the (zero-initialized) Uout buffer, so dormqr
    // writes the result U_new in place.
    Scalar* Uout_data = &(*Uout.getImplementation())[0];
    std::fill(Uout_data, Uout_data + nRows * rankOut, 0.0);
    for (UnsignedInteger c = 0; c < rankOut; ++c)
      std::copy(Ps_data + c * k1, Ps_data + c * k1 + k1, Uout_data + c * nRows);
    {
      char side = 'L';
      char trans = 'N';
      int mm = static_cast<int>(nRows);
      int nn = static_cast<int>(rankOut);
      int kk = static_cast<int>(k1);
      int lda = static_cast<int>(nRows);
      int ldc = static_cast<int>(nRows);
      int lside = 1;
      int ltrans = 1;
      double lworkQuery = 0.0;
      int lwork = -1;
      int info = 0;
      dormqr_(&side, &trans, &mm, &nn, &kk, &Ucat[0], &lda, &tau[0], Uout_data, &ldc,
              &lworkQuery, &lwork, &info, &lside, &ltrans);
      lwork = static_cast<int>(lworkQuery);
      Point work(lwork);
      dormqr_(&side, &trans, &mm, &nn, &kk, &Ucat[0], &lda, &tau[0], Uout_data, &ldc,
              &work[0], &lwork, &info, &lside, &ltrans);
      if (info != 0)
        throw InternalException(HERE) << "dormqr failed (info=" << info << ")";
    }
    Scalar* Vout_data = &(*Vout.getImplementation())[0];
    if (useMergedGram)
    {
      MatrixImplementation RPi(totalRank, rankOut);
      {
        int mm = static_cast<int>(totalRank);
        int nn = static_cast<int>(rankOut);
        int kk = static_cast<int>(k1);
        double one = 1.0;
        double zero = 0.0;
        int ldR = static_cast<int>(k1);
        int ldP = static_cast<int>(k1);
        int ldT = static_cast<int>(totalRank);
        HODLRDgemm("T", "N", &mm, &nn, &kk, &one, &Rcat[0], &ldR, &Pi[0], &ldP, &zero, &RPi[0], &ldT);
      }
      {
        int mm = static_cast<int>(nCols);
        int nn = static_cast<int>(rankOut);
        int kk = static_cast<int>(totalRank);
        double one = 1.0;
        double zero = 0.0;
        int ldV = static_cast<int>(nCols);
        int ldT = static_cast<int>(totalRank);
        int ldVn = static_cast<int>(nCols);
        HODLRDgemm("N", "N", &mm, &nn, &kk, &one, &Vcat[0], &ldV, &RPi[0], &ldT, &zero, Vout_data, &ldVn);
      }
    }
    else
    {
      int mm = static_cast<int>(nCols);
      int nn = static_cast<int>(rankOut);
      int kk = static_cast<int>(k1);
      double one = 1.0;
      double zero = 0.0;
      int ldM = static_cast<int>(nCols);
      int ldP = static_cast<int>(k1);
      int ldV = static_cast<int>(nCols);
      HODLRDgemm("N", "N", &mm, &nn, &kk, &one, &Mcat[0], &ldM, &Pi[0], &ldP, &zero, Vout_data, &ldV);
    }
    g_factorTiming.recompFinal += std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    bt.final = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  }
  g_factorTiming.blocks.push_back(bt);
}

// --- HODLRCorrectedEvaluator factory implementations ---

Pointer<HODLRCorrectedEvaluator> HODLRCorrectedEvaluator::create(
    Pointer<const HODLREntryEvaluator> original,
    UnsignedInteger offset,
    UnsignedInteger size,
    const Matrix& U1,
    const Matrix& K,
    Scalar lambda)
{
  const UnsignedInteger nRows = U1.getNbRows();
  const UnsignedInteger rank = K.getNbRows();
  Matrix UK(nRows, rank);
  if (rank > 0)
  {
    int m = static_cast<int>(nRows);
    int k = static_cast<int>(rank);
    int l = static_cast<int>(rank);
    double one = 1.0, zero = 0.0;
    HODLRDgemm("N", "N", &m, &k, &l, &one,
           const_cast<double*>(&U1(0, 0)), &m,
           const_cast<double*>(&K(0, 0)), &l,
           &zero, &UK(0, 0), &m);
  }

  Correction corr;
  corr.offset = offset;
  corr.size = size;
  corr.rank = rank;
  corr.U1 = Matrix(nRows, rank);
  if (rank > 0)
    std::copy(&U1(0, 0), &U1(0, 0) + nRows * rank, &corr.U1(0, 0));
  corr.UK = UK;
  corr.lambda = lambda;

  std::vector<Correction> corrections;
  corrections.push_back(corr);

  return new HODLRCorrectedEvaluator(original, offset, size, corrections);
}

Pointer<HODLRCorrectedEvaluator> HODLRCorrectedEvaluator::flatten(
    const HODLRCorrectedEvaluator& existing,
    const Matrix& newU1,
    const Matrix& newK,
    Scalar newLambda,
    UnsignedInteger newOffset,
    UnsignedInteger newSize)
{
  // Build UK = newU1 * newK via dgemm
  const UnsignedInteger nRows = newU1.getNbRows();
  const UnsignedInteger rank = newK.getNbRows();
  Matrix UK(nRows, rank);
  if (rank > 0)
  {
    int m = static_cast<int>(nRows);
    int k = static_cast<int>(rank);
    int l = static_cast<int>(rank);
    double one = 1.0, zero = 0.0;
    HODLRDgemm("N", "N", &m, &k, &l, &one,
           const_cast<double*>(&newU1(0, 0)), &m,
           const_cast<double*>(&newK(0, 0)), &l,
           &zero, &UK(0, 0), &m);
  }

  // Copy existing corrections
  std::vector<Correction> allCorrections = existing.corrections_;

  // Append new correction
  Correction newCorr;
  newCorr.offset = newOffset;
  newCorr.size = newSize;
  newCorr.rank = rank;
  newCorr.U1 = Matrix(nRows, rank);
  if (rank > 0)
    std::copy(&newU1(0, 0), &newU1(0, 0) + nRows * rank, &newCorr.U1(0, 0));
  newCorr.UK = UK;
  newCorr.lambda = newLambda;
  allCorrections.push_back(newCorr);

  return new HODLRCorrectedEvaluator(existing.getOriginal(),
                                     existing.getOffset(),
                                     existing.getSize(),
                                     allCorrections);
}

void HODLRNode::computeCholesky()
{
  g_factorTiming.printProfile = ResourceMap::GetAsBool("HODLRMatrix-ProfileFactorization");
  computeCholesky(std::vector<HODLRCorrectedEvaluator::Correction>());
}

void HODLRNode::computeCholesky(const std::vector<HODLRCorrectedEvaluator::Correction>& corrections)
{
  HODLRBlasGuard blasGuard;

  if (isLeaf_)
  {
    const auto t0 = std::chrono::steady_clock::now();
    factorizeLeafCholesky(corrections);
    g_factorTiming.leaf += std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    ++g_factorTiming.nLeaf;
    logDet_ = 0.0;
    {
      const MatrixImplementation& Sfact = *Sfactor_.getImplementation();
      for (UnsignedInteger n = 0; n < size_; ++n)
        logDet_ += std::log(Sfact[n + n * size_]);
    }
    return;
  }

  const UnsignedInteger s0 = size_ / 2;
  const UnsignedInteger s1 = size_ - s0;

  // Correct this node's off-diagonal block A01' = A01 - sum corrections.
  // The kernel approximation factors computed at assembly time are reused and
  // recompressed by SVD, avoiding a full re-assembly with a corrected evaluator.
  if (!corrections.empty() && ResourceMap::GetAsBool("HODLRMatrix-RecompressCorrections"))
  {
    const auto t0 = std::chrono::steady_clock::now();
    recompressLowRank(U_[1], V_[0], start_ + s0, s1, start_, s0, corrections, tolerance_);
    g_factorTiming.recomp += std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    rank_ = U_[1].getNbColumns();
    U_[0] = V_[0];
    V_[1] = U_[1];
  }

  // 1. Recursively compute L00 = chol(A00')
  p_child0_->computeCholesky(corrections);

  totalRank_ = rank_ + p_child0_->getTotalRank();
  numLeaves_ = p_child0_->getNumLeaves();

  if (rank_ > 0)
  {
    // 2. Compute W = L00^{-1} * V_[0]
    W_ = V_[0];
    {
      const auto t0 = std::chrono::steady_clock::now();
      p_child0_->applyInverseFactor(W_);
      g_factorTiming.W += std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    }

    // 3. Build K = W^T * W  (rank_ x rank_) via dgemm
    Matrix K(rank_, rank_);
    {
      int mK = static_cast<int>(rank_);
      int nK = static_cast<int>(rank_);
      int kK = static_cast<int>(s0);
      double one = 1.0, zero = 0.0;
      int ldW = static_cast<int>(s0);
      int ldK = static_cast<int>(rank_);
      const auto t0 = std::chrono::steady_clock::now();
      HODLRDgemm("T", "N", &mK, &nK, &kK, &one, &W_(0, 0), &ldW, &W_(0, 0), &ldW, &zero, &K(0, 0), &ldK);
      g_factorTiming.Kgram += std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    }

    // 4. Schur complement: factorize A11' = A11 - U1 * K * U1^T
    // Start with no regularization; on first failure jump directly to a
    // meaningful value (1e-4) to avoid wasting attempts on tiny lambdas.
    // Then double geometrically, capped at maxLambda0.
    Scalar lambda = 0.0;
    const Bool useDenseFallback = (denseThreshold_ > 0) && (s1 <= denseThreshold_);
    const Scalar maxLambda0 = ResourceMap::GetAsScalar("HODLRMatrix-MaxRegularization");
    const Scalar lambdaFactor = ResourceMap::GetAsScalar("HODLRMatrix-RegularizationFactor");
    const UnsignedInteger maxAttempts = ResourceMap::GetAsUnsignedInteger("HODLRMatrix-RegularizationAttempts");
    if (maxAttempts == 0)
      throw InvalidArgumentException(HERE) << "HODLRMatrix-RegularizationAttempts must be positive";
    for (UnsignedInteger attempt = 0; attempt < maxAttempts; ++attempt)
    {
      if (useDenseFallback)
      {
        p_child1_ = new HODLRNode(p_eval_, p_diag_, start_ + s0, s1,
                                  s1 + 1, maxRank_, tolerance_, 1, this);
        p_child1_->setShift(shift_);
        try
        {
          p_child1_->factorizeLeafCholeskyCorrected(K, U_[1], lambda, corrections);
          numLeaves_ += 1;
          break;
        }
        catch (const InternalException&)
        {
          if (lambda == 0.0)
            lambda = 1e-12;
          else if (lambda < maxLambda0)
            lambda = std::min(lambda * lambdaFactor, maxLambda0);
          else
            lambda *= lambdaFactor;
          if (attempt == maxAttempts - 1)
            throw InternalException(HERE) << "Dense Schur complement not SPD even with lambda=" << lambda;
        }
      }
      else
      {
        // Hierarchical Schur complement: propagate the new correction to child1's
        // subtree in place. The factors assembled with the plain kernel evaluator
        // are recompressed by SVD at each level (see recompressLowRank), so no
        // tree rebuild with a corrected evaluator is required.
        std::vector<HODLRCorrectedEvaluator::Correction> childCorrections = corrections;
        HODLRCorrectedEvaluator::Correction newCorr;
        newCorr.offset = start_ + s0;
        newCorr.size = s1;
        newCorr.rank = rank_;
        newCorr.U1 = U_[1];
        {
          Matrix UK(s1, rank_);
          int m = static_cast<int>(s1);
          int k = static_cast<int>(rank_);
          int l = static_cast<int>(rank_);
          double one = 1.0, zero = 0.0;
          const auto t0 = std::chrono::steady_clock::now();
          HODLRDgemm("N", "N", &m, &k, &l, &one,
                 const_cast<double*>(&U_[1](0, 0)), &m,
                 const_cast<double*>(&K(0, 0)), &l,
                 &zero, &UK(0, 0), &m);
          g_factorTiming.UK += std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
          newCorr.UK = UK;
        }
        newCorr.lambda = lambda;
        childCorrections.push_back(newCorr);
        try
        {
          p_child1_->computeCholesky(childCorrections);
          totalRank_ += p_child1_->getTotalRank();
          numLeaves_ += p_child1_->getNumLeaves();
          break;
        }
        catch (const InternalException&)
        {
          if (lambda == 0.0)
            lambda = 1e-12;
          else if (lambda < maxLambda0)
            lambda = std::min(lambda * lambdaFactor, maxLambda0);
          else
            lambda *= lambdaFactor;
          if (attempt == maxAttempts - 1)
            throw InternalException(HERE) << "Hierarchical Schur complement not SPD even with lambda=" << lambda;
        }
      }
    }
  }
  else
  {
    p_child1_->computeCholesky(corrections);
    totalRank_ += p_child1_->getTotalRank();
    numLeaves_ += p_child1_->getNumLeaves();
  }

  logDet_ = p_child0_->getLogDeterminant() + p_child1_->getLogDeterminant();
}

void HODLRNode::factorizeLeafCholesky(const std::vector<HODLRCorrectedEvaluator::Correction>& corrections)
{
  // Leaf: assemble the corrected dense matrix from the kernel block cached
  // at construction (deep-copied so the in-place dgemm corrections and dpotrf
  // below do not destroy leafKernel_), then apply every accumulated Schur
  // complement correction via batch dgemm.
  Sfactor_ = Matrix(*leafKernel_.getImplementation());
  {
    MatrixImplementation& Sfact = *Sfactor_.getImplementation();

    // Apply global shift to all diagonals (from addIdentity)
    if (shift_ != 0.0)
      for (UnsignedInteger i = 0; i < size_; ++i)
        Sfact[i + i * size_] += shift_;

    // Apply each correction via batch dgemm
    const UnsignedInteger n = size_;
    const UnsignedInteger leafStart = start_;
    const auto t0 = std::chrono::steady_clock::now();
    for (const auto& corr : corrections)
    {
      const SignedInteger localOffset =
          static_cast<SignedInteger>(corr.offset) - static_cast<SignedInteger>(leafStart);
      const SignedInteger localEnd =
          localOffset + static_cast<SignedInteger>(corr.size);
      if (localEnd <= 0) continue;   // correction entirely before leaf
      if (localOffset >= static_cast<SignedInteger>(n)) break;  // correction beyond leaf

      const UnsignedInteger i0 = static_cast<UnsignedInteger>(std::max(SignedInteger(0), localOffset));
      const UnsignedInteger i1 = static_cast<UnsignedInteger>(std::min(static_cast<SignedInteger>(n), localEnd));
      const UnsignedInteger localN = i1 - i0;

      if (localN == 0 || corr.rank == 0) continue;

      const UnsignedInteger ukRowOffset = i0 - static_cast<UnsignedInteger>(localOffset);

      // Sfact[i0:i1, i0:i1] -= UK[ukRowOffset:ukRowOffset+localN, :] * U1[ukRowOffset:ukRowOffset+localN, :]^T
      int m = static_cast<int>(localN);
      int k = static_cast<int>(corr.rank);
      int nMat = static_cast<int>(localN);
      double neg = -1.0, one = 1.0;
      int ldUK = static_cast<int>(corr.size);
      int ldU1 = static_cast<int>(corr.size);
      int ldS = static_cast<int>(n);
      HODLRDgemm("N", "T", &m, &nMat, &k, &neg,
             const_cast<double*>(&corr.UK(ukRowOffset, 0)), &ldUK,
             const_cast<double*>(&corr.U1(ukRowOffset, 0)), &ldU1,
             &one, &Sfact[i0 + i0 * n], &ldS);

      // Add lambda to diagonal within correction range
      if (corr.lambda != 0.0)
      {
        for (UnsignedInteger ii = i0; ii < i1; ++ii)
          Sfact[ii + ii * n] += corr.lambda;
      }
    }
    g_factorTiming.leafCorr += std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    ++g_factorTiming.nLeafCorr;
  }

  int info = 0;
  int n = static_cast<int>(size_);
  {
    MatrixImplementation& Sfact2 = *Sfactor_.getImplementation();
    const auto t0 = std::chrono::steady_clock::now();
    HODLRDpotrf("L", &n, &Sfact2[0], &n, &info);
    g_factorTiming.dpotrf += std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  }
  if (info != 0)
    throw InternalException(HERE) << "Cholesky factorization failed, info=" << info;
}

void HODLRNode::factorizeLeafCholeskyCorrected(const Matrix& K, const Matrix& U1, Scalar lambda,
                                               const std::vector<HODLRCorrectedEvaluator::Correction>& corrections)
{
  // Directly assemble A11' = A11 - sum corrections - U1 * K * U1^T + lambda * I
  // Uses the ORIGINAL evaluator (p_eval_) for A11 entries, computes
  // the low-rank corrections via dgemm for numerical stability.
  const UnsignedInteger n = size_;
  const UnsignedInteger rank = K.getNbRows();
  const auto ft0 = std::chrono::steady_clock::now();
  ++g_factorTiming.nLeafCorrected;

  Sfactor_ = Matrix(*leafKernel_.getImplementation());
  MatrixImplementation& Sfact = *Sfactor_.getImplementation();

  // 1b. Apply the accumulated Schur complement corrections
  const UnsignedInteger leafStart = start_;
  for (const auto& corr : corrections)
  {
    const SignedInteger localOffset =
        static_cast<SignedInteger>(corr.offset) - static_cast<SignedInteger>(leafStart);
    const SignedInteger localEnd =
        localOffset + static_cast<SignedInteger>(corr.size);
    if (localEnd <= 0) continue;
    if (localOffset >= static_cast<SignedInteger>(n)) break;

    const UnsignedInteger i0 = static_cast<UnsignedInteger>(std::max(SignedInteger(0), localOffset));
    const UnsignedInteger i1 = static_cast<UnsignedInteger>(std::min(static_cast<SignedInteger>(n), localEnd));
    const UnsignedInteger localN = i1 - i0;

    if (localN == 0 || corr.rank == 0) continue;

    const UnsignedInteger ukRowOffset = i0 - static_cast<UnsignedInteger>(localOffset);
    int m = static_cast<int>(localN);
    int k = static_cast<int>(corr.rank);
    int nMat = static_cast<int>(localN);
    double neg = -1.0, one = 1.0;
    int ldUK = static_cast<int>(corr.size);
    int ldU1 = static_cast<int>(corr.size);
    int ldS = static_cast<int>(n);
    HODLRDgemm("N", "T", &m, &nMat, &k, &neg,
           const_cast<double*>(&corr.UK(ukRowOffset, 0)), &ldUK,
           const_cast<double*>(&corr.U1(ukRowOffset, 0)), &ldU1,
           &one, &Sfact[i0 + i0 * n], &ldS);

    if (corr.lambda != 0.0)
      for (UnsignedInteger ii = i0; ii < i1; ++ii)
        Sfact[ii + ii * n] += corr.lambda;
  }

  // 2. Compute correction = U1 * K * U1^T via dgemm
  if (rank > 0)
  {
    // temp = U1 * K  (n x rank)
    Matrix temp(n, rank);
    {
      int m = static_cast<int>(n);
      int k = static_cast<int>(rank);
      int l = static_cast<int>(rank);
      double one = 1.0, zero = 0.0;
      HODLRDgemm("N", "N", &m, &k, &l, &one,
             const_cast<double*>(&U1(0, 0)), &m,
             const_cast<double*>(&K(0, 0)), &l,
             &zero, &temp(0, 0), &m);
    }
    // Sfact -= temp * U1^T  (n x n)
    {
      int m = static_cast<int>(n);
      int k = static_cast<int>(rank);
      int l = static_cast<int>(n);
      double neg = -1.0, one = 1.0;
      HODLRDgemm("N", "T", &m, &l, &k, &neg,
             &temp(0, 0), &m,
             const_cast<double*>(&U1(0, 0)), &m,
             &one, &Sfact[0], &m);
    }
  }

  // 3. Add lambda regularization to diagonal
  if (lambda != 0.0)
  {
    for (UnsignedInteger i = 0; i < n; ++i)
      Sfact[i + i * n] += lambda;
  }

  // 3b. Add global shift (from addIdentity) to diagonal
  if (shift_ != 0.0)
  {
    for (UnsignedInteger i = 0; i < n; ++i)
      Sfact[i + i * n] += shift_;
  }

  // 4. Cholesky factorization
  int info = 0;
  int nn = static_cast<int>(n);
  HODLRDpotrf("L", &nn, &Sfact[0], &nn, &info);
  if (info != 0)
    throw InternalException(HERE) << "Cholesky factorization failed, info=" << info;

  // 5. Accumulate log-det contribution: sum(log(diag(L))) for this leaf
  logDet_ = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
    logDet_ += std::log(Sfact[i + i * n]);
  g_factorTiming.leafCorrected += std::chrono::duration<double>(std::chrono::steady_clock::now() - ft0).count();
}

void HODLRNode::applyInverseFactor(Matrix& x) const
{
  // Applies L^{-1} * x in-place where L is this node's HODLR Cholesky factor.
  // x must have size_ rows.
  const UnsignedInteger nrhs = x.getNbColumns();
  const UnsignedInteger nxRows = x.getNbRows();
  x.copyOnWrite();
  Scalar* x_data = &(*x.getImplementation())[0];

  if (isLeaf_)
  {
    // Dense lower-triangular solve via dtrsm
    int n = static_cast<int>(size_);
    int nrhs_int = static_cast<int>(nrhs);
    double one = 1.0;
    HODLRDtrsm("L", "L", "N", "N", &n, &nrhs_int, &one,
           const_cast<double*>(&(*Sfactor_.getImplementation())[0]), &n, x_data, &n);
    return;
  }

  const UnsignedInteger s0 = size_ / 2;
  const UnsignedInteger s1 = size_ - s0;

  // Forward substitution with the 2x2 block structure:
  // [L00   0 ] * [y0] = [x0]
  // [L10  L11]   [y1]   [x1]
  //
  // 1. y0 = L00^{-1} * x0   (recursive on child0)
  // 2. y1 = L11^{-1} * (x1 - L10 * y0)
  //    where L10 = U_[1] * W_^T

  // Step 1: y0 = L00^{-1} * x0
  Matrix x0(s0, nrhs);
  MatrixImplementation& x0Impl = *x0.getImplementation();
  for (UnsignedInteger j = 0; j < nrhs; ++j)
    std::copy(x_data + j * nxRows, x_data + j * nxRows + s0, &x0Impl[0] + j * s0);
  p_child0_->applyInverseFactor(x0);

  if (rank_ > 0)
  {
    // Step 2: x1 = x1 - L10 * x0  where L10 = U_[1] * W_^T
    // temp = W_^T * x0  via dgemm  (rank_ x nrhs)
    int mT = static_cast<int>(rank_);
    int nT = static_cast<int>(nrhs);
    int kT = static_cast<int>(s0);
    double one = 1.0, zero = 0.0, neg = -1.0;
    int ldW = static_cast<int>(s0);
    int ldX0 = static_cast<int>(s0);
    int ldT = static_cast<int>(rank_);
    Matrix temp(mT, nT);
    MatrixImplementation& tempImpl = *temp.getImplementation();
    HODLRDgemm("T", "N", &mT, &nT, &kT, &one, const_cast<double*>(&(*W_.getImplementation())[0]), &ldW, &x0Impl[0], &ldX0, &zero, &tempImpl[0], &ldT);

    // x1 -= U_[1] * temp  via dgemm
    int mU = static_cast<int>(s1);
    int nU = static_cast<int>(nrhs);
    int kU = static_cast<int>(rank_);
    int ldU1 = static_cast<int>(s1);
    int ldTemp = static_cast<int>(rank_);
    int ldX = static_cast<int>(size_);
    HODLRDgemm("N", "N", &mU, &nU, &kU, &neg, const_cast<double*>(&(*U_[1].getImplementation())[0]), &ldU1, &tempImpl[0], &ldTemp, &one, &x_data[s0], &ldX);
  }

  // Step 3: x1 = L11^{-1} * x1  (recursive on child1)
  Matrix x1(s1, nrhs);
  MatrixImplementation& x1Impl = *x1.getImplementation();
  for (UnsignedInteger j = 0; j < nrhs; ++j)
    std::copy(x_data + s0 + j * nxRows, x_data + s0 + j * nxRows + s1, &x1Impl[0] + j * s1);

  p_child1_->applyInverseFactor(x1);

  // Write back
  for (UnsignedInteger j = 0; j < nrhs; ++j)
  {
    std::copy(&x0Impl[0] + j * s0, &x0Impl[0] + j * s0 + s0, x_data + j * nxRows);
    std::copy(&x1Impl[0] + j * s1, &x1Impl[0] + j * s1 + s1, x_data + s0 + j * nxRows);
  }
}

void HODLRNode::applyInverseFactorTranspose(Matrix& x) const
{
  // Applies L^{-T} * x in-place where L is this node's HODLR Cholesky factor.
  // x must have size_ rows.
  const UnsignedInteger nrhs = x.getNbColumns();
  const UnsignedInteger nxRows = x.getNbRows();
  x.copyOnWrite();
  Scalar* x_data = &(*x.getImplementation())[0];

  if (isLeaf_)
  {
    // Dense lower-triangular transpose solve via dtrsm: L^T * X = B
    int n = static_cast<int>(size_);
    int nrhs_int = static_cast<int>(nrhs);
    double one = 1.0;
    HODLRDtrsm("L", "L", "T", "N", &n, &nrhs_int, &one,
           const_cast<double*>(&(*Sfactor_.getImplementation())[0]), &n, x_data, &n);
    return;
  }

  const UnsignedInteger s0 = size_ / 2;
  const UnsignedInteger s1 = size_ - s0;

  // Backward substitution with the 2x2 block structure:
  // L^T = [L00^T, L10^T; 0, L11^T]
  // where L10 = U1 * W^T, so L10^T = W * U1^T
  //
  // 1. child1: L11^T * x1 = x1   (recursive)
  // 2. x0 = x0 - L10^T * x1 = x0 - W * U1^T * x1
  // 3. child0: L00^T * x0 = x0   (recursive)

  // Step 1: L11^T * x1 = x1  (recursive on child1)
  Matrix x1(s1, nrhs);
  MatrixImplementation& x1Impl = *x1.getImplementation();
  for (UnsignedInteger j = 0; j < nrhs; ++j)
    std::copy(x_data + s0 + j * nxRows, x_data + s0 + j * nxRows + s1, &x1Impl[0] + j * s1);
  p_child1_->applyInverseFactorTranspose(x1);

  if (rank_ > 0)
  {
    // Step 2: x0 -= L10^T * x1  where L10^T = W * U1^T
    // temp = U1^T * x1  via dgemm  (rank_ x nrhs)
    int mU = static_cast<int>(rank_);
    int nU = static_cast<int>(nrhs);
    int kU = static_cast<int>(s1);
    double one = 1.0, zero = 0.0, neg = -1.0;
    int ldU1 = static_cast<int>(s1);
    int ldX1 = static_cast<int>(s1);
    int ldT = static_cast<int>(rank_);
    Matrix temp(mU, nU);
    MatrixImplementation& tempImpl = *temp.getImplementation();
    HODLRDgemm("T", "N", &mU, &nU, &kU, &one, const_cast<double*>(&(*U_[1].getImplementation())[0]), &ldU1, &x1Impl[0], &ldX1, &zero, &tempImpl[0], &ldT);

    // x0 -= W * temp  via dgemm.
    // x_data is the node's (size_ x nrhs) buffer, so the stride between
    // RHS columns is nxRows, not s0; writing with ldc = s0 would misplace
    // every column past the first.
    int mW = static_cast<int>(s0);
    int nW = static_cast<int>(nrhs);
    int kW = static_cast<int>(rank_);
    int ldW = static_cast<int>(s0);
    int ldTemp = static_cast<int>(rank_);
    int ldX0 = static_cast<int>(nxRows);
    HODLRDgemm("N", "N", &mW, &nW, &kW, &neg, const_cast<double*>(&(*W_.getImplementation())[0]), &ldW, &tempImpl[0], &ldTemp, &one, x_data, &ldX0);
  }

  // Step 3: L00^T * x0 = x0  (recursive on child0)
  Matrix x0(s0, nrhs);
  MatrixImplementation& x0Impl = *x0.getImplementation();
  for (UnsignedInteger j = 0; j < nrhs; ++j)
    std::copy(x_data + j * nxRows, x_data + j * nxRows + s0, &x0Impl[0] + j * s0);
  p_child0_->applyInverseFactorTranspose(x0);

  // Write back
  for (UnsignedInteger j = 0; j < nrhs; ++j)
  {
    std::copy(&x0Impl[0] + j * s0, &x0Impl[0] + j * s0 + s0, x_data + j * nxRows);
    std::copy(&x1Impl[0] + j * s1, &x1Impl[0] + j * s1 + s1, x_data + s0 + j * nxRows);
  }
}

void HODLRNode::solve(Matrix& x) const
{
  HODLRBlasGuard blasGuard;
  // Cholesky: A = L * L^T, solve via forward + backward substitution
  applyInverseFactor(x);        // y = L^{-1} * x
  applyInverseFactorTranspose(x); // x = L^{-T} * y
}

void HODLRNode::solveLower(Matrix& x, Bool trans) const
{
  HODLRBlasGuard blasGuard;
  if (trans)
    applyInverseFactorTranspose(x);
  else
    applyInverseFactor(x);
}

Scalar HODLRNode::dotSolve(Matrix& x) const
{
  Matrix b(x);
  solve(b);
  Scalar result = 0.0;
  const UnsignedInteger nrhs = x.getNbColumns();
  const UnsignedInteger nxRows = x.getNbRows();
  const Scalar* x_data = &(*x.getImplementation())[0];
  const Scalar* b_data = &(*b.getImplementation())[0];
  for (UnsignedInteger j = 0; j < nrhs; ++j)
    for (UnsignedInteger i = 0; i < nxRows; ++i)
      result += x_data[i + j * nxRows] * b_data[i + j * nxRows];
  return result;
}

void HODLRNode::applyFactor(Matrix& y, const Matrix& x) const
{
  HODLRBlasGuard blasGuard;
  const UnsignedInteger nrhs = x.getNbColumns();
  const UnsignedInteger nxRows = x.getNbRows();
  const Scalar* x_data = &(*x.getImplementation())[0];
  y.copyOnWrite();
  const UnsignedInteger nyRows = y.getNbRows();
  Scalar* y_data = &(*y.getImplementation())[0];

  if (isLeaf_)
  {
    // y += L * x where L is the lower-triangular Cholesky factor in Sfactor_
    // Use dtrmv for each RHS column; allocate workspace once for all RHS.
    std::vector<double> tmp(size_ * nrhs);
    for (UnsignedInteger j = 0; j < nrhs; ++j)
      std::copy(x_data + start_ + j * nxRows, x_data + start_ + j * nxRows + size_, tmp.data() + j * size_);
    int n = static_cast<int>(size_);
    int incx = 1;
    const Scalar* L_data = &(*Sfactor_.getImplementation())[0];
    for (UnsignedInteger j = 0; j < nrhs; ++j)
    {
      HODLRDtrmv("L", "N", "N", &n, const_cast<double*>(L_data), &n, tmp.data() + j * size_, &incx);
      for (UnsignedInteger i = 0; i < size_; ++i)
        y_data[(start_ + i) + j * nyRows] += tmp[i + j * size_];
    }
    return;
  }

  const UnsignedInteger s0 = size_ / 2;
  const UnsignedInteger s1 = size_ - s0;

  // Recurse on children for diagonal blocks: L00, L11
  p_child0_->applyFactor(y, x);
  p_child1_->applyFactor(y, x);

  if (rank_ == 0)
    return;

  const Scalar* W_data = &(*W_.getImplementation())[0];
  const Scalar* U1_data = &(*U_[1].getImplementation())[0];

  // Off-diagonal: L[child1, child0] = U_[1] * W_^T
  // y[start_+s0:...] += U_[1] * (W_^T * x[start_:start_+s0])
  // temp = W_^T * x0   (rank_ x nrhs) via dgemm
  Matrix temp(rank_, nrhs, 0.0);
  {
    int m = static_cast<int>(rank_);
    int n = static_cast<int>(nrhs);
    int k = static_cast<int>(s0);
    double one = 1.0, zero = 0.0;
    int ldW = static_cast<int>(s0);
    int ldX = static_cast<int>(nxRows);
    int ldT = static_cast<int>(rank_);
    HODLRDgemm("T", "N", &m, &n, &k, &one, const_cast<double*>(W_data), &ldW,
               const_cast<double*>(x_data + start_), &ldX, &zero, &(*temp.getImplementation())[0], &ldT);
  }

  // y[s0:s0+s1] += U_[1] * temp via dgemm
  {
    int m = static_cast<int>(s1);
    int n = static_cast<int>(nrhs);
    int k = static_cast<int>(rank_);
    double one = 1.0;
    int ldU1 = static_cast<int>(s1);
    int ldT = static_cast<int>(rank_);
    int ldY = static_cast<int>(nyRows);
    HODLRDgemm("N", "N", &m, &n, &k, &one, const_cast<double*>(U1_data), &ldU1,
               &(*temp.getImplementation())[0], &ldT, &one, y_data + start_ + s0, &ldY);
  }
}

void HODLRNode::applyFactorTranspose(Matrix& y, const Matrix& x) const
{
  HODLRBlasGuard blasGuard;
  const UnsignedInteger nrhs = x.getNbColumns();
  const UnsignedInteger nxRows = x.getNbRows();
  const Scalar* x_data = &(*x.getImplementation())[0];
  y.copyOnWrite();
  const UnsignedInteger nyRows = y.getNbRows();
  Scalar* y_data = &(*y.getImplementation())[0];

  if (isLeaf_)
  {
    // y += L^T * x where L is the lower-triangular Cholesky factor in Sfactor_
    std::vector<double> tmp(size_ * nrhs);
    for (UnsignedInteger j = 0; j < nrhs; ++j)
      std::copy(x_data + start_ + j * nxRows, x_data + start_ + j * nxRows + size_, tmp.data() + j * size_);
    int n = static_cast<int>(size_);
    int incx = 1;
    const Scalar* L_data = &(*Sfactor_.getImplementation())[0];
    for (UnsignedInteger j = 0; j < nrhs; ++j)
    {
      HODLRDtrmv("L", "T", "N", &n, const_cast<double*>(L_data), &n, tmp.data() + j * size_, &incx);
      for (UnsignedInteger i = 0; i < size_; ++i)
        y_data[(start_ + i) + j * nyRows] += tmp[i + j * size_];
    }
    return;
  }

  const UnsignedInteger s0 = size_ / 2;
  const UnsignedInteger s1 = size_ - s0;

  // Recurse on children for the diagonal blocks: L00^T, L11^T
  p_child0_->applyFactorTranspose(y, x);
  p_child1_->applyFactorTranspose(y, x);

  if (rank_ == 0)
    return;

  const Scalar* W_data = &(*W_.getImplementation())[0];
  const Scalar* U1_data = &(*U_[1].getImplementation())[0];

  // Off-diagonal: L[child1, child0] = U_[1] * W_^T, so the transposed block
  // is L[child0, child1]^T = W_ * U_[1]^T:
  // y[start_:start_+s0] += W_ * (U_[1]^T * x[start_+s0:...])
  // temp = U_[1]^T * x1   (rank_ x nrhs) via dgemm
  Matrix temp(rank_, nrhs, 0.0);
  {
    int m = static_cast<int>(rank_);
    int n = static_cast<int>(nrhs);
    int k = static_cast<int>(s1);
    double one = 1.0, zero = 0.0;
    int ldU1 = static_cast<int>(s1);
    int ldX = static_cast<int>(nxRows);
    int ldT = static_cast<int>(rank_);
    HODLRDgemm("T", "N", &m, &n, &k, &one, const_cast<double*>(U1_data), &ldU1,
               const_cast<double*>(x_data + start_ + s0), &ldX, &zero, &(*temp.getImplementation())[0], &ldT);
  }

  // y[start_:start_+s0] += W_ * temp via dgemm
  {
    int m = static_cast<int>(s0);
    int n = static_cast<int>(nrhs);
    int k = static_cast<int>(rank_);
    double one = 1.0;
    int ldW = static_cast<int>(s0);
    int ldT = static_cast<int>(rank_);
    int ldY = static_cast<int>(nyRows);
    HODLRDgemm("N", "N", &m, &n, &k, &one, const_cast<double*>(W_data), &ldW,
               &(*temp.getImplementation())[0], &ldT, &one, y_data + start_, &ldY);
  }
}

size_t HODLRNode::getNnz() const
{
  if (isLeaf_)
    return static_cast<size_t>(size_) * size_;
  else
  {
    size_t nnz = static_cast<size_t>(U_[0].getNbRows()) * U_[0].getNbColumns()
               + static_cast<size_t>(U_[1].getNbRows()) * U_[1].getNbColumns();
    if (p_child0_) nnz += p_child0_->getNnz();
    if (p_child1_) nnz += p_child1_->getNnz();
    return nnz;
  }
}

END_NAMESPACE_OPENTURNS
