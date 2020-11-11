//                                               -*- C++ -*-
/**
 *  @brief The test file of class KarhunenLoeveP1Algorithm
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    Mesh mesh(IntervalMesher(Indices(1, 9)).build(Interval(-1.0, 1.0)));
    {
      // 1d
      AbsoluteExponential cov1D(Point(1, 1.0));
      KarhunenLoeveP1Algorithm algo(mesh, cov1D, 1e-6);
      algo.run();
      KarhunenLoeveResult result(algo.getResult());
      Point lambda(result.getEigenValues());
      ProcessSample KLModes(result.getModesAsProcessSample());
      fullprint << "KL modes=" << KLModes << std::endl;
      fullprint << "KL eigenvalues=" << lambda << std::endl;
      // Check the accuracy of the projection->lifting chain
      GaussianProcess process(cov1D, KLModes.getMesh());
      ProcessSample processSample(process.getSample(10));
      Sample coefficients(result.project(processSample));
      Scalar error = 0.0;
      for (UnsignedInteger i = 0; i < processSample.getSize(); ++i)
        error += (result.liftAsSample(coefficients[i]) - processSample[i]).computeRawMoment(2)[0];
      assert_almost_equal(error, 0.0, 0.0, 1.0e-12);
      error = 0.0;
      Point x(1, 0.25);
      for (UnsignedInteger i = 0; i < processSample.getSize(); ++i)
        error += std::abs((result.lift(coefficients[i]) - P1LagrangeEvaluation(Field(mesh, processSample[i])))(x)[0]);
      assert_almost_equal(error, 0.0, 0.0, 1.0e-12);
    }
    {
      // 1d + truncation
      AbsoluteExponential cov1D(Point(1, 1.0));
      KarhunenLoeveP1Algorithm algo(mesh, cov1D, 1e-6);
      algo.setNbModes(5);// out of 10
      algo.run();
      KarhunenLoeveResult result(algo.getResult());
      Point lambda(result.getEigenValues());
      ProcessSample KLModes(result.getModesAsProcessSample());
      fullprint << "KL modes=" << KLModes << std::endl;
      fullprint << "KL eigenvalues=" << lambda << std::endl;
      GaussianProcess process(cov1D, KLModes.getMesh());
      ProcessSample processSample(process.getSample(10));
      Sample coefficients(result.project(processSample));
      Scalar error = 0.0;
      for (UnsignedInteger i = 0; i < processSample.getSize(); ++i)
        error += (result.liftAsSample(coefficients[i]) - processSample[i]).computeRawMoment(2)[0];
      assert_almost_equal(error, 0.784901, 1e-6, 1.0e-6);
      fullprint << "Reconstruction error (as field/sample)=" << error << std::endl;
      error = 0.0;
      Point x(1, 0.25);
      for (UnsignedInteger i = 0; i < processSample.getSize(); ++i)
        error += std::abs((result.lift(coefficients[i]) - P1LagrangeEvaluation(Field(mesh, processSample[i])))(x)[0]);
      assert_almost_equal(error, 0.965275 , 1.0e-6, 1.0e-6);
    }
    {
      // 2d output
      CorrelationMatrix R(2);
      R(0, 1) = 0.5;
      Point scale(1, 1.0);
      Point amplitude(2);
      amplitude[0] = 1.0;
      amplitude[1] = 2.0;
      ExponentialModel cov2D(scale, amplitude, R);
      KarhunenLoeveP1Algorithm algo(mesh, cov2D, 1e-6);
      algo.run();
      KarhunenLoeveResult result(algo.getResult());
      Point lambda(result.getEigenValues());
      ProcessSample KLModes(result.getModesAsProcessSample());
      fullprint << "KL modes=" << KLModes << std::endl;
      fullprint << "KL eigenvalues=" << lambda << std::endl;
      GaussianProcess process(cov2D, KLModes.getMesh());
      ProcessSample processSample(process.getSample(10));
      Sample coefficients(result.project(processSample));
      Scalar error = 0.0;
      for (UnsignedInteger i = 0; i < processSample.getSize(); ++i)
        error += (result.liftAsSample(coefficients[i]) - processSample[i]).computeRawMoment(2).norm();
      assert_almost_equal(error, 0.0, 0.0, 1.0e-12);
      error = 0.0;
      Point x(1, 0.25);
      for (UnsignedInteger i = 0; i < processSample.getSize(); ++i)
        error += ((result.lift(coefficients[i]) - P1LagrangeEvaluation(Field(mesh, processSample[i])))(x)).norm();
      assert_almost_equal(error, 0.0, 0.0, 1.0e-12);
    }
    {
      // 2d input, to check that issue #1660 is solved
      Mesh mesh(IntervalMesher(Indices(2, 4)).build(Interval(Point(2, -1.2), Point(2, 1.0))));
      AbsoluteExponential cov2D(Point(2, 1.0));
      KarhunenLoeveP1Algorithm algo(mesh, cov2D, 1e-6);
      algo.run();
      KarhunenLoeveResult result(algo.getResult());
      Point lambda(result.getEigenValues());
      ProcessSample KLModesPS(result.getModesAsProcessSample());
      // The output is hidden due to near-zero nonreproducible values
      //fullprint << "KL modes (process sample)=" << KLModesPS << std::endl;
      ProcessSample KLScaledModesPS(result.getScaledModesAsProcessSample());
      // The output is hidden due to near-zero nonreproducible values
      //fullprint << "KL scaled modes (process sample)=" << KLScaledModesPS << std::endl;
      Basis KLModes(result.getModes());
      // The output is hidden due to near-zero nonreproducible values
      //fullprint << "KL modes (functions)=" << KLModes << std::endl;
      Basis KLScaledModes(result.getScaledModes());
      // The output is hidden due to near-zero nonreproducible values
      //fullprint << "KL scaled modes (functions)=" << KLScaledModes << std::endl;
      fullprint << "KL eigenvalues=" << lambda << std::endl;
      GaussianProcess process(cov2D, mesh);
      ProcessSample processSample(process.getSample(10));
      Sample coefficients(result.project(processSample));
      Scalar error = 0.0;
      for (UnsignedInteger i = 0; i < processSample.getSize(); ++i)
        error += (result.liftAsSample(coefficients[i]) - processSample[i]).computeRawMoment(2).norm();
      assert_almost_equal(error, 0.0, 0.0, 1.0e-12);
      error = 0.0;
      Point x(2, 0.25);
      for (UnsignedInteger i = 0; i < processSample.getSize(); ++i)
        error += ((result.lift(coefficients[i]) - P1LagrangeEvaluation(Field(mesh, processSample[i])))(x)).norm();
      assert_almost_equal(error, 0.0, 0.0, 1.0e-12);
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

