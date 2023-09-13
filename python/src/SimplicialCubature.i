// SWIG file SimplicialCubature.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Sample & adaptedNodesOut ($*ltype temp) %{ temp = OT::Sample(); $1 = &temp; %}
%typemap(argout) OT::Sample & adaptedNodesOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Sample(*$1), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN |  0 )); %}

%{
#include "openturns/SimplicialCubature.hxx"
%}

%include SimplicialCubature_doc.i

%include openturns/SimplicialCubature.hxx
namespace OT { %extend SimplicialCubature { SimplicialCubature(const SimplicialCubature & other) { return new OT::SimplicialCubature(other); } } }
