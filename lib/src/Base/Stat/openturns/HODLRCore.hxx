//                                               -*- C++ -*-
/**
 *  @file  HODLRCore.hxx
 *  @brief Internal HODLR recursive tree using LAPACK
 *
 *  Implemented with the OpenTURNS Matrix and LAPACK backend.
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
#ifndef OPENTURNS_HODLRCORE_HXX
#define OPENTURNS_HODLRCORE_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/StorageManager.hxx"  // Advocate
#include "openturns/Matrix.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/HODLRMatrixImplementation.hxx"

#include <vector>
#include <functional>

BEGIN_NAMESPACE_OPENTURNS

/**
 * RAII guard that pins BLAS to single-thread mode.
 * HODLR operates on small matrices (rank=10, leaf=32-256) where
 * multi-threaded BLAS overhead dominates performance.
 * Restores the previous thread count on destruction.
 */
class HODLRBlasGuard
{
public:
  HODLRBlasGuard();
  ~HODLRBlasGuard();
private:
  int savedNumThreads_ = 1;
};

/**
 * Evaluator that wraps an original evaluator and subtracts low-rank corrections.
 * Used for hierarchical Schur complement in HODLR Cholesky factorization.
 *
 * Stores the ROOT original evaluator and a flat list of all accumulated corrections
 * from every level of the recursion, avoiding nested evaluator chains that compound
 * numerical errors.
 *
 * corrected_eval(i, j) = root_original(i, j) - sum_k (U1_k * K_k * U1_k^T) + sum_k (lambda_k * delta(i, j))
 */
class HODLRCorrectedEvaluator : public HODLREntryEvaluator
{
public:
  /** Data for a single low-rank correction term */
  struct Correction {
    UnsignedInteger offset;
    UnsignedInteger size;
    UnsignedInteger rank;
    Matrix U1;   // n_rows x rank
    Matrix UK;   // U1 * K (precomputed, n_rows x rank)
    Scalar lambda;
  };

  /** Create a flattened evaluator with one or more corrections */
  HODLRCorrectedEvaluator(Pointer<const HODLREntryEvaluator> original,
                           UnsignedInteger offset,
                           UnsignedInteger size,
                           const std::vector<Correction>& corrections)
    : original_(original)
    , offset_(offset)
    , size_(size)
    , corrections_(corrections)
  {
  }

  /** Create a flattened evaluator from an existing one + a new correction */
  static Pointer<HODLRCorrectedEvaluator> flatten(
      const HODLRCorrectedEvaluator& existing,
      const Matrix& newU1,
      const Matrix& newK,
      Scalar newLambda,
      UnsignedInteger newOffset,
      UnsignedInteger newSize);

  /** Create a flattened evaluator with a single correction (first level) */
  static Pointer<HODLRCorrectedEvaluator> create(
      Pointer<const HODLREntryEvaluator> original,
      UnsignedInteger offset,
      UnsignedInteger size,
      const Matrix& U1,
      const Matrix& K,
      Scalar lambda);

  Scalar operator()(UnsignedInteger i, UnsignedInteger j) const override
  {
    Scalar val = (*original_)(i, j);
    for (const auto& corr : corrections_)
    {
      const SignedInteger local_i = static_cast<SignedInteger>(i) - static_cast<SignedInteger>(corr.offset);
      const SignedInteger local_j = static_cast<SignedInteger>(j) - static_cast<SignedInteger>(corr.offset);
      if (local_i >= 0 && local_j >= 0 &&
          static_cast<UnsignedInteger>(local_i) < corr.size &&
          static_cast<UnsignedInteger>(local_j) < corr.size)
      {
        const UnsignedInteger iu = static_cast<UnsignedInteger>(local_i);
        const UnsignedInteger ju = static_cast<UnsignedInteger>(local_j);
        const Scalar* uk_data = corr.UK.getImplementation()->data();
        const Scalar* u1_data = corr.U1.getImplementation()->data();
        const UnsignedInteger ukRows = corr.UK.getNbRows();
        const UnsignedInteger u1Rows = corr.U1.getNbRows();
        Scalar term = 0.0;
        for (UnsignedInteger q = 0; q < corr.rank; ++q)
          term += uk_data[iu + q * ukRows] * u1_data[ju + q * u1Rows];
        val -= term;
        if (i == j) val += corr.lambda;
      }
    }
    return val;
  }

  UnsignedInteger getSize() const override
  {
    return original_->getSize();
  }

  const Pointer<const HODLREntryEvaluator> getOriginal() const { return original_; }
  const std::vector<Correction>& getCorrections() const { return corrections_; }
  UnsignedInteger getOffset() const { return offset_; }
  UnsignedInteger getSizeLocal() const { return size_; }
  UnsignedInteger getNumCorrections() const { return corrections_.size(); }

private:
  Pointer<const HODLREntryEvaluator> original_;  // ALWAYS root original
  UnsignedInteger offset_;
  UnsignedInteger size_;
  std::vector<Correction> corrections_;

  // Friend declarations for internal access
  friend class HODLRNode;
};

/**
 * Internal HODLR tree node (symmetric matrices only).
 *
 * Implements the recursive HODLR structure.
 * A01 = U * V^T, A10 = V * U^T.
 */
class HODLRNode : public PersistentObject
{
  CLASSNAME
  // The only class allowed to create a node without building it, and the only
  // one that reaches the members: it owns the tree and the Advocate.
  friend class HODLRMatrixImplementation;
public:
  // For the persistence factory. Builds an empty node: the tree, the factors and
  // the factorization all arrive from load().
  HODLRNode();

  // Deep copy of the subtree, required by PersistentObject. The inherited
  // context (diagonal base pointer, evaluator, parent) is shared rather than
  // copied: it belongs to the matrix, not to the node. Note this is *not* what a
  // HODLRMatrix copy does -- that shares the node through its Pointer, so a copy
  // is a view of the same tree, not an independent operator.
  HODLRNode * clone() const override;

  // Deep copies. The persistence layer needs both (Factory::assign uses
  // operator=), and a deep copy is the only safe one here: the alternative would
  // share the children and the factor buffers.
  HODLRNode(const HODLRNode & other);
  HODLRNode & operator=(const HODLRNode & other);

  HODLRNode(Pointer<const HODLREntryEvaluator> eval,
            const Scalar* diag,
            UnsignedInteger start,
            UnsignedInteger size,
            UnsignedInteger minLeafSize,
            UnsignedInteger maxRank,
            Scalar tolerance,
            Scalar recompressionTolerance,
            SignedInteger direction = 0,
            HODLRNode* parent = nullptr);

  ~HODLRNode();

  // The copy constructor and assignment are declared with the rest of the
  // persistence surface above; they are deep copies, not the deletions this
  // class used to carry, because Factory::assign and clone() both need them.

  void computeCholesky();
  void solve(Matrix& x) const;
  Scalar dotSolve(Matrix& x) const;
  void solveLower(Matrix& x, Bool trans) const;  // L^{-1} or L^{-T} times x for the Cholesky factor L
  void applyFactor(Matrix& y, const Matrix& x) const;
  void applyFactorTranspose(Matrix& y, const Matrix& x) const;

  static void recompressLowRank(Matrix& Uout, Matrix& Vout,
      UnsignedInteger startRow, UnsignedInteger nRows,
      UnsignedInteger startCol, UnsignedInteger nCols,
      const std::vector<HODLRCorrectedEvaluator::Correction>& corrections,
      Scalar tolerance);

  Scalar getLogDeterminant() const { return logDet_; }
  UnsignedInteger getTotalRank() const { return totalRank_; }
  UnsignedInteger getNumLeaves() const { return numLeaves_; }
  size_t getNnz() const;
  void setShift(Scalar shift);
  // Largest diagonal shift applied locally to heal a non-SPD leaf during the
  // last factorization (0 if none was needed). Propagated up the tree so the
  // callers can report the actual regularization used.
  Scalar getMaxLeafShift() const { return maxLeafShift_; }

  // Persistence of the assembled and factorized tree. Each node is its own
  // PersistentObject and its own XML element, because the alternative -- writing
  // the nodes as attributes of HODLRMatrixImplementation -- collides: XML
  // attributes share one flat namespace per element, so the second node's
  // node_start_ is a redefinition of the first's and the file will not parse.
  //
  // What is written is the state the read operations need: the tree shape, the
  // off-diagonal factors, and the factorization (Sfactor_/W_/K_). What is *not*
  // written is p_eval_: an assembly evaluator is a Python callable as often as a
  // covariance model, and a callable cannot be serialized. So a loaded matrix can
  // be solved, multiplied, asked for its log determinant and measured, but not
  // re-assembled or re-factorized. That limit is reported rather than papered
  // over: see HODLRMatrixImplementation::load.
  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

  // p_diag_, p_eval_ and p_parent_ are inherited context, not node state, so
  // they cannot travel through the Advocate (whose load() takes no arguments).
  // The parent fixes them on its children once they are loaded; the root is
  // fixed by HODLRMatrixImplementation, which is why this walks the subtree.
  void adoptContext(const Scalar * diag, Pointer<const HODLREntryEvaluator> eval,
                    HODLRNode * parent);

private:
  // Deserialization constructor: it initializes every member to the same default
  // the public constructor starts from but assembles and factorizes nothing,
  // because loading must restore a state, not recompute one. The public
  // constructor delegates to this one and then builds, so the two member
  // initializations cannot drift apart.
  struct ForLoad
  {
  };
  HODLRNode(ForLoad, Pointer<const HODLREntryEvaluator> eval, const Scalar * diag,
            UnsignedInteger start, UnsignedInteger size,
            SignedInteger direction, HODLRNode * parent);

  // Everything except the inherited context, which is shared: the copy keeps
  // this node's own p_diag_/p_eval_/p_parent_.
  void assignDeep(const HODLRNode & other);

  UnsignedInteger lowRankApproxPartialPivot(UnsignedInteger startRow, UnsignedInteger nRows,
      UnsignedInteger startCol, UnsignedInteger nCols,
      Scalar tol, Matrix& Uout, Matrix& Vout);
  void factorizeLeafCholesky();
  void factorizeLeafCholesky(const std::vector<HODLRCorrectedEvaluator::Correction>& corrections,
                             bool rebuildStructure = true);
  void factorizeLeafCholeskyCorrected(const Matrix& K, const Matrix& U1, Scalar lambda,
                                      const std::vector<HODLRCorrectedEvaluator::Correction>& corrections);  // Bypass evaluator, use dgemm correction
  void computeCholesky(const std::vector<HODLRCorrectedEvaluator::Correction>& corrections,
                       bool rebuildStructure = true);
  void applyInverseFactor(Matrix& x) const;  // L^{-1} * x for HODLR Cholesky factor L
  void applyInverseFactorTranspose(Matrix& x) const;  // L^{-T} * x for HODLR Cholesky factor L

  const Scalar* p_diag_;
  Pointer<const HODLREntryEvaluator> p_eval_;
  UnsignedInteger start_;
  UnsignedInteger size_;
  SignedInteger direction_;
  UnsignedInteger rank_;
  UnsignedInteger maxRank_;
  UnsignedInteger minLeafSize_;
  UnsignedInteger denseThreshold_;
  Scalar tolerance_;
  // Truncation tolerance for the factor-stage SVD recompression of the
  // Schur-complement corrections (recompressLowRank). Kept looser than the
  // assembly tolerance on purpose: the corrections carry numerical noise
  // from the ACA and the triangular solves, and truncating them back to the
  // assembly tolerance preserves that noise in the factor, which then loses
  // positive-definiteness. Mirrors HMatrix-RecompressionEpsilon usage.
  Scalar recompressionTolerance_;
  // Random pivot selection for the partial-pivot ACA (hmat-oss AcaRandom
  // scheme): each iteration also considers pre-sampled random entries and
  // restarts from the random row when it beats the max-element pivot.
  Bool useRandomPivots_;
  bool isLeaf_;
  Scalar logDet_;
  Scalar shift_;
  Scalar maxLeafShift_;
  UnsignedInteger totalRank_;
  UnsignedInteger numLeaves_;
  UnsignedInteger numStarvedBlocks_;

  HODLRNode* p_parent_;         // non-owning back-reference
  Pointer<HODLRNode> p_child0_;
  Pointer<HODLRNode> p_child1_;

  Collection<Matrix> U_;
  Collection<Matrix> V_;

  Matrix Sfactor_;
  Matrix leafKernel_;   // raw kernel block of a leaf, cached at construction
  Matrix W_;   // L00^{-1} * V_[0] for internal nodes
  // W^T*W (rank_ x rank_), cached on first build: invariant w.r.t. the
  // regularization lambda, so Schur-complement retries reuse it instead of
  // recomputing the Gram product (and its subtree work) per attempt.
  Matrix K_;
  // Assembled-but-unregularized leaf matrix (kernel block minus all Schur
  // complement correction products, WITHOUT any lambda/shift on the diagonal).
  // Cached on first factorization. Since the parent retry loop's regularization
  // is only a diagonal term, a retry with a larger lambda restores this copy and
  // re-adds the diagonal terms instead of re-running every correction dgemm
  // (avoids the O(attempts) parent rebuild cascade on near-singular kernels).
  Matrix pristine_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HODLRCORE_HXX */
