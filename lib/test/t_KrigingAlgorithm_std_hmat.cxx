//                                               -*- C++ -*-
/**
 *  @brief The test file of KrigingAlgorithm class
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
  setRandomGenerator();
  ResourceMap::Set("KrigingAlgorithm-LinearAlgebra", "HMAT");

  try
  {

    // Set Numerical precision to 3
    PlatformInfo::SetNumericalPrecision(3);

    {
      UnsignedInteger sampleSize = 6;
      UnsignedInteger dimension = 1;

      // Create the function to estimate
      Description input(dimension);
      input[0] = "x0";
      Description formulas(1);
      formulas[0] = "x0 * sin(x0)";
      SymbolicFunction model(input, formulas);

      Sample X(sampleSize, dimension);
      Sample X2(sampleSize, dimension);
      for ( UnsignedInteger i = 0; i < sampleSize; ++ i )
      {
        X(i, 0) = 3.0 + i;
        X2(i, 0) = 2.5 + i;
      }
      X(0, 0) = 1.0;
      X(1, 0) = 3.0;
      X2(0, 0) = 2.0;
      X2(1, 0) = 4.0;
      Sample Y(model(X));
      Sample Y2(model(X2));

      Basis basis(ConstantBasisFactory(dimension).build());
      SquaredExponential covarianceModel(Point(1, 1e-05), Point(1, 4.11749));
      KrigingAlgorithm algo(X, Y, covarianceModel, basis);

      algo.run();

      // perform an evaluation
      KrigingResult result(algo.getResult());
      std::cout << "X=" << X << std::endl;
      std::cout << "f(X)=" << Y << std::endl;
      std::cout << "covariance parameter=" << result.getCovarianceModel().getParameter() << std::endl;
      assert_almost_equal(result.getMetaModel()(X), Y, 1e-3);

      Point residualRef(1, 5.57410e-06);
      assert_almost_equal(result.getResiduals(), residualRef, 1e-3, 5e-4);

      Point relativeErrorRef(1, 9.17605e-12);
      assert_almost_equal(result.getRelativeErrors(), relativeErrorRef, 1e-3, 5e-3);

      // Evaluation of the covariance on the X dataset
      CovarianceMatrix covMatrix(result.getConditionalCovariance(X));

      // Validation of the covariance ==> should be null on the learning set
      assert_almost_equal(Point(*covMatrix.getImplementation()), Point(sampleSize * sampleSize), 5.e-3, 5.e-3);

      // Covariance per marginal & extract variance component
      Collection<CovarianceMatrix> coll(result.getConditionalMarginalCovariance(X));

      for(UnsignedInteger k = 0; k < coll.getSize(); ++k)
        assert_almost_equal(Point(*coll[k].getImplementation()), Point(1, 0.0), 5.e-3, 5.e-3);

      // Validation of marginal variance
      const Point marginalVariance(result.getConditionalMarginalVariance(X));
      std::cerr << "Assert 0" << std::endl;
      std::cerr << marginalVariance << std::endl;
      assert_almost_equal(marginalVariance, Point(sampleSize), 5.e-3, 5.e-3);
    }

    {
      UnsignedInteger dimension = 2;
      UnsignedInteger sampleSize = 8;
      // Create the function to estimate
      Description input(dimension);
      input[0] = "x0";
      input[1] = "x1";
      Description formulas(1);
      formulas[0] = "5.-x1-0.5*(x0-0.1)^2";
      SymbolicFunction model(input, formulas);
      Sample X(sampleSize, dimension);
      X(0, 0) = -4.61611719;
      X(0, 1) = -6.00099547;
      X(1, 0) = 4.10469096;
      X(1, 1) = 5.32782448;
      X(2, 0) = 0.;
      X(2, 1) = -.5;
      X(3, 0) = -6.17289014;
      X(3, 1) = -4.6984743;
      X(4, 0) = 1.3109306;
      X(4, 1) = -6.93271427;
      X(5, 0) = -5.03823144;
      X(5, 1) = 3.10584743;
      X(6, 0) = -2.87600388;
      X(6, 1) = 6.74310541;
      X(7, 0) = 5.21301203;
      X(7, 1) = 4.26386883;
      Sample Y(model(X));

      // create algorithm
      Basis basis(ConstantBasisFactory(dimension).build());
      Point scale(2);
      scale[0] = 1e-05;
      scale[1] = 18.9;
      Point amplitude(1,  8.05);
      SquaredExponential covarianceModel(scale, amplitude);

      KrigingAlgorithm algo(X, Y, covarianceModel, basis);
      algo.run();

      // perform an evaluation
      KrigingResult result(algo.getResult());
      std::cout << "X=" << X << std::endl;
      std::cout << "f(X)=" << Y << std::endl;
      std::cout << "covariance parameter=" << result.getCovarianceModel().getParameter() << std::endl;

      std::cerr << "A" << std::endl;
      std::cerr << result.getMetaModel()(X) << Y << std::endl;
      assert_almost_equal(result.getMetaModel()(X), Y, 1e-3, 1e-3);

      Point residualRef(1, 1.17e-07);
      std::cerr << "B" << std::endl;
      std::cerr << result.getResiduals() << residualRef << std::endl;
      assert_almost_equal(result.getResiduals(), residualRef, 6.e-4, 6.e-4);

      Point relativeErrorRef(1, 1.48e-11);
      std::cerr << "C" << std::endl;
      std::cerr << result.getRelativeErrors() << relativeErrorRef << std::endl;
      assert_almost_equal(result.getRelativeErrors(), relativeErrorRef, 1e-3, 1e-5);

      std::cout << "df(X0)=" << model.gradient(X[1]) << std::endl;

      Function metaModel(result.getMetaModel());
      // Get the gradient computed by metamodel
      Matrix gradientKriging(metaModel.gradient(X[1]));

      // Set DF evaluation as gradient and validate
      metaModel.setGradient(new CenteredFiniteDifferenceGradient(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), metaModel.getEvaluation()));

      // Get the gradient computed by metamodel using FD
      Matrix gradientKrigingFD(metaModel.gradient(X[1]));

      // Validation of the gradient
      std::cout << "d^f(X0) & d^f(X0) FD similar ?" <<  std::endl;
      assert_almost_equal(Point(*gradientKriging.getImplementation()), Point(*gradientKrigingFD.getImplementation()), 1e-3, 1e-3);
      std::cout << "d^f(X0) & d^f(X0) FD are similar." <<  std::endl;

      // Covariance per marginal & extract variance component
      Collection<CovarianceMatrix> coll(result.getConditionalMarginalCovariance(X));

      for(UnsignedInteger k = 0; k < coll.getSize(); ++k)
        assert_almost_equal(Point(*coll[k].getImplementation()), Point(1, 0.0), 1.5e-2, 1.5e-2);

      // Validation of marginal variance
      const Point marginalVariance(result.getConditionalMarginalVariance(X));
      assert_almost_equal(marginalVariance, Point(sampleSize), 1.5e-2, 1.5e-2);

    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
