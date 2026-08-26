.. _regression_importance_indices:

Variance-based importance indices for linear regression
--------------------------------------------------------

Variance-based importance indices (VIMs) allocate the coefficient of
determination :math:`R^2` of a linear regression among its inputs.  They
are especially useful when the inputs are correlated.  This page summarizes
the LMG, PMVD and Johnson indices, as well as the variance inflation factor
(VIF), following [clouvel2025]_.

For independent inputs, the :ref:`squared standardized regression indices <ranking_src>`
can be used instead.

Linear regression setting
~~~~~~~~~~~~~~~~~~~~~~~~~

Consider the centered linear model

.. math::

    Y = \vect{X}^{\mathsf{T}}\vect{a} + \varepsilon,
    \qquad \vect{X}=(X_1,\ldots,X_d),

where :math:`\vect{a}` is the vector of regression coefficients and
:math:`\varepsilon` is the residual.  The coefficient of determination is

.. math::

    R^2 = \frac{\Var{\Expect{Y\,|\,\vect{X}}}}{\Var{Y}}.

For a subset :math:`u\subseteq\{1,\ldots,d\}`, let
:math:`R^2_{Y(\vect{X}_u)}` denote the coefficient of determination of
the regression using only the inputs in :math:`u`.  Define the marginal
contribution of input :math:`i` after the subset :math:`u` by

.. math::

    \Delta_i(u) = R^2_{Y(\vect{X}_{u\cup\{i\}})}
                 - R^2_{Y(\vect{X}_u)}.

The indices below are normalized so that their sum is :math:`R^2`.

Variance inflation factor
~~~~~~~~~~~~~~~~~~~~~~~~~

The VIF is a diagnostic for multicollinearity, rather than an importance
index.  For input :math:`X_i`, it is defined by

.. math::

    \operatorname{VIF}_i =
    \frac{1}{1-R^2_{X_i(\vect{X}_{-i})}},

where :math:`\vect{X}_{-i}` contains all inputs except :math:`X_i`.
The minimum value is one; large values indicate that the input is strongly
explained by the other inputs.

LMG indices
~~~~~~~~~~~

The Lindeman--Merenda--Gold index averages the marginal contribution over
all orderings of the inputs.  Let :math:`\pi=(\pi_1,\ldots,\pi_d)` denote
a permutation of :math:`\{1,\ldots,d\}`, and let :math:`\pi_k` be the input
at position :math:`k`.  In sums over :math:`\pi`, all :math:`d!` permutations
are considered.

.. math::

    \operatorname{LMG}_i =
    \frac{1}{d!}\sum_{\pi}
    \Delta_i\left(\{\pi_1,\ldots,\pi_{k-1}\}\right),
    \qquad \text{where } k \text{ is such that } \pi_k=i.

Equivalently, LMG can be written as an average over all
subsets:

.. math::

    \operatorname{LMG}_i =
    \frac{1}{d}\sum_{u\subseteq\{1,\ldots,d\}\setminus\{i\}}
    \binom{d-1}{|u|}^{-1}\Delta_i(u).

Thus, LMG is the Shapley value of the regression game.  It shares the
contribution of correlated inputs and can assign a nonzero value to an input
whose regression coefficient is zero but which is correlated with another
input.

PMVD indices
~~~~~~~~~~~~

The proportional marginal variance decomposition (PMVD) uses the same
marginal contributions, but weights the orderings proportionally.  For an
ordering :math:`\pi`, define

.. math::

    L(\pi) = \prod_{k=1}^{d-1}
    \left[
        R^2_{Y(\vect{X})}
        - R^2_{Y(\vect{X}_{\{\pi_1,\ldots,\pi_k\}})}
    \right]^{-1},

where the term in brackets is the difference between the coefficient of
determination of the full model and that of the model using the preceding
inputs.  The PMVD index is

.. math::

    \operatorname{PMVD}_i =
    \sum_{\pi}\frac{L(\pi)}{\sum_{\pi'}L(\pi')}
    \Delta_i\left(\{\pi_1,\ldots,\pi_{k-1}\}\right),
    \qquad \text{where } k \text{ is such that } \pi_k=i.

Unlike LMG, PMVD assigns zero importance to an input whose regression
coefficient is zero, including when that input is correlated with another
input.

Johnson indices
~~~~~~~~~~~~~~~

Johnson indices use an orthogonal transformation and are therefore suitable
for larger input dimensions.  First let
:math:`\mat{R}_{\vect{X},\vect{X}}` be the correlation matrix of the inputs.  Its
eigendecomposition gives

.. math::

    \mat{R}_{\vect{X},\vect{X}}
      = \mat{Q}\mat{\Delta}^2\mat{Q}^{\mathsf{T}},
    \qquad
    \mat{W}=\mat{Q}\mat{\Delta}\mat{Q}^{\mathsf{T}}.

Let :math:`\vect{r}_{\vect{X},Y}` be the vector of correlations between the
inputs and :math:`Y`.  Let us define:

.. math::

    \vect{\alpha} = \mat{W}^{-1}\vect{r}_{\vect{X},Y}.

The standardized Johnson index of input :math:`X_j` is then

.. math::

    J_j = \sum_{k=1}^{d} (\alpha_k)^2 (w_{kj})^2,

These indices are nonnegative and satisfy

.. math::

    \sum_{j=1}^{d}J_j=R^2.

LMG and Johnson indices are equal for two inputs and are generally close for
linear regression models.

.. topic:: API:

    - See :class:`~openturns.experimental.MulticollinearityAnalysis`

.. topic:: References:

    - [clouvel2025]_
