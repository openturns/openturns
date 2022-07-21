// SWIG file FejerAlgorithm.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Sample & adaptedNodesOut ($*ltype temp) %{ temp = OT::Sample(); $1 = &temp; %}
%typemap(argout) OT::Sample & adaptedNodesOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Sample(*$1), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN |  0 )); %}

%{
#include "openturns/FejerAlgorithm.hxx"
%}

%include FejerAlgorithm_doc.i

%include openturns/FejerAlgorithm.hxx
namespace OT { %extend FejerAlgorithm { FejerAlgorithm(const FejerAlgorithm & other) { return new OT::FejerAlgorithm(other); } } }
