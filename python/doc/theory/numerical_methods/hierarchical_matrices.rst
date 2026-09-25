.. _hierarchical_matrices:

Hierarchical Matrices
---------------------

Introduction
~~~~~~~~~~~~

In uncertainty quantification and surrogate modeling, we frequently
encounter dense matrices arising from pairwise interactions, such
as covariance matrices in Gaussian Process Regression (aka Kriging) or
Gaussian Process Sampling. For :math:`N` observations, assembling and factoring
these dense matrices scales as :math:`\mathcal{O}(N^2)`
in memory and :math:`\mathcal{O}(N^3)` in computational time, limiting
the use of standard linear algebra to a few thousand points.

Hierarchical matrices (or :math:`\mathcal{H}`-Matrices), introduced by
Hackbusch [hackbusch1999]_, provide a data-sparse approximation of specific
dense matrices. They allow memory and computational complexities to be
reduced to :math:`\mathcal{O}(N \log N)` or :math:`\mathcal{O}(N \log^2 N)`,
enabling large-scale applications.

Principle
~~~~~~~~~

The core idea of :math:`\mathcal{H}`-Matrices is to partition a large
matrix into a hierarchy of block sub-matrices. While blocks near the
diagonal represent strong, local interactions and remain dense,
blocks far from the diagonal represent long-distance interactions
and can be highly compressed using low-rank approximations.

The construction of an :math:`\mathcal{H}`-Matrix involves three main steps:

1. **Spatial Clustering (Cluster Trees):**
   To optimize memory locality and structure the hierarchical representation,
   the spatial coordinates (vertices) associated with the row and column index
   sets :math:`I` and :math:`J` are partitioned into cluster trees,
   denoted :math:`T_I` and :math:`T_J`.
   For symmetric applications such as covariance matrix assembly, :math:`I = J`.

   The partition is generated using a **Binary Space Partitioning (BSP)** algorithm:

   * **Initialization:** The root cell encompasses the complete set of vertices.
   * **Bisection:** At each recursion step, the bounding box of a cell is bisected
     orthogonally to its longest coordinate axis.
   * **Termination:** The recursive partitioning stops when a cell contains
     fewer than a given threshold of vertices (the leaf size :math:`n_{\text{leaf}}`),
     forming a leaf node in the cluster tree.

   To handle diverse spatial distributions of points, three bisection strategies are proposed:

   * **Median Clustering:** Bisects the cell along its longest axis such that both child
     cells contain an equal number of vertices (degrees of freedom). This guarantees a balanced
     cluster tree, though the physical volumes of the child cells may differ.
   * **Geometric Clustering:** Bisects the cell at the geometric midpoint of
     its longest dimension, resulting in two child cells of equal geometric volume.
     This maintains well-proportioned bounding boxes, but may produce unbalanced trees if vertex
     density is non-uniform.
   * **Hybrid Clustering:** Combines median and geometric approaches.
     It defaults to a median split; if the physical volumes of the
     resulting child cells differ significantly, it falls back to
     geometric bisection. This balances tree depth while preserving
     well-shaped spatial bounding boxes.

     .. figure:: ../../_static/block_clustering.png
         :align: center
         :alt: block_clustering
         :width: 50%

         *Figure 1.* Spatial clustering & h-matrix skeleton.

2. **Block Partitioning and Admissibility (Block Cluster Tree):**
   Once the row cluster tree :math:`T_I` and column cluster tree :math:`T_J` are constructed,
   they are combined to build a hierarchical representation of the matrix,
   structured as a **Block Cluster Tree** (a 4-tree or quadtree).

   The process constructs the structure of the :math:`\mathcal{H}`-Matrix by recursively evaluating
   whether matrix blocks can be represented using low-rank approximations.
   Two main admissibility criteria are supported:

   * **Strong Admissibility (Standard** :math:`\boldsymbol{\mathcal{H}\textbf{-Matrix}}`):
     For each block pair :math:`(\tau, \sigma)`, Axis-Aligned Bounding Boxes
     (:math:`\text{bbox}(\tau)` and :math:`\text{bbox}(\sigma)`) are computed around the physical
     coordinates of the vertices. The block is deemed **admissible** if it satisfies:

     .. math::
        \min\left(\text{diam}(\text{bbox}(\tau)), \text{diam}(\text{bbox}(\sigma))\right)
        \le \eta \cdot \text{dist}\left(\text{bbox}(\tau), \text{bbox}(\sigma)\right)

     where :math:`\text{dist}(\cdot, \cdot)` is the spatial distance between bounding boxes,
     :math:`\text{diam}(\cdot)` is their diameter, and :math:`\eta > 0` is the
     **admissibility factor**.
     Blocks that do not satisfy this geometric condition are split recursively into 4 sub-blocks
     until leaf clusters are reached (where non-admissible blocks remain dense).

     .. figure:: ../../_static/admissibility.png
         :align: center
         :alt: standard_admissibility
         :width: 50%

         *Figure 2.* Strong Admissibility.


   * **Weak Admissibility (HODLR - Hierarchical Off-Diagonal Low-Rank):**
     HODLR employs a purely structural partitioning that ignores spatial distance.
     At each level of the binary cluster tree, all off-diagonal sub-blocks
     (:math:`\tau \neq \sigma`) are unconditionally declared **admissible** and represented as
     low-rank approximations, while diagonal blocks (:math:`\tau = \sigma`) are recursively
     subdivided until the leaf level, where they are stored as small dense matrices.

   * **Termination & Leaf Assignment:**

     * If a block is **admissible** (via strong geometric check or HODLR weak criterion),
       recursion stops for that branch.
       The block is marked as a low-rank candidate :math:`A_{\tau, \sigma} \approx U V^T`.
     * If a block is **non-admissible** (in strong admissibility) or is a diagonal block
       at the leaf level in HODLR, recursion stops, and the block is stored as a standard
       full (dense) matrix block.

   The resulting quadtree structure yields an empty :math:`\mathcal{H}`-Matrix (or HODLR) topology
   whose leaf nodes uniquely partition the matrix into either low-rank approximation blocks
   or dense blocks, ready for numerical compression.

3. **Assembly and Low-Rank Compression:**
   Once the block cluster tree structure is built, the matrix entries are computed using an assembly process.
   This process uses a callable kernel function that evaluates pairwise interactions between spatial degrees of freedom.

   * **Full Blocks:** For non-admissible leaf blocks, the callable kernel is evaluated across all entry
     pairs :math:`(i, j)` in the block, forming a standard dense matrix block.
   * **Low-Rank Admissible Blocks:** For admissible leaf blocks :math:`A_{\tau, \sigma} \in \mathbb{R}^{m \times n}`,
     the goal is to build a factorized representation :math:`A_{\tau, \sigma} \approx U V^T` with
     :math:`U \in \mathbb{R}^{m \times k}` and :math:`V \in \mathbb{R}^{n \times k}` (where rank
     :math:`k \ll \min(m, n)`), controlled by a user-specified accuracy threshold :math:`\epsilon`
     (``assemblyEpsilon``).

     .. figure:: ../../_static/low_rank_approx.png
         :align: center
         :alt: lr_approx
         :width: 50%

         *Figure 3.* Low rank approximation of a block.

   The low-rank factors can be computed using two main compression strategies:

   * **Singular Value Decomposition (SVD):**
     The full sub-matrix :math:`A_{\tau, \sigma}` is first explicitly constructed by evaluating
     the kernel function across all :math:`m \times n` entries.
     An SVD decomposition (:math:`A_{\tau, \sigma} = U \Sigma V^T`) is computed, and rank :math:`k`
     is truncated such that the approximation error remains below ``assemblyEpsilon``.

     .. note::
        SVD produces the mathematically optimal low-rank approximation for a given tolerance.
        However, because it requires computing the full block and performing an SVD factorization
        beforehand, it carries high memory and computational overhead.

   * **Adaptive Cross Approximation (ACA) and Variants:**
     ACA builds the low-rank matrices :math:`U` and :math:`V` iteratively using a black-box pivot selection scheme.
     It evaluates only a small subset of rows and columns on-the-fly without assembling the full dense block.

     .. note::
        By evaluating only :math:`\mathcal{O}(k(m+n))` entries, ACA is significantly faster and more
        memory-efficient than SVD. However, because stopping criteria depend on algebraic heuristics
        rather than global matrix singular values, the actual approximation error may occasionally
        exceed ``assemblyEpsilon`` on complex or non-smooth kernels.

Matrix Arithmetic
-----------------

Once assembled, the :math:`\mathcal{H}`-Matrix structure supports standard yet
highly efficient linear algebra operations:

* **Matrix Factorization (In-Place Cholesky, LU, LDLt):** Enables approximate
  factorizations and linear solves via hierarchical block arithmetic. The complexity
  of Cholesky factorization (:math:`A = L L^T`) drops from :math:`\mathcal{O}(N^3)`
  to :math:`\mathcal{O}(N \log^2 N)`, which is critical for evaluating likelihoods
  in Gaussian Processes.
* **Matrix-Vector & Matrix-Matrix Products:** Evaluated recursively block by block.
  Dense blocks leverage standard BLAS routines, while low-rank blocks compute
  :math:`U (V^T x)`, reducing matrix-vector complexity from :math:`\mathcal{O}(mn)`
  to :math:`\mathcal{O}(k(m+n))`.

.. note::
   Degree-of-freedom indices are reordered during clustering to optimize spatial locality.
   As a result, operations such as Cholesky or LU factorizations produce permuted factors,
   whose raw entry layouts differ from standard dense matrix outputs.

Applications within OpenTURNS
-----------------------------

The integration of ``hmat-oss`` in OpenTURNS provides scalable linear algebra for
two major components of uncertainty quantification:

1. **Kriging (Gaussian Process Regression):**
   During hyperparameter estimation, maximizing the log-likelihood of observations
   requires solving linear systems and evaluating the determinant of the covariance
   matrix :math:`C`:

   .. math::
      \log \mathcal{L} \propto -\frac{1}{2} Y^T C^{-1} Y - \frac{1}{2} \log \det(C)

   Hierarchical matrices accelerate both the linear solve :math:`C^{-1}Y` and the
   determinant calculation (computed from the diagonal of :math:`L`) via the
   hierarchical Cholesky-like factorization :math:`C \approx L L^T`.

2. **Gaussian Process Generation & Field Sampling:**
   Simulating realizations of a Gaussian random field :math:`X \sim \mathcal{N}(\mu, C)`
   at :math:`N` spatial locations requires generating trajectories via:

   .. math::
      X = \mu + L \xi, \quad \xi \sim \mathcal{N}(0, I_N)

   where :math:`L` is the Cholesky-like factor satisfying :math:`C \approx L L^T`. Standard dense
   methods restrict sampling to small grids due to the :math:`\mathcal{O}(N^3)` factorization
   and :math:`\mathcal{O}(N^2)` memory bounds. With :math:`\mathcal{H}`-matrices, the
   hierarchical Cholesky factor :math:`L` is computed in :math:`\mathcal{O}(N \log^2 N)`,
   and subsequent sample realizations are generated in :math:`\mathcal{O}(N \log N)` time
   via hierarchical matrix-vector multiplication.

Together, these capabilities allow OpenTURNS to build surrogate models and generate random
fields on large spatial grids with tens or hundreds of thousands of degrees of freedom.

.. topic:: API:

    - See :class:`~openturns.HMatrix`
    - See :class:`~openturns.HMatrixFactory`

.. topic:: References:

    - [hackbusch1999]_
