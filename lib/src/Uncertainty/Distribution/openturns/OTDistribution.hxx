//                                               -*- C++ -*-
/**
 *  @brief The external header file of OpenTURNS for subdir Distribution
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

#ifndef OPENTURNS_OTDISTRIBUTION_HXX
#define OPENTURNS_OTDISTRIBUTION_HXX

#include "openturns/AliMikhailHaqCopula.hxx"
#include "openturns/AliMikhailHaqCopulaFactory.hxx"
#include "openturns/Arcsine.hxx"
#include "openturns/ArcsineFactory.hxx"
#include "openturns/ArcsineMuSigma.hxx"
#include "openturns/Bernoulli.hxx"
#include "openturns/BernoulliFactory.hxx"
#include "openturns/EmpiricalBernsteinCopula.hxx"
#include "openturns/BernsteinCopulaFactory.hxx"
#include "openturns/Beta.hxx"
#include "openturns/BetaFactory.hxx"
#include "openturns/BetaMuSigma.hxx"
#include "openturns/Binomial.hxx"
#include "openturns/BinomialFactory.hxx"
#include "openturns/BlockIndependentDistribution.hxx"
#include "openturns/Burr.hxx"
#include "openturns/BurrFactory.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/ChiSquareFactory.hxx"
#include "openturns/Chi.hxx"
#include "openturns/ChiFactory.hxx"
#include "openturns/ClaytonCopula.hxx"
#include "openturns/ClaytonCopulaFactory.hxx"
#include "openturns/ComposedCopula.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/CompositeDistribution.hxx"
#include "openturns/ConditionalDistribution.hxx"
#include "openturns/CumulativeDistributionNetwork.hxx"
#include "openturns/BayesDistribution.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/DiracFactory.hxx"
#include "openturns/Dirichlet.hxx"
#include "openturns/DirichletFactory.hxx"
#include "openturns/DiscreteCompoundDistribution.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Epanechnikov.hxx"
#include "openturns/Exponential.hxx"
#include "openturns/ExponentialFactory.hxx"
#include "openturns/ExtremeValueCopula.hxx"
#include "openturns/JoeCopula.hxx"
#include "openturns/FarlieGumbelMorgensternCopula.hxx"
#include "openturns/FarlieGumbelMorgensternCopulaFactory.hxx"
#include "openturns/FrankCopula.hxx"
#include "openturns/FrankCopulaFactory.hxx"
#include "openturns/FisherSnedecor.hxx"
#include "openturns/FisherSnedecorFactory.hxx"
#include "openturns/Frechet.hxx"
#include "openturns/FrechetFactory.hxx"
#include "openturns/Gamma.hxx"
#include "openturns/GammaFactory.hxx"
#include "openturns/GammaMuSigma.hxx"
#include "openturns/GeneralizedExtremeValue.hxx"
#include "openturns/GeneralizedExtremeValueFactory.hxx"
#include "openturns/GeneralizedPareto.hxx"
#include "openturns/GeneralizedParetoFactory.hxx"
#include "openturns/Geometric.hxx"
#include "openturns/GeometricFactory.hxx"
#include "openturns/Gumbel.hxx"
#include "openturns/GumbelCopula.hxx"
#include "openturns/GumbelCopulaFactory.hxx"
#include "openturns/GumbelFactory.hxx"
#include "openturns/GumbelLambdaGamma.hxx"
#include "openturns/GumbelMuSigma.hxx"
#include "openturns/Histogram.hxx"
#include "openturns/HistogramFactory.hxx"
#include "openturns/Hypergeometric.hxx"
#include "openturns/IndependentCopula.hxx"
#include "openturns/InverseChiSquare.hxx"
#include "openturns/InverseGamma.hxx"
#include "openturns/InverseNormal.hxx"
#include "openturns/InverseNormalFactory.hxx"
#include "openturns/InverseWishart.hxx"
#include "openturns/KPermutationsDistribution.hxx"
#include "openturns/KernelMixture.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/Laplace.hxx"
#include "openturns/LaplaceFactory.hxx"
#include "openturns/LeastSquaresDistributionFactory.hxx"
#include "openturns/Logistic.hxx"
#include "openturns/LogisticFactory.hxx"
#include "openturns/LogNormal.hxx"
#include "openturns/LogNormalFactory.hxx"
#include "openturns/LogNormalMuErrorFactor.hxx"
#include "openturns/LogNormalMuSigma.hxx"
#include "openturns/LogNormalMuSigmaOverMu.hxx"
#include "openturns/LogUniform.hxx"
#include "openturns/LogUniformFactory.hxx"
#include "openturns/OrderStatisticsMarginalChecker.hxx"
#include "openturns/OrdinalSumCopula.hxx"
#include "openturns/MarshallOlkinCopula.hxx"
#include "openturns/MarginalDistribution.hxx"
#include "openturns/MaximumDistribution.hxx"
#include "openturns/MaximumEntropyOrderStatisticsDistribution.hxx"
#include "openturns/MaximumEntropyOrderStatisticsCopula.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/MeixnerDistribution.hxx"
#include "openturns/MeixnerDistributionFactory.hxx"
#include "openturns/MethodOfMomentsFactory.hxx"
#include "openturns/MinCopula.hxx"
#include "openturns/MixedHistogramUserDefined.hxx"
#include "openturns/Mixture.hxx"
#include "openturns/Multinomial.hxx"
#include "openturns/MultinomialFactory.hxx"
#include "openturns/NegativeBinomial.hxx"
#include "openturns/NegativeBinomialFactory.hxx"
#include "openturns/NonCentralChiSquare.hxx"
#include "openturns/NonCentralStudent.hxx"
#include "openturns/Normal2DCDF.hxx"
#include "openturns/Normal3DCDF.hxx"
#include "openturns/Normal.hxx"
#include "openturns/NormalCopula.hxx"
#include "openturns/NormalCopulaFactory.hxx"
#include "openturns/NormalFactory.hxx"
#include "openturns/NormalGamma.hxx"
#include "openturns/ParametrizedDistribution.hxx"
#include "openturns/Pareto.hxx"
#include "openturns/ParetoFactory.hxx"
#include "openturns/PlackettCopula.hxx"
#include "openturns/PlackettCopulaFactory.hxx"
#include "openturns/Poisson.hxx"
#include "openturns/PoissonFactory.hxx"
#include "openturns/PosteriorDistribution.hxx"
#include "openturns/ProductDistribution.hxx"
#include "openturns/RandomMixture.hxx"
#include "openturns/RatioDistribution.hxx"
#include "openturns/Rayleigh.hxx"
#include "openturns/RayleighFactory.hxx"
#include "openturns/Rice.hxx"
#include "openturns/RiceFactory.hxx"
#include "openturns/Skellam.hxx"
#include "openturns/SkellamFactory.hxx"
#include "openturns/SmoothedUniform.hxx"
#include "openturns/SquaredNormal.hxx"
#include "openturns/Student.hxx"
#include "openturns/StudentFunctions.hxx"
#include "openturns/StudentFactory.hxx"
#include "openturns/Trapezoidal.hxx"
#include "openturns/TrapezoidalFactory.hxx"
#include "openturns/Triangular.hxx"
#include "openturns/TriangularFactory.hxx"
#include "openturns/TruncatedDistribution.hxx"
#include "openturns/TruncatedNormal.hxx"
#include "openturns/TruncatedNormalFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/UniformFactory.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/UserDefinedFactory.hxx"
#include "openturns/VonMises.hxx"
#include "openturns/VonMisesFactory.hxx"
#include "openturns/WeibullMax.hxx"
#include "openturns/WeibullMaxFactory.hxx"
#include "openturns/WeibullMaxMuSigma.hxx"
#include "openturns/WeibullMin.hxx"
#include "openturns/WeibullMinFactory.hxx"
#include "openturns/WeibullMinMuSigma.hxx"
#include "openturns/Wishart.hxx"
#include "openturns/ZipfMandelbrot.hxx"

#endif /* OPENTURNS_OTDISTRIBUTION_HXX */
