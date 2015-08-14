%module(package="openturns", docstring="Test routines.") testing
//%feature("autodoc","1");

%{
#include "OTconfig.hxx"
#include "OT.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Type */
%import typ_module.i
%import BaseTypTemplateDefs.i

/* Base/Stat */
%import statistics_module.i
%import BaseStatisticsTemplateDefs.i


%{
#include "OTtestcode.hxx"
%}

%include OTtestcode.hxx
