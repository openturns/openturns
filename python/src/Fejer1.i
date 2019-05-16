// SWIG file Fejer1.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Sample & adaptedNodesOut ($*ltype temp) %{ temp = OT::Sample(); $1 = &temp; %}
%typemap(argout) OT::Sample & adaptedNodesOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Sample(*$1), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN |  0 )); %}

%{
#include "openturns/Fejer1.hxx"
%}

%include Fejer1_doc.i

%include openturns/Fejer1.hxx
namespace OT { %extend Fejer1 { Fejer1(const Fejer1 & other) { return new OT::Fejer1(other); } } }
