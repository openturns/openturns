"""
Use case : Four Branch test function
=====================================

The Four Branch function is a benchmark problem in structural reliability analysis
introduced by Schueremans and van Gemert (2005). It represents a series system
with four distinct limit state components.

References
----------
* [Schueremans2005]_ L. Schueremans and D. van Gemert, "Benefit of splines and neural
  networks in simulation based structural reliability analysis," Structural Safety,
  vol. 27, no. 3, 2005.
* [Echard2011]_ B. Echard, N. Gayton, and M. Lemaire, "AK-MCS: An active learning
  reliability method combining Kriging and Monte Carlo simulation", Reliability
  Engineering and System Safety, vol. 96, pp. 145-154, 2011.
"""

import openturns as ot
import math


class FourBranchModel:
    r"""
    Data class for the Four Branch test function.

    The Four Branch function is a 2D limit state function defined by:

    .. math::
        f(\vect{x}, p) = \min \begin{Bmatrix}
        3 + 0.1(x_1 - x_2)^2 - \frac{x_1 + x_2}{\sqrt{2}} \\
        3 + 0.1(x_1 - x_2)^2 + \frac{x_1 + x_2}{\sqrt{2}} \\
        (x_1 - x_2) + \frac{p}{\sqrt{2}} \\
        (x_2 - x_1) + \frac{p}{\sqrt{2}}
        \end{Bmatrix}

    The standard benchmark uses independent standard normal inputs:

    .. math::
        X_1, X_2 \sim \mathcal{N}(0, 1) \quad \text{independent}

    with parameter :math:`p = 6` (default).

    The failure event is :math:`f(\vect{X}, p) \leq 0`.

    The reference failure probability is approximately :math:`P_f \approx 4.4 \times 10^{-3}`
    (MCS with 10^6 samples, Echard et al. 2011).

    This function is commonly used as a benchmark in reliability analysis,
    active learning (AK-MCS), and surrogate modeling.

    Attributes
    ----------
    dim : int
        The dimension of the problem: dim=2.

    p : float
        The constant parameter p (default: 6.0).

    X1 : :class:`~openturns.Normal`
        First marginal: standard Normal(0, 1).

    X2 : :class:`~openturns.Normal`
        Second marginal: standard Normal(0, 1).

    distribution : :class:`~openturns.JointDistribution`
        The joint distribution of the input parameters (independent standard normals).

    model : :class:`~openturns.PythonFunction`
        The Four Branch function with parameter p.

    References
    ----------
    * [Schueremans2005]_
    * [Echard2011]_

    Examples
    --------
    >>> from openturns.usecases import four_branch_function
    >>> # Load the standard Four Branch model (p=6, standard normal inputs)
    >>> fb = four_branch_function.FourBranchModel()
    >>> print("Inputs:", fb.model.getInputDescription())
    Inputs: [X1, X2]
    >>> print("Output:", fb.model.getOutputDescription())
    Output: [y]
    >>> print("p =", fb.p)
    p = 6.0
    >>> print("Distribution:", fb.distribution)
    Distribution: Independent copula
    X1 ~ Normal(mu=0, sigma=1)
    X2 ~ Normal(mu=0, sigma=1)

    Create a model with custom parameter p:

    >>> fb = four_branch_function.FourBranchModel(p=7.0)
    """

    def __init__(self, p=6.0):
        r"""
        Parameters
        ----------
        p : float, optional
            The constant parameter p in the linear branches.
            Default is 6.0 (standard benchmark value).
        """
        self.dim = 2
        self.p = p

        # First marginal : X1 ~ N(0, 1)
        self.X1 = ot.Normal(0.0, 1.0)
        self.X1.setName("X1")
        self.X1.setDescription(["X1"])

        # Second marginal : X2 ~ N(0, 1)
        self.X2 = ot.Normal(0.0, 1.0)
        self.X2.setName("X2")
        self.X2.setDescription(["X2"])

        # Input distribution: independent standard normals
        self.distribution = ot.JointDistribution([self.X1, self.X2])
        self.distribution.setDescription(["X1", "X2"])

        # Four Branch function with parameter p
        def four_branch(x):
            x1 = x[0]
            x2 = x[1]
            sqrt2 = math.sqrt(2.0)
            g1 = 3.0 + 0.1 * (x1 - x2) ** 2 - (x1 + x2) / sqrt2
            g2 = 3.0 + 0.1 * (x1 - x2) ** 2 + (x1 + x2) / sqrt2
            g3 = (x1 - x2) + self.p / sqrt2
            g4 = (x2 - x1) + self.p / sqrt2
            return [min(g1, g2, g3, g4)]

        self.model = ot.PythonFunction(self.dim, 1, four_branch)
        self.model.setInputDescription(["X1", "X2"])
        self.model.setOutputDescription(["y"])
