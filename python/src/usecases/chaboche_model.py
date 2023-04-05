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

    Parameters
    ----------
    strainMin : float
        The minimum value of the strain. Default is 0.0.

    strainMax : float
        The maximum value of the strain. Default is 0.07

    trueR : float
        The true value of the R parameter. Default is 750.0e6.

    trueC : float
        The true value of the C parameter. Default is 2750.0e6.

    trueGamma : float
        The true value of the Gamma parameter. Default is 10.0.


    Attributes
    ----------
    dim : The dimension of the problem
          dim=4.

    Strain : `Uniform` distribution
              ot.Uniform(strainMin, strainMax)

    R : `Dirac` distribution
        ot.Dirac(trueR)

    C : `Dirac` distribution
        ot.Dirac(trueC)

    Gamma : `Dirac` distribution
            ot.Dirac(trueGamma)

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

    def __init__(
        self,
        strainMin=0.0,
        strainMax=0.07,
        trueR=750.0e6,
        trueC=2750.0e6,
        trueGamma=10.0,
    ):
        self.dim = 4  # number of inputs

        if strainMin >= strainMax:
            raise ValueError(
                "Strain minimum = %s >= strainMax = %s." % (strainMin, strainMax)
            )
        self.strainMin = strainMin
        self.strainMax = strainMax
        self.Strain = ot.Uniform(strainMin, strainMax)
        self.Strain.setDescription(["Strain"])

        self.trueR = trueR
        self.R = ot.Dirac(self.trueR)
        self.R.setDescription(["R"])

        self.trueC = trueC
        self.C = ot.Dirac(self.trueC)
        self.C.setDescription(["C"])

        self.trueGamma = trueGamma
        self.Gamma = ot.Dirac(self.trueGamma)
        self.Gamma.setDescription(["Gamma"])

        self.inputDistribution = ot.ComposedDistribution(
            [self.Strain, self.R, self.C, self.Gamma]
        )

        self.model = ot.PythonFunction(4, 1, g)
        self.model.setInputDescription(["Strain", "R", "C", "Gamma"])
        self.model.setOutputDescription(["Sigma"])
