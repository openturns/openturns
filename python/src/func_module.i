// SWIG file func_module.i

%module(package="openturns", docstring="Function primitives.") func
#pragma SWIG nowarn=302,314,509
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTCommon.hxx"
#include "openturns/OTType.hxx"
#include "openturns/OTStat.hxx"
#include "openturns/OTGraph.hxx"
#include "openturns/OTFunc.hxx"
#include "openturns/OTGeom.hxx"
#include "openturns/OTAlgo.hxx"
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
%include ProductFunction.i
%include NumericalMathFunction.i
%include AggregatedFunction.i
%include SymbolicFunction.i
%include LinearCombinationFunction.i
%include ComposedFunction.i
%include ParametricFunction.i
%include DualLinearCombinationFunction.i
%include FieldFunctionImplementation.i
%include FieldFunction.i
%include IndicatorFunction.i
%include ValueFunction.i
%include VertexValueFunction.i
%include DualLinearCombinationEvaluationImplementation.i
%include DualLinearCombinationGradientImplementation.i
%include DualLinearCombinationHessianImplementation.i
%include LinearCombinationEvaluationImplementation.i
%include LinearCombinationGradientImplementation.i
%include LinearCombinationHessianImplementation.i
%include LinearFunction.i
%include QuadraticFunction.i
%include UniVariateFunctionImplementation.i
%include UniVariateFunction.i
%include MonomialFunction.i
%include UniVariatePolynomialImplementation.i
%include UniVariatePolynomial.i
%include AggregatedNumericalMathEvaluationImplementation.i
%include PiecewiseLinearEvaluationImplementation.i
%include PiecewiseHermiteEvaluationImplementation.i
#ifdef OPENTURNS_HAVE_MUPARSER
%include SymbolicEvaluation.i
%include SymbolicGradient.i
%include SymbolicHessian.i
#endif
%include DatabaseFunction.i
%include DatabaseNumericalMathEvaluationImplementation.i
%include IndicatorNumericalMathEvaluationImplementation.i
%include ProductPolynomialEvaluationImplementation.i
%include ParametricEvaluationImplementation.i
%include ParametricGradientImplementation.i
%include ParametricHessianImplementation.i
%include P1LagrangeEvaluationImplementation.i
%include BasisImplementation.i
%include Basis.i
%include BasisSequenceImplementation.i
%include BasisSequence.i
%include FunctionalBasisImplementation.i
%include FunctionalBasis.i
%include BasisSequenceFactoryImplementation.i
%include BasisSequenceFactory.i
%include UniVariateFunctionFactory.i
%include MonomialFunctionFactory.i
%include UniVariateFunctionFamily.i
%include TensorizedUniVariateFunctionFactory.i
%include LARS.i
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
