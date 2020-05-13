// SWIG file IterativeSkewness.i

%{
#include "openturns/IterativeSkewness.hxx"
%}

%include IterativeSkewness_doc.i

%include openturns/IterativeSkewness.hxx

namespace OT {
%extend IterativeSkewness {

IterativeSkewness(const IterativeSkewness & other)
{
return new OT::IterativeSkewness(other);
}

} // IterativeSkewness
} // OT

