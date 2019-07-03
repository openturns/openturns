// SWIG file IterativeMean.i

%{
#include "openturns/IterativeMean.hxx"
%}

%include IterativeMean_doc.i

OTTypedInterfaceObjectHelper(IterativeMean)

%include openturns/IterativeMean.hxx

namespace OT {
%extend IterativeMean {

IterativeMean(const IterativeMean & other)
{
return new OT::IterativeMean(other);
}

} // IterativeMean
} // OT

