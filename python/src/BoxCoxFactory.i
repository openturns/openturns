// SWIG file BoxCoxFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::LinearModelResult & linearModelResultOut ($*ltype temp) %{ temp = OT::LinearModelResult(); $1 = &temp; %}
%typemap(argout) OT::LinearModelResult & linearModelResultOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::LinearModelResult(*$1), SWIG_TypeQuery("OT::LinearModelResult *"),  SWIG_POINTER_OWN)); %}

%typemap(in, numinputs=0) OT::GeneralLinearModelResult & generalLinearModelResult ($*ltype temp) %{ temp = OT::GeneralLinearModelResult(); $1 = &temp; %}
%typemap(argout) OT::GeneralLinearModelResult & generalLinearModelResult %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::GeneralLinearModelResult(*$1), SWIG_TypeQuery("OT::GeneralLinearModelResult *"),  SWIG_POINTER_OWN)); %}

%typemap(in, numinputs=0) OT::Graph & graph ($*ltype temp) %{ temp = OT::Graph(); $1 = &temp; %}
%typemap(argout) OT::Graph & graph %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Graph(*$1), SWIG_TypeQuery("OT::Graph *"),  SWIG_POINTER_OWN)); %}


%{
#include "openturns/BoxCoxFactory.hxx"
%}


%include  BoxCoxFactory_doc.i

%copyctor OT::BoxCoxFactory;

%include openturns/BoxCoxFactory.hxx
