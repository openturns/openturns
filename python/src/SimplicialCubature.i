// SWIG file SimplicialCubature.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Sample & adaptedNodesOut ($*ltype temp) %{ temp = OT::Sample(); $1 = &temp; %}
%typemap(argout) OT::Sample & adaptedNodesOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Sample(*$1), SWIG_TypeQuery("OT::Sample *"),  SWIG_POINTER_OWN)); %}

%{
#include "openturns/SimplicialCubature.hxx"
%}

%include SimplicialCubature_doc.i

%copyctor OT::SimplicialCubature;

%include openturns/SimplicialCubature.hxx
