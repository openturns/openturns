// SWIG file func_module.i

%module(package="openturns", docstring="Function primitives.") func


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
%include BaseTypCollection.i
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
%include NormInfEnumerateFunction.i
%include FieldImplementation.i
%include Field.i
%include TimeSeries.i 
%include ProcessSampleImplementation.i
%include ProcessSample.i
%include EvaluationImplementation.i
%include Evaluation.i
%include GradientImplementation.i
%include Gradient.i
%include HessianImplementation.i
%include Hessian.i
%include FunctionImplementation.i
%include ProductFunction.i
%include Function.i
%include EvaluationProxy.i
%include MemoizeEvaluation.i
%include MemoizeFunction.i
%include AggregatedFunction.i
%include SymbolicFunction.i
%include LinearCombinationFunction.i
%include ComposedFunction.i
%include IdentityFunction.i
%include ParametricFunction.i
%include DualLinearCombinationFunction.i
%include FieldFunctionImplementation.i
%include FieldFunction.i
%include FieldToPointFunctionImplementation.i
%include FieldToPointFunction.i
%include PointToFieldFunctionImplementation.i
%include PointToFieldFunction.i
%include ParametricPointToFieldFunction.i
%include VertexValuePointToFieldFunction.i
%include FieldToFieldConnection.i
%include FieldToPointConnection.i
%include PointToFieldConnection.i
%include PointToPointEvaluation.i
%include PointToPointConnection.i
%include VertexFieldToPointFunction.i
%include IndicatorFunction.i
%include DistanceToDomainFunction.i
%include ValueFunction.i
%include VertexValueFunction.i
%include DualLinearCombinationEvaluation.i
%include LinearCombinationEvaluation.i
%include LinearFunction.i
%include TranslationFunction.i
%include QuadraticFunction.i
%include UniVariateFunctionImplementation.i
%include UniVariateFunction.i
%include MonomialFunction.i
%include UniVariatePolynomialImplementation.i
%include UniVariatePolynomial.i
%include AggregatedEvaluation.i
%include SymbolicEvaluation.i
%include ConstantFunction.i
%include DatabaseFunction.i
%include ProductPolynomialEvaluation.i
%include ParametricEvaluation.i
%include BasisImplementation.i
%include Basis.i
%include BasisSequenceImplementation.i
%include BasisSequence.i
%include BasisSequenceFactoryImplementation.i
%include BasisSequenceFactory.i
%include UniVariateFunctionFactory.i
%include MonomialFunctionFactory.i
%include UniVariateFunctionFamily.i
%include TensorizedUniVariateFunctionFactory.i
%include LARS.i
%include BoxCoxTransform.i
%include InverseBoxCoxTransform.i
%include TrendTransform.i
%include InverseTrendTransform.i

/* At last we include template definitions */
%include BaseFuncTemplateDefs.i
