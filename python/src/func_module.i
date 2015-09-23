// SWIG file func_module.i

%module(package="openturns", docstring="Function primitives.") func
#pragma SWIG nowarn=302,314,509
//%feature("autodoc","1");

%{
#include "OTconfig.hxx"
#include "OTCommon.hxx"
#include "OTType.hxx"
#include "OTStat.hxx"
#include "OTGraph.hxx"
#include "OTFunc.hxx"
#include "OTGeom.hxx"
#include "OTAlgo.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Common */
%import common_module.i
%import BaseCommonTemplateDefs.i

/* Base/Type */
%import typ_module.i
%import BaseTypTemplateDefs.i

/* Base/Geom */
%import geom_module.i
%import BaseGeomTemplateDefs.i

/* Base/Func */
%include EnumerateFunctionImplementation.i
%include EnumerateFunction.i
%include LinearEnumerateFunction.i
%include HyperbolicAnisotropicEnumerateFunction.i
%include FieldImplementation.i
%include Field.i
%include TimeSeries.i 
%include ProcessSample.i 
%include SpecFunc.i
%include NumericalMathEvaluationImplementation.i
%include NumericalMathGradientImplementation.i
%include NumericalMathHessianImplementation.i
%include NoNumericalMathEvaluationImplementation.i
%include NoNumericalMathGradientImplementation.i
%include NoNumericalMathHessianImplementation.i
%include LinearNumericalMathEvaluationImplementation.i
%include LinearNumericalMathGradientImplementation.i
%include QuadraticNumericalMathEvaluationImplementation.i
%include ConstantNumericalMathGradientImplementation.i
%include ConstantNumericalMathHessianImplementation.i
%include ComposedNumericalMathEvaluationImplementation.i
%include ComposedNumericalMathGradientImplementation.i
%include ComposedNumericalMathHessianImplementation.i
%include ProductNumericalMathEvaluationImplementation.i
%include ProductNumericalMathGradientImplementation.i
%include ProductNumericalMathHessianImplementation.i
%include NumericalMathFunctionImplementation.i
%include ProductNumericalMathFunction.i
%include NumericalMathFunction.i
%include DynamicalFunctionImplementation.i
%include DynamicalFunction.i
%include SpatialFunction.i
%include TemporalFunction.i
%include ComposedNumericalMathFunction.i
%include DualLinearCombinationEvaluationImplementation.i
%include DualLinearCombinationGradientImplementation.i
%include DualLinearCombinationHessianImplementation.i
%include LinearCombinationEvaluationImplementation.i
%include LinearCombinationGradientImplementation.i
%include LinearCombinationHessianImplementation.i
%include LinearNumericalMathFunction.i
%include QuadraticNumericalMathFunction.i
%include UniVariatePolynomialImplementation.i
%include UniVariatePolynomial.i
%include AggregatedNumericalMathEvaluationImplementation.i
%include PiecewiseLinearEvaluationImplementation.i
%include PiecewiseHermiteEvaluationImplementation.i
#ifdef OPENTURNS_HAVE_MUPARSER
%include AnalyticalNumericalMathEvaluationImplementation.i
%include AnalyticalNumericalMathGradientImplementation.i
%include AnalyticalNumericalMathHessianImplementation.i
#endif
%include DatabaseNumericalMathEvaluationImplementation.i
%include IndicatorNumericalMathEvaluationImplementation.i
%include ProductPolynomialEvaluationImplementation.i
%include ParametricEvaluationImplementation.i
%include ParametricGradientImplementation.i
%include ParametricHessianImplementation.i
%include BasisImplementation.i
%include Basis.i
%include BasisSequenceImplementation.i
%include BasisSequence.i
%include FunctionalBasisImplementation.i
%include FunctionalBasis.i
%include BasisSequenceFactoryImplementation.i
%include BasisSequenceFactory.i
%include LAR.i
%include BoxCoxEvaluationImplementation.i
%include BoxCoxTransform.i
%include InverseBoxCoxEvaluationImplementation.i
%include InverseBoxCoxTransform.i
%include TrendEvaluationImplementation.i
%include TrendTransform.i
%include InverseTrendEvaluationImplementation.i
%include InverseTrendTransform.i
%include BasisFactory.i
%include ConstantBasisFactory.i
%include LinearBasisFactory.i
%include QuadraticBasisFactory.i

/* At last we include template definitions */
%include BaseFuncTemplateDefs.i
