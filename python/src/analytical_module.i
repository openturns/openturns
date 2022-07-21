// SWIG file analytical_module.i

%module(package="openturns", docstring="Analytical uncertainty propagation algorithms.") analytical
#pragma SWIG nowarn=302,509
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTDistribution.hxx"
#include "openturns/OTTransformation.hxx"
#include "openturns/OTAnalytical.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

/* Uncertainty/Model */
/* Uncertainty/Distribution */
%import model_copula_module.i
%import metamodel_module.i
%import UncertaintyModelCopulaTemplateDefs.i

/* Uncertainty/Algorithm/Transformation */
%import transformation_module.i
%import UncertaintyTransformationTemplateDefs.i

/* Uncertainty/Algorithm/Analytical */
%include AnalyticalResult.i
%include Analytical.i
%include FORMResult.i
%include FORM.i
%include SORMResult.i
%include SORM.i
%include StrongMaximumTest.i
%include MultiFORMResult.i
%include SystemFORM.i
%include MultiFORM.i

/* At last we include template definitions */
%include UncertaintyAnalyticalTemplateDefs.i
