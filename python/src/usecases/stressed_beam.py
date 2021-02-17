"""
Use case : axial stressed beam
==============================
"""
from __future__ import print_function
import openturns as ot


class AxialStressedBeam():
    """
    Data class for the axial stressed beam example.


    Attributes
    ----------

    dim : The dimension of the problem
          dim=2.
 
    D : Constant
        Diameter D = 0.02 (m)

    model : `SymbolicFunction`
            The limit state function.

    muR : Constant
          muR=3.0e6, yield strength mean

    sigmaR : Constant
             sigmaR = 3.0e5, yield strength variance

    distribution_R : `LogNormalMuSigma` distribution of the yield strength
                      ot.LogNormalMuSigma(muR, sigmaR, 0.0).getDistribution()

    muF : Constant
          muF=750.0, traction load mean

    sigmaF : Constant
             sigmaR = 50.0, traction load variance

    distribution_F : `Normal` distribution of the traction load
                     ot.Normal(muF, sigmaF)

    distribution : `ComposedDistribution`
                   The joint distribution of the inpput parameters.

    Examples
    --------
    >>> from openturns.usecases import stressed_beam as stressed_beam
    >>> # Load the axial stressed beam
    >>> sm = stressed_beam.AxialStressedBeam()
    """

    def __init__(self):
        self.dim = 2
        self.D = 0.02
        # Random variable : R
        self.muR = 3.0e6
        self.sigmaR = 3.0e5
        # Random variable : F
        self.muF = 750.
        self.sigmaF = 50.
        # create the limit state function model
        self.model = ot.SymbolicFunction(['R', 'F'], ['R-F/(pi_/10000.0)'])

        # Yield strength
        self.distribution_R = ot.LogNormalMuSigma(self.muR, self.sigmaR, 0.0).getDistribution()
        self.distribution_R.setName('Yield strength')
        self.distribution_R.setDescription('R')
        # Traction load
        self.distribution_F = ot.Normal(self.muF, self.sigmaF)
        self.distribution_F.setName('Traction_load')
        self.distribution_F.setDescription('F')

        # Joint distribution of the input parameters
        self.distribution = ot.ComposedDistribution([self.distribution_R, self.distribution_F])

