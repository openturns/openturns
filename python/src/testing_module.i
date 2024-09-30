%module(package="openturns", docstring="Test routines.") testing
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OT.hxx"
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

/* Uncertainty/Model */
%import model_copula_module.i

%{
#include "openturns/OTtestcode.hxx"
%}

%include OTtestcode_doc.i

%ignore OT::Test::assert_equal;
%include openturns/OTtestcode.hxx
