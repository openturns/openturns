//                                               -*- C++ -*-
/**
 *  @brief The test file of class PenalizedLeastSquaresAlgorithm for the standard methods
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
    UnsignedInteger dimension = 2;
    // Reference function
    Description inVar(dimension);
    inVar[0] = "x1";
    inVar[1] = "x2";
    Description formula(1);
    formula[0] = "x1^3+1.5*x2^3-x1*x2";
    SymbolicFunction model(inVar, formula);

    // Basis upon which we will project the model
    PenalizedLeastSquaresAlgorithm::FunctionCollection coll(4);
    formula[0] = "x1";
    coll[0] = SymbolicFunction(inVar, formula);
    formula[0] = "x2";
    coll[1] = SymbolicFunction(inVar, formula);
    formula[0] = "x1^2";
    coll[2] = SymbolicFunction(inVar, formula);
    formula[0] = "x2^2";
    coll[3] = SymbolicFunction(inVar, formula);
    Indices indices(coll.getSize());
    indices.fill();

    // Input sample
    UnsignedInteger size = 5;
    Sample inputSample(size * size, dimension);
    Point weight(inputSample.getSize());
    for (UnsignedInteger i = 0; i < inputSample.getSize(); ++i)
    {
      inputSample[i][0] = Scalar(i % size) / size;
      inputSample[i][1] = Scalar (i / size) / size;
      weight[i] = (i % size + 1) * (i / size + 1);
    }
    Scalar penalizationFactor = 0.25;
    // Uniform weight, no penalization
    {
      PenalizedLeastSquaresAlgorithm algo(inputSample, model(inputSample), Point(inputSample.getSize(), 1.0), coll, indices);
      fullprint << "Uniform weight, no penalization" << std::endl;
      fullprint << "Coefficients=" << algo.getCoefficients() << std::endl;
      fullprint << "Residual=" << algo.getResidual() << std::endl;
      fullprint << "Relative error=" << algo.getRelativeError() << std::endl;
    }
    // Uniform weight, spherical penalization
    {
      PenalizedLeastSquaresAlgorithm algo(inputSample, model(inputSample), Point(inputSample.getSize(), 1.0), coll, indices, penalizationFactor);
      fullprint << "Uniform weight, spherical penalization" << std::endl;
      fullprint << "Coefficients=" << algo.getCoefficients() << std::endl;
      fullprint << "Residual=" << algo.getResidual() << std::endl;
    }
    // Non uniform weight, no penalization
    {
      PenalizedLeastSquaresAlgorithm algo(inputSample, model(inputSample), weight, coll, indices);
      fullprint << "Non uniform weight, no penalization" << std::endl;
      fullprint << "Coefficients=" << algo.getCoefficients() << std::endl;
      fullprint << "Residual=" << algo.getResidual() << std::endl;
    }
    // Non uniform weight, spherical penalization
    {
      PenalizedLeastSquaresAlgorithm algo(inputSample, model(inputSample), weight, coll, indices, penalizationFactor);
      fullprint << "Non uniform weight, spherical penalization" << std::endl;
      fullprint << "Coefficients=" << algo.getCoefficients() << std::endl;
      fullprint << "Residual=" << algo.getResidual() << std::endl;
    }
    // Non uniform weight, non spherical penalization
    {
      CovarianceMatrix penalizationMatrix(4);
      for (UnsignedInteger i = 0; i < 4; ++i)
        penalizationMatrix(i, i) = 1.0;
      for (UnsignedInteger i = 0; i < 3; ++i)
        penalizationMatrix(i, i + 1) = 1.0 / 8.0;
      PenalizedLeastSquaresAlgorithm algo(inputSample, model(inputSample), weight, coll, indices, penalizationFactor, penalizationMatrix);
      fullprint << "Non uniform weight, non spherical penalization" << std::endl;
      fullprint << "Coefficients=" << algo.getCoefficients() << std::endl;
      fullprint << "Residual=" << algo.getResidual() << std::endl;
    }
  }

  catch (OT::NotSymmetricDefinitePositiveException & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
