//                                               -*- C++ -*-
/**
 *  @brief The test file for Waarts noisy limit state function
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

typedef Collection<Distribution>            DistributionCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  setRandomGenerator();

  try
  {

    Description inputFunction(6);
    inputFunction[0] = "X1";
    inputFunction[1] = "X2";
    inputFunction[2] = "X3";
    inputFunction[3] = "X4";
    inputFunction[4] = "X5";
    inputFunction[5] = "X6";


    Description outputFunction(1);
    outputFunction[0] = "G";

    Description formulas(outputFunction.getSize());
    formulas[0] = "X1 + 2*X2 + 2*X3 + X4 - 5*X5 - 5*X6 +0.001*(sin(100*X1)+sin(100*X2)+sin(100*X3)+sin(100*X4)+sin(100*X5)+sin(100*X6))";

    Function EtatLimite(inputFunction, outputFunction, formulas);

    UnsignedInteger dim = EtatLimite.getInputDimension();
    fullprint << dim << std::endl;

    //   #########################################################################################################;
    //   # Probabilistic model;
    //   #########################################################################################################;

    Point mean(dim, 0.0);
    mean[0] = 120.0;
    mean[1] = 120.0;
    mean[2] = 120.0;
    mean[3] = 120.0;
    mean[4] = 50.0;
    mean[5] = 40.0;

    Point sigma(dim, 0.0);
    sigma[0] = 12.0;
    sigma[1] = 12.0;
    sigma[2] = 12.0;
    sigma[3] = 12.0;
    sigma[4] = 15.0;
    sigma[5] = 12.0;

    Scalar BorneInf = 0.0;

    Description component(1);

    DistributionCollection aCollection;
    {
      LogNormal marginal(mean[0], sigma[0], BorneInf, LogNormal::MUSIGMA);
      marginal.setName("First");
      component[0] = "One";
      marginal.setDescription(component);
      // Fill the first marginal of aCollection
      aCollection.add( marginal );
    }
    // Create a second marginal : distribution 1D
    {
      LogNormal marginal(mean[1], sigma[1], BorneInf, LogNormal::MUSIGMA);
      marginal.setName("Second");
      component[0] = "Two";
      marginal.setDescription(component);
      // Fill the first marginal of aCollection
      aCollection.add( marginal );
    }

    // Create a third marginal : distribution 1D
    {
      LogNormal marginal(mean[2], sigma[2], BorneInf, LogNormal::MUSIGMA);
      marginal.setName("Third");
      component[0] = "Three";
      marginal.setDescription(component);
      // Fill the first marginal of aCollection
      aCollection.add( marginal );
    }

    // Create a forth marginal : distribution 1D
    {
      LogNormal marginal(mean[3], sigma[3], BorneInf, LogNormal::MUSIGMA);
      marginal.setName("Forth");
      component[0] = "Four";
      marginal.setDescription(component);
      // Fill the first marginal of aCollection
      aCollection.add( marginal );
    }

    // Create a fifth marginal : distribution 1D
    {
      LogNormal marginal(mean[4], sigma[4], BorneInf, LogNormal::MUSIGMA);
      marginal.setName("Fifth");
      component[0] = "Five";
      marginal.setDescription(component);
      // Fill the first marginal of aCollection
      aCollection.add( marginal );
    }

    // Create a sixth marginal : distribution 1D
    {
      LogNormal marginal(mean[5], sigma[5], BorneInf, LogNormal::MUSIGMA);
      marginal.setName("Sixth");
      component[0] = "Six";
      marginal.setDescription(component);
      // Fill the first marginal of aCollection
      aCollection.add( marginal );
    }


    // Create a copula : IndependentCopula (pas de correlation
    IndependentCopula aCopula(aCollection.getSize());
    aCopula.setName("Independent copula");

    // Instantiate one distribution object
    ComposedDistribution myDistribution(aCollection, Copula(aCopula));
    myDistribution.setName("myDist");

    Point start(myDistribution.getMean());
    CovarianceMatrix Covariance = myDistribution.getCovariance();

    //   #########################################################################################################;
    //   # Limit state;
    //   #########################################################################################################;

    RandomVector vect(myDistribution);

    CompositeRandomVector output(EtatLimite, vect);

    ThresholdEvent myEvent(output, Less(), 0.0);

    //   #########################################################################################################;
    //   # Calculs;
    //   #########################################################################################################;
    //   ;
    //   #########################################################################################################;
    //   # FORM/SORM Cobyla;
    Cobyla myCobyla;
    myCobyla.setMaximumEvaluationNumber(100 * dim);
    myCobyla.setMaximumAbsoluteError(1.0e-4);
    myCobyla.setMaximumRelativeError(1.0e-4);
    myCobyla.setMaximumResidualError(1.0e-4);
    myCobyla.setMaximumConstraintError(1.0e-4);

    FORM myAlgoC(myCobyla, myEvent, start);

    myAlgoC.run();

    FORMResult resultC(myAlgoC.getResult());

    //   #########################################################################################################;
    //   # FORM/SORM Abdo Rackwitz;
    AbdoRackwitz myAbdoRackwitz;
    myAbdoRackwitz.setMaximumIterationNumber(1000 * dim);
    myAbdoRackwitz.setMaximumAbsoluteError(1.0e-6);
    myAbdoRackwitz.setMaximumRelativeError(1.0e-6);
    myAbdoRackwitz.setMaximumResidualError(1.0e-6);
    myAbdoRackwitz.setMaximumConstraintError(1.0e-6);

    FORM myAlgoAR(myAbdoRackwitz, myEvent, start);

    myAlgoAR.run();

    FORMResult resultAR(myAlgoAR.getResult());

    //   #########################################################################################################;
    //   # Monte Carlo;
    Scalar CoV_MC = 0.5;
    MonteCarlo myMC(myEvent);
    myMC.setMaximumOuterSampling(1000);
    myMC.setBlockSize(100);
    myMC.setMaximumCoefficientOfVariation(CoV_MC);
    myMC.run();

    //   #########################################################################################################;
    //   # LHS;
    Scalar CoV_LHS = 0.1;
    LHS myLHS(myEvent);
    myLHS.setMaximumOuterSampling(1000);
    myLHS.setBlockSize(10);
    myLHS.setMaximumCoefficientOfVariation(CoV_LHS);
    myLHS.run();

    //   #########################################################################################################
    //   # Directional Sampling
    Scalar CoV_DS = 0.1;
    DirectionalSampling myDS1(myEvent, RootStrategy(RiskyAndFast()), SamplingStrategy(RandomDirection()));
    myDS1.setMaximumOuterSampling(1000);
    myDS1.setBlockSize(10);
    myDS1.setMaximumCoefficientOfVariation(CoV_DS);
    myDS1.run();

    DirectionalSampling myDS2(myEvent, RootStrategy(MediumSafe()), SamplingStrategy(RandomDirection()));
    myDS2.setMaximumOuterSampling(1000);
    myDS2.setBlockSize(10);
    myDS2.setMaximumCoefficientOfVariation(CoV_DS);
    myDS2.run();

    DirectionalSampling myDS3(myEvent, RootStrategy(SafeAndSlow()), SamplingStrategy(RandomDirection()));
    myDS3.setMaximumOuterSampling(1000);
    myDS3.setBlockSize(10);
    myDS3.setMaximumCoefficientOfVariation(CoV_DS);
    myDS3.run();

    /*  ##########################################################################################################
        ##Importance Sampling avec Standard Event*/
    Point meanSE(dim, 0.0);
    for (UnsignedInteger i = 0; i < resultAR.getStandardSpaceDesignPoint().getDimension(); ++i)
      meanSE[i] = resultAR.getStandardSpaceDesignPoint()[i];
    Point sigmaSE(dim, 1.0);
    IdentityMatrix CorrSE(dim);
    Normal myImportanceSE(meanSE, sigmaSE, CorrSE);

    StandardEvent myStandardEvent(myEvent);

    ImportanceSampling myISS(myStandardEvent, Distribution(myImportanceSE));
    myISS.setMaximumOuterSampling(1000);
    myISS.setBlockSize(10);
    myISS.setMaximumCoefficientOfVariation(0.1);
    myISS.run();

    //   # Importance Sampling avec Event
    Point meanE(dim, 0.0);
    for (UnsignedInteger i = 0; i < resultC.getPhysicalSpaceDesignPoint().getDimension(); ++i)
      meanE[i] = resultAR.getPhysicalSpaceDesignPoint()[i];

    Point sigmaE(dim, 0.0);
    for (UnsignedInteger i = 0; i < resultC.getPhysicalSpaceDesignPoint().getDimension(); ++i)
      sigmaE[i] = std::sqrt(Covariance(i, i));

    IdentityMatrix CorrE(dim);
    Normal myImportanceE(meanE, sigmaE, CorrE);

    ImportanceSampling myIS(myEvent, Distribution(myImportanceE));
    myIS.setMaximumOuterSampling(1000);
    myIS.setBlockSize(10);
    myIS.setMaximumCoefficientOfVariation(0.1);
    myIS.run();



    /*#########################################################################################################;

      #########################################################################################################;
      # Results;
      #########################################################################################################;

      #########################################################################################################;
      # FORM/SORM Cobyla*/
    Scalar PfC = resultC.getEventProbability();
    Scalar Beta_generalizedC = resultC.getGeneralisedReliabilityIndex();
    Point u_starC = resultC.getStandardSpaceDesignPoint();
    Point x_starC = resultC.getPhysicalSpaceDesignPoint();
    Bool PtC = resultC.getIsStandardPointOriginInFailureSpace();
    Point gammaC = resultC.getImportanceFactors();
    Point gammaCC = resultC.getImportanceFactors(true);
    Scalar beta_hasoferC = resultC.getHasoferReliabilityIndex();
    Analytical::Sensitivity SensitivityC = resultC.getEventProbabilitySensitivity();

    //   #########################################################################################################;
    // //   # FORM/SORM Abdo Rackwitz;
    Scalar PfAR = resultAR.getEventProbability();
    Scalar Beta_generalizedAR = resultAR.getGeneralisedReliabilityIndex();
    Point u_starAR = resultAR.getStandardSpaceDesignPoint();
    Point x_starAR = resultAR.getPhysicalSpaceDesignPoint();
    Bool PtAR = resultAR.getIsStandardPointOriginInFailureSpace();
    Point gammaAR = resultAR.getImportanceFactors();
    Point gammaCAR = resultAR.getImportanceFactors(true);
    Scalar beta_hasoferAR = resultAR.getHasoferReliabilityIndex();
    Analytical::Sensitivity SensitivityAR = resultAR.getEventProbabilitySensitivity();


    //   ######################################/*###################################################################;
    //   # Monte Carlo*/;
    SimulationResult ResultMC = myMC.getResult();
    Scalar PFMC = ResultMC.getProbabilityEstimate();
    Scalar CVMC = ResultMC.getCoefficientOfVariation();
    Scalar Variance_PF_MC = ResultMC.getVarianceEstimate();
    Scalar length90MC = ResultMC.getConfidenceLength(0.90);

    //   #########################################################################################################;
    //   # LHS;
    SimulationResult ResultLHS = myLHS.getResult();
    Scalar PFLHS = ResultLHS.getProbabilityEstimate();
    Scalar CVLHS = ResultLHS.getCoefficientOfVariation();
    Scalar Variance_PF_LHS = ResultLHS.getVarianceEstimate();
    Scalar length90LHS = ResultLHS.getConfidenceLength(0.90);


    //   ###########################################################################################################
    //   ### Directional Sampling
    SimulationResult ResultDS1 = myDS1.getResult();
    Scalar PFDS1 = ResultDS1.getProbabilityEstimate();
    Scalar CVDS1 = ResultDS1.getCoefficientOfVariation();
    Scalar Variance_PF_DS1 = ResultDS1.getVarianceEstimate();
    Scalar length90DS1 = ResultDS1.getConfidenceLength(0.90);

    SimulationResult ResultDS2 = myDS2.getResult();
    Scalar PFDS2 = ResultDS2.getProbabilityEstimate();
    Scalar CVDS2 = ResultDS2.getCoefficientOfVariation();
    Scalar Variance_PF_DS2 = ResultDS2.getVarianceEstimate();
    Scalar length90DS2 = ResultDS2.getConfidenceLength(0.90);

    SimulationResult ResultDS3 = myDS3.getResult();
    Scalar PFDS3 = ResultDS3.getProbabilityEstimate();
    Scalar CVDS3 = ResultDS3.getCoefficientOfVariation();
    Scalar Variance_PF_DS3 = ResultDS3.getVarianceEstimate();
    Scalar length90DS3 = ResultDS3.getConfidenceLength(0.90);

    //   ##########################################################################################################
    //   ##Importance Sampling
    SimulationResult ResultISS = myISS.getResult();
    Scalar  PFISS = ResultISS.getProbabilityEstimate();
    Scalar CVISS = ResultISS.getCoefficientOfVariation();
    Scalar Variance_PF_ISS = ResultISS.getVarianceEstimate();
    Scalar length90ISS = ResultISS.getConfidenceLength(0.90);

    SimulationResult ResultIS = myIS.getResult();
    Scalar PFIS = ResultIS.getProbabilityEstimate();
    Scalar CVIS = ResultIS.getCoefficientOfVariation();
    Scalar Variance_PF_IS = ResultIS.getVarianceEstimate();
    Scalar length90IS = ResultIS.getConfidenceLength(0.90);



    //   #########################################################################################################;
    //   # Printting;
    //   #########################################################################################################;
    fullprint <<  "" << std::endl;
    fullprint <<  "" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "***************************************** FORM  COBYLA *****************************************" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "event probability =" << PfC << std::endl;
    fullprint <<  "generalized reliability index =" << Beta_generalizedC << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    for (UnsignedInteger i = 0; i < u_starC.getDimension(); i++)
      fullprint << "standard space design point =" << u_starC[i] << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    for (UnsignedInteger i = 0; i < x_starC.getDimension(); i++)
      fullprint << "physical space design point =" << x_starC[i] << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint << "is standard point origin in failure space? " << PtC << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    for (UnsignedInteger i = 0; i < gammaC.getDimension(); i++)
      fullprint << "importance factors =" << gammaC[i] << std::endl;
    for (UnsignedInteger i = 0; i < gammaCC.getDimension(); i++)
      fullprint << "importance factors (classical)=" << gammaCC[i] << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "Hasofer reliability index =" << beta_hasoferC << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    for (UnsignedInteger i = 0; i < SensitivityC.getSize(); ++i)
      for (UnsignedInteger j = 0; j < SensitivityC[i].getDimension(); ++j)
        fullprint <<  "Pf sensitivity =" << SensitivityC[i][j] << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "************************************** FORM ABDO RACKWITZ **************************************" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "event probability =" << PfAR << std::endl;
    fullprint <<  "generalized reliability index =" << Beta_generalizedAR << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    for (UnsignedInteger i = 0; i < u_starAR.getDimension(); i++)
      fullprint << "standard space design point =" << u_starAR[i] << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    for (UnsignedInteger i = 0; i < x_starAR.getDimension(); i++)
      fullprint << "physical space design point =" << x_starAR[i] << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint << "is standard point origin in failure space? " << PtAR << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    for (UnsignedInteger i = 0; i < gammaAR.getDimension(); i++)
      fullprint << "importance factors =" << gammaAR[i] << std::endl;
    for (UnsignedInteger i = 0; i < gammaCAR.getDimension(); i++)
      fullprint << "importance factors (classical)=" << gammaCAR[i] << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "Hasofer reliability index =" << beta_hasoferAR << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    for (UnsignedInteger i = 0; i < SensitivityAR.getSize(); ++i)
      for (UnsignedInteger j = 0; j < SensitivityAR[i].getDimension(); ++j)
        fullprint <<  "Pf sensitivity =" << SensitivityAR[i][j] << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "**************************************** MONTE CARLO *******************************************" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "Pf estimation =" << PFMC << std::endl;
    fullprint <<  "Pf Variance estimation =" << Variance_PF_MC << std::endl;
    fullprint <<  "CoV =" << CVMC << std::endl;
    fullprint <<  "90% Confidence Interval =" << length90MC << std::endl;
    fullprint <<  "CI at 90% =[" << PFMC - 0.5 * length90MC << ";" << PFMC + 0.5 * length90MC << "]" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "******************************************* L H S **********************************************" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "Pf estimation =" << PFLHS << std::endl;
    fullprint <<  "Pf Variance estimation =" << Variance_PF_LHS << std::endl;
    fullprint <<  "CoV =" << CVLHS << std::endl;
    fullprint <<  "90% Confidence Interval =" << length90LHS << std::endl;
    fullprint <<  "CI at 90% =[" << PFLHS - 0.5 * length90LHS << ";" << PFLHS + 0.5 * length90LHS << "]" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "******************************************* D S ************************************************" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "Pf estimation =" << PFDS1 << std::endl;
    fullprint <<  "Pf Variance estimation =" << Variance_PF_DS1 << std::endl;
    fullprint <<  "CoV =" << CVDS1 << std::endl;
    fullprint <<  "90% Confidence Interval =" << length90DS1 << std::endl;
    fullprint <<  "CI at 90% =[" << PFDS1 - 0.5 * length90DS1 << ";" << PFDS1 + 0.5 * length90DS1 << "]" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "Pf estimation =" << PFDS2 << std::endl;
    fullprint <<  "Pf Variance estimation =" << Variance_PF_DS2 << std::endl;
    fullprint <<  "CoV =" << CVDS2 << std::endl;
    fullprint <<  "90% Confidence Interval =" << length90DS2 << std::endl;
    fullprint <<  "CI at 90% =[" << PFDS2 - 0.5 * length90DS2 << ";" << PFDS2 + 0.5 * length90DS2 << "]" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "Pf estimation =" << PFDS3 << std::endl;
    fullprint <<  "Pf Variance estimation =" << Variance_PF_DS3 << std::endl;
    fullprint <<  "CoV =" << CVDS3 << std::endl;
    fullprint <<  "90% Confidence Interval =" << length90DS3 << std::endl;
    fullprint <<  "CI at 90% =[" << PFDS3 - 0.5 * length90DS3 << ";" << PFDS3 + 0.5 * length90DS3 << "]" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "******************************************* I S ************************************************" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "Pf estimation =" << PFISS << std::endl;
    fullprint <<  "Pf Variance estimation =" << Variance_PF_ISS << std::endl;
    fullprint <<  "CoV =" << CVISS << std::endl;
    fullprint <<  "90% Confidence Interval =" << length90ISS << std::endl;
    fullprint <<  "CI at 90% =[" << PFISS - 0.5 * length90ISS << ";" << PFISS + 0.5 * length90ISS << "]" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "" << std::endl;
    fullprint <<  "Pf estimation =" << PFIS << std::endl;
    fullprint <<  "Pf Variance estimation =" << Variance_PF_IS << std::endl;
    fullprint <<  "CoV =" << CVIS << std::endl;
    fullprint <<  "90% Confidence Interval =" << length90IS << std::endl;
    fullprint <<  "CI at 90% =[" << PFIS - 0.5 * length90IS << ";" << PFIS + 0.5 * length90IS << "]" << std::endl;
    fullprint <<  "************************************************************************************************" << std::endl;
    fullprint <<  "" << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;

}
