// SWIG file Fejer2.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Sample & adaptedNodesOut ($*ltype temp) %{ temp = OT::Sample(); $1 = &temp; %}
%typemap(argout) OT::Sample & adaptedNodesOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Sample(*$1), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN |  0 )); %}

%{
#include "openturns/Fejer2.hxx"
%}

%include Fejer2_doc.i

%include openturns/Fejer2.hxx
namespace OT { %extend Fejer2 { Fejer2(const Fejer2 & other) { return new OT::Fejer2(other); } } }
