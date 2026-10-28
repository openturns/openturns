//                                               -*- C++ -*-
/**
 *  @brief The test file of class Dirichlet for standard methods
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
  RandomGenerator::SetSeed(0);

  try
  {
    for (UnsignedInteger dim = 1; dim <= 2; ++dim)
    {
      Point theta(dim + 1);
      Scalar sumTheta = 0.0;
      for (UnsignedInteger i = 0; i <= dim; ++i)
      {
        theta[i] = 1.0 + (i + 1.0) / 4.0;
        sumTheta += theta[i];
      }
      Dirichlet distribution(theta);
      assert_equal(distribution.getDimension(), dim);
      assert_equal(distribution.isContinuous(), true);
      assert_equal(distribution.isElliptical(), false);

      Description description(dim);
      for (UnsignedInteger j = 1; j <= dim; ++j)
      {
        OSS oss;
        oss << "Marginal " << j;
        description[j - 1] = oss;
      }
      distribution.setDescription(description);

      // Moments and covariance
      const Point mean = distribution.getMean();
      const Point standardDeviation = distribution.getStandardDeviation();
      const Point skewness = distribution.getSkewness();
      const Point kurtosis = distribution.getKurtosis();
      const CovarianceMatrix covariance = distribution.getCovariance();
      for (UnsignedInteger i = 0; i < dim; ++i)
      {
        assert_almost_equal(mean[i], theta[i] / sumTheta, 1e-12, 0.0);
        assert_almost_equal(covariance(i, i), standardDeviation[i] * standardDeviation[i], 1e-12, 0.0);
        assert_almost_equal(covariance(i, i), theta[i] * (sumTheta - theta[i]) / (sumTheta * sumTheta * (sumTheta + 1.0)), 1e-12, 0.0);
        if (!std::isfinite(skewness[i]))
          throw TestFailed("The skewness must be finite");
        if (!std::isfinite(kurtosis[i]))
          throw TestFailed("The kurtosis must be finite");
      } // i
      if (!std::isfinite(distribution.computeEntropy()))
        throw TestFailed("The entropy must be finite");

      // Realization and sample: only the first dim coordinates are returned,
      // their sum is 1 - X_{dim+1}, hence strictly between 0 and 1
      const Point oneRealization = distribution.getRealization();
      for (UnsignedInteger i = 0; i < dim; ++i)
        if (!(oneRealization[i] > 0.0))
          throw TestFailed("The realization must have positive coordinates");
      if (!((oneRealization.norm1() > 0.0) && (oneRealization.norm1() <= 1.0)))
        throw TestFailed("The realization must lie inside the simplex");
      const UnsignedInteger size = 10000;
      const Sample oneSample(distribution.getSample(size));
      assert_equal(oneSample.getSize(), size);
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        const Scalar sum = Point(oneSample[i]).norm1();
        if (!((sum > 0.0) && (sum <= 1.0)))
          throw TestFailed("The sample must lie inside the simplex");
        for (UnsignedInteger j = 0; j < dim; ++j)
          if (!(oneSample(i, j) > 0.0))
            throw TestFailed("The sample must have positive coordinates");
      } // i
      assert_almost_equal(oneSample.computeMean(), mean, 0.05, 0.0);

      // PDF, log-PDF and CDF at a point inside the simplex
      const Point point(dim, 0.5 / dim);
      const Scalar logPDF = distribution.computeLogPDF(point);
      const Scalar PDF = distribution.computePDF(point);
      const Scalar CDF = distribution.computeCDF(point);
      if (!std::isfinite(logPDF))
        throw TestFailed("The log-PDF must be finite");
      if (!(PDF > 0.0))
        throw TestFailed("The PDF must be positive");
      assert_almost_equal(logPDF, std::log(PDF), 1e-12, 0.0);
      if (!((CDF >= 0.0) && (CDF <= 1.0)))
        throw TestFailed("The CDF must be a probability");
      const Point quantile = distribution.computeQuantile(0.95);
      assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 0.0);

      // One-dimensional Dirichlet is a Beta distribution
      if (dim == 1)
      {
        const Beta beta(theta[0], theta[1], 0.0, 1.0);
        assert_almost_equal(PDF, beta.computePDF(point[0]), 1e-12, 0.0);
        assert_almost_equal(logPDF, beta.computeLogPDF(point[0]), 1e-12, 0.0);
        assert_almost_equal(CDF, beta.computeCDF(point[0]), 1e-12, 0.0);
      } // dim == 1

      // Parameters collection round trip
      const Dirichlet::PointWithDescriptionCollection parametersCollection(distribution.getParametersCollection());
      Dirichlet tmp(Point(dim + 1, 1.0));
      tmp.setParametersCollection(parametersCollection);
      assert_almost_equal(tmp.getParameter(), distribution.getParameter(), 1e-12, 0.0);

      // Conditional and sequential conditional functions
      const Scalar x = 0.6;
      const Point y(dim - 1, 0.2);
      if (!(distribution.computeConditionalPDF(x, y) > 0.0))
        throw TestFailed("The conditional PDF must be positive");
      const Scalar conditionalCDF = distribution.computeConditionalCDF(x, y);
      if (!((conditionalCDF >= 0.0) && (conditionalCDF <= 1.0)))
        throw TestFailed("The conditional CDF must be a probability");
      const Scalar conditionalQuantile = distribution.computeConditionalQuantile(x, y);
      if (!((conditionalQuantile >= 0.0) && (conditionalQuantile <= 1.0)))
        throw TestFailed("The conditional quantile must be a probability");
      Point pt(dim);
      for (UnsignedInteger i = 0; i < dim; ++i)
        pt[i] = 0.1 * i + 0.05;
      const Point sequentialPDF = distribution.computeSequentialConditionalPDF(pt);
      for (UnsignedInteger i = 0; i < dim; ++i)
        if (!(sequentialPDF[i] > 0.0))
          throw TestFailed("The sequential conditional PDF must be positive");
      const Point sequentialCDF = distribution.computeSequentialConditionalCDF(pt);
      for (UnsignedInteger i = 0; i < dim; ++i)
        if (!((sequentialCDF[i] >= 0.0) && (sequentialCDF[i] <= 1.0)))
          throw TestFailed("The sequential conditional CDF must be a probability");
      const Point sequentialQuantile = distribution.computeSequentialConditionalQuantile(sequentialCDF);
      assert_almost_equal(sequentialQuantile, pt, 1e-6, 0.0);

      // Marginals
      for (UnsignedInteger i = 0; i < dim; ++i)
      {
        const Distribution margin(distribution.getMarginal(i));
        assert_equal(margin.getDimension(), UnsignedInteger(1));
        if (!(margin.computePDF(Point(1, 0.5)) > 0.0))
          throw TestFailed("The marginal PDF must be positive");
        const Scalar marginCDF = margin.computeCDF(Point(1, 0.5));
        if (!((marginCDF >= 0.0) && (marginCDF <= 1.0)))
          throw TestFailed("The marginal CDF must be a probability");
        const Point marginQuantile = margin.computeQuantile(0.95);
        if (!((marginQuantile[0] >= 0.0) && (marginQuantile[0] <= 1.0)))
          throw TestFailed("The marginal quantile must be a probability");
        const Point marginRealization = margin.getRealization();
        if (!((marginRealization[0] >= 0.0) && (marginRealization[0] <= 1.0)))
          throw TestFailed("The marginal realization must be a probability");
      } // i
      if (dim >= 2)
      {
        Indices indices(2, 0);
        indices[0] = 1;
        indices[1] = 0;
        const Distribution margins(distribution.getMarginal(indices));
        assert_equal(margins.getDimension(), UnsignedInteger(2));
        if (!(margins.computePDF(Point(2, 0.25)) > 0.0))
          throw TestFailed("The bivariate marginal PDF must be positive");
        const Scalar marginsCDF = margins.computeCDF(Point(2, 0.25));
        if (!((marginsCDF >= 0.0) && (marginsCDF <= 1.0)))
          throw TestFailed("The bivariate marginal CDF must be a probability");
        const Point marginsQuantile = margins.computeQuantile(0.95);
        assert_almost_equal(margins.computeCDF(marginsQuantile), 0.95, 1e-6, 0.0);
        const Point marginsRealization = margins.getRealization();
        for (UnsignedInteger i = 0; i < 2; ++i)
          if (!((marginsRealization[i] >= 0.0) && (marginsRealization[i] <= 1.0)))
            throw TestFailed("The bivariate marginal realization must be a probability");
      } // dim >= 2
    } // dim
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
