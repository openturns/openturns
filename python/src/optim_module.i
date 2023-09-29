// SWIG file optim_module.i

%module(package="openturns", docstring="Optimization routines.") optim
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTCommon.hxx"
#include "openturns/OTType.hxx"
#include "openturns/OTStat.hxx"
#include "openturns/OTGraph.hxx"
#include "openturns/OTFunc.hxx"
#include "openturns/OTDiff.hxx"
#include "openturns/OTExperiments.hxx"
#include "openturns/OTGeom.hxx"
#include "openturns/OTOptim.hxx"
%}

%init %{
OT::Ceres::Initialize();
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

/* Base/Experiment */
%import experiment_module.i
%import BaseExperimentTemplateDefs.i

/* Base/Optim */
%include OptimizationProblemImplementation.i
%include OptimizationProblem.i
%include NearestPointProblem.i
%include LeastSquaresProblem.i
%include OptimizationResult.i
%include OptimizationAlgorithmImplementation.i
%include OptimizationAlgorithm.i
%include AbdoRackwitz.i
%include Bonmin.i
%include Ceres.i
%include CMinpack.i
%include Cobyla.i
%include Dlib.i
%include Ipopt.i
%include TNC.i
%include SQP.i
%include MultiStart.i
%include NLopt.i
%include Pagmo.i

/* Base/Geom */
%import geom_module.i
%import BaseGeomTemplateDefs.i
%include LevelSet.i
%include LevelSetMesher.i

/* At last we include template definitions */
%include BaseOptimTemplateDefs.i
