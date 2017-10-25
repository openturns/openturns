Generalized Nataf Transformation
--------------------------------

The Generalized Nataf transformation is an isoprobabilistic
transformation (refer to ) which is used under the following context :
:math:`\vect{X}` is the input random vector, :math:`F_i` the
cumulative density functions of its components and :math:`C` its
copula, which is supposed to be elliptical.

Let us denote by :math:`\vect{d}` a deterministic vector,
:math:`g(\vect{X}\,,\,\vect{d})` the limit state function of the
model,
:math:`\cD_f = \{\vect{X} \in \Rset^n \, / \, g(\vect{X}\,,\,\vect{d}) \le 0\}`
the event considered here and g(,) = 0 its boundary.

One way to evaluate the probability content of the event
:math:`\cD_f`:

.. math::

    \label{PfX}
        P_f = \Prob{g(\vect{X}\,,\,\vect{d})\leq 0}=   \int_{\cD_f}  \pdf\, d\vect{x}

is to use the Generalized Nataf transformation :math:`T` which is a
diffeomorphism from :math:`\supp{\vect{X}}` into the standard space
:math:`\Rset^n`, where distributions are spherical, with zero mean,
unit variance and unit correlation matrix. The type of the spherical
distribution is the type of the elliptical copula :math:`C`.

The Generalized Nataf transformation presented here is a generalisation
of the traditional Nataf transformation (see [Nataf1962]_): the reference
[Lebrun2009a]_ shows that the Nataf transformation can be used
only if the copula of :math:`\vect{X}` is normal. The Generalized Nataf
transformation (see [Lebrun2009b]_) extends the Nataf
transformation to elliptical copulas.

Let us recall some definitions.
A random vector :math:`\vect{X}` in :math:`\Rset^n` has an *elliptical
distribution* if and only if there exists a deterministic vector
:math:`\vect{\mu}` such that the characteristic function of
:math:`\vect{X} - \vect{\mu}` is a scalar function of the quadratic
form :math:`\vect{u}^t\mat{\Sigma}\, \vect{u}`:

.. math::

    \begin{aligned}
        \varphi_{\vect{X}-\vect{\mu}}(\vect{u})=\psi(\vect{u}^t\,\mat{\Sigma}\, \vect{u})
      \end{aligned}

with :math:`\mat{\Sigma}` a symmetric positive definite matrix of
rank :math:`p`. As :math:`\mat{\Sigma}` is symmetric positive, it can
be written in the form :math:`\mat{\Sigma}=\mat{D}\,\mat{R}\,\mat{D}`,
where :math:`\mat{D}` is the diagonal matrix
:math:`\mat{\diag{\sigma_i}}` with :math:`\sigma_i=\sqrt{\Sigma_{ii}}`
and :math:`R_{ij}=\frac{\Sigma_{ij}}{\sqrt{\Sigma_{ii}\Sigma_{jj}}}`.

With a specific choice of normalisation for :math:`\psi`, in the case
of finite second moment, the covariance matrix of :math:`\vect{X}` is
:math:`\mat{\Sigma}` and :math:`\mat{R}` is then its linear
correlation matrix. The matrix :math:`\mat{R}` is always well-defined,
even if the distribution has no finite second moment: even in this
case, we call it the correlation matrix of the distribution. We note
:math:`\vect{\sigma}=(\sigma_1,\dots,\sigma_n)`.

We denote by :math:`E_{\vect{\mu},\vect{\sigma},\mat{R},\psi}` the
cumulative distribution function of the elliptical distribution
:math:`\cE_{\vect{\mu},\vect{\sigma}, \mat{R},\psi}`.

An *elliptical copula* :math:`C^E_{\mat{R},\psi}` is the copula of an
elliptical distribution
:math:`\cE_{\vect{\mu},\vect{\sigma},\mat{R},\psi}`.

The *generic elliptical representative* of an elliptical distribution
family :math:`\cE_{\vect{\mu},\vect{\sigma},\mat{R},\psi}` is the
elliptical distribution whose cumulative distribution function is
:math:`E_{\vect{0},\vect{1},\mat{R},\psi}`.

The *standard spherical representative* of an elliptical distribution
family :math:`\cE_{\vect{\mu},\vect{\sigma},\mat{R},\psi}` is the
spherical distribution whose cumulative distribution function is
:math:`E_{\vect{0},\vect{1},\mat{I}_n,\psi}`.

The family of distributions with marginal cumulative distribution
functions are :math:`F_1,\dots,F_n` and any elliptical copula
:math:`C^E_{\mat{R},\psi}` is denoted by
:math:`{\cD}_{F_1,\dots,F_n,C^E_{\mat{R},\psi}}`. The cumulative
distribution function of this distribution is noted
:math:`D_{F_1,\dots,F_n,C^E_{\mat{R},\psi}}`.

The random vector :math:`\vect{X}` is supposed to be continuous and
with full rank. It is also supposed that its cumulative marginal
distribution functions :math:`F_i` are strictly increasing (so they
are bijective) and that the matrix :math:`\mat{R}` of its elliptical
copula is symmetric positive definite.

**Generalized Nataf transformation**: Let :math:`\vect{X}` in
:math:`\Rset^n` be a continuous random vector following the
distribution :math:`D_{F_1,\dots,F_n,C^E_{\mat{R},\psi}}`. The
*Generalized Nataf transformation* :math:`T_{Nataf}^{gen}` is defined
by:

.. math:: \vect{u} = T_{Nataf}^{gen}(\vect{X})=T_3\circ T_2\circ T_1(\vect{X})

where the three transformations :math:`T_1`, :math:`T_2` and
:math:`T_3` are given by:

.. math::

    \begin{array}{l}
          \begin{array}{rcl}
            T_1 : \Rset^n & \rightarrow & \Rset^n\\
            \vect{x} & \mapsto & \vect{w}=\Tr{(F_1(x_1),\dots,F_n(x_n))}
          \end{array}\\
          \begin{array}{rcl}
            T_2 : \Rset^n & \rightarrow & \Rset^n\\
            \vect{w} & \mapsto & \vect{v}=\Tr{(E^{-1}(w_1),\dots,E^{-1}(w_n))}
          \end{array}\\
          \begin{array}{rcl}
            T_3 : \Rset^n & \rightarrow & \Rset^n\\
            \vect{v} & \mapsto & \vect{u}=\mat{\Gamma}\,\vect{v}
          \end{array}
        \end{array}

where :math:`E` is the cumulative distribution function of the
standard 1-dimensional elliptical distribution with characteristic
generator :math:`\psi` and :math:`\mat{\Gamma}` is the inverse of the
Cholesky factor of :math:`\mat{R}`.

The distribution of :math:`\vect{W}=T_2\circ T_1(\vect{X})` is the
generic elliptical representative associated to the copula of
:math:`\vect{X}`. The step :math:`T_3` maps this distribution into its
standard representative, following exactly the same algebra as the
normal copula. Thus, in the Generalized Nataf standard space, the
random vector :math:`\vect{U}` follows the standard representative
distribution of the copula of the physical random vector
:math:`\vect{X}`.

If the copula of :math:`\vect{X}` is normal, :math:`\vect{U}` follows
the standard normal distribution with independent components.


.. topic:: API:

    - See the available :ref:`Nataf transformations <isoprobabilistic_transformations>`.

.. topic:: References:

    - O. Ditlevsen and H.O. Madsen, 2004, "Structural reliability methods," Department of mechanical engineering technical university of Denmark - Maritime engineering, internet publication.
    - J. Goyet, 1998, "Sécurité probabiliste des structures - Fiabilité d'un élément de structure," Collège de Polytechnique.
    - A. Der Kiureghian, P.L. Liu, 1986,"Structural Reliability Under Incomplete Probabilistic Information", Journal of Engineering Mechanics, vol 112, no. 1, pp85-104.
    - [Lebrun2009a]_
    - [Lebrun2009b]_
    - [Lebrun2009c]_
    - H.O. Madsen, Krenk, S., Lind, N. C., 1986, "Methods of Structural Safety," Prentice Hall.
    - [Nataf1962]_

