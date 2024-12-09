// SWIG file FejerExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Sample & adaptedNodesOut ($*ltype temp) %{ temp = OT::Sample(); $1 = &temp; %}
%typemap(argout) OT::Sample & adaptedNodesOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Sample(*$1), SWIGTYPE_p_OT__Sample,  SWIG_POINTER_OWN)); %}

%{
#include "openturns/FejerExperiment.hxx"
%}

%include FejerExperiment_doc.i

%copyctor OT::FejerExperiment;

%include openturns/FejerExperiment.hxx
