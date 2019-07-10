// SWIG file IterativeVariance.i

%{
#include "openturns/IterativeVariance.hxx"
%}

%include IterativeVariance_doc.i

%include openturns/IterativeVariance.hxx

namespace OT {
%extend IterativeVariance {

IterativeVariance(const IterativeVariance & other)
{
return new OT::IterativeVariance(other);
}

} // IterativeVariance
} // OT

