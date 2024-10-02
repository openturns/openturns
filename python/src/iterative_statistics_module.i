// SWIG file iterative_statistics_module.i

%module(package="openturns", docstring="Iterative statistics.") iterative_statistics

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTCommon.hxx"
#include "openturns/OTType.hxx"
#include "openturns/OTIterativeStat.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Common */
%import common_module.i

/* Base/Type */
%import typ_module.i

/* Base/IterativeStat */
%include IterativeAlgorithmImplementation.i
%include IterativeAlgorithm.i
%include IterativeExtrema.i
%include IterativeThresholdExceedance.i
%include IterativeMoments.i
