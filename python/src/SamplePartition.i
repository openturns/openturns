// SWIG file SamplePartition.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::SamplePartition & clusters ($*ltype temp) %{ temp = OT::SamplePartition(); $1 = &temp; %}
%typemap(argout) OT::SamplePartition & clusters %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::SamplePartition(*$1), SWIGTYPE_p_OT__SamplePartition, SWIG_POINTER_OWN |  0 )); %}

%{
#include "openturns/SamplePartition.hxx"
%}

%include SamplePartition_doc.i

%copyctor OT::SamplePartition;

%include openturns/SamplePartition.hxx
