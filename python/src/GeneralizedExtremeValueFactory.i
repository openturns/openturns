// SWIG file GeneralizedExtremeValueFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & logLikelihoodOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & logLikelihoodOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIGTYPE_p_OT__Point, SWIG_POINTER_OWN |  0 )); %}

%{
#include "openturns/GeneralizedExtremeValueFactory.hxx"
%}

%include GeneralizedExtremeValueFactory_doc.i

%include openturns/GeneralizedExtremeValueFactory.hxx
namespace OT { %extend GeneralizedExtremeValueFactory { GeneralizedExtremeValueFactory(const GeneralizedExtremeValueFactory & other) { return new OT::GeneralizedExtremeValueFactory(other); } } }
