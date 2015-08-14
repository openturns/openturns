// SWIG file uncertainty_module.i

%module(package="openturns", docstring="Probabilistic meta-package.") uncertainty
#pragma SWIG nowarn=302,509
//%feature("autodoc","1");

%{
#include "OTconfig.hxx"
#include "OTBase.hxx"
#include "OTUncertainty.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

/* Uncertainty/Model */
/* Uncertainty/Distribution */
%import model_copula_module.i
%import UncertaintyModelCopulaTemplateDefs.i
%import randomvector_module.i
%import dist_bundle1_module.i
%import dist_bundle2_module.i

/* FIXME: Wrong place in Base/Stat */
/* Base/Stat */
%include CorrelationAnalysis.i
%include SensitivityAnalysis.i

/* Uncertainty/Algorithm/Experiment */
%import weightedexperiment_module.i

/* Uncertainty/Algorithm/Classification */
%import classification_module.i
%import UncertaintyClassificationTemplateDefs.i

/* Uncertainty/Algorithm/OrthogonalBasis */
%import orthogonalbasis_module.i
%import UncertaintyOrthogonalBasisTemplateDefs.i

/* Uncertainty/Algorithm/MetaModel */
%import metamodel_module.i
%import UncertaintyMetaModelTemplateDefs.i

/* Uncertainty/Algorithm/QuadraticCumul */
%include QuadraticCumul.i

/* Uncertainty/Algorithm/Sensitivity */
%include ANCOVA.i
%include FAST.i

/* Uncertainty/Algorithm/Transformation */
%import transformation_module.i
%import UncertaintyTransformationTemplateDefs.i

/* Uncertainty/Algorithm/Analytical */
%import analytical_module.i
%import UncertaintyAnalyticalTemplateDefs.i

/* Uncertainty/Algorithm/Simulation */
%import simulation_module.i
%import UncertaintySimulationTemplateDefs.i

/* Uncertainty/StatTests */
%import stattests_module.i
%import UncertaintyStatTestsTemplateDefs.i

/* Uncertainty/Process*/
%import model_process_module.i
%import UncertaintyProcessTemplateDefs.i 

/* At last we include template definitions */
%include UncertaintyTemplateDefs.i
