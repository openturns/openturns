"""
Use case : Wing weight test function
====================================
"""

import openturns as ot
import math as m


def functionWingWeight(x):
    Sw = x[0]
    Wfw = x[1]
    A = x[2]
    Lambda = x[3]
    q = x[4]
    ll = x[5]
    tc = x[6]
    Nz = x[7]
    Wdg = x[8]
    Wp = x[9]
    return [
        (
            0.036
            * Sw**0.758
            * Wfw**0.0035
            * (A / m.cos(m.pi / 180 * Lambda) ** 2) ** 0.6
            * q**0.006
            * ll**0.04
            * (100 * tc / m.cos(m.pi / 180 * Lambda)) ** (-0.3)
            * (Nz * Wdg) ** 0.49
            + Sw * Wp
        )
    ]


class WingWeightModel:
    """
    Data class for the Wing weight model.

    Attributes
    ----------

    dim : int
        Dimension of the problem, dim = 10

    Sw : :class:`~openturns.Uniform`
        Wing area (ft^2)  distribution
        First marginal, ot.Uniform(150, 200)

    Wfw : :class:`~openturns.Uniform`
        Weight of fuel in the wing (lb) distribution
        Second marginal, ot.Uniform(220, 300)

    A : :class:`~openturns.Uniform`
        Aspect ratio (-) distribution
        Third marginal, ot.Uniform(6, 10)

    Lambda : :class:`~openturns.Uniform`
        Quarter chord sweep (deg),  distribution
        Fourth marginal, ot.Uniform(-10, 10)

    q : :class:`~openturns.Uniform`
        Dynamic pressure at cruise (lb/ft^2) distribution
        Fifth marginal, ot.Uniform(16, 45 )

    l : :class:`~openturns.Uniform`
        Taper ratio (-) distribution
        Sixth marginal, ot.Uniform(0.5, 1)

    tc : :class:`~openturns.Uniform`
        Airfoil thickness to chord ratio (-) distribution
        Seventh marginal, ot.Uniform(0.08, 0.18)

    Nz : :class:`~openturns.Uniform`
        Ultimate load factor (-) distribution
        Eighth marginal, ot.Uniform(2.5, 6)

    Wdg : :class:`~openturns.Uniform`
        Flight design gross weight (lb) distribution
        Nineth marginal, ot.Uniform(1700, 2500)

    Wp : :class:`~openturns.Uniform`
        Paint weight (lb/ft^2) distribution
        Tenth marginal, ot.Uniform(0.025, 0.08)

    inputDistribution : :class:`~openturns.JointDistribution`
        The joint distribution of the input parameters.

    model : :class:`~openturns.PythonFunction`
        The Wing weight model with Sw, Wfw, A, Lambda, q, l, tc, Nz, Wdg and Wp as variables.

    Examples
    --------
    >>> from openturns.usecases import wingweight_function
    >>> # Load the Wing weight model
    >>> ww = wingweight_function.WingWeightModel()
    """

    def __init__(self):
        # dimension
        self.dim = 10

        # First marginal : Sw
        self.Sw = ot.Uniform(150.0, 200.0)
        self.Sw.setName("Sw")

        # Second marginal : Wfw
        self.Wfw = ot.Uniform(220.0, 300.0)
        self.Wfw.setName("Wfw")

        # Third marginal : A
        self.A = ot.Uniform(6.0, 10.0)
        self.A.setName("A")

        # Fourth marginal : Lambda
        self.Lambda = ot.Uniform(-10.0, 10.0)
        self.Lambda.setName("Lambda")

        # Fifth marginal : q
        self.q = ot.Uniform(16.0, 45.0)
        self.q.setName("q")

        # Sixth marginal : l
        self.ll = ot.Uniform(0.5, 1.0)
        self.ll.setName("l")

        # Seventh marginal : tc
        self.tc = ot.Uniform(0.08, 0.18)
        self.tc.setName("tc")

        # Eighth marginal : Nz
        self.Nz = ot.Uniform(2.5, 6.0)
        self.Nz.setName("Nz")

        # Nineth marginal : Wdg
        self.Wdg = ot.Uniform(1700.0, 2500.0)
        self.Wdg.setName("Wdg")

        # Tenth marginal : Wp
        self.Wp = ot.Uniform(0.025, 0.08)
        self.Wp.setName("Wp")

        # Input distribution
        self.inputDistribution = ot.JointDistribution(
            [
                self.Sw,
                self.Wfw,
                self.A,
                self.Lambda,
                self.q,
                self.ll,
                self.tc,
                self.Nz,
                self.Wdg,
                self.Wp,
            ]
        )
        self.inputDistribution.setDescription(
            ["Sw", "Wfw", "A", "Lambda", "q", "l", "tc", "Nz", "Wdg", "Wp"]
        )

        # The Wing weight model
        self.model = ot.PythonFunction(10, 1, functionWingWeight)
