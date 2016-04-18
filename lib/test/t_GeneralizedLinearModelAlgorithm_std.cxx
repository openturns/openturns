//                                               -*- C++ -*-
/**
 *  @brief The test file of GeneralizedLinearModelAlgorithm class
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

static String printNumericalPoint(const NumericalPoint & point, const UnsignedInteger digits)
{
  OSS oss;
  oss << "[";
  NumericalScalar eps = pow(0.1, 1.0 * digits);
  for (UnsignedInteger i = 0; i < point.getDimension(); i++)
  {
    oss << std::scientific << std::setprecision(digits) << (i == 0 ? "" : ",") << Bulk<double>((std::abs(point[i]) < eps) ? std::abs(point[i]) : point[i]);
  }
  oss << "]";
  return oss;
}


int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Set Numerical precision to 3
    UnsignedInteger precision = PlatformInfo::GetNumericalPrecision();
    PlatformInfo::SetNumericalPrecision(3);

    std::cout << "=============" << std::endl;
    std::cout << "Test standard" << std::endl;
    std::cout << "=============" << std::endl;
    UnsignedInteger sampleSize = 6;
    UnsignedInteger spatialDimension = 1;

    // Create the function to estimate
    Description input(spatialDimension);
    input[0] = "x0";
    Description foutput(1);
    foutput[0] = "f0";
    Description formulas(1);
    formulas[0] = "x0";
    NumericalMathFunction model(input, foutput, formulas);

    NumericalSample X(sampleSize, spatialDimension);
    NumericalSample X2(sampleSize, spatialDimension);
    for ( UnsignedInteger i = 0; i < sampleSize; ++ i )
      {
        X[i][0] = 3.0 + i;
        X2[i][0] = 2.5 + i;
      }
    X[0][0] = 1.0;
    X[1][0] = 3.0;
    X2[0][0] = 2.0;
    X2[1][0] = 4.0;
    NumericalSample Y = model(X);
    for ( UnsignedInteger i = 0; i < sampleSize; ++ i )
      {
        Y[i][0] += 0.01 * DistFunc::rNormal();
      }
    // Add a small noise to data
    NumericalSample Y2 = model(X2);

    Basis basis = LinearBasisFactory(spatialDimension).build();
    DiracCovarianceModel covarianceModel(spatialDimension);
    GeneralizedLinearModelAlgorithm algo(X, Y, covarianceModel, basis);
    algo.run();

    // perform an evaluation
    GeneralizedLinearModelResult result = algo.getResult();
    NumericalMathFunction metaModel = result.getMetaModel();
    CovarianceModel conditionalCovariance = result.getCovarianceModel();
    const NumericalSample residual = metaModel(X) - Y;
    assert_almost_equal(residual.computeCenteredMoment(2), NumericalPoint(1, 0.00013144), 1e-5, 1e-5);
    assert_almost_equal(conditionalCovariance.getParameter(), NumericalPoint(1, 0.011464782674211804), 1e-5, 1e-3);
    std::cout << "Test Ok" << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
