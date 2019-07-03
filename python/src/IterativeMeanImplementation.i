// SWIG file IterativeMeanImplementation.i

%{
#include "openturns/IterativeMeanImplementation.hxx"
%}

%include IterativeMeanImplementation_doc.i

%include openturns/IterativeMeanImplementation.hxx

namespace OT {
%extend IterativeMeanImplementation {

IterativeMeanImplementation(const IterativeMeanImplementation & other)
{
return new OT::IterativeMeanImplementation(other);
}

} // IterativeMeanImplementation
} // OT

