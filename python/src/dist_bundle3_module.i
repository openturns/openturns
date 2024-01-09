// SWIG file dist_module.i

%module(package="openturns", docstring="Probabilistic distributions.") dist_bundle3
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
%include OrderStatisticsMarginalChecker.i
%include ParametrizedDistribution.i
%include Poisson.i
%include PoissonFactory.i
%include ProductDistribution.i
%include QuantileMatchingFactory.i
%include RandomMixture.i
%include Rayleigh.i
%include RayleighFactory.i
%include Rice.i
%include RiceFactory.i
%include Skellam.i
%include SkellamFactory.i
%include SmoothedUniform.i
%include SquaredNormal.i
%include Student.i
%include StudentFactory.i
%include Trapezoidal.i
%include TrapezoidalFactory.i
%include Triangular.i
%include TriangularFactory.i
%include TruncatedNormal.i
%include TruncatedNormalFactory.i
%include Uniform.i
%include UniformFactory.i
%include UniformMuSigma.i
%include UniformOverMesh.i
%include UserDefined.i
%include UserDefinedFactory.i
%include VonMises.i
%include VonMisesFactory.i
%include WeibullMin.i
%include WeibullMinFactory.i
%include WeibullMinMuSigma.i
%include Wishart.i
%include ZipfMandelbrot.i


/* At last we include template definitions */
%include UncertaintyDistTemplateDefs.i
