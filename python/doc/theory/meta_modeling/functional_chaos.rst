.. _functional_chaos:

Functional Chaos Expansion
--------------------------

Introduction
~~~~~~~~~~~~

Accounting for the joint probability density function (PDF)
:math:`f_{\vect{X}}(\vect{x})` of the input random vector
:math:`\vect{X}`, one seeks the joint PDF of the model response
:math:`\vect{Y} = h(\vect{X})`. This may be achieved using
Monte Carlo (MC) simulation, i.e. by evaluating the model :math:`h`
at a large number of realizations :math:`\vect{x}^{(i)}` of
:math:`\vect{X}` and then by estimating the empirical
distribution of the corresponding model output sample
:math:`h(\vect{x}^{(i)})`. However, the MC
method may require a large number of model evaluations, i.e. a great
computational cost, in order to obtain accurate results.
In fact, when using MC simulation, each model run is performed
independently. Thus, whereas it is expected that
:math:`h(\vect{x}^{(i)}) \approx h(\vect{x}^{(j)})` if
:math:`\vect{x}^{(i)} \approx \vect{x}^{(j)}`, the model is
evaluated twice without accounting for this information. In other
words, the functional dependence between :math:`\vect{X}` and
:math:`\vect{Y}` is lost.

A possible solution to overcome this problem and thereby to reduce the
computational cost of MC simulation is to represent the random
response :math:`\vect{Y}` in a suitable functional space, such as
the Hilbert space :math:`\cL^2` of square-integrable functions with
respect to the PDF :math:`f_{\vect{X}}(\vect{x})`.
More precisely, we may consider an expansion of the model response
onto an orthonormal basis of :math:`\cL^2`.
As an example of this type of expansion, one can mention responses by
wavelet expansion, polynomial chaos expansion, etc.
The principles of the building of a (infinite numerable) basis of this
space, i.e. the PC basis, are described in the sequel.

Physical model
~~~~~~~~~~~~~~

We consider the output random vector:

.. math::

    \vect{Y} = g(\vect{X})

where :math:`g: \Rset^{n_X} \rightarrow \Rset^{n_Y}` is the physical model,
:math:`\vect{X} \sim \mu_{\vect{X}}` is the physical input,
:math:`\mu_{\vect{X}}` is the distribution of the physical input random vector,
:math:`n_X \in \Nset` is the input dimension,
:math:`n_Y \in \Nset` is the output dimension.
We assume that :math:`\vect{Y}` has finite variance i.e.
:math:`g\in L_{\mu_{\vect{X}}}^2(\Rset^{n_X}, \Rset^{n_Y})`.

When the dimension of the output is greater than 1, i.e. when
:math:`n_Y > 1`, the functional chaos algorithm is used on each marginal
of :math:`\vect{Y}`, using the same multivariate orthonormal basis for
all the marginals.
Thus, the method is detailed here for a scalar output :math:`Y` and
:math:`g: \Rset^{n_X} \rightarrow \Rset`.

Iso-probabilistic transformation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`T: \Rset^{n_X} \rightarrow \Rset^{n_X}` be an isoprobabilistic transformation
such that :math:`\vect{Z} = T(\vect{X}) \sim \mu_{\vect{Z}}` where :math:`\mu_{\vect{Z}}` is the
distribution of the standardized random vector :math:`\vect{Z}`,
i.e. the standard :math:`n_X`-dimensional normal distribution.
The distribution is called the *measure* below.
Let :math:`f_{\vect{Z}}` be the probability density function of the standardized
random vector.
Let :math:`h` be the function defined by the equation:

.. math::
    h = g \circ T^{-1}.

Therefore :math:`h \in L_{\mu_{\vect{Z}}}^2(\Rset^{n_X}, \Rset)`.


Orthonormal basis
~~~~~~~~~~~~~~~~~

The distribution :math:`\mu_{\vect{Z}}` defines the scalar product that defines the orthogonality
property of the functional basis.
Let :math:`(\Psi_k)_{k \in \Nset}` be an orthonormal multivariate basis of
:math:`\cL^2_{\mu_{\vect{Z}}}(\Rset^{n_X},\Rset)`.
By hypothesis, the orthonormality property of the functional chaos basis implies:

.. math::

    \langle \Psi_i, \Psi_{j}\rangle
    = \int_{\cZ}\Psi_i(\vect{z}) \Psi_{j}(\vect{z}) ~ f_{\vect{Z}}(\vect{z}) d \vect{z} = \delta_{i,j}.

where :math:`\cZ \subseteq \Rset^{n_X}` is the domain of the standardized
random vector and :math:`\delta_{i,j} =1` is the Kronecker symbol:

.. math::

  \delta_{i,j}
  =
  \begin{cases}
  1 & \textrm{ if } i = j \\
  0 & \textrm{otherwise.}
  \end{cases}

This basis can be created using polynomials.
More details on this topic are presented in :ref:`chaos_basis`.

Functional chaos expansion
~~~~~~~~~~~~~~~~~~~~~~~~~~

The functional chaos decomposition of *h* is (see [lemaitre2010]_ page 39):

.. math::

    h = \sum_{k \geq 0} a_k \Psi_k.

Let :math:`\cK \subset \Nset` be a finite subset of the coefficients
representing the truncation of the infinite series.
Truncating this series can be done using one enumeration rule,
as presented in :ref:`enumeration_strategy`.
If the number of coefficients in :math:`\cK` is too large,
this can lead to *overfitting*.
This may happen e.g. if the total polynomial order we choose is too large.
In order to limit this effect, one method is to select the coefficients which
best predict the output, as presented in :ref:`polynomial_sparse_least_squares`.

The metamodel :math:`\widetilde{h}(\vect{z})` is represented by a
*finite* subset of coefficients in a *truncated* basis:

.. math::

    \widetilde{h}(\vect{z})= \sum_{k \in \cK}  a_k \Psi_k(\vect{z})


The approximation :math:`\widetilde{h}` can be used to build an efficient
random generator of :math:`Y` based on the random vector :math:`\vect{Z}`,
using the equation:

.. math::

    \widetilde{Y} = \widetilde{h}(\vect{Z})

This equation can be used to simulate independent random observations
from the PCE.
This can be done by first simulating independent observations from
the distribution of the standardized random vector :math:`\vect{Z}`,
then push forward these observations through the expansion.
See the :class:`~openturns.FunctionalChaosRandomVector` class
for more details on this topic.

The functional chaos decomposition can be used to build a meta model of *g*,
using the equation:

.. math::

    \widetilde{g} = \widetilde{h} \circ T

If the basis :math:`(\Psi_k)_{k \in \Nset}` is based on the tensor product of
univariate orthonormal polynomials, then the distribution
:math:`\mu_{\vect{Z}}` is the product
:math:`\mu_{\vect{Z}} = \prod_{i=1}^{n_X} \mu_{\vect{Z},i}`.
In that case only, the Sobol' indices can easily be computed from the
coefficients :math:`a_k`.

.. topic:: API:

    - See :class:`~openturns.FunctionalChaosAlgorithm`


.. topic:: Examples:

    - See :doc:`/auto_meta_modeling/polynomial_chaos_metamodel/plot_functional_chaos`


.. topic:: References:

    - [lemaitre2010]_
    - [sullivan2015]_, chapter 11 section 11.3 page 237
    - [xiu2010]_

