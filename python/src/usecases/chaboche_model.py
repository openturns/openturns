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
    r"""
    Data class for the Chaboche mechanical model.

    Parameters
    ----------
    strainMin : float, optional
        The minimum value of the strain. The default is 0.0.

    strainMax : float, optional
        The maximum value of the strain. The default is 0.07

    trueR : float, optional
        The true value of the R parameter. The default is 750.0e6.

    trueC : float, optional
        The true value of the C parameter. The default is 2750.0e6.

    trueGamma : float, optional
        The true value of the Gamma parameter. The default is 10.0.


    Attributes
    ----------
    dim : The dimension of the problem
          dim=4.

    Strain : :class:`~openturns.Uniform` distribution
              `ot.Uniform(strainMin, strainMax)`

    R : :class:`~openturns.Dirac` distribution
        `ot.Dirac(trueR)`

    C : :class:`~openturns.Dirac` distribution
        `ot.Dirac(trueC)`

    Gamma : :class:`~openturns.Dirac` distribution
            `ot.Dirac(trueGamma)`

    inputDistribution : :class:`~openturns.ComposedDistribution`
                        The joint distribution of the input parameters.

    model : :class:`~openturns.PythonFunction`
            The Chaboche mechanical law.
            The model has input dimension 4 :math:`\boldsymbol{X} = (\epsilon, R,
            C, \gamma)` and output dimension 1 :math:`Y = \sigma`.

    data : :class:`~openturns.Sample` of size 10 and dimension 2
        A data set which contains noisy observations of the strain (column 0)
        and the stress (column 1).

    Examples
    --------
    >>> from openturns.usecases import chaboche_model
    >>> # Load the Chaboche model
    >>> cm = chaboche_model.ChabocheModel()
    >>> print(cm.data[:5])
            [ Strain      Stress (Pa) ]
    0 : [ 0           7.56e+08    ]
    1 : [ 0.0077      7.57e+08    ]
    2 : [ 0.0155      7.85e+08    ]
    3 : [ 0.0233      8.19e+08    ]
    4 : [ 0.0311      8.01e+08    ]
    >>> print("Inputs:", cm.model.getInputDescription())
    Inputs: [Strain,R,C,Gamma]
    >>> print("Outputs:", cm.model.getOutputDescription())
    Outputs: [Sigma]
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

        self.data = ot.Sample(
            [
                [0.0, 7.56e08],
                [0.0077, 7.57e08],
                [0.0155, 7.85e08],
                [0.0233, 8.19e08],
                [0.0311, 8.01e08],
                [0.0388, 8.42e08],
                [0.0466, 8.49e08],
                [0.0544, 8.79e08],
                [0.0622, 8.85e08],
                [0.07, 8.96e08],
            ]
        )
        self.data.setDescription(["Strain", "Stress (Pa)"])
