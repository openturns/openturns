// SWIG file IterativeQuantile.i

%{
#include "openturns/IterativeQuantile.hxx"
%}

%include IterativeQuantile_doc.i

%include openturns/IterativeQuantile.hxx

namespace OT {
%extend IterativeQuantile {

IterativeQuantile(const IterativeQuantile & other)
{
return new OT::IterativeQuantile(other);
}

} // IterativeQuantile
} // OT

