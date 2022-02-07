#include <iostream>
// OT includes
#include <openturns/OT.hxx>

using namespace OT;

int main(int, char *[])
{
  const UnsignedInteger dimension = 3;
  const UnsignedInteger size = 25;
  ComposedDistribution::DistributionCollection coll(dimension, Uniform(0.0, 1.0));
  ComposedDistribution distribution(coll);
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
  SpaceFillingPhiP spaceFillingPhiP(10);
  SpaceFillingC2 spaceFillingC2;
  SpaceFillingMinDist spaceFillingMinDist;

  // Print the criteria on this design
  std::cout << "PhiP=" << spaceFillingPhiP.evaluate(design)
            << ", C2=" << spaceFillingC2.evaluate(design)
            << ", MinDist=" << spaceFillingMinDist.evaluate(design)
            << std::endl;

  // --------------------------------------------------
  // -------------  Simulated annealing  --------------
  // --------------------------------------------------

  // Geometric profile
  const Scalar T0(10.0);
  const UnsignedInteger iMax(2000);
  const Scalar c(0.95);
  const GeometricProfile geomProfile(T0, c, iMax);

  // 1) Simulated Annealing LHS with geometric temperature profile, C2 optimization
  SimulatedAnnealingLHS optimalSA_C2(lhs, spaceFillingC2, geomProfile);

  // print lhs
  std::cout << "optimal lhs=" << optimalSA_C2 << std::endl;

  Sample design_C2(optimalSA_C2.generate());
  LHSResult result_C2(optimalSA_C2.getResult());

  std::cout << "Best design using geometric temperature and C2 space filling=" << design_C2 << std::endl;
  std::cout << "Final criteria: C2=" << result_C2.getC2()
            << ", PhiP=" << result_C2.getPhiP()
            << ", MinDist=" << result_C2.getMinDist()
            << std::endl;

  // 2) Simulated Annealing LHS with linear temperature profile, PhiP optimization
  const LinearProfile linearProfile(T0, iMax);
  SimulatedAnnealingLHS optimalSA_PhiP(lhs, spaceFillingPhiP, linearProfile);
  // print lhs
  std::cout << "optimal lhs=" << optimalSA_PhiP << std::endl;

  Sample design_PhiP(optimalSA_PhiP.generate());
  LHSResult result_PhiP(optimalSA_PhiP.getResult());

  std::cout << "Best design using linear temperature profile and PhiP space filling=" << design_PhiP << std::endl;
  std::cout << "Final criteria: C2=" << result_PhiP.getC2()
            << ", PhiP=" << result_PhiP.getPhiP()
            << ", MinDist=" << result_PhiP.getMinDist()
            << std::endl;

  // 3) Simulated Annealing LHS with linear temperature profile, PhiP optimization and nStart > 0
  const UnsignedInteger nStart = 10;
  Sample design_PhiP_restart(optimalSA_PhiP.generateWithRestart(nStart));
  LHSResult result_PhiP_restart(optimalSA_PhiP.getResult());

  std::cout << "Best design using linear temperature profile and PhiP space filling=" << design_PhiP_restart << std::endl;
  std::cout << "Final criteria: C2=" << result_PhiP_restart.getC2()
            << ", PhiP=" << result_PhiP_restart.getPhiP()
            << ", MinDist=" << result_PhiP_restart.getMinDist()
            << std::endl;
  for(UnsignedInteger i = 0; i < nStart; ++i)
  {
    Sample design_PhiP_i(result_PhiP_restart.getOptimalDesign(i));
    std::cout << "  Intermediate design for restart iteration number " << i << " " << design_PhiP_i << std::endl;
    std::cout << "  Final criteria: C2=" << result_PhiP_restart.getC2(i)
              << ", PhiP=" << result_PhiP_restart.getPhiP(i)
              << ", MinDist=" << result_PhiP_restart.getMinDist()
              << std::endl;
  }

  return 0;
}
