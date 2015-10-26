//                                               -*- C++ -*-
/**
 *  @brief The test file of KrigingRandomVector class
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

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    // Kriging use case
    UnsignedInteger spatialDimension(1);
    UnsignedInteger sampleSize(201);

    const NumericalScalar xMax(10.0);
    const NumericalScalar xMin(0.0);
    const NumericalScalar dx = (xMax - xMin) / (sampleSize - 1.0);

    // Learning data
    NumericalSample inputSample(sampleSize, spatialDimension);
    // Validation data
    NumericalSample inputValidSample(sampleSize, spatialDimension);
    for (UnsignedInteger k = 0; k < sampleSize; ++k)
    {
      inputSample[k][0] = k * dx;
      inputValidSample[k][0] = 0.5 * (2.0 * k + 1.0) * dx;
    }

    // Set the functions
    NumericalMathFunction f(Description(1, "x0"), Description(1, "y"), Description(1, "x0 * sin(x0)"));
    NumericalMathFunction g(Description(1, "x0"), Description(1, "y"), Description(1, "3 + cos(x0*x0)"));
    NumericalMathFunction h(Description(1, "x0"), Description(1, "y"), Description(1, "cos(0.5*x0*x0) + sin(x0)"));
    // The final function
    Collection<NumericalMathFunction> modelColl(0);
    // add functions
    modelColl.add(f);
    modelColl.add(g);
    modelColl.add(h);
    // finally, the model
    NumericalMathFunction model(modelColl);

    // Evaluation of the model on input samples
    NumericalSample outputSample(model(inputSample));
    NumericalSample outputValidSample( model(inputValidSample));

    // 2) Definition of exponential model
    // The parameters have been calibrated using TNC optimization
    // and AbsoluteExponential models
    NumericalPoint amplitude(3, 1.0);
    NumericalPoint scale(spatialDimension, 2.0);

    // The covariance model
    ExponentialModel expCovModel(spatialDimension, amplitude, scale);

    // 3) Basis definition
    Collection<Basis> basisCollection;
    basisCollection.add(ConstantBasisFactory(spatialDimension).build());
    basisCollection.add(LinearBasisFactory(spatialDimension).build());
    basisCollection.add(Basis());

    // Kriging algorithm
    KrigingAlgorithm algo(inputSample, outputSample, basisCollection, expCovModel);
    algo.run();
    KrigingResult result(algo.getResult());
    //print("KrigingResult=", repr(result))
    // Get meta model
    NumericalMathFunction metaModel(result.getMetaModel());
    // Evaluation of kriging model on data
    NumericalSample outData(metaModel(inputValidSample));

    // 4) Errors
    // Interpolation
    assert_almost_equal(outputSample,  metaModel(inputSample));
    // Estimation
    // rtol & a tol fixed to 1e-1
    assert_almost_equal(outputValidSample,  metaModel(inputValidSample), 1.e-1, 1e-1);

    // 5) Kriging variance
    // It should be 0 on learning points
    CovarianceMatrix covMatrix( result.getConditionalCovariance(inputSample));

    //# assert_almost_equal could not be applied to matrices
    //# application to NumericalPoint
    NumericalPoint covariancePoint(covMatrix.getImplementation());
    // Total size
    UnsignedInteger covSize(  expCovModel.getDimension() * sampleSize );
    // Build the theoretical covariance
    NumericalPoint theoricalVariance(covSize * covSize);
    assert_almost_equal(covariancePoint, theoricalVariance, 1e-12, 1e-12);


    // Random vector evaluation
    KrigingRandomVector rvector(result, inputValidSample[0]);
    std::cout << "KRV=" << rvector << std::endl;

    NumericalPoint realization(rvector.getRealization());

    std::cout << "Realization of the KRV=" << realization << std::endl;

    // Get a sample of size 10
    NumericalSample realizations(rvector.getSample(10));
    std::cout << "Sample of realizations of the KRV=" << realizations << std::endl;
  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
