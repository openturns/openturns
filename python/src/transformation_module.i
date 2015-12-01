// SWIG file transformation_module.i

%module(package="openturns", docstring="Transformations.") transformation
#pragma SWIG nowarn=302,509
//%feature("autodoc","1");

%{
#include "OTconfig.hxx"
#include "OTBase.hxx"
#include "OTModel.hxx"
#include "OTDistribution.hxx"
#include "OTTransformation.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%include BaseFuncCollection.i
%import base_module.i

/* Uncertainty/Model */
/* Uncertainty/Distribution */
%include UncertaintyModelCopulaCollection.i
%import model_copula_module.i
%import UncertaintyModelCopulaTemplateDefs.i

/* Uncertainty/Algorithm/Transformation */
%include MarginalTransformationEvaluation.i
%include MarginalTransformationGradient.i
%include MarginalTransformationHessian.i
%include NatafEllipticalCopulaEvaluation.i
%include NatafEllipticalCopulaGradient.i
%include NatafEllipticalCopulaHessian.i
%include InverseNatafEllipticalCopulaEvaluation.i
%include InverseNatafEllipticalCopulaGradient.i
%include InverseNatafEllipticalCopulaHessian.i
%include NatafIndependentCopulaEvaluation.i
%include NatafIndependentCopulaGradient.i
%include NatafIndependentCopulaHessian.i
%include InverseNatafIndependentCopulaEvaluation.i
%include InverseNatafIndependentCopulaGradient.i
%include InverseNatafIndependentCopulaHessian.i
%include NatafEllipticalDistributionEvaluation.i
%include NatafEllipticalDistributionGradient.i
%include NatafEllipticalDistributionHessian.i
%include InverseNatafEllipticalDistributionEvaluation.i
%include InverseNatafEllipticalDistributionGradient.i
%include InverseNatafEllipticalDistributionHessian.i
%include RosenblattEvaluation.i
%include InverseRosenblattEvaluation.i

/* Uncertainty/Algorithm/Transformation/ProcessTransformation */
%import metamodel_module.i
%include BoxCoxFactory.i
%include TrendFactory.i

/* At last we include template definitions */
%include UncertaintyTransformationTemplateDefs.i
