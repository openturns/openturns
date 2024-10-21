"""
Use case : Branin test function
===============================
"""

import openturns as ot


class BraninModel:
    r"""
    Data class for the Branin test function.

    Attributes
    ----------

    dim : int
        The dimension of the problem, dim=2.

    model : :class:`~openturns.Function`
        The Branin function

    trueNoiseFunction : float
        Constant, small noise :math:`\epsilon=0.1`.

    noiseModel : :class:`~openturns.Function`
        The noise function

    lowerbound : :class:`~openturns.Point` in dimension dim.
        Default is 0.0 for each dimension.

    upperbound : :class:`~openturns.Point` in dimension dim.
        Default is 1.0 for each dimension.

    xexact1 : :class:`~openturns.Point`
        First minima location :math:`[0.123895, 0.818329]`.

    xexact2 : :class:`~openturns.Point`
        Second minima location :math:`[0.542773, 0.151666]`.

    xexact3 : :class:`~openturns.Point`
        Third minima location :math:`[0.961652, 0.165000]`.

    xexact : :class:`~openturns.Sample`
        All global minimas location gathered.

    Examples
    --------
    >>> from openturns.usecases import branin_function
    >>> # Load the Branin model
    >>> bm = branin_function.BraninModel()
    """

    def __init__(self):
        self.dim = 2
        self.trueNoiseFunction = 0.1

        self.branin = ot.SymbolicFunction(
            ["x1", "x2"],
            [
                "((x2-(5.1/(4*pi_^2))*x1^2+5*x1/pi_-6)^2+10*(1-1/(8*pi_))*cos(x1)+10-54.8104)/51.9496"
            ],
        )
        self.transfo = ot.SymbolicFunction(["u1", "u2"], ["15*u1-5", "15*u2"])

        self.model = ot.ComposedFunction(self.branin, self.transfo)
        self.noiseModel = ot.SymbolicFunction(
            ["x1", "x2"], [str(self.trueNoiseFunction)]
        )
        # %%
        self.lowerbound = ot.Point([0.0] * self.dim)
        self.upperbound = ot.Point([1.0] * self.dim)

        self.objectiveFunction = self.model

        # Minimas
        self.xexact1 = ot.Point([0.123895, 0.818329])
        self.xexact2 = ot.Point([0.542773, 0.151666])
        self.xexact3 = ot.Point([0.961652, 0.165000])
        self.xexact = ot.Sample([self.xexact1, self.xexact2, self.xexact3])
