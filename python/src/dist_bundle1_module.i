// SWIG file dist_module.i

%module(package="openturns", docstring="Probabilistic distributions.") dist_bundle1
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
%import model_copula_module.i

/* Uncertainty/Distribution (except copulas) */
%include DistFunc.i
%include Arcsine.i
%include ArcsineFactory.i
%include ArcsineMuSigma.i
%include Bernoulli.i
%include BernoulliFactory.i
%include BernsteinCopulaFactory.i
%include Beta.i
%include BetaFactory.i
%include BetaMuSigma.i
%include Binomial.i
%include BinomialFactory.i
%include Burr.i
%include BurrFactory.i
%include Chi.i
%include ChiFactory.i
%include ChiSquare.i
%include ChiSquareFactory.i
%include CompositeDistribution.i
%include Dirac.i
%include DiracFactory.i
%include Dirichlet.i
%include DirichletFactory.i
%include Epanechnikov.i
%include Exponential.i
%include ExponentialFactory.i
%include FisherSnedecor.i
%include FisherSnedecorFactory.i
%include Frechet.i
%include Gamma.i
%include GammaFactory.i
%include GammaMuSigma.i
%include GeneralizedExtremeValue.i
%include GeneralizedPareto.i
%include GeneralizedParetoFactory.i
%include Geometric.i
%include GeometricFactory.i
%include Gumbel.i
%include GumbelFactory.i
%include GumbelAB.i
%include GumbelMuSigma.i

/* At last we include template definitions */
%include UncertaintyDistTemplateDefs.i
