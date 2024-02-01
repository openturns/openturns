//                                               -*- C++ -*-
/**
 *  @brief The test file of class IsoProbabilisticTransformation for standard methods
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

typedef Distribution::IsoProbabilisticTransformation IsoProbabilisticTransformation;
typedef Distribution::InverseIsoProbabilisticTransformation InverseIsoProbabilisticTransformation;

Point clean(Point in)
{
  UnsignedInteger dim = in.getDimension();
  for(UnsignedInteger i = 0; i < dim; i++)
    if (std::abs(in[i]) < 1.e-10) in[i] = 0.0;
  return in;
}

SymmetricTensor clean(SymmetricTensor in)
{
  UnsignedInteger rowDim = in.getNbRows();
  UnsignedInteger colDim = in.getNbColumns();
  UnsignedInteger sheetDim = in.getNbSheets();
  for(UnsignedInteger i = 0; i < rowDim; i++)
    for(UnsignedInteger j = 0; j < colDim; j++)
      for (UnsignedInteger k = 0; k < sheetDim; k++)
        if (std::abs(in(i, j, k)) < 1.e-6) in(i, j, k) = 0.0;
  return in;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Instantiate one distribution object
    UnsignedInteger dim = 3;
    Point meanPoint(dim, 1.0);
    meanPoint[0] = 0.5;
    meanPoint[1] = -0.5;
    Point sigma(dim, 1.0);
    sigma[0] = 2.0;
    sigma[1] = 3.0;
    CorrelationMatrix R(dim);
    for (UnsignedInteger i = 1; i < dim; i++)
    {
      R(i, i - 1) = 0.5;
    }

    Normal distribution(meanPoint, sigma, R);

    // Test for sampling
    UnsignedInteger size = 10000;
    Sample sample = distribution.getSample( size );
    fullprint << "sample first=" << sample[0] << " last=" << sample[size - 1] << std::endl;
    fullprint << "sample mean=" << sample.computeMean() << std::endl;
    fullprint << "sample covariance=" << sample.computeCovariance() << std::endl;

    IsoProbabilisticTransformation transform(distribution.getIsoProbabilisticTransformation());
    fullprint << "isoprobabilistic transformation=" << transform << std::endl;
    Sample transformedSample(transform(sample));
    fullprint << "transformed sample first=" << transformedSample[0] << " last=" << transformedSample[size - 1] << std::endl;
    fullprint << "transformed sample mean=" << transformedSample.computeMean() << std::endl;
    fullprint << "transformed sample covariance=" << transformedSample.computeCovariance() << std::endl;

    // Test for evaluation
    InverseIsoProbabilisticTransformation inverseTransform(distribution.getInverseIsoProbabilisticTransformation());
    fullprint << "inverse isoprobabilistic transformation=" << inverseTransform << std::endl;
    Sample transformedBackSample(inverseTransform(transformedSample));
    fullprint << "transformed back sample first=" << transformedBackSample[0] << " last=" << transformedBackSample[size - 1] << std::endl;
    fullprint << "transformed back sample mean=" << transformedBackSample.computeMean() << std::endl;
    fullprint << "transformed back sample covariance=" << transformedBackSample.computeCovariance() << std::endl;
    Point point(dim, 1.0);
    fullprint << "point=" << point << std::endl;
    Point transformedPoint(transform(point));
    fullprint << "transform value at point        =" << transformedPoint << std::endl;
    fullprint << "transform gradient at point     =" << transform.gradient(point).clean(1.0e-6) << std::endl;
    fullprint << "transform gradient at point (FD)=" << CenteredFiniteDifferenceGradient(1.0e-5, transform.getEvaluation()).gradient(point).clean(1.0e-6) << std::endl;
    fullprint << "transform hessian at point      =" << clean(transform.hessian(point)) << std::endl;
    fullprint << "transform hessian at point (FD) =" << clean(CenteredFiniteDifferenceHessian(1.0e-4, transform.getEvaluation()).hessian(point)) << std::endl;
    fullprint << "inverse transform value at transformed point        =" << inverseTransform(transformedPoint) << std::endl;
    fullprint << "inverse transform gradient at transformed point (FD)=" << inverseTransform.gradient(transformedPoint).clean(1.0e-6) << std::endl;
    fullprint << "inverse transform gradient at transformed point     =" << CenteredFiniteDifferenceGradient(1.0e-5, inverseTransform.getEvaluation()).gradient(transformedPoint).clean(1.0e-6) << std::endl;
    fullprint << "inverse transform hessian at transformed point      =" << clean(inverseTransform.hessian(transformedPoint)) << std::endl;
    fullprint << "inverse transform hessian at transformed point (FD) =" << clean(CenteredFiniteDifferenceHessian(1.0e-4, inverseTransform.getEvaluation()).hessian(transformedPoint)) << std::endl;

    // Test for parameters
    fullprint << "parameters gradient at point=" << transform.parameterGradient(point).clean(1.0e-6) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
