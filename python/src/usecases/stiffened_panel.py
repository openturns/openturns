"""
Use case : stiffened panel
==========================
"""

import openturns as ot


class StiffenedPanel:
    r"""
    Data class for the stiffened panel model.


    Attributes
    ----------

    dim : int
        The dimension of the problem, dim=10

    model : :class:`~openturns.SymbolicFunction`
        Model of the critical shearing load.
        The model has input dimension 10 and output dimension 1.
        More precisely, we have :math:`\vect{X} = (E, \nu, h_c, \ell, f_1, f_2, t, a, b_0, p)`
        and :math:`Y = (N_{xy})_{cr}`.

    E : :class:`~openturns.TruncatedNormal`
        Young modulus distribution (Pa),
        `ot.TruncatedNormal(110.0e9, 55.0e9, 99.0e9, 121.0e9)`

    nu : :class:`~openturns.Uniform`
        Poisson coefficient (-) distribution
        `ot.Uniform(0.3675, 0.3825)`

    h_c : :class:`~openturns.Uniform`
        Distance between the mean surface of the hat and the foot of the Stiffener (m) distribution
        `ot.Uniform(0.0285, 0.0315)`

    ell : :class:`~openturns.Uniform`
        Length of the stiffener flank (m) distribution
        `ot.Uniform(0.04655, 0.05145)`

    f_1 : :class:`~openturns.Uniform`
        Width of the stiffener foot (m) distribution
        `ot.Uniform(0.0266, 0.0294)`

    f_2 : :class:`~openturns.Uniform`
        Width of the stiffener hat (m) distribution
        `ot.Uniform(0.00627, 0.00693)`

    t : :class:`~openturns.Uniform`
        Thickness of the panel and the stiffener (m) distribution
        `ot.Uniform(8.02e-5, 8.181e-5)`

    a : :class:`~openturns.Uniform`
        Width of the panel (m)  distribution
        `ot.Uniform(0.6039, 0.6161)`

    b_0 : :class:`~openturns.Uniform`
        Distance between two stiffeners (m) distribution
        `ot.Uniform(0.04455, 0.04545)`

    p : :class:`~openturns.Uniform`
        Half-width of the stiffener (m) distribution
        `ot.Uniform(0.03762, 0.03838)`

    distribution : :class:`~openturns.JointDistribution`
        The joint distribution of the input parameters.

    Examples
    --------
    >>> from openturns.usecases import stiffened_panel
    >>> # Load the stiffened panel model
    >>> panel = stiffened_panel.StiffenedPanel()
    >>> print("Inputs:", panel.model.getInputDescription())
    Inputs: [F,L,a,De,di,E]
    >>> print("Outputs:", panel.model.getOutputDescription())
    [Deflection,Left angle,Right angle]
    """

    def __init__(self):
        self.dim = 10

        # Here we use substitutions in order to get a unique formula for the model.
        # This way, we get a symbolic derivation of the gradient and the hessian,
        # which is not possible if we use the exprtk embedded language
        formula = "(5.35 + 4 * (b_0 / a)^2) * (pi_^2 / b_0^2 * E * t^3 / (12 * (1 - nu^2))) * (1 + (2 * p * (h - 2 * h_0) - h_c  * (f_1 - f_2)) / (4 * h_0 * ell))"
        # Substitutions
        a = "(ell*t)"
        a_bar = "(A+t*(p+(f_1-f_2)/2))"
        h_0 = "((A*(h_c+2*t)+t^2*(f_1-f_2))/(2*A_bar))"
        h = "(h_c + t)"
        formula = formula.replace("h_0", h_0)
        formula = formula.replace("A_bar", a_bar)
        formula = formula.replace("A", a)
        # Attention! If the substitution of h by its formula is done without
        # caution h_c will be replaced by (h_c + t)_c!
        # First, we protect h_c
        formula = formula.replace("h_c", "H_C")
        # Then we replace h by its formula
        formula = formula.replace("h", h)
        # On restaure les occurrences de h_c
        formula = formula.replace("H_C", "h_c")
        self.model = ot.SymbolicFunction(
            ["E", "nu", "h_c", "ell", "f_1", "f_2", "t", "a", "b_0", "p"], [formula]
        )
        self.model.setOutputDescription([r"(N_{xy})_{cr}"])

        self.youngModulus = ot.TruncatedNormal(110.0e9, 55.0e9, 99.0e9, 121.0e9)
        self.youngModulus.setDescription(["E (Pa)"])

        self.nu = ot.Uniform(0.3675, 0.3825)
        self.nu.setDescription(["nu (-)"])

        self.h_c = ot.Uniform(0.0285, 0.0315)
        self.h_c.setDescription(["h_c (m)"])

        self.ell = ot.Uniform(0.04655, 0.05145)
        self.ell.setDescription(["ell (m)"])

        self.f_1 = ot.Uniform(0.0266, 0.0294)
        self.f_1.setDescription(["f_1 (m)"])

        self.f_2 = ot.Uniform(0.00627, 0.00693)
        self.f_2.setDescription(["f_2 (m)"])

        self.t = ot.Uniform(8.02e-5, 8.181e-5)
        self.t.setDescription(["t (m)"])

        self.a = ot.Uniform(0.6039, 0.6161)
        self.a.setDescription(["a (m)"])

        self.b_0 = ot.Uniform(0.04455, 0.04545)
        self.b_0.setDescription(["b_0 (m)"])

        self.p = ot.Uniform(0.03762, 0.03838)
        self.p.setDescription(["p (m)"])

        self.distribution = ot.JointDistribution(
            [
                self.youngModulus,
                self.nu,
                self.h_c,
                self.ell,
                self.f_1,
                self.f_2,
                self.t,
                self.a,
                self.b_0,
                self.p,
            ]
        )
