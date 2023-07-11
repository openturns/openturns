"""
Use case : two degree-of-fredom primary/secondary damped oscillator
===================================================================
"""
import openturns as ot


class Oscillator:
    """
    Data class for the oscillator example.

    Attributes
    ----------

    dim : Constant
          dim=8, dimension of the problem

    model : `SymbolicFunction`
            The limit state function

    muMp : Constant
          muMp = 1.5, mean of the mass of the primary system

    sigmaOverMuMp : Constant
          sigmaOverMuMp = 0.1, coefficient of variation of the mass of the primary system
    
    distributionMp : `LogNormalMuSigmaOverMu` distribution of the mass of the primary system
                      ot.LogNormalMuSigmaOverMu(muMp, sigmaOverMuMp).getDistribution()

    muMs : Constant
          muMs = 0.01, mean of the mass of the primary system

    sigmaOverMuMs : Constant
          sigmaOverMuMs = 0.1, coefficient of variation of the mass of the primary system
    
    distributionMs : `LogNormalMuSigmaOverMu` distribution of the mass of the secondary system
                      ot.LogNormalMuSigmaOverMu(muMs, sigmaOverMuMs).getDistribution()

    muKp : Constant
          muKp = 1, mean of the spring stiffness of the primary system

    sigmaOverMuKp : Constant
          sigmaOverMuKp = 0.2, coefficient of variation of the spring stiffness of the primary system
    
    distributionKp : `LogNormalMuSigmaOverMu` distribution of the spring stiffness of the primary system
                      ot.LogNormalMuSigmaOverMu(muKp, sigmaOverMuKp).getDistribution()

    muKs : Constant
          muKs = 0.01, mean of the spring stiffness of the secondary system

    sigmaOverMuKs : Constant
          sigmaOverMuKs = 0.2, coefficient of variation of the spring stiffness of the secondary system
    
    distributionKs : `LogNormalMuSigmaOverMu` distribution of the spring stiffness of the secondary system
                      ot.LogNormalMuSigmaOverMu(muKs, sigmaOverMuKs).getDistribution()

    muZetap : Constant
          muZetap = 0.05, mean of the damping ratio of the primary system

    sigmaOverMuZetap : Constant
          sigmaOverMuZetap = 0.4, coefficient of variation of the damping ratio of the primary system
    
    distributionZetap : `LogNormalMuSigmaOverMu` distribution of the damping ratio of the primary system
                      ot.LogNormalMuSigmaOverMu(muZetap, sigmaOverMuZetap).getDistribution()

    muZetas : Constant
          muZetas = 0.02, mean of the damping ratio of the secondary system

    sigmaOverMuZetas : Constant
          sigmaOverMuZetas = 0.5, coefficient of variation of the damping ratio of the secondary system
    
    distributionZetas : `LogNormalMuSigmaOverMu` distribution of the damping ratio of the secondary system
                      ot.LogNormalMuSigmaOverMu(muZetas, sigmaOverMuZetas).getDistribution()

    muFs : Constant
          muFs = 27.5, mean of the force capacity of the secondary spring

    sigmaOverFs : Constant
          sigmaOverFs = 0.1, coefficient of variation of the force capacity of the secondary spring
    
    distributionFs : `LogNormalMuSigmaOverMu` distribution of the force capacity of the secondary spring
                      ot.LogNormalMuSigmaOverMu(muFs, sigmaOverFs).getDistribution()

    muS0 : Constant
          muS0 = 100, mean of the intensity of the white noise

    sigmaOverS0 : Constant
          sigmaOverS0 = 0.1, coefficient of variation of the intensity of the white noise
    
    distributionS0 : `LogNormalMuSigmaOverMu` distribution of the intensity of the white noise
                      ot.LogNormalMuSigmaOverMu(muS0, sigmaOverS0).getDistribution()


    distribution : `ComposedDistribution`
                   The joint distribution of the input parameters

    Examples
    --------
    >>> from openturns.usecases import oscillator
    >>> # Load the oscillator
    >>> osc = oscillator.Oscillator()
    """

    def __init__(self):
        self.dim = 8

        # Random variable : mp
        self.muMp = 1.5
        self.sigmaOverMuMp = 0.1

        # Random variable : ms
        self.muMs = 0.01
        self.sigmaOverMuMs = 0.1

        # Random variable : kp
        self.muKp = 1.0
        self.sigmaOverMuKp = 0.2

        # Random variable : ks
        self.muKs = 0.01
        self.sigmaOverMuKs = 0.2

        # Random variable : zetap
        self.muZetap = 0.05
        self.sigmaOverMuZetap = 0.4

        # Random variable : zetas
        self.muZetas = 0.02
        self.sigmaOverMuZetas = 0.5

        # Random variable : Fs
        self.muFs = 27.5
        self.sigmaOverMuFs = 0.1

        # Random variable : S0
        self.muS0 = 100.0
        self.sigmaOverMuS0 = 0.1

        # create the limit state function model
        formula = "Fs-3*ks*sqrt("
        formula += "pi_*S0*(zetap/2+zetas/2)*zetas*(zetap*sqrt(kp/mp)^3+zetas*sqrt(ks/ms)^3)*sqrt(kp/mp)/"
        formula += "(4*zetas*sqrt(ks/ms)^3*(zetap*zetas*(4*(zetap/2+zetas/2)^2+"
        formula += "((sqrt(kp/mp)-sqrt(ks/ms))/(sqrt(kp/mp)/2+sqrt(ks/ms)/2))^2)+"
        formula += "(ms/mp)*(zetap/2+zetas/2)^2)*4*(zetap/2+zetas/2)*(sqrt(kp/mp)/2+sqrt(ks/ms)/2)^4))"

        self.model = ot.SymbolicFunction(
            ["mp", "ms", "kp", "ks", "zetap", "zetas", "Fs", "S0"], [formula]
        )

        # Mass of primary system
        self.distributionMp = ot.LogNormalMuSigmaOverMu(
            self.muMp, self.sigmaOverMuMp
        ).getDistribution()
        self.distributionMp.setName("Mass of primary system")
        self.distributionMp.setDescription(["mp"])

        # Mass of secondary system
        self.distributionMs = ot.LogNormalMuSigmaOverMu(
            self.muMs, self.sigmaOverMuMs
        ).getDistribution()
        self.distributionMs.setName("Mass of secondary system")
        self.distributionMs.setDescription(["ms"])

        # Spring stiffness of primary system
        self.distributionKp = ot.LogNormalMuSigmaOverMu(
            self.muKp, self.sigmaOverMuKp
        ).getDistribution()
        self.distributionKp.setName("Spring stiffness of primary system")
        self.distributionKp.setDescription(["kp"])

        # Spring stiffness of secondary system
        self.distributionKs = ot.LogNormalMuSigmaOverMu(
            self.muKs, self.sigmaOverMuKs
        ).getDistribution()
        self.distributionKs.setName("Spring stiffness of secondary system")
        self.distributionKs.setDescription(["ks"])

        # Damping ratio of primary system
        self.distributionZetap = ot.LogNormalMuSigmaOverMu(
            self.muZetap, self.sigmaOverMuZetap
        ).getDistribution()
        self.distributionZetap.setName("Damping ratio of primary system")
        self.distributionZetap.setDescription(["zetap"])

        # Damping ratio of secondary system
        self.distributionZetas = ot.LogNormalMuSigmaOverMu(
            self.muZetas, self.sigmaOverMuZetas
        ).getDistribution()
        self.distributionZetas.setName("Damping ratio of secondary system")
        self.distributionZetas.setDescription(["zetas"])

        # Force capacity of secondary spring
        self.distributionFs = ot.LogNormalMuSigmaOverMu(
            self.muFs, self.sigmaOverMuFs
        ).getDistribution()
        self.distributionFs.setName("Force capacity of secondary spring")
        self.distributionFs.setDescription(["Fs"])

        # Intensity of white noise excitation
        self.distributionS0 = ot.LogNormalMuSigmaOverMu(
            self.muS0, self.sigmaOverMuS0
        ).getDistribution()
        self.distributionS0.setName("Intensity of white noise excitation")
        self.distributionS0.setDescription(["S0"])

        # Joint distribution of the input parameters
        self.distribution = ot.ComposedDistribution(
            [
                self.distributionMp,
                self.distributionMs,
                self.distributionKp,
                self.distributionKs,
                self.distributionZetap,
                self.distributionZetas,
                self.distributionFs,
                self.distributionS0,
            ]
        )
