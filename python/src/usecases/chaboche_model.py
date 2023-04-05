"""
Use case : the Chaboche mechanical model
========================================
"""
import openturns as ot
import math as m


def g(X):
    strain, R, C, gamma = X
    stress = R - C * m.expm1(-gamma * strain) / gamma
    return [stress]


class ChabocheModel:
    """
    Data class for the Chaboche mechanical model.


    Attributes
    ----------

    dim : The dimension of the problem
          dim=4.

    Strain : `Uniform` distribution
              ot.Uniform(0.0, 0.07)

    R : `Dirac` distribution
        ot.Dirac(750.0e6)

    C : `Dirac` distribution
        ot.Dirac(2750.0e6)

    Gamma : `Dirac` distribution
            ot.Dirac(10.0)
    
    trueR : float
        The true value of the R parameter.

    trueC : float
        The true value of the C parameter.

    trueGamma : float
        The true value of the Gamma parameter.

    inputDistribution : `ComposedDistribution`
                        The joint distribution of the input parameters.

    model : `PythonFunction`
            The Chaboche mechanical law.


    Examples
    --------
    >>> from openturns.usecases import chaboche_model
    >>> # Load the Chaboche model
    >>> cm = chaboche_model.ChabocheModel()
    """

    def __init__(self):
        self.dim = 4  # number of inputs

        self.Strain = ot.Uniform(0.0, 0.07)
        self.Strain.setDescription(["Strain"])

        self.trueR = 750.0e6
        self.R = ot.Dirac(self.trueR)
        self.R.setDescription(["R"])

        self.trueC = 2750.0e6
        self.C = ot.Dirac(self.trueC)
        self.C.setDescription(["C"])

        self.trueGamma = 10.0
        self.Gamma = ot.Dirac(self.trueGamma)
        self.Gamma.setDescription(["Gamma"])

        self.inputDistribution = ot.ComposedDistribution(
            [self.Strain, self.R, self.C, self.Gamma]
        )

        self.model = ot.PythonFunction(4, 1, g)
        self.model.setInputDescription(["Strain", "R", "C", "Gamma"])
        self.model.setOutputDescription(["Sigma"])
