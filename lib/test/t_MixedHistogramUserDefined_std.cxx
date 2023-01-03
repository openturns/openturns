//                                               -*- C++ -*-
/**
 *  @brief The test file of class MixedHistogramUserDefined for standard methods
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

using namespace OT;

int main(int, char *[])
{
  Collection<Point> ticksCollection(0);
  {
    Point point;
    point.add(0.0);
    point.add(1.0);
    point.add(3.0);
    point.add(6.0);
    ticksCollection.add(point);
  }
  {
    Point point;
    point.add(1.0);
    point.add(2.0);
    point.add(3.0);
    ticksCollection.add(point);
  }
  Indices kind(0);
  kind.add(MixedHistogramUserDefined::CONTINUOUS);
  kind.add(MixedHistogramUserDefined::DISCRETE);
  Point probabilityTable(0);
  probabilityTable.add(0.05);
  probabilityTable.add(0.1);
  probabilityTable.add(0.15);
  probabilityTable.add(0.2);
  probabilityTable.add(0.25);
  probabilityTable.add(0.3);
  probabilityTable.add(0.35);
  probabilityTable.add(0.4);
  probabilityTable.add(0.45);
  // Instantiate one distribution object
  MixedHistogramUserDefined distribution(ticksCollection, kind, probabilityTable);
  std::cout << "Distribution " << distribution.__repr__() << std::endl;
  std::cout << "Distribution " << distribution << std::endl;

  Point mean = distribution.getMean();
  std::cout << "mean=" << mean << std::endl;

  CovarianceMatrix covariance = distribution.getCovariance();
  std::cout << "covariance=" << covariance << std::endl;

  // Is this distribution elliptical ?
  std::cout << "Elliptical = " << (distribution.isElliptical() ? "true" : "false") << std::endl;

  // Is this distribution continuous ?
  std::cout << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

  // Test for realization of distribution
  Point oneRealization = distribution.getRealization();
  std::cout << "oneRealization=" << oneRealization << std::endl;

  // Test for sampling
  UnsignedInteger size = 10000;
  Sample oneSample = distribution.getSample( size );
  std::cout << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;

  std::cout << "mean=" << oneSample.computeMean() << std::endl;
  std::cout << "covariance=" << oneSample.computeCovariance() << std::endl;

  // Define a point
  Point point( distribution.getDimension(), 1.0 );
  std::cout << "Point= " << point << std::endl;

  // Show PDF and CDF of point
  Scalar LPDF = distribution.computeLogPDF( point );
  std::cout << "log pdf=" << LPDF << std::endl;
  Scalar PDF = distribution.computePDF( point );
  std::cout << "pdf     =" << PDF << std::endl;
  Scalar CDF = distribution.computeCDF( point );
  std::cout << "cdf=" << CDF << std::endl;
  Scalar CCDF = distribution.computeComplementaryCDF( point );
  std::cout << "ccdf=" << CCDF << std::endl;
  Point quantile = distribution.computeQuantile( 0.95 );
  std::cout << "quantile=" << quantile << std::endl;
  std::cout << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
  Point standardDeviation = distribution.getStandardDeviation();
  std::cout << "standard deviation=" << standardDeviation << std::endl;
  Point skewness = distribution.getSkewness();
  std::cout << "skewness=" << skewness << std::endl;
  Point kurtosis = distribution.getKurtosis();
  std::cout << "kurtosis=" << kurtosis << std::endl;
  std::cout << "Standard representative=" << distribution.getStandardRepresentative() << std::endl;
  return 0;
}
