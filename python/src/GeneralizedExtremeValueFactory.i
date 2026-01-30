// SWIG file GeneralizedExtremeValueFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & logLikelihoodOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & logLikelihoodOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"),  SWIG_POINTER_OWN)); %}

%{
#include "openturns/GeneralizedExtremeValueFactory.hxx"
%}

%include GeneralizedExtremeValueFactory_doc.i

%copyctor OT::GeneralizedExtremeValueFactory;

%include openturns/GeneralizedExtremeValueFactory.hxx
