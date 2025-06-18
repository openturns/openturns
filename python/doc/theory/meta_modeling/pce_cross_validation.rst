.. _pce_cross_validation:

Cross validation of PCE models
------------------------------

Introduction
~~~~~~~~~~~~

The cross-validation of a polynomial chaos expansion
uses the theory presented in :ref:`cross_validation`.
In [blatman2009]_ page 84, the author applies the LOO equation to polynomial
chaos expansion
(see appendix D page 203 for a proof).
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
The expansion is:

.. math::
    \metaModel(\vect{x})
    = \sum_{k = 0}^{P - 1} \widehat{a}_k \psi_k(\vect{x})

where :math:`(\widehat{a}_k)_{k = 0,..., P}`\ ’s is the vector of estimates of the
coefficients.
Assume that the coefficients are estimated using linear least squares.
The design matrix :math:`\boldsymbol{\Psi} \in \Rset^{n \times P}` is:

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

.. topic:: API:

    - See :class:`~openturns.FunctionalChaosValidation`

.. topic:: References:

    - [blatman2009]_
