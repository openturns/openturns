// SWIG file solver_module.i

%module(package="openturns", docstring="Solvers.") solver
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
#include "OTSolver.hxx"
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
%import diff_module.i
%import BaseDiffTemplateDefs.i

/* Base/Optim */
%import optim_module.i
%import BaseOptimTemplateDefs.i

/* Base/Solver */
%include SolverImplementation.i
%include Solver.i
%include Bisection.i
%include Secant.i
%include Brent.i
%include ODESolverImplementation.i
%include ODESolver.i
%include RungeKutta.i

/* At last we include template definitions */
%include BaseSolverTemplateDefs.i
