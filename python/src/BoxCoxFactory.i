// SWIG file BoxCoxFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::LinearModelResult & linearModelResult ($*ltype temp) %{ temp = OT::LinearModelResult(); $1 = &temp; %}
%typemap(argout) OT::LinearModelResult & linearModelResult %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::LinearModelResult(*$1), SWIGTYPE_p_OT__LinearModelResult, SWIG_POINTER_OWN |  0 )); %}

%typemap(in, numinputs=0) OT::GeneralLinearModelResult & generalLinearModelResult ($*ltype temp) %{ temp = OT::GeneralLinearModelResult(); $1 = &temp; %}
%typemap(argout) OT::GeneralLinearModelResult & generalLinearModelResult %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::GeneralLinearModelResult(*$1), SWIGTYPE_p_OT__GeneralLinearModelResult, SWIG_POINTER_OWN |  0 )); %}

%typemap(in, numinputs=0) OT::Graph & graph ($*ltype temp) %{ temp = OT::Graph(); $1 = &temp; %}
%typemap(argout) OT::Graph & graph %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Graph(*$1), SWIGTYPE_p_OT__Graph, SWIG_POINTER_OWN |  0 )); %}


%{
#include "openturns/BoxCoxFactory.hxx"
%}


%include  BoxCoxFactory_doc.i

%include openturns/BoxCoxFactory.hxx
namespace OT { %extend BoxCoxFactory { BoxCoxFactory(const BoxCoxFactory & other) { return new OT::BoxCoxFactory(other); } } }
