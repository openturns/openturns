.. _sensitivity_sobol_from_pce:

Sensitivity analysis using Sobol' indices from polynomial chaos expansion
-------------------------------------------------------------------------

In this page, we introduce the method to compute Sobol'
sensitivity indices from a polynomials chaos expansion.
Sobol' indices are introduced in :ref:`sensitivity_sobol` and
polynomial chaos expansion (PCE) is introduced in :ref:`functional_chaos`.

Introduction
~~~~~~~~~~~~

Sobol'-Hoeffding is the decomposition of a function on a basis
made of orthogonal functions.
Since the PCE expansion is also an orthogonal decomposition,
the Sobol' decomposition of a function can be expressed depending
on its PCE (see (Knio & Le Maitre, 2006) page 139).
As a result, Sobol' indices can be obtained analytically from the
coefficients of the PCE (see (Sudret, 2006), (Sudret, 2008)).

Consider the input random vector
:math:`\vect{X} = \left( X_1,\ldots,X_{n_X} \right)`
and the output random variable :math:`Y` of the physical model:

.. math::
    Y = \operatorname{g}(\vect{X}).

Variance and part of variance of a PCE
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`n_X` be the dimension of the input random vector.
Let :math:`P \in \Nset` be the number of coefficients in the functional
basis.
Let :math:`\mathcal{J}_P \subseteq \Nset^{n_X}` the set of multi-indices up
to the index :math:`P`.
Depending on the way the coefficients are computed, the set of multi-indices
is the consequence of the choice of the polynomial degree, the enumeration rule,
and, if necessary, the selection method (e.g. the LARS selection method).
Let :math:`\tilde{h}(\bdZ)` be the polynomial chaos expansion:

.. math::
    \tilde{h}(\bdZ)
    &= \sum_{\idx \in \mathcal{J}_P} a_\idx \psi_\idx(\bdZ)

where :math:`\bdZ` is the standardized input random vector,
:math:`\{a_\idx\}_{\idx \in \mathcal{J}_P}` are the coefficients
and :math:`\{\psi_\idx\}_{\idx \in \mathcal{J}_P}` are the functions in the
functional basis.

The variance of the polynomial chaos expansion is:

.. math::
    \Var{\tilde{h}(\bdZ)} = \sum_{\idx \in \mathcal{J}_P} a_\idx^2 \|\psi_\idx\|^2.

In the previous expression, let us emphasise that the variance is a sum
of squares, excepted the :math:`a_0` coefficient.
If the polynomial basis is orthonormal, the expression is particularly
simple (see (Le Gratiet, et. al., 2017) eq. 38.43 page 1301):

.. math::
    \Var{\tilde{h}(\bdZ)} = \sum_{\idx \in \mathcal{J}_P} a_\idx^2.

The part of variance of the multi-index :math:`\idx` is:

.. math::
    \operatorname{PoV}_\idx
    = \frac{a_\idx^2 \|\psi_\idx\|^2}{\Var{\tilde{h}(\bdZ)}}.

The sum of the part of variances of all multi-indices is equal to 1:

.. math::
    \sum_{\idx \in \mathcal{J}_P} \operatorname{PoV}_\idx = 1.

Hence, we can identify the multi-indices which contribute
more significantly to the variance of the output by sorting the
multi-indices by decreasing order of their part of variance.
This result is printed by the `str` representation of the
`FunctionalChaosSobolIndices` class and is accessed by the
`print` function: see an example of this below.

All the Sobol' indices that we introduce in this section depend on a
specific set of multi-indices.

Sets of multi-indices
~~~~~~~~~~~~~~~~~~~~~

Let :math:`\mathcal{J}^S \subseteq \mathcal{J}_P` a subset of the multi-indices
involved in the polynomial chaos expansion.
Let :math:`\operatorname{S}^{PCE}` be the function of the coefficients associated
to the multi-indices :math:`\mathcal{J}^S`, defined by:

.. math::
    \operatorname{S}^{PCE}\left(\mathcal{J}^S\right)
    = \frac{\sum_{\idx \in \mathcal{J}^S} a_\idx^2 \|\psi_\idx\|^2}{\Var{\tilde{h}(\bdZ)}}.

Then any Sobol' index :math:`S` can be defined by the equation:

.. math::
    S = \operatorname{S}^{PCE}\left(\mathcal{J}^S\right).

If the polynomial basis is orthonormal, therefore:

.. math::
    \operatorname{S}^{PCE}\left(\mathcal{J}^S\right)
    = \frac{\sum_{\idx \in \mathcal{J}^S} a_\idx^2}{\Var{\tilde{h}(\bdZ)}}.


Hence, in the methods presented below, each Sobol' index is defined
by its corresponding set of multi-indices.

Classical Sobol' indices of a single variable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`i \in \{0, ..., n_X - 1\}` the index of an input
variable.
Let :math:`\mathcal{J}_i^S` the set of multi-indices such that
:math:`\alpha_i > 0` and the other components of the multi-indices are
zero (see (Le Gratiet, et. al., 2017) eq. 38.44 page 1301):

.. math::
    \mathcal{J}_i^S
    =\left\{\idx=(0, ..., 0, \alpha_i, 0, ..., 0) \in \mathcal{J}_P, \quad
    \alpha_i > 0 \right\}.

Therefore, the first order Sobol' index :math:`S_i` of the variable
:math:`X_i` is:

.. math::
    S_i = \operatorname{S}^{PCE}\left(\mathcal{J}_i^S\right).

Let :math:`\mathcal{J}_i^T` the set of multi-indices such that
:math:`\alpha_i > 0` (see (Le Gratiet, et. al., 2017) eq. 38.45 page 1301):

.. math::
    \mathcal{J}_i^T
    =\left\{\idx = (\alpha_0,...,\alpha_i,...,\alpha_{n_X - 1}) \in \mathcal{J}_P, \quad
    \alpha_i > 0 \right\}.

Therefore, the total index :math:`S^T_i` is:

.. math::
    S^T_i = \operatorname{S}^{PCE}\left(\mathcal{J}_i^T\right).

Interaction Sobol' indices of a group of variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`\bdu \subseteq \{0, ..., n_X - 1\}` the list of variable indices
in the group.
Let :math:`\mathcal{J}_\bdu` the set of multi-indices:

.. math::
    \mathcal{J}_\bdu
    =\left\{\idx \in \mathcal{J}_P,
    \quad \idx \neq \boldsymbol{0}, \quad
    \alpha_i > 0 \textrm{ if } i \in \bdu, \quad
    \alpha_i = 0 \textrm{ if } i \not \in \bdu, \quad
    i = 1, \ldots, n_X \right\}.

Therefore, the interaction (high order) Sobol' index :math:`S_\bdu` is:

.. math::
    S_\bdu = \operatorname{S}^{PCE}\left(\mathcal{J}_\bdu\right).


Let :math:`\mathcal{J}_\bdu` the set of multi-indices:

.. math::
    \mathcal{J}_\bdu^{T, i}
    = \left\{\boldsymbol{\alpha} \in \mathcal{J}_P, \quad \boldsymbol{\alpha} \neq \boldsymbol{0}, \quad
    \alpha_i > 0 \textrm{ if } i \in \bdu, \quad
    i = 1, \ldots, n_X \right\}.

Therefore, the total interaction (high order) Sobol' index :math:`S_\bdu` is:

.. math::
    S_\bdu^{T, i} = \operatorname{S}^{PCE}\left(\mathcal{J}_\bdu^{T, i}\right).

Closed first order and total Sobol' indices of a group of variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`\mathcal{J}_\bdu^{S, \operatorname{cl}}` the set of multi-indices such that
each component of :math:`\idx` is contained in the group :math:`\bdu`:

.. math::
    \mathcal{J}_\bdu^{S, \operatorname{cl}}
    = \left\{\idx \in \mathcal{J}_P, \quad
      \idx \neq \boldsymbol{0}, \quad \alpha_i = 0 \quad \textrm{ if } \quad
      i \not \in \bdu, \quad
      i = 1, \ldots, n_X \right\}.

Therefore, the first order (closed) Sobol' index :math:`S^{\operatorname{cl}}_\bdu`
is:

.. math::
    S^{\operatorname{cl}}_\bdu
    = \operatorname{S}^{PCE}\left(\mathcal{J}_\bdu^{S, \operatorname{cl}}\right).

Let :math:`\mathcal{J}_\bdu^T` the set of multi-indices:

.. math::
    \mathcal{J}_\bdu^T
    = \left\{\idx \in\mathcal{J}_P, \quad \idx \neq \boldsymbol{0}, \quad
      \exists i \in \{1, \ldots, n_X\} \quad
      \textrm{s.t.}  \quad i \in \bdu \textrm{ and } \alpha_i > 0 \right\}.

Therefore, the total Sobol' index :math:`S^T_\bdu` is:

.. math::
    S^T_\bdu = \operatorname{S}^{PCE}\left(\mathcal{J}_\bdu^T\right).



Summary
~~~~~~~

The next table presents the multi-indices involved in each Sobol' index.

+-------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
| Single variable or group            | Sensitivity Index    | Multi-indices                                                                                                            |
+=====================================+======================+==========================================================================================================================+
| One single variable :math:`i`       | First order          | :math:`\alpha_j > 0 \textrm{ if } j = i, \textrm{ and } \alpha_j = 0 \textrm{ if } j \neq i, \quad j=1, \ldots, n_X`     |
+-------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
|                                     | Total                | :math:`\alpha_i > 0`                                                                                                     |
+-------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
| Interaction of a group :math:`\bdu` | First order          | :math:`\alpha_i > 0 \textrm{ if } i \in \bdu, \quad \alpha_i = 0 \textrm{ if } i \not \in \bdu`                          |
+-------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
|                                     | Total interaction    | :math:`\alpha_i >0 \textrm{ if } i \in \bdu`                                                                             |
+-------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
| Group (closed) :math:`\bdu`         | First order (closed) | :math:`\alpha_i = 0 \textrm{ if } i \not\in \bdu, \quad i = 1, \ldots, n_X`                                              |
+-------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
|                                     | Total                | :math:`\exists i\in\{1,\ldots, n_X\} \quad \textrm{s.t.} \quad i \in \bdu \textrm{ and } \alpha_i > 0`                   |
+-------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+

**Table 1.** Multi-indices involved in the first order and total Sobol' indices of a single variable :math:`i` or a group :math:`\bdu`.

.. topic:: API:

    - See :class:`~openturns.SobolIndicesAlgorithm` for indices based on sampling
    - See :class:`~openturns.FunctionalChaosSobolIndices` for indices based on chaos expansion

.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/sensitivity_analysis/plot_sensitivity_sobol`
    - See :doc:`/auto_reliability_sensitivity/sensitivity_analysis/plot_functional_chaos_sensitivity`

.. topic:: References:

    - Sudret, B. (2006). Global sensitivity analysis using polynomial chaos expansions. In. Proceedings of the 5th International Conference on Computational Stochastic Mechanics (CSM5), Rhodos (2006)
    - Sudret, B. (2008). Global sensitivity analysis using polynomial chaos expansions. *Reliability engineering & system safety*, *93* (7), 964-979.
    - Knio, O. M., & Le Maitre, O. P. (2006). Uncertainty propagation in CFD using polynomial chaos decomposition. *Fluid dynamics research*, *38* (9), 616.
    - Le Gratiet, L., Marelli, S., & Sudret, B. (2017). Metamodel-based sensitivity analysis: polynomial chaos expansions and Gaussian processes. In Handbook of uncertainty quantification (pp. 1289-1325). Springer, Cham.

