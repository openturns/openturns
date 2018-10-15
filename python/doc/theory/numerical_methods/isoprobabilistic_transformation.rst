.. _isoprobabilistic_transformation:

Isoprobabilistic transformations
--------------------------------

The isoprobabilistic transformation is used in the following context:
:math:`\vect{X}` is the input random vector, :math:`F_i` the
cumulative density functions of its components and :math:`C` its
copula.
Let us denote by :math:`\vect{d}` a deterministic vector,
:math:`g(\vect{X}\,,\,\vect{d})` the limit state function of the
model,
:math:`\cD_f = \{\vect{X} \in \Rset^n \, / \, g(\vect{X}\,,\,\vect{d}) \le 0\}`
the event considered here and g(,) = 0 its boundary.
One way to evaluate the probability content :math:`P_f` of the event
:math:`\cD_f`:

.. math::
  :label: PfXIsoProb

    P_f = \Prob{g(\vect{X}\,,\,\vect{d})\leq 0}=   \int_{\cD_f} \pdf\, d\vect{x}

is to introduce an isoprobabilistic transformation :math:`T` which is
a diffeomorphism from :math:`\supp{\vect{X}}` into :math:`\Rset^n`,
such that the distribution of the random vector
:math:`\vect{U}=T(\vect{X})` has the following properties :
:math:`\vect{U}` and :math:`\mat{R}\,\vect{U}` have the same
distribution for all rotations :math:`\mat{R}\in{\cS\cO}_n(\Rset)`.
Such transformations exist and the most widely used are:

-  the Generalized Nataf transformation (refer to [lebrun2009b]_),

-  the Rosenblatt transformation (refer to [rosenblatt1952]_).

If we suppose that the numerical model :math:`g` has suitable properties
of differentiability, the evaluation of the probability :eq:`PfXIsoProb`
can be transformed in the evaluation of the probability:

.. math::
 :label: StandardSpace

    P_f = \Prob{G(\vect{U}\,,\,\vect{d})\leq 0} = \int_{\Rset^n} \boldsymbol{1}_{G(\vect{u}\,,\,\vect{d}) \leq 0}\,f_{\vect{U}}(\vect{u})\,d\vect{u}

where :math:`T` is a :math:`C^1`-diffeomorphism called an
*isoprobabilistic transformation*, :math:`f_{\vect{U}}` the
probability density function of :math:`\vect{U}=T(\vect{X})` and
:math:`G=f\circ T^{-1}`.
The vector :math:`\vect{U}` is said to be in the *standard space*,
whereas :math:`\vect{X}` is in the *physical space*.
The interest of such a transformation is the rotational invariance of
the distributions in the standard space : the random vector
:math:`\vect{U}` has a spherical distribution, which means that the
density function :math:`f_{\vect{U}}` is a function of
:math:`\|\vect{u}\|`. Thus, without loss of generality, it is possible
to map the general failure domain :math:`{\cD}` to a domain
:math:`{\cD}'` for which the design point :math:`{\vect{u}^{*}}'` (the
point of the event boundary at minimal distance from the center of the
standard space) is supported by the last axis.

The following transformations verify that property, under
some specific conditions on the dependence structure of the random
vector :math:`\vect{X}` :

- the Nataf transformation (see [nataf1962]_, [lebrun2009a]_): :math:`\vect{X}` must
  have a normal copula,

- the Generalized Nataf transformation (see [lebrun2009b]_):
  :math:`\vect{X}` must have an elliptical copula,

- the Rosenblatt transformation (see [rosenblatt1952]_, [lebrun2009c]_): there is no
  condition on the copula of :math:`\vect{X}` .

The Generalized Nataf transformation is automatically used when
the copula is elliptical and the Rosenblatt transformation for any other
case.


.. topic:: API:

    - See the available :ref:`isoprobabilistic transformations <isoprobabilistic_transformations>`.

.. topic:: References:

    - A. Der Kiureghian, P.L. Liu, 1986,"Structural Reliability Under Incomplete Probabilistic Information", Journal of Engineering Mechanics, vol 112, no. 1, pp85-104.
    - [lebrun2009a]_
    - [lebrun2009b]_
    - [lebrun2009c]_
    - [nataf1962]_
    - [rosenblatt1952]_
