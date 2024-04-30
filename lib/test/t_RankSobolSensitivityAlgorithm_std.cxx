//                                               -*- C++ -*-
/**
 *  @brief The test file of class SobolIndicesAlgorithm
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

int main()
{
    RandomGenerator::SetSeed(0);

    UnsignedInteger inputDimension = 3;
    //UnsignedInteger outputDimension = 1;

    RandomGenerator::SetSeed(0);
    Description inputName(inputDimension);
    inputName = {"X1", "X2", "X3"};
    Description formula(1);
    formula[0] = "sin(pi_*X1)+7*sin(pi_*X2)*sin(pi_*X2)+0.1*((pi_*X3)*(pi_*X3)*(pi_*X3)*(pi_*X3))*sin(pi_*X1)";

    SymbolicFunction model(inputName, formula);

    JointDistribution::DistributionCollection marginals(inputDimension, Uniform(-1.0, 1.0));
    JointDistribution maDistribution(JointDistribution(marginals, IndependentCopula(inputDimension)));
    
    const UnsignedInteger size = 250;  
    const Sample inputDesign(maDistribution.getSample(size));
    const Sample outputDesign(model(inputDesign)); 
    RankSobolSensitivityAlgorithm rankAlgorithm(inputDesign, outputDesign);
    
    const Point firstOrderIndices(rankAlgorithm.getFirstOrderIndices());
    OT::Test::assert_almost_equal(firstOrderIndices, Point({0.208654, 0.493591, -0.0669488}), 1e-4, 1e-2);
    
    const Interval indicesInterval(rankAlgorithm.getFirstOrderIndicesInterval());
    
    OT::Test::assert_almost_equal(indicesInterval.getLowerBound(), Point({0.117529, 0.409688, -0.176039}), 1e-4, 1e-2);
    OT::Test::assert_almost_equal(indicesInterval.getUpperBound(), Point({0.340675, 0.560271, 0.08570}), 1e-4, 1e-2);   
    
    return 0;
}
