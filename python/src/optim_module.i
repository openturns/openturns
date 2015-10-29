// SWIG file optim_module.i

%module(package="openturns", docstring="Optimization routines.") optim
//%feature("autodoc","1");

%{
#include "OTconfig.hxx"
#include "OTCommon.hxx"
#include "OTType.hxx"
#include "OTStat.hxx"
#include "OTGraph.hxx"
#include "OTFunc.hxx"
#include "OTDiff.hxx"
#include "OTOptim.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Common */
%import common_module.i

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
%import diff_module.i
%import BaseDiffTemplateDefs.i

/* Base/Optim */
%include OptimizationProblemImplementation.i
%include OptimizationProblem.i
%include OptimizationResult.i
%include OptimizationSolverImplementation.i
%include OptimizationSolver.i
%include NearestPointCheckerResult.i
%include NearestPointChecker.i
%include AbdoRackwitzSpecificParameters.i
%include AbdoRackwitz.i
%include CobylaSpecificParameters.i
%include Cobyla.i
%include TNCSpecificParameters.i
%include TNC.i
%include SQPSpecificParameters.i
%include SQP.i
%include NLopt.i

/* At last we include template definitions */
%include BaseOptimTemplateDefs.i
