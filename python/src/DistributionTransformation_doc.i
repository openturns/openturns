%feature("docstring") OT::DistributionTransformation
R"RAW(Isoprobabilistic transformation.

Parameters
----------
left, right : :class:`~openturns.Distribution`
    The transformation that maps *left* into *right*.

Notes
-----
The DistributionTransformation maps the distribution *left* into the distribution *right*
with the transformation :math:`T`.

Let :math:`d_1` be the *left* distribution and :math:`d_2` be the *right* distribution, each
of dimension :math:`d`. The transformation is defined by:

.. math::
   T(d_1) = d_2

We recall here the definition of the distribution of the standard space associated to a given
distribution. It is defined as follows:

- If the copula is normal, it is the normal distribution with zero mean, unit variance and identity
  correlation matrix. It is obtained with the Nataf transformation.

- If the copula is elliptical, it is the associated elliptical distribution (obtained by a marginal
  transformation) which is finally transformed into a spherical distribution by a linear transformation.
  It is obtained with the generalized Nataf transformation.

- If the copula is not elliptical, it is the normal distribution with zero mean, unit variance and
  identity correlation matrix. It is obtained with the Rosenblatt transformation.

The library treats several cases.

**Case 1**: Both distributions :math:`d_1` and :math:`d_2` are identical, then :math:`T` is the
Identity transformation:

.. math::
   T = Id

**Case 2**: Both distributions share the same copula. Then, the transformation is a marginal transformation:
each component :math:`d_1^i` is transformed into the associated component :math:`d_2^i` by the
transformation :math:`T_i = (F_2^i)^{-1} \circ F_1^i`. Then :math:`T` is defined by:

.. math::
   T = (T_1, \dots, T_d)

**Case 3**: Both distributions share the same standard space. Then we can define the transformation
:math:`T_i` that maps :math:`d_i` into the distribution of its standard space. As we have
:math:`T_1(d_1) = T_2(d_2)`, we get: 

.. math::
   T = T_2^{-1}\circ T_1

Case 4: both distributions have different standard spaces. Then, we denote by :math:`T_i` the
transformation that transforms :math:`d_i` into the normal distribution with zero mean, unit variance
and identity correlation matrix. The final transformation :math:`T` is defined by:


.. math::
   T = T_2^{-1}\circ T_1

Examples
--------
We create a transformation :math:`T` that maps the bivariate random vector :math:`(X_1, X_2)` such that :math:`X_1`
and
:math:`X_2` follow a Uniform distribution on :math:`[-1,1]`, with a Clayton copula parameterized by
:math:`\theta = 1.2`, into the bivariate Gaussian random vector :math:`(Y_1, Y_2)` with zero mean, unit variance and
independent components.

>>> import openturns as ot
>>> distLeft = ot.JointDistribution([ot.Uniform(-1, 1)]*2, ot.ClaytonCopula(1.2))
>>> distRight = ot.Normal(2)
>>> transf = ot.DistributionTransformation(distLeft, distRight)
>>> pointOutput = transf([0, 0])
>>> print(point_output)
[0,-0.150769]
>>> transfInverse = transf.inverse()
>>> pointInit = transfInverse(pointOutput)
[0,-1.11022e-15]
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DistributionTransformation::inverse
R"RAW(Inverse isoprobabilistic transformation.

Returns
-------
inverseT : :class:`~openturns.DistributionTransformation`
    The inverse transformation.#
    

