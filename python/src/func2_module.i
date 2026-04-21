// SWIG file func2_module.i

%module(package="openturns", docstring="Function primitives.") func2
#pragma SWIG nowarn=302

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
%import func_module.i
%import BaseFuncTemplateDefs.i

/* Base/Func */
%include NoGradient.i
%include NoHessian.i
%include ConstantHessian.i
%include ComposedHessian.i
%include ProductHessian.i
%include DualLinearCombinationHessian.i
%include LinearCombinationHessian.i
%include SymbolicHessian.i
%include ParametricHessian.i
%include NullHessian.i
%include MarginalHessian.i
%include ComposedGradient.i
%include ConstantGradient.i
%include DualLinearCombinationGradient.i
%include LinearGradient.i
%include LinearCombinationGradient.i
%include MarginalGradient.i
%include ParametricGradient.i
%include ProductGradient.i
%include SymbolicGradient.i
%include NoGradient.i
%include NoEvaluation.i
%include TranslationEvaluation.i
%include LinearEvaluation.i
%include QuadraticEvaluation.i
%include ComposedEvaluation.i
%include ProductEvaluation.i
%include PiecewiseLinearEvaluation.i
%include PiecewiseHermiteEvaluation.i
%include IndicatorEvaluation.i
%include DistanceToDomainEvaluation.i
%include MarginalEvaluation.i
%include BasisFactory.i
%include ConstantBasisFactory.i
%include LinearBasisFactory.i
%include QuadraticBasisFactory.i
%include TrendEvaluation.i
%include InverseTrendEvaluation.i
%include BoxCoxEvaluation.i
%include InverseBoxCoxEvaluation.i
