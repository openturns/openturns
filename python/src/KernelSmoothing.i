// SWIG file KernelSmoothing.i

%{
#include "KernelSmoothing.hxx"
%}

%include KernelSmoothing_doc.i

%include KernelSmoothing.hxx
namespace OT { %extend KernelSmoothing { KernelSmoothing(const KernelSmoothing & other) { return new OT::KernelSmoothing(other); } } }
