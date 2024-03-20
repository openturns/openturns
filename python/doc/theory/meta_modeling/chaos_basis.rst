.. _chaos_basis:

Polynomial chaos basis
----------------------

This page is focused on a specific kind of functional chaos
representation that has been implemented, namely
*polynomial chaos expansions*.

Polynomial chaos expansion
~~~~~~~~~~~~~~~~~~~~~~~~~~

We consider the notations introduced in :ref:`functional_chaos`.
Let :math:`T: \Rset^{n_X} \rightarrow \Rset^{n_X}` be an isoprobabilistic transformation mapping the input physical random vector
:math:`\vect{X}` into the standardized input random vector :math:`\vect{Z}`.
More precisely, we have:

.. math::
    \vect{Z} = T(\vect{X}) \sim \mu_{\vect{Z}}

where :math:`\mu_{\vect{Z}}` is the
distribution of the standardized random vector :math:`\vect{Z}`.
The model response is assumed to be a
*scalar* random variable :math:`Y = h(\vect{Z})`. However, the
following derivations hold in case of a vector-valued response.
Let us assume that:

-  :math:`Y = h(\vect{Z})` has a finite variance, i.e.
   :math:`\Var{h(\vect{Z})} < \infty`;

-  :math:`\vect{Z}` has independent components.


For any functions :math:`h_1, h_2 \in \cL^2(\Rset^{n_X}, \Rset)`,
we consider the scalar product:

.. math::
    \left\langle h_1, \; h_2 \right\rangle
    = \Expect{h_1(\vect{Z}) \; h_2(\vect{Z})}
    = \int_{\cZ} h_1(\vect{z}) h_2(\vect{z}) f_{\vect{Z}}(\vect{z}) d\vect{z}

where :math:`\cZ \subseteq \Rset^{n_X}` is the domain of the standardized
input random vector :math:`\vect{Z}` and
:math:`f_{\vect{Z}}` is the probability density function
of the standardized random vector :math:`\vect{Z}`.
Let :math:`\{\psi_k : \Rset^{n_X} \rightarrow \Rset\}_{k \geq 0}`
be a set of multivariate polynomials
orthonormal with respect to the joint PDF
:math:`f_{\vect{Z}}(\vect{z})`, that is:

.. math::
    \langle \psi_j, \; \psi_k \rangle
    =  \delta_{j,k}

for any integers :math:`j, k \in \Nset` where :math:`\delta_{j,k}`
is the Kronecker symbol:

.. math::

  \delta_{i,j}
  =
  \begin{cases}
  1 & \textrm{ if } i = j \\
  0 & \textrm{otherwise.}
  \end{cases}

Under some hypotheses on the function :math:`h` and the functional basis
:math:`\{\psi_k\}_{k \geq 0}`, it can be proved that :math:`Y` can
be expanded onto the PC basis as follows (see [lemaitre2010]_ page 39):

.. math::
    :label: PC

    Y = h(\vect{Z})
    =  \sum_{k \geq 0} \; a_k \; \psi_k(\vect{Z}),

where :math:`\{a_k \in \Rset\}_{k \geq 0}` are real coefficients.
The coefficients :math:`a_k`\ ’s are deterministic coefficients that fully
characterize the response :math:`\vect{Y}`.

The goal of the next two sections is to present methods to create
the basis :math:`\{\psi_k \}_{k \geq 0}` in two different cases:

- if the input random vector :math:`\vect{X}` has independent marginals,
- if the input random vector :math:`\vect{X}` has a non-trivial copula.

PC basis with independent random variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We first consider the case of *independent* input random variables.
In this case, we will show how to create a orthogonal basis of
multi-variate polynomials by using the *tensor product* of
univariate orthogonal polynomials.

In practice, the components :math:`X_i` of random vector
:math:`\vect{X}` are standardized using a specific mapping
:math:`T_i`, usually referred to as an *isoprobabilistic
transformation* (see :ref:`isoprobabilistic_transformation` for
details on this topic).
The set of standardized random variables is:

.. math::
    :label: PC_isotransfo

    Z_i  =  T_i(X_i), \quad \, i=1,\dots,n_X

Common choices for :math:`Z_i` are standard distributions such as a
standard normal distribution (which leads to the Hermite orthogonal
polynomial basis) or a uniform distribution over :math:`[-1,1]` (which leads
to the Legendre orthogonal polynomial basis).

Notice that the independence of the input random
variables implies that the input joint PDF is:

.. math::
    :label: 3.010qua

    f_{\vect{Z}}(\vect{z}) = \prod_{i=1}^{n_X} f_{Z_i}(z_i)

where :math:`f_{Z_i}(z_i)` is the marginal PDF of :math:`Z_i`.
For any input marginal index :math:`i \in \{1, ..., n_X\}`,
let us consider a family :math:`\left\{\pi^{(i)}_j, j \in \Nset\right\}` of
orthonormal polynomials with respect to :math:`f_{Z_i}` where
:math:`j` represents the degree of the polynomial :math:`\pi^{(i)}_j`.
By hypothesis, the polynomials are orthonormal:

.. math::
    :label: 3.010cinq

    \left\langle \pi^{(i)}_j \; , \; \pi^{(i)}_{k} \right\rangle
    = \Expect{\pi^{(i)}_j(Z_i) \;  \pi^{(i)}_{k}(Z_i)}
    =  \delta_{j,k}

for any :math:`j, k \in \Nset`.
More details about orthogonal polynomials are presented in
:ref:`orthogonal_polynomials`.
We make the hypothesis that :math:`\pi^{(i)}_{0} = 1` for :math:`i=1,\dots,n_X`.
It can be proved that the *tensor product* of the :math:`n_X` univariate
polynomials produces a set of orthonormal multivariate polynomials
:math:`\{\psi_{\idx}, \; \idx \in \Nset^{n_X}\}` defined by:

.. math::
    :label: 3.010six

    \psi_{\idx}(\vect{z})
    = \pi^{(1)}_{\alpha_1}(z_1) \times \cdots \times \pi^{(n_X)}_{\alpha_{n_X}}(z_{n_X})

where :math:`\idx = (\alpha_1,\dots,\alpha_{n_x}) \in \Nset^{n_X}` is
a multi-index representing the marginal polynomial degrees.

PC basis with dependent random variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In the case of *dependent* variables, it is possible to build up an
orthonormal basis as follows (see [soizeghanem2004]_):

.. math::
    :label: 3.010seven

    \psi_{\idx}(\vect{z})
    = \left( \frac{f_1(z_1) \cdots f_{n_X}(z_{n_X})}{f_{\vect{Z}}(\vect{z})}\right)^{\frac{1}{2}}
    \prod_{i=1}^{n_X} \pi^{(i)}_{\alpha_{i}}(z_i).

Notice that such a basis is no longer made of polynomials. When
dealing with independent random variables, each basis element may be recast as in
:eq:`3.010six`.
Computing :math:`\psi_{\idx}(\vect{z})` can be expensive.
This is the reason why we now present other methods for specific types
of input random vectors.

If :math:`\vect{X}` has an elliptical copula instead of an independent
one, it may be recast as a random vector :math:`\vect{Z}` with
independent components using a suitable mapping
:math:`T : \vect{X} \mapsto \vect{Z}` such as the Nataf transformation
for example.
The so-called Rosenblatt transformation may also be applied in the case
of a Normal copula.

Link with classical deterministic polynomial approximation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In a deterministic setting (i.e. when the input parameters are
considered to be deterministic), it is of common practice to substitute
the model function :math:`h` by a polynomial approximation over its
whole domain of definition. Actually this approach is
strictly equivalent to:

#. Regarding the input parameters as random uniform random variables

#. Expanding any quantity of interest provided by the model onto a PC
   expansion made of Legendre polynomials


.. topic:: API:

    - See the available :ref:`orthogonal basis <orthogonal_basis>`.


.. topic:: Examples:

    - See :doc:`/auto_meta_modeling/polynomial_chaos_metamodel/plot_functional_chaos`


.. topic:: References:

    - [soizeghanem2004]_
    - [ghanem1991]_
    - [lemaitre2010]_
