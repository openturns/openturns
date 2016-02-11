// SWIG file KernelSmoothing.i

%{
#include "openturns/KernelSmoothing.hxx"
%}

%include KernelSmoothing_doc.i

%include openturns/KernelSmoothing.hxx
namespace OT { %extend KernelSmoothing { KernelSmoothing(const KernelSmoothing & other) { return new OT::KernelSmoothing(other); } } }
