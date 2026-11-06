.. _use-case-four-branch:

The Four Branch function
========================

.. figure:: ../_static/four_branch_function.png
    :align: center
    :alt: Four Branch function contours with limit state and input PDF
    :width: 80%

    Four Branch function: contours, limit state (red), input PDF (transparent)

The Four Branch function is a 2D limit state function introduced by
Schueremans and van Gemert (2005) [Schueremans2005]_ as a benchmark for
structural reliability analysis. It represents a series system with four
distinct failure modes (branches). The function has been widely used in
reliability literature, particularly for testing active learning methods
such as AK-MCS [Echard2011]_.

Definition
----------

Let the function :math:`f : \mathbb{R}^2 \times \mathbb{R} \rightarrow \mathbb{R}`
defined by:

.. math::
   :nowrap:

   \begin{align*}
   f(\vect{x}, p) = \min \begin{Bmatrix}
   3 + 0.1(x_1 - x_2)^2 - \frac{x_1 + x_2}{\sqrt{2}} \\
   3 + 0.1(x_1 - x_2)^2 + \frac{x_1 + x_2}{\sqrt{2}} \\
   (x_1 - x_2) + \frac{p}{\sqrt{2}} \\
   (x_2 - x_1) + \frac{p}{\sqrt{2}}
   \end{Bmatrix}
   \end{align*}

The function consists of four branches:
- Two quadratic branches (g1, g2) forming a parabolic shape opening upward
- Two linear branches (g3, g4) forming a diamond shape

The parameter :math:`p` controls the position of the linear branches.
The standard benchmark value is :math:`p = 6`.

Probabilistic Inputs (Standard Benchmark)
-----------------------------------------

The two input variables are modeled as independent standard normal random variables:

.. math::
   X_1, X_2 \sim \mathcal{N}(0, 1) \quad \text{independent}

.. list-table:: **Table 1.** Distribution of the input random variables (standard benchmark).
   :header-rows: 1
   :widths: 30 70

   * - Input variable
     - Distribution
   * - :math:`X_1`
     - Normal(:math:`\mu=0`, :math:`\sigma=1`)
   * - :math:`X_2`
     - Normal(:math:`\mu=0`, :math:`\sigma=1`)

The failure event is defined as:

.. math::
   F = \{ \vect{x} \in \mathbb{R}^2 : f(\vect{x}, p) \leq 0 \}

Reference Failure Probability
-----------------------------

The failure probability :math:`P_f = \mathbb{P}[f(\vect{X}, 6) \leq 0]` has been
estimated by several methods in the literature:

.. list-table:: **Table 2.** Reference values for the failure probability with :math:`p=6`.
   :header-rows: 1
   :widths: 15 15 25 15 30

   * - Method
     - N
     - :math:`\hat{P}_f`
     - COV
     - Source
   * - MCS
     - :math:`10^6`
     - :math:`4.416\times10^{-3}`
     - 0.15%
     - Echard et al. (2011)
   * - IS
     - 1469
     - :math:`4.9\times10^{-3}`
     - -
     - Echard et al. (2011)

Analysis
--------

The Four Branch function creates a non-convex failure domain with four distinct
branches meeting at the origin. The failure domain is the union of four regions:

1. **Region 1** (quadratic): :math:`3 + 0.1(x_1 - x_2)^2 - (x_1 + x_2)/\sqrt{2} \leq 0`
2. **Region 2** (quadratic): :math:`3 + 0.1(x_1 - x_2)^2 + (x_1 + x_2)/\sqrt{2} \leq 0`
3. **Region 3** (linear): :math:`x_1 - x_2 + 6/\sqrt{2} \leq 0`
4. **Region 4** (linear): :math:`x_2 - x_1 + 6/\sqrt{2} \leq 0`

This function is particularly challenging for reliability methods because:
1. The failure domain is non-convex with four distinct components
2. The limit state function is non-smooth (minimum of four functions)
3. The probability of failure is small (rare event, ~0.45%)

It serves as a benchmark for:
- Active learning reliability algorithms (AK-MCS, PC-Kriging, etc.)
- Subset Sampling and NAIS algorithms
- Gaussian Process / Polynomial Chaos metamodeling

References
----------

* [Schueremans2005]_ L. Schueremans and D. van Gemert, "Benefit of splines and neural
  networks in simulation based structural reliability analysis," *Structural Safety*,
  vol. 27, no. 3, pp. 241-258, 2005. DOI: 10.1016/j.strusafe.2004.11.001

* [Echard2011]_ B. Echard, N. Gayton, and M. Lemaire, "AK-MCS: An active learning
  reliability method combining Kriging and Monte Carlo simulation",
  *Reliability Engineering and System Safety*, vol. 96, no. 2, pp. 145-154, 2011.

Load the use case
-----------------

Standard benchmark version (independent standard normals, p=6):

.. code-block:: python

    >>> from openturns.usecases import four_branch_function
    >>> fb = four_branch_function.FourBranchModel()
    >>> print(fb.p)
    6.0
    >>> print(fb.distribution)
    Independent copula
    X1 ~ Normal(mu=0, sigma=1)
    X2 ~ Normal(mu=0, sigma=1)

Custom parameter p:

.. code-block:: python

    >>> fb = four_branch_function.FourBranchModel(p=7.0)
    >>> print(fb.p)
    7.0

API documentation
-----------------

.. currentmodule:: openturns.usecases.four_branch_function

.. autoclass:: FourBranchModel
    :noindex:

Examples based on this use case
-------------------------------

.. minigallery:: openturns.usecases.four_branch_function.FourBranchModel
