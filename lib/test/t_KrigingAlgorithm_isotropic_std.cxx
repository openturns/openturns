//                                               -*- C++ -*-
/**
 *  @brief The test file of KrigingAlgorithm class using IsotropicCovarianceModel
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <iostream>
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;


static KrigingResult fitKriging(const CovarianceModel & covarianceModel)
{
  Sample coordinates(9, 2);
  coordinates(0, 0) = 1.0;
  coordinates(0, 1) = 1.0;
  coordinates(1, 0) = 5.0;
  coordinates(1, 1) = 1.0;
  coordinates(2, 0) = 9.0;
  coordinates(2, 1) = 1.0;
  coordinates(3, 0) = 1.0;
  coordinates(3, 1) = 3.5;
  coordinates(4, 0) = 5.0;
  coordinates(4, 1) = 3.5;
  coordinates(5, 0) = 9.0;
  coordinates(5, 1) = 3.5;
  coordinates(6, 0) = 1.0;
  coordinates(6, 1) = 6.0;
  coordinates(7, 0) = 5.0;
  coordinates(7, 1) = 6.0;
  coordinates(8, 0) = 9.0;
  coordinates(8, 1) = 6.0;

  Sample observations(9, 1);
  observations(0, 0) = 25.0;
  observations(1, 0) = 25.0;
  observations(2, 0) = 10.0;
  observations(3, 0) = 20.0;
  observations(4, 0) = 25.0;
  observations(5, 0) = 20.0;
  observations(6, 0) = 15.0;
  observations(7, 0) = 25.0;
  observations(8, 0) = 25.0;

  Basis basis = ConstantBasisFactory(2).build();
  KrigingAlgorithm algo(coordinates, observations, covarianceModel, basis);
  algo.run();
  return algo.getResult();
}


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    PlatformInfo::SetNumericalPrecision(3);

    {
      IsotropicCovarianceModel myIsotropicKernel(SquaredExponential(), 2);
      CovarianceModel krigingFittedCovarianceModel = fitKriging(myIsotropicKernel).getCovarianceModel();
      assert_almost_equal(krigingFittedCovarianceModel.getScale()[0], 2.86427, 0.0, 1e-4);
      assert_almost_equal(krigingFittedCovarianceModel.getAmplitude()[0], 6.65231, 0.0, 1e-4);
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
