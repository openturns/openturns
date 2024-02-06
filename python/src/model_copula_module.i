// SWIG file model_copula_module.i

%module(package="openturns", docstring="Copulas.") model_copula
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

/* Uncertainty/Model */
%include DistributionImplementation.i
%include Distribution.i
%include DistributionFactoryResult.i
%include DistributionParametersImplementation.i
%include DistributionParameters.i
%include DistributionFactoryImplementation.i
%include DistributionFactory.i
%include ContinuousDistribution.i
%include EllipticalDistribution.i
%include DiscreteDistribution.i
%include ArchimedeanCopula.i
%include SklarCopula.i

/* Uncertainty/Distribution (copulas) */
%include AliMikhailHaqCopula.i
%include AliMikhailHaqCopulaFactory.i
%include IndependentCopula.i
%include IndependentCopulaFactory.i
%include MinCopula.i
%include NormalCopula.i
%include NormalCopulaFactory.i
%include FarlieGumbelMorgensternCopula.i
%include FarlieGumbelMorgensternCopulaFactory.i
%include FrankCopula.i
%include FrankCopulaFactory.i
%include ClaytonCopula.i
%include ClaytonCopulaFactory.i
%include GumbelCopula.i
%include GumbelCopulaFactory.i
%include BlockIndependentDistribution.i
%include JointDistribution.i
%include CumulativeDistributionNetwork.i
%include BlockIndependentCopula.i
%include MarshallOlkinCopula.i
%include OrdinalSumCopula.i
%include PlackettCopula.i
%include PlackettCopulaFactory.i

/* At last we include template definitions */
%include UncertaintyModelCopulaTemplateDefs.i
