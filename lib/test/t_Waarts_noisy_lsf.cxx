//                                               -*- C++ -*-
/**
 *  @brief The test file for Waarts noisy limit state function
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

typedef Collection<Distribution>            DistributionCollection;

int main(int argc, char *argv[])
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

    NumericalMathFunction EtatLimite(inputFunction, outputFunction, formulas);

    UnsignedInteger dim = EtatLimite.getInputDimension();
    fullprint << dim << std::endl;

    //   #########################################################################################################;
    //   # Probabilistic model;
    //   #########################################################################################################;

    NumericalPoint mean(dim, 0.0);
    mean[0] = 120.0;
    mean[1] = 120.0;
    mean[2] = 120.0;
    mean[3] = 120.0;
    mean[4] = 50.0;
    mean[5] = 40.0;

    NumericalPoint sigma(dim, 0.0);
    sigma[0] = 12.0;
    sigma[1] = 12.0;
    sigma[2] = 12.0;
    sigma[3] = 12.0;
    sigma[4] = 15.0;
    sigma[5] = 12.0;

    NumericalScalar BorneInf = 0.0;

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

    // Instanciate one distribution object
    ComposedDistribution myDistribution(aCollection, Copula(aCopula));
    myDistribution.setName("myDist");

    NumericalPoint start(myDistribution.getMean());
    CovarianceMatrix Covariance = myDistribution.getCovariance();

    //   #########################################################################################################;
    //   # Limit state;
    //   #########################################################################################################;

    RandomVector vect(myDistribution);

    RandomVector output(EtatLimite, vect);

    Event myEvent(output, Less(), 0.0);

    //   #########################################################################################################;
    //   # Calculs;
    //   #########################################################################################################;
    //   ;
    //   #########################################################################################################;
    //   # FORM/SORM Cobyla;
    Cobyla myCobyla;
    myCobyla.setSpecificParameters(CobylaSpecificParameters());
    myCobyla.setMaximumIterationsNumber(100);
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
    myAbdoRackwitz.setSpecificParameters(AbdoRackwitzSpecificParameters());
    myAbdoRackwitz.setMaximumIterationsNumber(1000);
    myAbdoRackwitz.setMaximumAbsoluteError(1.0e-6);
    myAbdoRackwitz.setMaximumRelativeError(1.0e-6);
    myAbdoRackwitz.setMaximumResidualError(1.0e-6);
    myAbdoRackwitz.setMaximumConstraintError(1.0e-6);

    FORM myAlgoAR(myAbdoRackwitz, myEvent, start);

    myAlgoAR.run();

    FORMResult resultAR(myAlgoAR.getResult());

    //   #########################################################################################################;
    //   # Monte Carlo;
    NumericalScalar CoV_MC = 0.5;
    MonteCarlo myMC(myEvent);
    myMC.setMaximumOuterSampling(1000);
    myMC.setBlockSize(100);
    myMC.setMaximumCoefficientOfVariation(CoV_MC);
    myMC.run();

    //   #########################################################################################################;
    //   # LHS;
    NumericalScalar CoV_LHS = 0.1;
    LHS myLHS(myEvent);
    myLHS.setMaximumOuterSampling(1000);
    myLHS.setBlockSize(10);
    myLHS.setMaximumCoefficientOfVariation(CoV_LHS);
    myLHS.run();

    //   #########################################################################################################
    //   # Directional Sampling
    NumericalScalar CoV_DS = 0.1;
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
    NumericalPoint meanSE(dim, 0.0);
    for (UnsignedInteger i = 0; i < resultAR.getStandardSpaceDesignPoint().getDimension(); ++i)
      meanSE[i] = resultAR.getStandardSpaceDesignPoint()[i];
    NumericalPoint sigmaSE(dim, 1.0);
    IdentityMatrix CorrSE(dim);
    Normal myImportanceSE(meanSE, sigmaSE, CorrSE);

    StandardEvent myStandardEvent(myEvent);

    ImportanceSampling myISS(myStandardEvent, Distribution(myImportanceSE));
    myISS.setMaximumOuterSampling(1000);
    myISS.setBlockSize(10);
    myISS.setMaximumCoefficientOfVariation(0.1);
    myISS.run();

    //   # Importance Sampling avec Event
    NumericalPoint meanE(dim, 0.0);
    for (UnsignedInteger i = 0; i < resultC.getPhysicalSpaceDesignPoint().getDimension(); ++i)
      meanE[i] = resultAR.getPhysicalSpaceDesignPoint()[i];

    NumericalPoint sigmaE(dim, 0.0);
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
    NumericalScalar PfC = resultC.getEventProbability();
    NumericalScalar Beta_generalizedC = resultC.getGeneralisedReliabilityIndex();
    NumericalPoint u_starC = resultC.getStandardSpaceDesignPoint();
    NumericalPoint x_starC = resultC.getPhysicalSpaceDesignPoint();
    Bool PtC = resultC.getIsStandardPointOriginInFailureSpace();
    NumericalPoint gammaC = resultC.getImportanceFactors();
    NumericalPoint gammaCC = resultC.getImportanceFactors(true);
    NumericalScalar beta_hasoferC = resultC.getHasoferReliabilityIndex();
    Analytical::Sensitivity SensitivityC = resultC.getEventProbabilitySensitivity();

    //   #########################################################################################################;
    // //   # FORM/SORM Abdo Rackwitz;
    NumericalScalar PfAR = resultAR.getEventProbability();
    NumericalScalar Beta_generalizedAR = resultAR.getGeneralisedReliabilityIndex();
    NumericalPoint u_starAR = resultAR.getStandardSpaceDesignPoint();
    NumericalPoint x_starAR = resultAR.getPhysicalSpaceDesignPoint();
    Bool PtAR = resultAR.getIsStandardPointOriginInFailureSpace();
    NumericalPoint gammaAR = resultAR.getImportanceFactors();
    NumericalPoint gammaCAR = resultAR.getImportanceFactors(true);
    NumericalScalar beta_hasoferAR = resultAR.getHasoferReliabilityIndex();
    Analytical::Sensitivity SensitivityAR = resultAR.getEventProbabilitySensitivity();


    //   ######################################/*###################################################################;
    //   # Monte Carlo*/;
    SimulationResult ResultMC = myMC.getResult();
    NumericalScalar PFMC = ResultMC.getProbabilityEstimate();
    NumericalScalar CVMC = ResultMC.getCoefficientOfVariation();
    NumericalScalar Variance_PF_MC = ResultMC.getVarianceEstimate();
    NumericalScalar length90MC = ResultMC.getConfidenceLength(0.90);

    //   #########################################################################################################;
    //   # LHS;
    SimulationResult ResultLHS = myLHS.getResult();
    NumericalScalar PFLHS = ResultLHS.getProbabilityEstimate();
    NumericalScalar CVLHS = ResultLHS.getCoefficientOfVariation();
    NumericalScalar Variance_PF_LHS = ResultLHS.getVarianceEstimate();
    NumericalScalar length90LHS = ResultLHS.getConfidenceLength(0.90);


    //   ###########################################################################################################
    //   ### Directional Sampling
    SimulationResult ResultDS1 = myDS1.getResult();
    NumericalScalar PFDS1 = ResultDS1.getProbabilityEstimate();
    NumericalScalar CVDS1 = ResultDS1.getCoefficientOfVariation();
    NumericalScalar Variance_PF_DS1 = ResultDS1.getVarianceEstimate();
    NumericalScalar length90DS1 = ResultDS1.getConfidenceLength(0.90);

    SimulationResult ResultDS2 = myDS2.getResult();
    NumericalScalar PFDS2 = ResultDS2.getProbabilityEstimate();
    NumericalScalar CVDS2 = ResultDS2.getCoefficientOfVariation();
    NumericalScalar Variance_PF_DS2 = ResultDS2.getVarianceEstimate();
    NumericalScalar length90DS2 = ResultDS2.getConfidenceLength(0.90);

    SimulationResult ResultDS3 = myDS3.getResult();
    NumericalScalar PFDS3 = ResultDS3.getProbabilityEstimate();
    NumericalScalar CVDS3 = ResultDS3.getCoefficientOfVariation();
    NumericalScalar Variance_PF_DS3 = ResultDS3.getVarianceEstimate();
    NumericalScalar length90DS3 = ResultDS3.getConfidenceLength(0.90);

    //   ##########################################################################################################
    //   ##Importance Sampling
    SimulationResult ResultISS = myISS.getResult();
    NumericalScalar  PFISS = ResultISS.getProbabilityEstimate();
    NumericalScalar CVISS = ResultISS.getCoefficientOfVariation();
    NumericalScalar Variance_PF_ISS = ResultISS.getVarianceEstimate();
    NumericalScalar length90ISS = ResultISS.getConfidenceLength(0.90);

    SimulationResult ResultIS = myIS.getResult();
    NumericalScalar PFIS = ResultIS.getProbabilityEstimate();
    NumericalScalar CVIS = ResultIS.getCoefficientOfVariation();
    NumericalScalar Variance_PF_IS = ResultIS.getVarianceEstimate();
    NumericalScalar length90IS = ResultIS.getConfidenceLength(0.90);



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
