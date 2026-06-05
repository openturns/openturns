// SWIG file experimental_module.i

%module(package="openturns", docstring="Experimental features.") experimental


%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTAlgorithm.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTDistribution.hxx"
#include "openturns/OTProcess.hxx"
#include "openturns/OTWeightedExperiments.hxx"
#include "openturns/OTOrthogonalBasis.hxx"
#include "openturns/OTMetaModel.hxx"
#include "openturns/OTSimulation.hxx"
#include "openturns/OTSolver.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Type */
%include BaseTypCollection.i
%import typ_module.i

/* Base/Func */
%include BaseFuncCollection.i
%import base_module.i

/* Base/Optim */
%import optim_module.i
%include LinearProblem.i
%include HiGHS.i

/* Uncertainty/Distribution */
%import model_copula_module.i
%import dist_bundle2_module.i
%import dist_bundle3_module.i

/* Uncertainty/Algorithm/OrthogonalBasis */
%import orthogonalbasis_module.i
%import UncertaintyOrthogonalBasisTemplateDefs.i
%include FiniteOrthogonalFunctionFactory.i

/* Uncertainty/Algorithm/WeightedExperiment */
%import weightedexperiment_module.i
%include UncertaintyWeightedExperimentTemplateDefs.i
%include FejerExperiment.i

/* Uncertainty */
%import metamodel_module.i
%include simulation_module.i

/* Uncertainty/Process */
%import model_process_module.i

/* Uncertainty/Distribution */
%include MarginalUniformOrderStatistics.i

/* Uncertainty/Algorithm/Transformation */
%{
namespace OT {
  template <>
  struct traitsPythonType<OT::Distribution>
  {
    typedef _PyObject_ Type;
  };
  template <>
  inline bool canConvert< _PyObject_, OT::Distribution >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::Distribution *"), SWIG_POINTER_NO_NULL))) {
      OT::Distribution * p_it = reinterpret_cast< OT::Distribution * >(ptr);
      return p_it != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::DistributionImplementation *"), SWIG_POINTER_NO_NULL))) {
      OT::DistributionImplementation * p_impl = reinterpret_cast< OT::DistributionImplementation * >(ptr);
      return p_impl != NULL;
    }
    return false;
  }
  template <>
  inline OT::Distribution convert< _PyObject_, OT::Distribution >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::Distribution *"), SWIG_POINTER_NO_NULL))) {
      OT::Distribution * p_it = reinterpret_cast< OT::Distribution * >(ptr);
      return *p_it;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::DistributionImplementation *"), SWIG_POINTER_NO_NULL))) {
      OT::DistributionImplementation * p_impl = reinterpret_cast< OT::DistributionImplementation * >(ptr);
      return *p_impl;
    } else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Distribution";
    }
    return OT::Distribution();
  }
}
%}
%include MarginalTransformationFunction.i

/* Uncertainty/Algorithm/Metamodel */
%include LinearModelValidation.i
%include EfficientGlobalOptimization.i
%include GaussianProcessRegressionCrossValidation.i

