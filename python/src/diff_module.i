// SWIG file diff_module.i

%module(package="openturns", docstring="Differential algorithms.") diff
//%feature("autodoc","1");

%{
#include "OTconfig.hxx"
#include "OTCommon.hxx"
#include "OTType.hxx"
#include "OTStat.hxx"
#include "OTGraph.hxx"
#include "OTFunc.hxx"
#include "OTDiff.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Common */
%import common_module.i

/* Wrapper */
%import wrapper_module.i
%import BaseWrapperTemplateDefs.i

/* Base/Type */
%import typ_module.i
%import BaseTypTemplateDefs.i

/* Base/Stat */
%import statistics_module.i
%import BaseStatisticsTemplateDefs.i

/* Base/Func */
%import func_module.i
%import BaseFuncTemplateDefs.i

/* Base/Diff */
%include FiniteDifferenceStepImplementation.i
%include FiniteDifferenceStep.i
%include ConstantStep.i
%include BlendedStep.i
%include FiniteDifferenceGradient.i
%include FiniteDifferenceHessian.i
%include CenteredFiniteDifferenceGradient.i
%include CenteredFiniteDifferenceHessian.i
%include NonCenteredFiniteDifferenceGradient.i

/* At last we include template definitions */
%include BaseDiffTemplateDefs.i
