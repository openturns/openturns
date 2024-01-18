#include <iostream>
// OT includes
#include <openturns/OT.hxx>

using namespace OT;

int main(int, char *[])
{
  const UnsignedInteger dimension = 3;
  const UnsignedInteger size = 25;
  JointDistribution::DistributionCollection coll(dimension, Uniform(0.0, 1.0));
  JointDistribution distribution(coll);
  LHSExperiment lhs(distribution, size);
  lhs.setRandomShift(false); // centered
  lhs.setAlwaysShuffle(true); // randomized

  // print lhs
  std::cout << lhs << std::endl;
  std::cout << "Bounds of uniform distributions=" << distribution.getRange() << std::endl;

  // Generate design without optimization
  Sample design(lhs.generate());
  std::cout << "design=" << design << std::endl;

  // Define space fillings
  SpaceFillingPhiP spaceFillingPhiP;
  SpaceFillingC2 spaceFillingC2;
  SpaceFillingMinDist spaceFillingMinDist;

  // Print the criteria on this design
  std::cout << "PhiP=" << spaceFillingPhiP.evaluate(design)
            << ", C2=" << spaceFillingC2.evaluate(design)
            << ", MinDist=" << spaceFillingMinDist.evaluate(design)
            << std::endl;

  // --------------------------------------------------
  //  ------------ MonteCarlo algorithm  -------------
  // --------------------------------------------------

  // RandomBruteForce MonteCarlo with N designs
  const UnsignedInteger N = 1000;

  // 1) LHS with C2 optimization
  MonteCarloLHS optimalMC_C2(lhs, N, spaceFillingC2);

  // print lhs
  std::cout << "optimal lhs=" << optimalMC_C2 << std::endl;

  const Sample design_C2(optimalMC_C2.generate());
  LHSResult result_C2(optimalMC_C2.getResult());

  std::cout << "Best design with MonteCarlo and C2 space filling=" << design_C2 << std::endl;
  std::cout << "Final criteria: C2=" << result_C2.getC2()
            << ", PhiP=" << result_C2.getPhiP()
            << ", MinDist=" << result_C2.getMinDist()
            << std::endl;

  // 2) LHS with PhiP optimization
  MonteCarloLHS optimalMC_PhiP(lhs, N, spaceFillingPhiP);
  // print lhs
  std::cout << "optimal lhs=" << optimalMC_PhiP << std::endl;

  Sample design_PhiP(optimalMC_PhiP.generate());
  LHSResult result_PhiP(optimalMC_PhiP.getResult());

  std::cout << "Best design with MonteCarlo and PhiP space filling=" << design_PhiP << std::endl;
  std::cout << "Final criteria: C2=" << result_PhiP.getC2()
            << ", PhiP=" << result_PhiP.getPhiP()
            << ", MinDist=" << result_PhiP.getMinDist()
            << std::endl;

  // 3) LHS with MinDist optimization (=mindist optim)
  MonteCarloLHS optimalMC_MinDist(lhs, N, spaceFillingMinDist);
  // print lhs
  std::cout << "optimal lhs=" << optimalMC_MinDist << std::endl;

  Sample design_MinDist(optimalMC_MinDist.generate());
  LHSResult result_MinDist(optimalMC_MinDist.getResult());

  std::cout << "Best design with MonteCarlo and MinDist space filling=" << design_MinDist << std::endl;
  std::cout << "Final criteria: C2=" << result_MinDist.getC2()
            << ", PhiP=" << result_MinDist.getPhiP()
            << ", MinDist=" << result_MinDist.getMinDist()
            << std::endl;


  return 0;
}
