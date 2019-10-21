#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()


def cleanScalar(inScalar):
    if (fabs(inScalar) < 1.e-10):
        inScalar = 0.0
    return inScalar

#
# initialize the random generator
#


RandomGenerator.SetSeed(0)

try:

    #
    # Physical model
    #

    inputFunction = Description(2)
    inputFunction[0] = "u1"
    inputFunction[1] = "u2"

    formulas = Description(1)
    formulas[
        0] = "min(0.1 * (u1 - u2)^2.0 - (u1 + u2) / sqrt(2.0) + 3.0, 0.1 * (u1 - u2)^2.0 + (u1 + u2) / sqrt(2.0) + 3.0, u1 - u2 + 3.5 * sqrt(2.0), -u1 + u2 + 3.5 * sqrt(2.0))"

    limitState = SymbolicFunction(inputFunction, formulas)
    limitState.setGradient(CenteredFiniteDifferenceGradient(ResourceMap.GetAsScalar(
        "CenteredFiniteDifferenceGradient-DefaultEpsilon"), limitState.getEvaluation()))
    limitState.setHessian(CenteredFiniteDifferenceHessian(ResourceMap.GetAsScalar(
        "CenteredFiniteDifferenceHessian-DefaultEpsilon"), limitState.getEvaluation()))
    dim = limitState.getInputDimension()

    #
    # Probabilistic model
    #

    mean = Point(dim, 0.0)

    sigma = Point(dim, 1.0)

    R = IdentityMatrix(dim)
    myDistribution = Normal(mean, sigma, R)

    start = myDistribution.getMean()

    #
    # Limit state
    #

    vect = RandomVector(myDistribution)

    output = CompositeRandomVector(limitState, vect)

    myEvent = ThresholdEvent(output, Less(), 0.0)

    #
    # Computation
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
    myAbdoRackwitz.setMaximumIterationNumber(100)
    myAbdoRackwitz.setMaximumAbsoluteError(1.0e-10)
    myAbdoRackwitz.setMaximumRelativeError(1.0e-10)
    myAbdoRackwitz.setMaximumResidualError(1.0e-10)
    myAbdoRackwitz.setMaximumConstraintError(1.0e-10)

    myAlgoAR = FORM(myAbdoRackwitz, myEvent, start + Point(2, 1))
    myAlgoAR2 = SORM(myAbdoRackwitz, myEvent, start + Point(2, 1))

    myAlgoAR.run()
    myAlgoAR2.run()

    resultAR = FORMResult(myAlgoAR.getResult())
    resultAR2 = SORMResult(myAlgoAR2.getResult())

    #
    # Monte Carlo
    CoV_MC = 0.1
    myMC = ProbabilitySimulationAlgorithm(myEvent, MonteCarloExperiment())
    myMC.setMaximumOuterSampling(1000000)
    myMC.setBlockSize(1)
    myMC.setMaximumCoefficientOfVariation(CoV_MC)
    myMC.run()

    #
    # LHS
    CoV_LHS = 0.1
    myLHS = LHS(myEvent)
    myLHS.setMaximumOuterSampling(1000000)
    myLHS.setBlockSize(1)
    myLHS.setMaximumCoefficientOfVariation(CoV_LHS)
    myLHS.run()

    #
    # Results
    #
    #
    # FORM/SORM Cobyla
    PfC = resultC.getEventProbability()
    Beta_generalizedC = resultC.getGeneralisedReliabilityIndex()
    u_starC = resultC.getStandardSpaceDesignPoint()
    x_starC = resultC.getPhysicalSpaceDesignPoint()
    PtC = resultC.getIsStandardPointOriginInFailureSpace(
    ) and "true" or "false"
    gammaC = resultC.getImportanceFactors()
    beta_hasoferC = resultC.getHasoferReliabilityIndex()
    SensitivityC = resultC.getEventProbabilitySensitivity()

    PFBreitC2 = resultC2.getEventProbabilityBreitung()
    BetaBreitC2 = resultC2.getGeneralisedReliabilityIndexBreitung()
    PFHBC2 = resultC2.getEventProbabilityHohenBichler()
    BetaHBC2 = resultC2.getGeneralisedReliabilityIndexHohenBichler()
    PFTvedtC2 = resultC2.getEventProbabilityTvedt()
    BetaTvedtC2 = resultC2.getGeneralisedReliabilityIndexTvedt()
    CurvC2 = resultC2.getSortedCurvatures()
    u_starC2 = resultC2.getStandardSpaceDesignPoint()
    x_starC2 = resultC2.getPhysicalSpaceDesignPoint()
    PtC2 = resultC2.getIsStandardPointOriginInFailureSpace(
    ) and "true" or "false"
    gammaC2 = resultC2.getImportanceFactors()
    beta_hasoferC2 = resultC2.getHasoferReliabilityIndex()

    #
    # FORM/SORM Abdo Rackwitz
    PfAR = resultAR.getEventProbability()
    Beta_generalizedAR = resultAR.getGeneralisedReliabilityIndex()
    u_starAR = resultAR.getStandardSpaceDesignPoint()
    x_starAR = resultAR.getPhysicalSpaceDesignPoint()
    PtAR = resultAR.getIsStandardPointOriginInFailureSpace(
    ) and "true" or "false"
    gammaAR = resultAR.getImportanceFactors()
    beta_hasoferAR = resultAR.getHasoferReliabilityIndex()
    SensitivityAR = resultAR.getEventProbabilitySensitivity()

    PFBreitAR2 = resultAR2.getEventProbabilityBreitung()
    BetaBreitAR2 = resultAR2.getGeneralisedReliabilityIndexBreitung()
    PFHBAR2 = resultAR2.getEventProbabilityHohenBichler()
    BetaHBAR2 = resultAR2.getGeneralisedReliabilityIndexHohenBichler()
    PFTvedtAR2 = resultAR2.getEventProbabilityTvedt()
    BetaTvedtAR2 = resultAR2.getGeneralisedReliabilityIndexTvedt()
    CurvAR2 = resultAR2.getSortedCurvatures()
    u_starAR2 = resultAR2.getStandardSpaceDesignPoint()
    x_starAR2 = resultAR2.getPhysicalSpaceDesignPoint()
    PtAR2 = resultAR2.getIsStandardPointOriginInFailureSpace(
    ) and "true" or "false"
    gammaAR2 = resultAR2.getImportanceFactors()
    beta_hasoferAR2 = resultAR2.getHasoferReliabilityIndex()

    #
    # Monte Carlo
    ResultMC = myMC.getResult()
    PFMC = ResultMC.getProbabilityEstimate()
    CVMC = ResultMC.getCoefficientOfVariation()
    Variance_PF_MC = ResultMC.getVarianceEstimate()
    length90MC = ResultMC.getConfidenceLength(0.90)

    #
    # LHS
    ResultLHS = myLHS.getResult()
    PFLHS = ResultLHS.getProbabilityEstimate()
    CVLHS = ResultLHS.getCoefficientOfVariation()
    Variance_PF_LHS = ResultLHS.getVarianceEstimate()
    length90LHS = ResultLHS.getConfidenceLength(0.90)

    #

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
    print("HohenBichler event probability = %.5e" % PFHBC2)
    print("HohenBichler generalized reliability index = %.5f" % BetaHBC2)
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
    print("HohenBichler event probability = %.5e" % PFHBAR2)
    print("HohenBichler generalized reliability index = %.5f" % BetaHBAR2)
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
    print("t_Waarts_system_series.py", sys.exc_info()[0], sys.exc_info()[1])
