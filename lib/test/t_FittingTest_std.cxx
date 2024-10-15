//                                               -*- C++ -*-
/**
 *  @brief The test file of class Test
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  Collection<Distribution> continuousDistributionCollection;
  Collection<Distribution> discreteDistributionCollection;
  Collection<Distribution> distributionCollection;

  Beta beta(2.0, 1.0, 0.0, 1.0);
  distributionCollection.add(beta);
  continuousDistributionCollection.add(beta);

  Gamma gamma(1.0, 2.0, 3.0);
  distributionCollection.add(gamma);
  continuousDistributionCollection.add(gamma);

  Gumbel gumbel(1.0, 2.0);
  distributionCollection.add(gumbel);
  continuousDistributionCollection.add(gumbel);

  LogNormal lognormal(1.0, 1.0, 2.0);
  distributionCollection.add(lognormal);
  continuousDistributionCollection.add(lognormal);

  Logistic logistic(1.0, 1.0);
  distributionCollection.add(logistic);
  continuousDistributionCollection.add(logistic);

  Normal normal(1.0, 2.0);
  distributionCollection.add(normal);
  continuousDistributionCollection.add(normal);

  TruncatedNormal truncatednormal(1.0, 1.0, 0.0, 3.0);
  distributionCollection.add(truncatednormal);
  continuousDistributionCollection.add(truncatednormal);

  Student student(10.0, 10.0, 1.0);
  distributionCollection.add(student);
  continuousDistributionCollection.add(student);

  Triangular triangular(-1.0, 2.0, 4.0);
  distributionCollection.add(triangular);
  continuousDistributionCollection.add(triangular);

  Uniform uniform(1.0, 2.0);
  distributionCollection.add(uniform);
  continuousDistributionCollection.add(uniform);

  WeibullMin weibull(1.0, 1.0, 2.0);
  distributionCollection.add(weibull);
  continuousDistributionCollection.add(weibull);

  Geometric geometric(0.5);
  distributionCollection.add(geometric);
  discreteDistributionCollection.add(geometric);

  Binomial binomial(10, 0.25);
  distributionCollection.add(binomial);
  discreteDistributionCollection.add(binomial);

  ZipfMandelbrot zipf(20, 5.25, 2.5);
  distributionCollection.add(zipf);
  discreteDistributionCollection.add(zipf);

  Poisson poisson(5.0);
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

  UnsignedInteger size = 100;

  UnsignedInteger continuousDistributionNumber = continuousDistributionCollection.getSize();
  UnsignedInteger discreteDistributionNumber = discreteDistributionCollection.getSize();
  UnsignedInteger distributionNumber = continuousDistributionNumber + discreteDistributionNumber;
  Collection<Sample> sampleCollection(distributionNumber);
  Collection<Sample> continuousSampleCollection(continuousDistributionNumber);
  Collection<Sample> discreteSampleCollection(discreteDistributionNumber);
  for (UnsignedInteger i = 0; i < continuousDistributionNumber; i++)
  {
    continuousSampleCollection[i] = continuousDistributionCollection[i].getSample(size);
    sampleCollection[i] = continuousSampleCollection[i];
  }
  for (UnsignedInteger i = 0; i < discreteDistributionNumber; i++)
  {
    discreteSampleCollection[i] = discreteDistributionCollection[i].getSample(size);
    sampleCollection[continuousDistributionNumber + i] = discreteSampleCollection[i];
  }
  FittingTest::DistributionFactoryCollection factoryCollection;
  factoryCollection.add(UniformFactory());
  factoryCollection.add(BetaFactory());
  factoryCollection.add(NormalFactory());
  Sample aSample(Uniform(-1.5, 2.5).getSample(size));
  TestResult bestResult;
  Scalar bestBIC = -1.0;
  Scalar bestAIC = -1.0;
  Scalar bestAICc = -1.0;
  fullprint << "best model BIC=" << FittingTest::BestModelBIC(aSample, factoryCollection, bestBIC) << std::endl;
  fullprint << "best model Kolmogorov=" << FittingTest::BestModelLilliefors(aSample, factoryCollection, bestResult) << std::endl;
  fullprint << "best model AIC=" << FittingTest::BestModelAIC(aSample, factoryCollection, bestAIC) << std::endl;
  fullprint << "best model AICC=" << FittingTest::BestModelAICC(aSample, factoryCollection, bestAICc) << std::endl;

  SquareMatrix resultBIC(distributionNumber);
  for (UnsignedInteger i = 0; i < distributionNumber; i++)
  {
    for (UnsignedInteger j = 0; j < distributionNumber; j++)
    {
      const Scalar value = FittingTest::BIC(sampleCollection[i], distributionCollection[j], 0);
      resultBIC(i, j) = (std::abs(value) < 1.0e-6 ? 0.0 : value);
    }
  }
  fullprint << "resultBIC=" << resultBIC << std::endl;
  SquareMatrix resultKolmogorov(continuousDistributionNumber);
  for (UnsignedInteger i = 0; i < continuousDistributionNumber; i++)
  {
    for (UnsignedInteger j = 0; j < continuousDistributionNumber; j++)
    {
      const Scalar value = FittingTest::Kolmogorov(continuousSampleCollection[i], continuousDistributionCollection[j], 0.05).getPValue();
      resultKolmogorov(i, j) = (std::abs(value) < 1.0e-6 ? 0.0 : value);
    }
  }
  fullprint << "resultKolmogorov=" << resultKolmogorov << std::endl;
  SquareMatrix resultChiSquared(discreteDistributionNumber);
  for (UnsignedInteger i = 0; i < discreteDistributionNumber; i++)
  {
    for (UnsignedInteger j = 0; j < discreteDistributionNumber; j++)
    {
      try
      {
        const Scalar value = FittingTest::ChiSquared(discreteSampleCollection[i], discreteDistributionCollection[j], 0.05, 0).getPValue();
        resultChiSquared(i, j) = (std::abs(value) < 1.0e-6 ? 0.0 : value);
      }
      catch (...)
      {
        fullprint << "Sample=" << discreteSampleCollection[i] << " is not compatible with distribution=" << discreteDistributionCollection[j] << std::endl;
      }
    }
  }
  fullprint << "resultChiSquared=" << resultChiSquared << std::endl;
  // Example taken from the R documentation of chisq.test
  Sample s(89, Point(1, 0.0));
  s.add(Sample(37, Point(1, 1.0)));
  s.add(Sample(30, Point(1, 2.0)));
  s.add(Sample(28, Point(1, 3.0)));
  s.add(Sample(2, Point(1, 4.0)));
  Sample support(0, 1);
  support.add(Point(1, 0.0));
  support.add(Point(1, 1.0));
  support.add(Point(1, 2.0));
  support.add(Point(1, 3.0));
  support.add(Point(1, 4.0));
  Point probabilities(0);
  probabilities.add(0.4);
  probabilities.add(0.2);
  probabilities.add(0.2);
  probabilities.add(0.15);
  probabilities.add(0.05);
  UserDefined d(support, probabilities);
  fullprint << "R example p-value=" << FittingTest::ChiSquared(s, d).getPValue() << std::endl;

  return ExitCode::Success;
}
