.. _orthogonal_polynomials:

Orthogonal polynomials
----------------------

| This section provides some mathematical details on sequences of
  orthogonal polynomials. Some of these sequences will be used to
  construct the basis of the so-called *polynomial chaos expansion*.
| **Mathematical framework**

| The orthogonal polynomials are associated to an inner product, defined
  as follows:
| Given an *interval of orthogonality* :math:`[\alpha,\beta]`
  (:math:`\alpha \in \Rset \cup \{-\infty\}`,
  :math:`\beta \in \Rset \cup \{\infty\}`, :math:`\alpha < \beta`) and a
  weight function :math:`w(x)> 0`, every pair of polynomials :math:`P`
  and :math:`Q` are orthogonal if:

  .. math::

      \langle P,Q \rangle = \int_{\alpha}^{\beta}P(x)Q(x)~w(x) dx = 0

Therefore, a sequence of orthogonal polynomials :math:`(P_n)_{n\geq 0}`
(:math:`P_n`: polynomial of degree :math:`n`) verifies:

.. math::

    \langle P_m,P_n\rangle = 0 \text{~~for every~~} m \neq n

The chosen inner product induces a norm on polynomials in the usual
way:

.. math::

    \parallel P_n\parallel=\langle P_n,P_n \rangle^{1/2}

In the following, we consider weight functions :math:`w(x)`
corresponding to *probability density functions*, which satisfy:

.. math::

    \int_{\alpha}^{\beta} \; w(x) \;  dx \, \, = \,\, 1

Moreover, we consider families of *orthonormal* polynomials
:math:`(P_n)_{n\geq 0}`, that is polynomials with a unit norm:

.. math::

    \|P_n\| \, \, = \, \, 1

| Any sequence of orthogonal polynomials has a recurrence formula
  relating any three consecutive polynomials as follows:

  .. math::

      P_{n+1}\ =\ (a_nx+b_n)\ P_n\ +\ c_n\ P_{n-1}

| **Orthogonormal polynomials with respect to usual probability
  distributions**

| Below, a table showing an example of particular (normalized)
  orthogonal polynomials associated with *continuous* weight functions.
| Note that the orthonormal polynomials are
  orthonormal with respect to the standard representative distribution
  of the given distribution.

+-----------------+------------------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Ortho. poly.    | :math:`P_n(x)`                                                                           | Weight :math:`w(x)^{\strut}`                                                                                        | Recurrence coefficients :math:`(a_n,b_n,c_n)`                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
+=================+==========================================================================================+=====================================================================================================================+===================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
| Hermite         | :math:`{He}_n(x)^{\strut}`                                                               | :math:`\displaystyle \frac{1}{\sqrt{2 \pi}} e^{-\frac{x^2}{2}}`                                                     | :math:`\begin{array}{ccc} a_n & = & \frac{1}{\sqrt{n+1}} \\     b_n & = & 0 \\ c_n & = &  - \sqrt{\frac{n}{n+1}} \end{array}`                                                                                                                                                                                                                                                                                                                                                                                     |
+-----------------+------------------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Legendre        | :math:`\begin{array}{c} {Le}_n(x) \\ \\ \alpha>-1 \\ \end{array}`                        | :math:`\displaystyle \frac{1}{2}^{\strut} \times \mathbb{I}_{[-1,1]}(x)`                                            | :math:`\begin{array}{ccc} a_n & = & \frac{\sqrt{(2n+1)(2n+3)}}{n+1} \\     b_n & = & 0 \\ c_n & = &  -\frac{ n \sqrt{2n+3} }{ (n+1)\sqrt{2n-1} } \end{array}`                                                                                                                                                                                                                                                                                                                                                     |
+-----------------+------------------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Laguerre        | :math:`L_n^{(\alpha)}(x)`                                                                | :math:`\displaystyle \frac{x^{k-1}}{\Gamma(k)}~e^{-x} \mathbb{I}_{[0,+\infty[}(x)`                                  | :math:`\begin{array}{ccc}  \omega_{n} & = & \left((n+1)(n+k+1) \right)^{-1/2} \\ a_n & = & \omega_{n} \\     b_n & = & -(2n+k+1)~\omega_{n} \\ c_n & = &  -\sqrt{(n+k)n}~\omega_{n} \end{array}`                                                                                                                                                                                                                                                                                                                  |
+-----------------+------------------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Jacobi          | :math:`\begin{array}{c} J^{(\alpha,\beta)}_n(x) \\ \\ \\ \alpha,\beta>-1 \\ \end{array}` | :math:`\frac{(1-x)^{\alpha}(1+x)^{\beta}}{2^{\alpha + \beta + 1} B(\beta + 1, \alpha + 1)} \mathbb{I}_{[-1,1]}(x)`  | :math:`\begin{array}{ccc}  K_{1,n} & = & \frac{2n+\alpha + \beta + 3}{(n+1)(n+\alpha+1)(n+\beta+1)(n+\alpha+\beta+1)} \\ \\ K_{2,n} & = & \frac{1}{2} \sqrt{(2n + \alpha + \beta + 1) K_{1,n}} \\ \\a_n & = & K_{2,n}(2n+\alpha + \beta + 2)  \\   \\  b_n & = & K_{2,n}\frac{(\alpha - \beta)(\alpha + \beta)}{2n+\alpha+\beta} \\ \\ c_n & = & - \frac{2n+\alpha+\beta + 2}{2n+\alpha+\beta} \Big[(n+\alpha)(n+\beta) \\ & & \times (n+\alpha+\beta)n\frac{K_{1,n}}{2n+\alpha+\beta-1}\Big]^{1/2}  \end{array}` |
+-----------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Furthermore, two families of orthonormal polynomials with respect to
*discrete* probability distributions are available, namely
the so-called Charlier and Krawtchouk polynomials:

+----------------------------------+-------------------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Ortho. poly.                     | :math:`P_n(x)`                                                                            | Probability mass function                                                                                      | Recurrence coefficients :math:`(a_n,b_n,c_n)`                                                                                                                                                                           |
+==================================+===========================================================================================+================================================================================================================+=========================================================================================================================================================================================================================+
| Charlier                         | :math:`\begin{array}{c} Ch^{(\lambda)}_n(x) \\ \\ \lambda>0 \\ \end{array}`               | :math:`\begin{array}{c} \displaystyle{\frac{\lambda^k}{k!}~e^{-\lambda}} \\ \\ k=0,1,2,\dots \\ \end{array}`   | :math:`\begin{array}{ccc} a_n & = & - \frac{1}{\sqrt{\lambda (n+1)}} \\   \\  b_n & = & \frac{n+\lambda}{\sqrt{\lambda (n+1)}} \\ \\ c_n & = &  - \sqrt{1 - \frac{1}{n+1}} \end{array}`                                 |
+----------------------------------+-------------------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Krawtchouk\ :math:`^{\dagger}`   | :math:`\begin{array}{c} Kr^{(m,p)}_n(x) \\ \\ m \in \Nset~,~p \in [0,1] \\ \end{array}`   | :math:`\begin{array}{c} \displaystyle{\binom{m}{k}p^k (1-p)^{m-k}} \\ \\ k=0,1,2,\dots \\ \end{array}`         | :math:`\begin{array}{ccc} a_n & = & - \frac{1}{\sqrt{(n+1)(m-n)p(1-p)}} \\   \\  b_n & = & \frac{p(m-n)+n(1-p)}{\sqrt{(n+1)(m-n)p(1-p)}} \\ \\ c_n & = &  - \sqrt{(1 - \frac{1}{n+1})(1+\frac{1}{m-n})} \end{array}`    |
+----------------------------------+-------------------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

| :math:`^{\dagger}` The Krawtchouk polynomials are only defined up to a
  degree :math:`n` equal to :math:`m-1`. Indeed, for :math:`n=m`, some
  factors of the denominators of the recurrence coefficients would be
  equal to zero.
| To sum up, the distribution type are reported in
  the table below together with the associated families of orthonormal
  polynomials.

+-------------------------------------+-------------------------+----------------------------------+
| Distribution                        | Support                 | Polynomial                       |
+=====================================+=========================+==================================+
| Normal :math:`\cN(0,1)`             | :math:`\Rset`           | Hermite                          |
+-------------------------------------+-------------------------+----------------------------------+
| Uniform :math:`\cU(-1,1)`           | :math:`[-1,1]`          | Legendre                         |
+-------------------------------------+-------------------------+----------------------------------+
| Gamma :math:`\Gamma(k,1,0)`         | :math:`(0,+\infty)`     | Laguerre                         |
+-------------------------------------+-------------------------+----------------------------------+
| Beta :math:`B(\alpha,\beta,-1,1)`   | :math:`(-1,1)`          | Jacobi                           |
+-------------------------------------+-------------------------+----------------------------------+
| Poisson :math:`\cP(\lambda)`        | :math:`\Nset`           | Charlier                         |
+-------------------------------------+-------------------------+----------------------------------+
| Binomial :math:`\cB(m,p)`           | :math:`\{0,\dots,m\}`   | Krawtchouk\ :math:`^{\dagger}`   |
+-------------------------------------+-------------------------+----------------------------------+

| :math:`^{\dagger}` It is recalled that the Krawtchouk polynomials are
  only defined up to a degree :math:`n` equal to :math:`m-1`.
| **Orthogonal polynomials with respect to arbitrary probability
  distributions**

| It is also possible to generate a family of orthonormal polynomials
  with respect to an arbitrary probability distribution :math:`w(x)`.
  The well-known *Gram-Schmidt* algorithm can be used to this end. Note
  that this algorithm gives a constructive proof of the existence of
  orthonormal bases.
| However it is known to be numerically unstable, so alternative
  procedures are often used in practice. The available orthonormalization
  algorithm is the *Stieltjes* algorithm.


.. topic:: API:

    - See the available :ref:`orthogonal basis <orthogonal_basis>`.


.. topic:: Examples:

    - See :doc:`/examples/meta_modeling/functional_chaos`

