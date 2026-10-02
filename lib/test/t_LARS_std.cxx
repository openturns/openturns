//                                               -*- C++ -*-
/**
 *  @brief The test file of class LAR for standard methods
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Problem parameters
    UnsignedInteger dimension = 3;
    Scalar a = 7.0;
    Scalar b = 0.1;

    // Create the Ishigami function
    Description inputVariables(dimension);
    inputVariables[0] = "xi1";
    inputVariables[1] = "xi2";
    inputVariables[2] = "xi3";
    Description formula(1);
    formula[0] = (OSS() << "sin(xi1) + (" << a << ") * (sin(xi2)) ^ 2 + (" << b << ") * xi3^4 * sin(xi1)");
    SymbolicFunction model(inputVariables, formula);

    // Create the input distribution
    Collection<Distribution> marginalX(dimension);
    marginalX[0] = Uniform(-M_PI, M_PI);
    marginalX[1] = Uniform(-M_PI, M_PI);
    marginalX[2] = Uniform(-M_PI, M_PI);
    JointDistribution distribution(marginalX);

    // Create the orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
    polynomialCollection[0] = LegendreFactory();
    polynomialCollection[1] = LegendreFactory();
    polynomialCollection[2] = LegendreFactory();
    LinearEnumerateFunction enumerateFunction(dimension);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    // design experiment
    UnsignedInteger samplingSize = 75;
    Experiment experiment(LowDiscrepancyExperiment(SobolSequence(dimension), distribution, samplingSize));

    // build the isoprobabilistic transformation
    Collection<Distribution> marginalZ;
    for (UnsignedInteger i = 0; i < dimension; ++ i )
    {
      marginalZ.add(productBasis.getMeasure().getMarginal(i));
    }
    MarginalTransformationEvaluation evaluationT(MarginalTransformationEvaluation(marginalX, marginalZ));
    MarginalTransformationGradient gradientT(evaluationT);
    MarginalTransformationHessian hessianT(evaluationT);
    Function xToU(evaluationT.clone(), gradientT.clone(), hessianT.clone());

    // generate samples
    Sample x( experiment.generate() );
    Sample u( xToU(x) );
    Sample y( model(x) );

    // build basis
    UnsignedInteger degree = 10;
    UnsignedInteger basisSize = enumerateFunction.getStrataCumulatedCardinal(degree);
    Collection<Function> coll;
    for ( UnsignedInteger i = 0; i < basisSize; ++ i )
    {
      coll.add(productBasis.build(i));
    }
    Indices indices(basisSize);
    indices.fill();

    // run algorithm
    LARS factory;
    fullprint << "factory = " << factory << std::endl;

    BasisSequence seq = factory.build( u, y, coll, indices);

    // Verify the selected indices
    Indices expectedIndices({0, 30, 115, 10, 1, 77, 187, 15, 162, 40, 89, 7, 51, 55, 156, 278, 179, 232, 35, 157});

    UnsignedInteger first = 20;
    if (seq.getSize() < first)
      throw TestFailed("Not enough LARS iterations");
    assert_equal(seq.getIndices(first - 1), expectedIndices);
  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
