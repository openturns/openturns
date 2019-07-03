// SWIG file IterativeMoments.i

%{
#include "openturns/IterativeMoments.hxx"
%}

%include IterativeMoments_doc.i

%include openturns/IterativeMoments.hxx

namespace OT {
%extend IterativeMoments {

IterativeMoments(const IterativeMoments & other)
{
return new OT::IterativeMoments(other);
}

} // IterativeMoments
} // OT

