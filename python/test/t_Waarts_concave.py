#! /usr/bin/env python

from __future__ import print_function
from openturns import *
# from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    #
    # Physical model
    #

    EtatLimite = Function(
        ['X1', 'X2'], ['G'], ["-0.5*(X1-X2)*(X1-X2) - (X1+X2)/(sqrt(2)) + 3"])

    dim = EtatLimite.getInputDimension()
    print(dim)

    #
    # probabilistic model
    #

    mean = [0.] * dim
    sigma = [1.] * dim
    R = IdentityMatrix(dim)

    myDistribution = Normal(mean, sigma, R)

    start = myDistribution.getMean()
    Covariance = myDistribution.getCovariance()

    #
    # limit state
    #

    vect = RandomVector(myDistribution)

    output = RandomVector(EtatLimite, vect)

    myEvent = ThresholdEvent(output, Less(), 0.0)

    #
    # Calculation
    #
    #
    # FORM/SORM Cobyla
    myCobyla = Cobyla()
    myCobyla.setMaximumEvaluationNumber(100 * dim)
    myCobyla.setMaximumAbsoluteError(1.0e-10)
    myCobyla.setMaximumRelativeError(1.0e-10)
    myCobyla.setMaximumResidualError(1.0e-10)
    myCobyla.setMaximumConstraintError(1.0e-10)

    myAlgoC = FORM(myCobyla, myEvent, start)
    myAlgoC2 = SORM(myCobyla, myEvent, start)

    myAlgoC.run()
    myAlgoC2.run()

    resultC = FORMResult(myAlgoC.getResult())
    resultC2 = SORMResult(myAlgoC2.getResult())

    #
    # FORM/SORM Abdo Rackwitz
    myAbdoRackwitz = AbdoRackwitz()
    myAbdoRackwitz.setMaximumIterationNumber(100 * dim)
    myAbdoRackwitz.setMaximumAbsoluteError(1.0e-10)
    myAbdoRackwitz.setMaximumRelativeError(1.0e-10)
    myAbdoRackwitz.setMaximumResidualError(1.0e-10)
    myAbdoRackwitz.setMaximumConstraintError(1.0e-10)

    myAlgoAR = FORM(myAbdoRackwitz, myEvent, start)
    myAlgoAR2 = SORM(myAbdoRackwitz, myEvent, start)

    myAlgoAR.run()
    # myAlgoAR2.run()

    resultAR = FORMResult(myAlgoAR.getResult())
    # resultAR2 = SORMResult(myAlgoAR2.getResult())

    #
    # Monte Carlo
    CoV_MC = 0.5
    myMC = MonteCarlo(myEvent)
    myMC.setMaximumOuterSampling(100000)
    myMC.setBlockSize(1)
    myMC.setMaximumCoefficientOfVariation(CoV_MC)
    myMC.run()

    #
    # LHS
    CoV_LHS = 0.1
    myLHS = LHS(myEvent)
    myLHS.setMaximumOuterSampling(100000)
    myLHS.setBlockSize(1)
    myLHS.setMaximumCoefficientOfVariation(CoV_LHS)
    myLHS.run()

    #
    # Outputs
    #
    print("")
    print("")
    print(
        "************************************************************************************************")
    print(
        "***************************************** FORM  COBYLA *****************************************")
    print(
        "************************************************************************************************")
    print("event probability = %.5e" % PfC)
    print("generalized reliability index = %.5f" % Beta_generalizedC)
    print(
        "************************************************************************************************")
    for i in range(u_starC.getDimension()):
        print("standard space design point = %.5f" % u_starC[i])
    print(
        "************************************************************************************************")
    for i in range(x_starC.getDimension()):
        print("physical space design point = %.5f" % x_starC[i])
    print(
        "************************************************************************************************")
    print("is standard point origin in failure space? ", PtC)
    print(
        "************************************************************************************************")
    for i in range(gammaC.getDimension()):
        print("importance factors = %.5f" % gammaC[i])
    print(
        "************************************************************************************************")
    print("Hasofer reliability index = %.5f" % beta_hasoferC)
    print(
        "************************************************************************************************")
    for i in range(SensitivityC.getSize()):
        for j in range(SensitivityC[i].getDimension()):
            print("Pf sensitivity = %.5f" % SensitivityC[i][j])
    print(
        "************************************************************************************************")
    print("")
    print(
        "************************************************************************************************")
    print(
        "************************************** FORM ABDO RACKWITZ **************************************")
    print(
        "************************************************************************************************")
    print("event probability = %.5e" % PfAR)
    print("generalized reliability index = %.5f" % Beta_generalizedAR)
    print(
        "************************************************************************************************")
    for i in range(u_starAR.getDimension()):
        print("standard space design point = %.5f" % u_starAR[i])
    print(
        "************************************************************************************************")
    for i in range(x_starAR.getDimension()):
        print("physical space design point = %.5f" % x_starAR[i])
    print(
        "************************************************************************************************")
    print("is standard point origin in failure space? ", PtAR)
    print(
        "************************************************************************************************")
    for i in range(gammaAR.getDimension()):
        print("importance factors = %.5f" % gammaAR[i])
    print(
        "************************************************************************************************")
    print("Hasofer reliability index = %.5f" % beta_hasoferAR)
    print(
        "************************************************************************************************")
    for i in range(SensitivityAR.getSize()):
        for j in range(SensitivityAR[i].getDimension()):
            print("Pf sensitivity = %.5f" % SensitivityAR[i][j])
    print(
        "************************************************************************************************")
    print("")
    print(
        "************************************************************************************************")
    print(
        "***************************************** SORM  COBYLA *****************************************")
    print(
        "************************************************************************************************")
    print("Breitung event probability = %.5e" % PFBreitC2)
    print("Breitung generalized reliability index = %.5f" % BetaBreitC2)
    print("Hohenbichler event probability = %.5e" % PFHBC2)
    print("Hohenbichler generalized reliability index = %.5f" % BetaHBC2)
    print("Tvedt event probability = %.5e" % PFTvedtC2)
    print("Tvedt generalized reliability index = %.5f" % BetaTvedtC2)
    print(
        "************************************************************************************************")
    for i in range(CurvC2.getDimension()):
        print("sorted curvatures = %.5f" % cleanScalar(CurvC2[i]))
    print(
        "************************************************************************************************")
    for i in range(u_starC2.getDimension()):
        print("standard space design point = %.5f" % u_starC2[i])
    print(
        "************************************************************************************************")
    for i in range(x_starC2.getDimension()):
        print("physical space design point = %.5f" % x_starC2[i])
    print(
        "************************************************************************************************")
    print(
        "************************************************************************************************")
    print("is standard point origin in failure space? ", PtC2)
    print(
        "************************************************************************************************")
    for i in range(gammaC2.getDimension()):
        print("importance factors = %.5f" % gammaC2[i])
    print(
        "************************************************************************************************")
    print("Hasofer reliability index = %.5f" % beta_hasoferC2)
    print(
        "************************************************************************************************")
    print("")
    print(
        "************************************************************************************************")
    print(
        "************************************** SORM ABDO RACKWITZ **************************************")
    print(
        "************************************************************************************************")
    print("Breitung event probability = %.5e" % PFBreitAR2)
    print("Breitung generalized reliability index = %.5f" % BetaBreitAR2)
    print("Hohenbichler event probability = %.5e" % PFHBAR2)
    print("Hohenbichler generalized reliability index = %.5f" % BetaHBAR2)
    print("Tvedt event probability = %.5e" % PFTvedtAR2)
    print("Tvedt generalized reliability index = %.5f" % BetaTvedtAR2)
    print(
        "************************************************************************************************")
    for i in range(CurvAR2.getDimension()):
        print("sorted curvatures = %.5f" % cleanScalar(CurvAR2[i]))
    print(
        "************************************************************************************************")
    for i in range(u_starAR2.getDimension()):
        print("standard space design point = %.5f" % u_starAR2[i])
    print(
        "************************************************************************************************")
    for i in range(x_starAR2.getDimension()):
        print("physical space design point = %.5f" % x_starAR2[i])
    print(
        "************************************************************************************************")
    print(
        "************************************************************************************************")
    print("is standard point origin in failure space? ", PtAR2)
    print(
        "************************************************************************************************")
    for i in range(gammaAR2.getDimension()):
        print("importance factors = %.5f" % gammaAR2[i])
    print(
        "************************************************************************************************")
    print("Hasofer reliability index = %.5f" % beta_hasoferAR2)
    print(
        "************************************************************************************************")
    print("")
    print(
        "************************************************************************************************")
    print(
        "**************************************** MONTE CARLO *******************************************")
    print(
        "************************************************************************************************")
    print("Pf estimation = %.5e" % PFMC)
    print("Pf Variance estimation = %.5e" % Variance_PF_MC)
    print("CoV = %.5f" % CVMC)
    print("90% Confidence Interval =", "%.5e" % length90MC)
    print("CI at 90% =[", "%.5e" % (PFMC - 0.5 * length90MC),
          "; %.5e" % (PFMC + 0.5 * length90MC), "]")
    print(
        "************************************************************************************************")
    print("")
    print(
        "************************************************************************************************")
    print(
        "******************************************* L H S **********************************************")
    print(
        "************************************************************************************************")
    print("Pf estimation = %.5e" % PFLHS)
    print("Pf Variance estimation = %.5e" % Variance_PF_LHS)
    print("CoV = %.5f" % CVLHS)
    print("90% Confidence Interval =", "%.5e" % length90LHS)
    print("CI at 90% =[", "%.5e" % (PFLHS - 0.5 * length90LHS),
          "; %.5e" % (PFLHS + 0.5 * length90LHS), "]")
    print(
        "************************************************************************************************")
    print("")

except:
    import sys
    print("t_Waarts_concave.py", sys.exc_info()[0], sys.exc_info()[1])
