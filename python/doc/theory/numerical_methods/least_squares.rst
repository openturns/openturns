.. _least_squares:

Least squares problems numerical methods
----------------------------------------

| This section presents numerical methods that can be used in order to
  solve least squares problems, which can be encountered when the
  construction of a *response surface* (i.e. of a meta-model) is of
  interest, or when one wishes to perform a statistical regression.

| Given a matrix :math:`\matr{\Psi}~\in~\Rset^{N\times P}`, :math:`N>P`,
  and a vector :math:`\underline{y}~\in~\Rset^{N}`, we want to find a
  vector :math:`\underline{a}~\in \Rset^{P}` such that
  :math:`\matr{\Psi}\: \underline{a}` is the best approximation to
  :math:`\underline{y}` in the least squares sense. Mathematically
  speaking, we want to solve the following minimization problem:

  .. math::

    \min_{\underline{a}} \, \, = \, \, \left\| \; \matr{\Psi} \, \underline{a} \, - \, \underline{y} \; \right\|_2

  In the following, it is assumed that the rank of matrix
  :math:`\matr{\Psi}` is equal to :math:`P`.

| Several algorithms can be applied to compute the least squares
  solution, as shown in the sequel.

**Method of normal equations**

| It is shown that the solution of the least squares problem satisfies
  the so-called *normal equations*, which read using a matrix notation:

  .. math::

      \matr{\Psi}^{\mbox{\scriptsize \textsf{T}}} \; \matr{\Psi} \; \underline{a} \, \, = \, \, \matr{\Psi}^{\mbox{\scriptsize \textsf{T}}} \; \underline{y}

| The matrix
  :math:`\matr{C} \equiv \matr{\Psi}^{\mbox{\scriptsize \textsf{T}}} \; \matr{\Psi}`
  is symmetric and positive definite. The system can be solved using the
  following Cholesky factorization:

  .. math::

      \matr{C} \, \, = \, \, \matr{R}^{\mbox{\scriptsize \textsf{T}}} \; \matr{R}

| where :math:`\matr{R}` is an upper triangular matrix with positive
  diagonal entries. Solving the normal equations is equivalent to
  solving the two following triangular systems, which can be easily
  solved by backwards substitution:

  .. math::

     \begin{aligned}
         \matr{R}^{\mbox{\scriptsize \textsf{T}}} \; \underline{z} \, \, = \, \, \matr{\Psi}^{\mbox{\scriptsize \textsf{T}}} \; \underline{y}
         \qquad , \qquad \matr{R} \; \underline{a} \, \, = \, \, \underline{z}
       \end{aligned}

| It has to be noted that this theoretical approach is seldom used in
  practice though. Indeed the resulting least squares solution is quite
  sensitive to a small change in the data (i.e. in :math:`\underline{y}`
  and :math:`\matr{\Psi}`). More precisely, the normal equations are
  always more badly conditioned than the original overdetermined system,
  as their condition number is squared compared to the original problem:

  .. math::

      \kappa(\matr{\Psi}^{\mbox{\scriptsize \textsf{T}}} \; \matr{\Psi}) \, \, = \, \, \kappa(\matr{\Psi})^2

  As a consequence more robust numerical methods should be adopted.

| **Method based on QR factorization**

| It is shown that the matrix :math:`\matr{\Psi}` can be factorized as
  follows:

  .. math::

      \matr{\Psi} \, \, = \, \, \matr{Q} \; \matr{R}

| where :math:`\matr{Q}` is a :math:`N`-by-:math:`P`-matrix with
  *orthonormal* columns and :math:`\matr{R}` is a
  :math:`P`-by-:math:`P`-upper triangular matrix. Such a *QR
  decomposition* may be constructed using several schemes, such as
  *Gram-Schmidt orthogonalization*, *Householder reflections* or *Givens
  rotations*.

| In this setup the least squares problem is equivalent to solving:

  .. math::

      \matr{R} \; \underline{a} \, \, = \, \, \matr{Q}^{\mbox{\scriptsize \textsf{T}}} \; \underline{y}

| This upper triangular system can be solved using backwards
  substitution.

| The solving scheme based on Householder QR factorization leads to a
  relative error that is proportional to:

  .. math::

    \kappa(\matr{\Psi}) + \|\underline{r}\|_2 \kappa(\matr{\Psi})^2

| where
  :math:`\underline{r} = \matr{\Psi} \, \underline{a} \, - \, \underline{y}`.
  Thus this error is expected to be much smaller than the one associated
  with the normal equations provided that the residual
  :math:`\underline{r}` is “small”.

| **Method based on singular value decomposition**

| The so-called *singular value decomposition* (SVD) of matrix
  :math:`\matr{\Psi}` reads:

  .. math::

      \matr{\Psi} \quad = \quad \matr{U} \; \matr{S} \; \matr{V}^{\mbox{\scriptsize \textsf{T}}}

| where :math:`\matr{U}~\in \Rset^{N \times N}` and
  :math:`\matr{V}~\in \Rset^{P \times P}` are orthogonal matrices, and
  :math:`\matr{S}~\in \Rset^{N \times P}` can be cast as:

  .. math::

     \begin{aligned}
         \matr{S} \quad = \quad \left(
         \begin{array}{c}
           \matr{S}_1 \\
           \matr{0} \\
         \end{array}
         \right)
       \end{aligned}

| In the previous equation, :math:`\matr{S}_1~\in \Rset^{P \times P}`
  is a diagonal matrix containing the singular values
  :math:`\sigma_1 \geq \sigma_2 \geq \dots \geq \sigma_P > 0` of
  :math:`\matr{\Psi}`.

| It can be shown that the least squares solution is equal to:

  .. math::

     \begin{aligned}
         \widehat{\underline{a}} \quad = \quad \matr{V} \; \left( \begin{array}{c}
           \matr{S}_1^{-1} \\
           \matr{0}  \\
         \end{array}\right)
         \; \matr{U}^{\mbox{\scriptsize \textsf{T}}} \; \underline{y}
       \end{aligned}

| In practice it is not common to compute a “full” SVD as shown above.
  Instead, it is often sufficient and more economical in terms of time
  and memory to compute a *reduced* version of SVD. The latter reads:

  .. math::

      \matr{\Psi} \quad = \quad \matr{U}_1 \; \matr{S}_1 \; \matr{V}^{\mbox{\scriptsize \textsf{T}}}

| where :math:`\matr{U}_1` is obtained by extracting the :math:`P`
  first columns of :math:`\matr{U}`.

| Note that it is also possible to perform SVD in case of a
  rank-deficient matrix :math:`\matr{\Psi}`. In this case the resulting
  vector :math:`\widehat{\underline{a}}` corresponds to the *minimum
  norm* least squares solution.

| The computational cost of the method is proportional to
  :math:`(NP^2 + P^3)` with a factor ranging from 4 to 10, depending on
  the numerical scheme used to compute the SVD decomposition. This cost
  is higher than those associated with the normal equations and with QR
  factorization. However SVD is relevant insofar as it provides a very
  valuable information, that is the singular values of matrix
  :math:`\matr{\Psi}`.

**Comparison of the methods**

Several conclusions may be drawn concerning the various methods
considered so far:

-  If :math:`N \approx P`, normal equations and Householder QR
   factorization require about the same computational work. If
   :math:`N >> P`, then the QR approach requires about twice as much
   work as normal equations.

-  However QR appears to be more accurate than normal equations, so it
   should be almost always preferred in practice.

-  SVD is also robust but it reveals the most computationally expensive
   scheme. Nonetheless the singular values are obtained as a by-product,
   which may be particularly useful for analytical and computational
   purposes.


.. topic:: API:

    - See the available :ref:`least squares methods <least_squares_methods>`.
    - See :class:`~openturns.PenalizedLeastSquaresAlgorithm`

.. topic:: References:

    - A. Bjorck, 1996, "Numerical methods for least squares problems", SIAM Press, Philadelphia, PA.

