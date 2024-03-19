.. _pce_cross_validation:

Cross validation of PCE models
------------------------------

Introduction
~~~~~~~~~~~~

The cross-validation of a polynomial chaos expansion
uses the theory presented in :ref:`cross_validation`.
In [blatman2009]_ page 84, the author applies the LOO equation to polynomial
chaos expansion.
The appendice D page 203 gives a proof.
If the coefficients are estimated from integration, the same derivation cannot,
in theory, be applied.

Polynomial chaos expansion from linear least squares regression
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`n \in \Nset` be an integer representing the number of
observations in the experimental design.
Let :math:`\set{D} \subseteq \set{X}` be a set of :math:`n` independent
observations of the random vector :math:`\vect{X}`:

.. math::
    \set{D} = \left\{\vect{x}^{(1)}, ..., \vect{x}^{(n)}\right\}

Let :math:`P \in \Nset` be an integer representing the number of
coefficients in the polynomial chaos expansion.
The :math:`P`-term polynomial expansion of the model is:

.. math::
    \metamodel(\vect{x})
    = \sum_{k = 0}^{P - 1} \widehat{a}_k \psi_k(\vect{x})

where the :math:`\widehat{a}_k`\ ’s are estimates of the
coefficients.
Assume that the coefficients are estimated using linear least squares
regression.
The design matrix is the matrix :math:`\boldsymbol{\Psi} \in \Rset^{n \times P}`
defined by the equation:

.. math::
    \boldsymbol{\Psi}_{ik}  =  \psi_k\left(\vect{x}^{(i)}\right),

for :math:`i = 1, \dots, n` and :math:`k = 0, \dots, P-1`.

Cross-validation of a PCE
~~~~~~~~~~~~~~~~~~~~~~~~~

If the coefficients of the PCE are estimated using linear least squares,
then the fast methods presented in :ref:`cross_validation` can be applied:

- the fast leave-one-out cross-validation,
- the fast K-Fold cross-validation.

Fast methods are implemented in :class:`~openturns.FunctionalChaosValidation`.

If model selection is used (such as :class:`LARS`), then only the active
functions in the basis from the training step must be used.
Indeed, with naive cross-validation, the selection method may consider different
active bases.
The fast method, on the other hand, only consider the active basis which is the
result of the training step.
If the selection method is not stable in the cross-validation method, then
the results with the naive and fast cross-validation methods may be different.

.. topic:: API:

    - See :class:`~openturns.FunctionalChaosValidation`

.. topic:: References:

    - [blatman2009]_
