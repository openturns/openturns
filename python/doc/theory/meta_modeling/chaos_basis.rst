.. _chaos_basis:

Polynomial chaos basis
----------------------

The current section is focused on a specific kind of functional chaos
representation that has been implemented, namely
*polynomial chaos expansions*.

Mathematical framework
~~~~~~~~~~~~~~~~~~~~~~

Throughout this section, the model response is assumed to be a
*scalar* random variable :math:`Y = h(\vect{X})`. However, the
following derivations hold in case of a vector-valued response.
Let us suppose that:

-  :math:`Y = h(\vect{X})` has a finite variance, i.e.
   :math:`\Var{h(\vect{X})} < \infty`;

-  :math:`\vect{X}` has independent components.

Then it is shown that :math:`\vect{Y}` may be expanded onto the
PC basis as follows:

  .. math::
    :label: PC

      Y \, \,  \equiv \, \,  h(\vect{X}) \, \, = \, \, \sum_{j=0}^{\infty} \; a_{j} \; \psi_{j}(\vect{X})

where the :math:`\psi_{j}`\ ’s are multivariate polynomials that are
orthonormal with respect to the joint PDF
:math:`f_{\vect{X}}(\vect{x})`, that is:

  .. math::
      \langle \psi_{j}(\vect{X}) \; , \; \psi_{k}(\vect{X}) \rangle \, \,
      \equiv \, \, \Expect{\psi_{j}(\vect{X}) \; \psi_{k}(\vect{X})} \, \, = \, \, \delta_{j,k}

where :math:`\delta_{j,k} = 1` if :math:`j=k` and 0 otherwise, and
the :math:`a_{j}`\ ’s are deterministic coefficients that fully
characterize the response :math:`\vect{Y}`.

Building of the PC basis – independent random variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We first consider the case of *independent* input random variables. In
practice, the components :math:`X_i` of random vector
:math:`\vect{X}` are rescaled using a specific mapping
:math:`T_i`, usually referred to as an *isoprobabilistic
transformation* (see ). The set of scaled random variables reads:

  .. math::
    :label: PC_isotransfo

      U_i \, \, = \, \, T_i(X_i) \quad \quad , \quad \, i=1,\dots,n

Common choices for :math:`U_i` are standard distributions such as a
standard normal distribution or a uniform distribution over
:math:`[-1,1]`. For simplicity, it is assumed from now on that the
components of the original input random vector :math:`\vect{X}`
have been already scaled, i.e. :math:`X_i = U_i`.

Let us first notice that due to the independence of the input random
variables, the input joint PDF may be cast as:

  .. math::
    :label: 3.010qua

      f_{\vect{X}}(\vect{x}) \, = \, \prod_{i=1}^{n} f_{X_i}(x_{i})

where :math:`f_{X_i}(x_{i})` is the marginal PDF of :math:`X_i`. Let
us consider a family :math:`\{\pi^{(i)}_{j}, j \in \Nset\}` of
orthonormal polynomials with respect to :math:`f_{X_i}`, :

  .. math::
    :label: 3.010cinq

      \langle \pi^{(i)}_{j}(X_{i}) \; , \; \pi^{(i)}_{k}(X_{i}) \rangle  \, \, \equiv \, \, \Expect{\pi^{(i)}_{j}(X_{i}) \;  \pi^{(i)}_{k}(X_{i})} \, \, = \, \, \delta_{j,k}

The reader is referred to  for details on the selection of suitable
families of orthogonal polynomials. It is assumed that the degree of
:math:`\pi^{(i)}_{j}` is :math:`j` for :math:`j>0` and
:math:`\pi^{(i)}_{0} \equiv 1` (:math:`i=1,\dots,n`). Upon tensorizing
the :math:`n` resulting families of univariate polynomials, one gets a
set of orthonormal multivariate polynomials
:math:`\{\psi_{\idx}, \idx \in \Nset^n\}` defined by:

  .. math::
    :label: 3.010six

      \psi_{\idx}(\vect{x}) \, \, \equiv \,\, \pi^{(1)}_{\alpha_{1}}(x_{1}) \times \cdots \times \pi^{(n)}_{\alpha_{n}}(x_{n})

where the multi-index notation
:math:`\idx \equiv \{\alpha_{1},\dots,\alpha_{M}\}` has been
introduced.

Building of the PC basis – dependent random variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In case of *dependent* variables, it is possible to build up an
orthonormal basis as follows:

  .. math::
    :label: 3.010seven

      \psi_{\idx}(\vect{x}) \, \, = \,\,  K(\vect{x}) \;\prod_{i=1}^M \pi^{(i)}_{\alpha_{i}}(x_{i})

where :math:`K(\vect{x})` is a function of the copula of
:math:`\vect{X}`. Note that such a basis is no longer polynomial. When
dealing with independent random variables, one gets
:math:`K(\vect{x})=1` and each basis element may be recast as in
:eq:`3.010six`. Determining :math:`K(\vect{x})` is usually
computationally expensive though, hence an alternative strategy for
specific types of input random vectors.

If :math:`\vect{X}` has an elliptical copula instead of an independent
one, it may be recast as a random vector :math:`\vect{U}` with
independent components using a suitable mapping
:math:`T : \vect{X} \mapsto \vect{U}` such as the Nataf transformation.
The so-called Rosenblatt transformation may also be applied in case
of a Gaussian copula . Thus the model response :math:`Y` may be
regarded as a function of :math:`\vect{U}` and expanded onto a
*polynomial* chaos expansion with basis elements cast as in :eq:`3.010six`.

Link with classical deterministic polynomial approximation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In a deterministic setting (i.e. when the input parameters are
considered to be deterministic), it is of common practice to substitute
the model function :math:`h` by a polynomial approximation over its
whole domain of definition as shown in . Actually this approach is
strictly equivalent to:

- Regarding the input parameters as random uniform random variables

- Expanding any quantity of interest provided by the model onto a PC
  expansion made of Legendre polynomials


.. topic:: API:

    - See the available :ref:`orthogonal basis <orthogonal_basis>`.


.. topic:: Examples:

    - See :doc:`/auto_meta_modeling/polynomial_chaos_metamodel/plot_functional_chaos`


.. topic:: References:

    - R. Ghanem and P. Spanos, 1991, "Stochastic finite elements -- A spectral approach", Springer Verlag. (Reedited by Dover Publications, 2003).
