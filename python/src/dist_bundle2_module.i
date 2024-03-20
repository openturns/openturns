// SWIG file dist_module.i

%module(package="openturns", docstring="Probabilistic distributions.") dist_bundle2
#pragma SWIG nowarn=302,509
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTDistribution.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i
%include UncertaintyModelCopulaCollection.i
%import model_copula_module.i

/* Uncertainty/Distribution (except copulas) */
%include Histogram.i
%include HistogramFactory.i
%include Hypergeometric.i
%include BayesDistribution.i
%include InverseChiSquare.i
%include InverseGamma.i
%include InverseNormal.i
%include InverseNormalFactory.i
%include InverseWishart.i
%include KPermutationsDistribution.i
%include KernelMixture.i
%include Laplace.i
%include LaplaceFactory.i
%include LeastSquaresDistributionFactory.i
%include Logistic.i
%include LogisticFactory.i
%include LogNormal.i
%include LogNormalFactory.i
%include LogNormalMuErrorFactor.i
%include LogNormalMuSigma.i
%include LogNormalMuSigmaOverMu.i
%include LogUniform.i
%include LogUniformFactory.i
%include MarginalDistribution.i
%include MaximumDistribution.i
%include MaximumEntropyOrderStatisticsDistribution.i
%include MaximumEntropyOrderStatisticsCopula.i
%include MaximumLikelihoodFactory.i
%include MeixnerDistribution.i
%include MeixnerDistributionFactory.i
%include MethodOfMomentsFactory.i
%include Mixture.i
%include MixedHistogramUserDefined.i
%include TruncatedDistribution.i
/* Must be after TruncatedDistribution */
%include KernelSmoothing.i
/* Must be after Mixture */
%include ConditionalDistribution.i
%include Multinomial.i
%include MultinomialFactory.i
%include NegativeBinomial.i
%include NegativeBinomialFactory.i
%include NonCentralChiSquare.i
%include NonCentralStudent.i
%include Normal.i
%include NormalFactory.i
%include NormalGamma.i

/* At last we include template definitions */
%include UncertaintyDistTemplateDefs.i
