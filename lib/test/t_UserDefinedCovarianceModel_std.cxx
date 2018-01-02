//                                               -*- C++ -*-
/**
 *  @brief The test file of class UserDefinedCovarianceModel
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include <iostream>
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;


int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /* Default constructor */
    UserDefinedCovarianceModel myDefautModel;
    fullprint << "myDefautModel = " << myDefautModel << std::endl;

    // Default dimension parameter to evaluate the model
    const UnsignedInteger dimension = 1;

    // Spatial dimension of the model
    UnsignedInteger spatialDimension = 1;
    /* Amplitude values */
    Point amplitude(dimension);

    /* Scale values */
    Point scale(spatialDimension, 1.0);
    /* Spatial correclation */
    CorrelationMatrix spatialCorrelation(dimension);
    for (UnsignedInteger index = 0 ; index < dimension; ++index)
    {
      // constant amplitude
      amplitude[index] = (index + 1.0) / dimension ;
      if (index > 0) spatialCorrelation(index, index - 1) = 1.0 / index;
    }

    /* Sample a CauchyModel */
    ExponentialModel referenceModel(scale, amplitude, spatialCorrelation);

    UnsignedInteger size = 20;
    RegularGrid timeGrid(0.0, 0.1, size);
    const UnsignedInteger collectionSize = size * (size + 1) / 2;
    UserDefinedCovarianceModel::CovarianceMatrixCollection covarianceCollection(collectionSize);
    UnsignedInteger k = 0;
    for (UnsignedInteger i = 0; i < timeGrid.getN(); ++i)
    {
      const Scalar t = timeGrid.getValue(i);
      for (UnsignedInteger j = 0; j <= i; ++j)
      {
        const Scalar s = timeGrid.getValue(j);
        covarianceCollection[k] = referenceModel(t, s);
        ++k;
      }
    }

    /* Create a UserDefinedCovarianceModel */
    UserDefinedCovarianceModel myModel(timeGrid, covarianceCollection);
    fullprint << "myModel= " << myModel << std::endl;

    /* Sample the UserDefinedCovarianceModel */
    for (UnsignedInteger i = 0; i < timeGrid.getN(); ++i)
    {
      const Scalar t = timeGrid.getValue(i);
      for (UnsignedInteger j = i; j < timeGrid.getN(); ++j)
      {
        const Scalar s = timeGrid.getValue(j);
        // We look for cov(s,t) ==> when adding to the collection, we compute cov(t,s)
        // Because of symmetry, we check the right index computation
        fullprint << "myModel = " << myModel(s, t)(0, 0) << ", referenceModel = " << referenceModel(s, t)(0 , 0) << std::endl;
      }
    }
    fullprint << "myModel.discretize()=" << myModel.discretize(timeGrid) << std::endl;
    // Test the drawing method as a nonstationary model, in the covariance range
    Graph graph = myModel.draw(0, 0, 0.0, 2.0, 21, false, false);
    fullprint << graph << std::endl;
    // Test the drawing method as a nonstationary model, in the correlation range
    graph = myModel.draw(0, 0, 0.0, 2.0, 21, false, true);
    fullprint << graph << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
