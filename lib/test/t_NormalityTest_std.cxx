//                                               -*- C++ -*-
/**
 *  @brief The test file of class Test
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

  Collection<Distribution> continuousDistributionCollection;
  Collection<Distribution> discreteDistributionCollection;
  Collection<Distribution> distributionCollection;

  Beta beta(2., 3., 0., 1.);
  distributionCollection.add(beta);
  continuousDistributionCollection.add(beta);

  Gamma gamma(1., 2., 3.);
  distributionCollection.add(gamma);
  continuousDistributionCollection.add(gamma);

  Gumbel gumbel(1., 2.);
  distributionCollection.add(gumbel);
  continuousDistributionCollection.add(gumbel);

  LogNormal lognormal(1., 1., 2.);
  distributionCollection.add(lognormal);
  continuousDistributionCollection.add(lognormal);

  Logistic logistic(1., 1.);
  distributionCollection.add(logistic);
  continuousDistributionCollection.add(logistic);

  Normal normal(1., 2.);
  distributionCollection.add(normal);
  continuousDistributionCollection.add(normal);

  TruncatedNormal truncatednormal(1., 1., 0., 3.);
  distributionCollection.add(truncatednormal);
  continuousDistributionCollection.add(truncatednormal);

  Student student(4., 10.);
  distributionCollection.add(student);
  continuousDistributionCollection.add(student);

  Triangular triangular(-1., 2., 4.);
  distributionCollection.add(triangular);
  continuousDistributionCollection.add(triangular);

  Uniform uniform(1., 2.);
  distributionCollection.add(uniform);
  continuousDistributionCollection.add(uniform);

  WeibullMin weibull(1., 1., 2.);
  distributionCollection.add(weibull);
  continuousDistributionCollection.add(weibull);

  Geometric geometric(.5);
  distributionCollection.add(geometric);
  discreteDistributionCollection.add(geometric);

  Poisson poisson(2.);
  distributionCollection.add(poisson);
  discreteDistributionCollection.add(poisson);

  Sample x(3, 1);
  x[0][0] = 1.0;
  x[1][0] = 2.0;
  x[2][0] = 3.0;
  Point p(3);
  p[0] = 0.3;
  p[1] = 0.2;
  p[2] = 0.5;
  UserDefined userdefined(x, p);
  distributionCollection.add(userdefined);
  discreteDistributionCollection.add(userdefined);

  UnsignedInteger size = 10000;

  UnsignedInteger continuousDistributionNumber = continuousDistributionCollection.getSize();
  UnsignedInteger discreteDistributionNumber = discreteDistributionCollection.getSize();
  UnsignedInteger distributionNumber = continuousDistributionNumber + discreteDistributionNumber;
  Collection<Sample> sampleCollection(distributionNumber);
  Collection<Sample> continuousSampleCollection(continuousDistributionNumber);
  Collection<Sample> discreteSampleCollection(discreteDistributionNumber);
  for (UnsignedInteger i = 0; i < continuousDistributionNumber; i++)
  {
    continuousSampleCollection[i] = continuousDistributionCollection[i].getSample(size);
    continuousSampleCollection[i].setName(continuousDistributionCollection[i].getName());
    sampleCollection[i] = continuousSampleCollection[i];
  }
  for (UnsignedInteger i = 0; i < discreteDistributionNumber; i++)
  {
    discreteSampleCollection[i] = discreteDistributionCollection[i].getSample(size);
    discreteSampleCollection[i].setName(discreteDistributionCollection[i].getName());
    sampleCollection[continuousDistributionNumber + i] = discreteSampleCollection[i];
  }
  // Test the normality of several samples using the Anderson Darling test
  Point andersonDarlingResult(distributionNumber);
  for (UnsignedInteger i = 0; i < distributionNumber; i++)
  {
    TestResult result(NormalityTest::AndersonDarlingNormal(sampleCollection[i]));
    andersonDarlingResult[i] = result.getBinaryQualityMeasure();
    fullprint << "sample " << sampleCollection[i].getName() << " result=" << andersonDarlingResult[i] << std::endl;
  }
  fullprint << "andersonDarlingResult=" << andersonDarlingResult << std::endl;
  // Test the normality of several samples using the Cramer Von Mises test
  Point cramerVonMisesResult(distributionNumber);
  for (UnsignedInteger i = 0; i < distributionNumber; i++)
  {
    TestResult result(NormalityTest::CramerVonMisesNormal(sampleCollection[i]));
    cramerVonMisesResult[i] = result.getBinaryQualityMeasure();
    fullprint << "sample " << sampleCollection[i].getName() << " result=" << cramerVonMisesResult[i] << std::endl;
  }
  fullprint << "cramerVonMisesResult=" << cramerVonMisesResult << std::endl;
  return ExitCode::Success;
}
